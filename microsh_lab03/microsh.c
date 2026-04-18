#define _GNU_SOURCE
#include "common.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 * Lab 3 — microsh (lite) (Systems Programming)
 *
 * - shell loop: getline -> tokenize -> parse -> fork/exec -> waitpid
 * - redirects: <, > and >> using open + dup2 + close
 * - one pipeline: cmd1 | cmd2 using pipe() + 2 forks + correct closes
 * - prompt: displays the exit status of the last command
 *
 * Simplifications (as in slides):
 * - require spaces around | < > >> (e.g., "cat < in.txt | wc >> out.txt")
 * - no quotes/escaping, no ;, no background jobs
 * - only built-in: exit
 */

#define MAX_TOK 128
#define MAX_ARGS 64

typedef struct Cmd {
    char *argv[MAX_ARGS];
    int argc;
    char *in_path;
    char *out_path;
    int append; /* 1 for '>>', 0 for '>' */
} Cmd;

/*
 * Tokenize the line on whitespace.
 * Returns number of tokens, or -1 if too many tokens.
 */
static int tokenize_ws(char *line, char *tokv[], int maxtok)
{
    int n = 0;
    char *save = NULL;

    for (char *t = strtok_r(line, " \t\r\n", &save);
         t != NULL;
         t = strtok_r(NULL, " \t\r\n", &save)) {
        if (n >= maxtok) return -1;
        tokv[n++] = t;
    }

    return n;
}

/*
 * Parse tokens into a Cmd:
 *   - argv[] for execvp
 *   - in_path for '<'
 *   - out_path for '>' or '>>'
 *
 * Notes:
 * - This function does NOT allow '|'.
 * - Returns 0 on success, -1 on syntax/validation error.
 */
static int parse_cmd(char *tokv[], int ntok, Cmd *cmd)
{
    memset(cmd, 0, sizeof(*cmd));

    for (int i = 0; i < ntok; ) {
        char *t = tokv[i];

        if (strcmp(t, "<") == 0) {
            if (cmd->in_path) return -1;          /* duplicate '<' */
            if (i + 1 >= ntok) return -1;         /* missing file */
            cmd->in_path = tokv[i + 1];
            i += 2;
            continue;
        }

        if (strcmp(t, ">") == 0) {
            if (cmd->out_path) return -1;         /* duplicate '>' */
            if (i + 1 >= ntok) return -1;         /* missing file */
            cmd->out_path = tokv[i + 1];
            i += 2;
            continue;
        }

        if (strcmp(t, ">>") == 0) {
            if (cmd->out_path) return -1;         /* duplicate '>' or '>>' */
            if (i + 1 >= ntok) return -1;         /* missing file */
            cmd->out_path = tokv[i + 1];
            cmd->append = 1;                      /* flag for O_APPEND */
            i += 2;
            continue;
        }

        if (strcmp(t, "|") == 0) {
            return -1; /* pipe handled elsewhere */
        }

        if (cmd->argc >= MAX_ARGS - 1) return -1; /* keep room for NULL */
        cmd->argv[cmd->argc++] = t;
        i++;
    }

    cmd->argv[cmd->argc] = NULL;
    return (cmd->argc > 0) ? 0 : -1;
}

/*
 * Apply < and/or > redirections inside a child process.
 * allow_in/allow_out let us enforce simple rules for pipelines:
 *   - in redirection only on left cmd
 *   - out redirection only on right cmd
 *
 * On error, print perror/warn and _exit(127).
 */
static void apply_redirects(const Cmd *cmd, int allow_in, int allow_out)
{
    if (cmd->in_path && !allow_in) {
        warn_syntax("input redirection not allowed here");
        _exit(127);
    }
    if (cmd->out_path && !allow_out) {
        warn_syntax("output redirection not allowed here");
        _exit(127);
    }

    if (cmd->in_path) {
        int fd = open(cmd->in_path, O_RDONLY);
        if (fd < 0) { perror("open <"); _exit(127); }
        if (dup2(fd, STDIN_FILENO) < 0) { perror("dup2 <"); _exit(127); }
        close(fd); /* important: avoid FD leaks */
    }

    if (cmd->out_path) {
        /* choose O_APPEND if '>>', else O_TRUNC for '>' */
        int fd = cmd->append == 1 ? open(cmd->out_path, O_WRONLY | O_CREAT | O_APPEND, 0644) : open(cmd->out_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) { perror("open >"); _exit(127); }
        if (dup2(fd, STDOUT_FILENO) < 0) { perror("dup2 >"); _exit(127); }
        close(fd);
    }
}

