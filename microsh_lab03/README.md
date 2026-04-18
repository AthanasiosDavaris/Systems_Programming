# Lab 3 — microsh+ (Systems Programming)

- Ονοματεπώνυμο: Αθανάσιος Ντάβαρης
- Α.Μ.: 2022202200150

This folder contains the C code for the extended micro-shell (microsh+):
- `microsh.c`: interactive micro-shell supporting `<`, `>`, `>>`, and `|`
- `common.h`: small helpers for consistent error/syntax handling

## Build

```bash
make
```

or

```bash
gcc -O2 -g -Wall -Wextra -Wpedantic -o microsh microsh.c
```

## Run

```bash
./microsh
```

## Quick self-check

Inside `microsh>` try:

- `echo hello`
- `echo first > out.txt`
- `echo second >> out.txt`
- `cat < out.txt`
- `ls -1 | wc -l`
- `ls -1 | wc -l >> out.txt`
- `invalid_command`

> Note: This lab version requires **spaces** around `| < > >>`.
