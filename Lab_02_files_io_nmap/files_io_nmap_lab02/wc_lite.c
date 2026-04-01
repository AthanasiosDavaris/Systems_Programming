/*
 * wc_lite.c - "wc-lite" for Lab (Systems Programming)
 *
 * Counts:
 *  - bytes
 *  - lines (number of '\n' bytes)
 *
 * Constraints (per lab/assignment):
 *  - Use only file descriptors: open/read/close
 *  - No stdio read loops (no fgets, no fread)
 */

#include "common.h"

#include <fcntl.h>
#include <unistd.h>

#define BUFSZ (64 * 1024)

int main(int argc, char **argv)
{
  if (argc != 2) usage_wclite(argv[0]);

  const char *path = argv[1];
  int fd = -1;
  unsigned char buf[BUFSZ];
  size_t bytes = 0;
  size_t lines = 0;
  int rc = 1;

  fd = open(path, O_RDONLY);
  if (fd < 0) {
    perror("open");
    goto cleanup;
  }

  for (;;) {
    ssize_t r = read(fd, buf, sizeof(buf));
    if (r < 0) {
      if (errno == EINTR) continue;
      perror("read");
      goto cleanup;
    }
    if (r == 0) break; /* EOF */

    bytes += (size_t)r;
    for (ssize_t i = 0; i < r; i++) {
      if (buf[i] == '\n') lines++;
    }
  }

  /*
   * Output format matches mmap-view for easy strace comparison.
   */
  printf("bytes=%zu lines=%zu\n", bytes, lines);
  rc = 0;

cleanup:
  if (fd >= 0) close(fd);
  return rc;
}
