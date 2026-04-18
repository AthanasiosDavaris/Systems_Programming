#ifndef SYSPROG_LAB2_COMMON_H
#define SYSPROG_LAB2_COMMON_H

/*
 * common.h
 * --------
 * Μικρό κοινό header για τα παραδείγματα.
 * απλό error handling με perror() και ένα ξεκάθαρο exit.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/* Τερμάτισε με μήνυμα βασισμένο στο errno (όταν μια κλήση αποτυγχάνει). */
#define DIE(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

#endif /* SYSPROG_LAB2_COMMON_H */