/*
 * Run a single command (optionally with < and >).
 * Parent waits for the child and returns its status.
 */
static int run_simple(const Cmd *cmd)
{
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        apply_redirects(cmd, 1, 1);
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp");
        _exit(127);
    }

    int status = 0;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return -1;
    }

    /* extract actual exit code or return 128 on abnormal termination */
    return WIFEXITED(status) ? WEXITSTATUS(status) : 128;
}

/*
 * Run a single pipeline: left | right
 * - Creates one pipe
 * - Forks two children
 * - Wires stdout of left -> pipe write end
 * - Wires stdin  of right <- pipe read end
 * - Closes unused ends in both children and in parent
 */
static int run_pipe(const Cmd *left, const Cmd *right)
{
    int pfd[2];
    if (pipe(pfd) < 0) { perror("pipe"); return -1; }

    pid_t p1 = fork();
    if (p1 < 0) { perror("fork"); return -1; }

    if (p1 == 0) {
        /* child 1: cmd1 writes into pipe */
        if (dup2(pfd[1], STDOUT_FILENO) < 0) { perror("dup2 pipe->1"); _exit(127); }
        close(pfd[0]);
        close(pfd[1]);

        /* allow < only on the left side */
        apply_redirects(left, 1, 0);
        execvp(left->argv[0], left->argv);
        perror("execvp left");
        _exit(127);
    }

    pid_t p2 = fork();
    if (p2 < 0) { perror("fork"); return -1; }

    if (p2 == 0) {
        /* child 2: cmd2 reads from pipe */
        if (dup2(pfd[0], STDIN_FILENO) < 0) { perror("dup2 pipe->0"); _exit(127); }
        close(pfd[0]);
        close(pfd[1]);

        /* allow > only on the right side */
        apply_redirects(right, 0, 1);
        execvp(right->argv[0], right->argv);
        perror("execvp right");
        _exit(127);
    }

    /* parent: must close BOTH ends, otherwise reader may never see EOF */
    close(pfd[0]);
    close(pfd[1]);

    int st1 = 0, st2 = 0;
    if (waitpid(p1, &st1, 0) < 0) perror("waitpid p1");
    if (waitpid(p2, &st2, 0) < 0) perror("waitpid p2");

    /* common convention: pipeline status = status of the rightmost command */
    return WIFEXITED(st2) ? WEXITSTATUS(st2) : 128;
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    char *line = NULL;
    size_t cap = 0;
    int last_status = 0; /* track exit status of last command */

    for (;;) {
        /* display prompt with last status */
        fprintf(stdout, "microsh [st=%d]> ", last_status);
        fflush(stdout);

        ssize_t nread = getline(&line, &cap, stdin);
        if (nread < 0) break; /* EOF */

        char *tokv[MAX_TOK];
        int ntok = tokenize_ws(line, tokv, MAX_TOK);
        if (ntok <= 0) continue;

        /* built-in: exit */
        if (strcmp(tokv[0], "exit") == 0) break;

        /* find a single | (if present) */
        int pipe_pos = -1;
        for (int i = 0; i < ntok; i++) {
            if (strcmp(tokv[i], "|") == 0) { pipe_pos = i; break; }
        }

        if (pipe_pos < 0) {
            Cmd c;
            if (parse_cmd(tokv, ntok, &c) < 0) {
                warn_syntax("bad command");
                continue;
            }
            last_status = run_simple(&c); /* update status */
        } else {
            Cmd l, r;
            if (parse_cmd(tokv, pipe_pos, &l) < 0 ||
                parse_cmd(tokv + pipe_pos + 1, ntok - pipe_pos - 1, &r) < 0) {
                warn_syntax("bad pipeline");
                continue;
            }
            last_status = run_pipe(&l, &r); /* update status */
        }
    }

    free(line);
    return 0;
}
