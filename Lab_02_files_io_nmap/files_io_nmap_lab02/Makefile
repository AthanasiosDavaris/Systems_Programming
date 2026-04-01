# Makefile for Lab 4
#
# Note: We set RECIPEPREFIX so we do NOT need literal TABs.
# (Some editors/users prefer spaces-only files.)

.RECIPEPREFIX := >

CC ?= gcc
CFLAGS ?= -O2 -Wall -Wextra -Wpedantic

BINS = cp-lite wc-lite mmap-view

all: $(BINS)

cp-lite: cp_lite.c common.h
>$(CC) $(CFLAGS) -o $@ cp_lite.c

wc-lite: wc_lite.c common.h
>$(CC) $(CFLAGS) -o $@ wc_lite.c

mmap-view: mmap_view.c common.h
>$(CC) $(CFLAGS) -o $@ mmap_view.c

clean:
>rm -f $(BINS) *.o *.bin

.PHONY: all clean
