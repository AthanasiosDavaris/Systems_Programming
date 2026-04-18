/*
 * proc_inspector.c
 * ---------------
 * Starter για /proc x-ray.
 * Εκτυπώνει PID και παγώνει για 60s, ώστε να επιθεωρήσετε από άλλο terminal:
 *   - /proc/<pid>/status
 *   - /proc/<pid>/fd/
 *   - /proc/<pid>/maps
 */

#include <stdio.h>
#include <sys/types.h> /* pid_t */
#include <unistd.h>

int main(void)
{
    pid_t pid = getpid();

    printf("PID: %ld\n", (long)pid);
    printf("Τώρα σε άλλο terminal:\n");
    printf("  cat /proc/%ld/status | head\n", (long)pid);
    printf("  ls -l /proc/%ld/fd\n", (long)pid);
    printf("  head -n 15 /proc/%ld/maps\n", (long)pid);
    fflush(stdout);

    sleep(60);
    return 0;
}
