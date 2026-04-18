/*
 * hello_errno.c
 * ------------
 * Άσκηση hello-errno:
 * Κάνουμε 3 αποτυχημένες κλήσεις και τυπώνουμε κατανοητά μηνύματα.
 * Στόχος: return codes + errno + perror/strerror.
 *
 * Tip για strace:
 *   $ strace -o trace.txt -e trace=openat,read,write,close,chdir ./bin/hello_errno
 */

#include <errno.h>
#include <fcntl.h>     // open
#include <stdio.h>
#include <string.h>    // strerror
#include <unistd.h>    // close, chdir

static void print_errno(const char *op)
{
    /* errno: αριθμητικό code, strerror(errno): ανθρώπινη περιγραφή */
    fprintf(stderr, "[FAIL] %s: errno=%d (%s)\n", op, errno, strerror(errno));
}

int main(void)
{
    /* 1) open: ENOENT (το αρχείο δεν υπάρχει) */
    errno = 0;
    int fd = open("no-such-file", O_RDONLY);
    if (fd < 0) {
        print_errno("open(\"no-such-file\")");
        perror("perror(open)");
    } else {
        /* Αν ποτέ συμβεί να υπάρχει το αρχείο, κλείνουμε κανονικά. */
        close(fd);
    }

    /* 2) chdir: ENOENT (ο φάκελος δεν υπάρχει) */
    errno = 0;
    if (chdir("/no-such-dir") < 0) {
        print_errno("chdir(\"/no-such-dir\")");
        perror("perror(chdir)");
    }

    /* 3) close: EBADF (invalid file descriptor) */
    errno = 0;
    if (close(-1) < 0) {
        print_errno("close(-1)");
        perror("perror(close)");
    }

    puts("Done. (Expected: 3 failures above)");
    return 0;
}
