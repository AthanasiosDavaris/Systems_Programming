#ifndef COMMON_H
#define COMMON_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * die_perror: Print perror(msg) and exit(1).
 * Use for unrecoverable errors in the parent process.
 */
static inline void die_perror(const char *msg)
{
    perror(msg);
    exit(1);
}

/*
 * die_msg: Print a simple message and exit(1).
 */
static inline void die_msg(const char *msg)
{
    fprintf(stderr, "ERR: %s\n", msg);
    exit(1);
}

/*
 * usage_microsh: Print usage/help and exit(2).
 */
static inline void usage_microsh(const char *prog)
{
    fprintf(stderr,
            "Usage: %s\n"
            "  Interactive micro-shell (lite):\n"
            "    - built-in: exit\n"
            "    - supports: <  >  and a single |\n"
            "    - requires spaces around | < >\n",
            prog);
    exit(2);
}

/*
 * warn_syntax: Non-fatal syntax warning; the shell keeps running.
 */
static inline void warn_syntax(const char *msg)
{
    fprintf(stderr, "Syntax: %s\n", msg);
}

#endif
