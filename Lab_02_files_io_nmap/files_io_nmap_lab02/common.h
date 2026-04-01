#ifndef COMMON_H
#define COMMON_H

/*
 * common.h - Tiny helpers for Lab (System Programming)
 *
 * Goals:
 *  - Keep error/usage handling consistent across small utilities.
 *  - Prefer simple, deterministic exit codes:
 *      0: success
 *      1: runtime error (syscall failure, invalid file, etc.)
 *      2: usage error (wrong arguments)
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

static inline void die_perror(const char *msg)
{
  perror(msg);
  exit(1);
}

static inline void usage_cplite(const char *prog)
{
  fprintf(stderr, "Usage: %s [-b BUFSZ] SRC DST\n", prog);
  exit(2);
}

static inline void usage_wclite(const char *prog)
{
  fprintf(stderr, "Usage: %s FILE\n", prog);
  exit(2);
}

static inline void usage_mmapview(const char *prog)
{
  fprintf(stderr, "Usage: %s FILE\n", prog);
  exit(2);
}

#endif
