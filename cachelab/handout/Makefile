#
# Student makefile for Cache Lab
# Note: requires a 64-bit x86-64 system
#

# NOTE: you are PROHIBITED from changing the followingtwo lines

CC := gcc
CFLAGS := -g -Wall -Wextra -Werror -std=c99 -m64 -O0
LDFLAGS := -lm

.PHONY: all clean handin

all: csim test-trans tracegen

csim : csim.o cachelab.o

test-trans: test-trans.o trans.o cachelab.o

tracegen: tracegen.o trans.o cachelab.o

clean:
	rm -rvf *.o *.tar csim test-trans tracegen trace.all trace.f* .csim_results .marker

handin: handin.tar

handin.tar : csim.c trans.c
	tar -cvf $@ $^
