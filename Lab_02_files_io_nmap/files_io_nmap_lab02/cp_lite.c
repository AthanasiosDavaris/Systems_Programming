/*
 * cp_lite.c - "cp-lite" for Lab (Systems Programming)
 *
 * Implements file copy using low-level syscalls:
 *   open/read/write/close
 *
 * Features:
 *  - Optional buffer size via -b BUFSZ
 *  - [-v] Verbose report printing statistics to stderr
 *  -[-p] Preserve permissions from source to destination
 *  - Robust read loop (handles EINTR)
 *  - full_write() to handle partial writes + EINTR
 *  - Single cleanup path (goto cleanup)
 */

#include "common.h"

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>

#define DEFAULT_BUFSZ (64 * 1024)

static size_t parse_bufsz(const char *s)
{
  char *end = NULL;
  long v = strtol(s, &end, 10);
  if (!s || !s[0])
    return 0;
  if (end && *end)
    return 0;
  if (v <= 0)
    return 0;
  return (size_t)v;
}

/*
 * Write exactly n bytes.
 *
 * Why: write(fd, buf, n) may write fewer bytes than requested.
 * That is not always an error (partial write). We loop until done.
 */
static int full_write(int fd, const void *buf, size_t n)
{
  const unsigned char *p = (const unsigned char *)buf;
  size_t left = n;

  while (left > 0)
  {
    ssize_t w = write(fd, p, left);
    if (w < 0)
    {
      if (errno == EINTR)
        continue;
      return -1;
    }
    if (w == 0)
      return -1;

    p += (size_t)w;
    left -= (size_t)w;
  }
  return 0;
}

int main(int argc, char **argv)
{
  size_t bufsz = DEFAULT_BUFSZ;
  int opt;
  int verbose = 0;  /* 0: false, 1: true */
  int preserve = 0; /* 0: false, 1: true */

  while ((opt = getopt(argc, argv, "b:vp")) != -1)
  {
    if (opt == 'b')
    {
      size_t v = parse_bufsz(optarg);
      if (v == 0)
        usage_cplite(argv[0]);
      bufsz = v;
    }
    else if (opt == 'v')
    {
      verbose = 1; /* Recognizes the -v flag */
    }
    else if (opt == 'p')
    {
      preserve = 1; /* Recognizes the -p flag */
    }
    else
    {
      usage_cplite(argv[0]);
    }
  }

  if (optind + 2 != argc)
    usage_cplite(argv[0]);

  const char *src_path = argv[optind];
  const char *dst_path = argv[optind + 1];

  int src = -1;
  int dst = -1;
  unsigned char *buf = NULL;
  int rc = 1;

  /* Variables for (A) */
  size_t total_bytes = 0;
  struct timespec start_time, end_time;

  src = open(src_path, O_RDONLY);
  if (src < 0)
  {
    perror("open src");
    goto cleanup;
  }

  /*
   * 0644 is a reasonable default for lab tools.
   * The user's umask may further restrict permissions.
   */
  dst = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (dst < 0)
  {
    perror("open dst");
    goto cleanup;
  }

  buf = (unsigned char *)malloc(bufsz);
  if (!buf)
  {
    perror("malloc");
    goto cleanup;
  }

  /* Starts the countdown. */
  if (verbose)
  {
    clock_gettime(CLOCK_MONOTONIC, &start_time);
  }

  /* Basic I/O Loop */
  for (;;)
  {
    ssize_t r = read(src, buf, bufsz);
    if (r < 0)
    {
      if (errno == EINTR)
        continue;
      perror("read");
      goto cleanup;
    }
    if (r == 0)
      break; /* EOF */

    if (full_write(dst, buf, (size_t)r) < 0)
    {
      perror("write");
      goto cleanup;
    }

    total_bytes += (size_t)r;
  }

  /* (B) */
  if (preserve)
  {
    struct stat st;

    /* Reads the metadata of the source file */
    if (fstat(src, &st) < 0)
    {
      perror("fstat");
      goto cleanup;
    }

    /* Applies permissions to the destination file */
    if (fchmod(dst, st.st_mode & 0777) < 0)
    {
      perror("fchmod");
      goto cleanup;
    }
  }

  /* Calculating and displaying statistics */
  if (verbose)
  {
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    double elapsed = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;

    if (elapsed <= 0.0)
      elapsed = 0.000001;

    double throughput_mib = (total_bytes / 1048576.0) / elapsed;

    fprintf(stderr, "bytes copied: %zu\n", total_bytes);
    fprintf(stderr, "bufsz       : %zu\n", bufsz);
    fprintf(stderr, "elapsed time: %.6f s\n", elapsed);
    fprintf(stderr, "throughput  : %.2f MiB/s\n", throughput_mib);
  }

  rc = 0;

cleanup:
  if (buf)
    free(buf);
  if (src >= 0)
    close(src);
  if (dst >= 0)
    close(dst);
  return rc;
}
