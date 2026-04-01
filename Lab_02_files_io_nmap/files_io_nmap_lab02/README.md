# Lab 2 — Low-level I/O utilities + mmap 

Περιεχόμενα:
- `cp-lite`: αντιγραφή αρχείων με `open/read/write` (robust loops, partial writes)
- `wc-lite`: μετρητής `bytes/lines` με **μόνο** `open/read/close`
- `mmap-view`: `mmap()` αρχείο και scan χωρίς `read()` loop

## Build

```bash
make
```

> Σημείωση: Το Makefile χρησιμοποιεί `.RECIPEPREFIX` για να μη χρειάζονται TABs (λειτουργεί σε GNU make).

Εναλλακτικά (χωρίς make):

```bash
gcc -O2 -Wall -Wextra -Wpedantic -o cp-lite cp_lite.c
gcc -O2 -Wall -Wextra -Wpedantic -o wc-lite wc_lite.c
gcc -O2 -Wall -Wextra -Wpedantic -o mmap-view mmap_view.c
```

## Quick demo file (μερικά MiB)

```bash
dd if=/dev/urandom of=big.bin bs=1M count=32
```

## Run

### cp-lite

```bash
./cp-lite big.bin out.bin
cmp -s big.bin out.bin && echo OK

./cp-lite -b 4096 big.bin out4k.bin
cmp -s big.bin out4k.bin && echo OK
```

### wc-lite

```bash
./wc-lite big.bin
```

### mmap-view

```bash
./mmap-view big.bin
```

## strace mini-comparison 

> Στόχος: patterns και syscall counts.

```bash
strace -c ./cp-lite -b 4096 big.bin out.bin
strace -c ./cp-lite -b 65536 big.bin out.bin

strace -c ./wc-lite big.bin
strace -c ./mmap-view big.bin

# εστίαση σε βασικά syscalls
strace -e trace=read,write,openat,close ./wc-lite big.bin
strace -e trace=mmap,munmap,openat,close ./mmap-view big.bin
```

Αναμενόμενο:
- `cp-lite` και `wc-lite` εμφανίζουν πολλά `read` (και `write` στο `cp-lite`).
- `mmap-view` εμφανίζει `mmap/munmap` (και όχι `read` σε loop).

## Exit codes
- `0`: success
- `1`: runtime error
- `2`: usage error
