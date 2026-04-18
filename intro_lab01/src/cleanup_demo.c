/*
 * cleanup_demo.c
 * --------------
 * Μικρό demo για το pattern goto cleanup.
 * Διαβάζει έως 4096 bytes από ένα αρχείο (path από argv) και τα γράφει στο stdout.
 *
 * Δοκιμή:
 *   $ make cleanup_demo
 *   $ ./bin/cleanup_demo /etc/hostname
 *   $ ./bin/cleanup_demo /nope   (θα δείτε perror)
 */

#include "common.h"

#include <fcntl.h>   // open
#include <unistd.h>  // read, close, write

int main(int argc, char **argv)
{
    int ret = 0;
    int fd = -1;
    char *buf = NULL;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return 1;
    }

    /* Ανοίγουμε το αρχείο (αν αποτύχει, errno θα έχει λόγο) */
    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open");
        ret = 1;
        goto cleanup;
    }

    buf = malloc(4096);
    if (!buf) {
        perror("malloc");
        ret = 1;
        goto cleanup;
    }

    /* Διαβάζουμε λίγα bytes (δεν μας νοιάζει να είναι τέλειο I/O demo) */
    ssize_t n = read(fd, buf, 4096);
    if (n < 0) {
        perror("read");
        ret = 1;
        goto cleanup;
    }

    /* Γράφουμε ό,τι διαβάσαμε */
    if (n > 0) {
        (void)write(STDOUT_FILENO, buf, (size_t)n);
    }

cleanup:
    free(buf);
    if (fd >= 0) {
        close(fd);
    }
    return ret;
}
