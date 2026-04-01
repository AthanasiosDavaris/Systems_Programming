/*
 * mmap_view.c - "mmap-view" for Lab (Systems Programming)
 *
 * Maps a regular file into memory (read-only) and scans it without a read() loop.
 *
 * Steps:
 *  - open
 *  - fstat (get file size)
 *  - mmap(PROT_READ, MAP_PRIVATE)
 *  - scan bytes/lines
 *  - munmap + close
 */

#include "common.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

static int is_regular_file(mode_t m)
{
  return S_ISREG(m) ? 1 : 0;
}

int main(int argc, char **argv)
{
  if (argc != 2) usage_mmapview(argv[0]);

  const char *path = argv[1];
  int fd = -1;
  struct stat st;
  void *addr = MAP_FAILED;
  size_t n = 0;
  size_t lines = 0;
  int rc = 1;

  fd = open(path, O_RDONLY);
  if (fd < 0) {
    perror("open");
    goto cleanup;
  }

  if (fstat(fd, &st) < 0) {
    perror("fstat");
    goto cleanup;
  }

  if (!is_regular_file(st.st_mode)) {
    fprintf(stderr, "Not a regular file\n");
    goto cleanup;
  }

  if (st.st_size <= 0) {
    /* Empty file (or size unknown): treat as 0. */
    printf("bytes=0 lines=0\n");
    rc = 0;
    goto cleanup;
  }

  /*
   * For lab purposes we cast to size_t.
   * On typical 64-bit Linux, size_t is 64-bit.
   */
  n = (size_t)st.st_size;

  addr = mmap(NULL, n, PROT_READ, MAP_PRIVATE, fd, 0);
  if (addr == MAP_FAILED) {
    perror("mmap");
    goto cleanup;
  }

  const unsigned char *p = (const unsigned char *)addr;
  for (size_t i = 0; i < n; i++) {
    if (p[i] == '\n') lines++;
  }

  printf("bytes=%zu lines=%zu\n", n, lines);
  rc = 0;

cleanup:
  if (addr != MAP_FAILED && n > 0) munmap(addr, n);
  if (fd >= 0) close(fd);
  return rc;
}
