# Sysprog — Εργαστήριο 1 (Linux env, debugging, /proc)

Αυτό το πακέτο περιέχει **όλα τα αρχεία** για τα demos/παραδείγματα που εμφανίζονται στις διαφάνειες του πρώτου εργαστηρίου.

## Δομή

```
sysprog-lab2/
  Makefile
  include/
    common.h
  src/
    hello.c
    buggy.c
    proc_inspector.c
    hello_errno.c
    cleanup_demo.c
  bin/      (δημιουργείται με make)
  build/    (δημιουργείται με make)
```

## Γρήγορο build/run

```bash
make
make run-hello
make run-buggy
make run-proc
make run-errno
make run-cleanup
```

## Demo 1: gdb (buggy)

```bash
gdb --args ./bin/buggy
(gdb) run
(gdb) bt
(gdb) frame 1
(gdb) info args
(gdb) list
```

## Demo 2: strace (syscalls)

```bash
strace ./bin/proc_inspector
strace -o trace.txt ./bin/proc_inspector
strace -f -o trace.txt ./bin/proc_inspector
strace -e trace=openat,read,write,close ./bin/cleanup_demo /etc/hostname
```

## Demo 3: ltrace (lib calls)

```bash
ltrace ./bin/buggy
ltrace -o ltrace.txt ./bin/buggy
ltrace -e malloc+free ./bin/cleanup_demo /etc/hostname
```

## /proc (proc_inspector)

1. Τρέξτε:
   ```bash
   make run-proc
   ```
2. Σε άλλο terminal, με το PID που τυπώθηκε:
   ```bash
   cat /proc/<pid>/status | head
   ls -l /proc/<pid>/fd
   head -n 15 /proc/<pid>/maps
   grep -E "\[heap\]|\[stack\]" /proc/<pid>/maps
   ```

## Σημείωση για flags

Για debugging κρατάμε:
- `-O0 -g` (όχι optimization)
- `-Wall -Wextra` (προειδοποιήσεις)
- `-std=c11`
