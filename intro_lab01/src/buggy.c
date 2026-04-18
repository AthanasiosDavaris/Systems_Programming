/*
 * buggy.c
 * -------
 * Μικρό πρόγραμμα για demo debugging με gdb.
 * Περιέχει σκόπιμο bug: αν τρέξει χωρίς όρισμα, κάνει segfault.
 *
 * Δοκιμή:
 *   $ make run-buggy
 *   $ gdb --args ./bin/buggy
 */

#include <stdio.h>
#include <stdlib.h>

static int parse_port(const char *s)
{
    /* BUG: αν s == NULL, θα crash-άρει (argc == 1) */
    return atoi(s);
}

int main(int argc, char **argv)
{
    (void)argc; /* το bug είναι ότι δεν το ελέγχουμε */

    int port = parse_port(argv[1]);
    printf("Port = %d\n", port);
    return 0;
}
