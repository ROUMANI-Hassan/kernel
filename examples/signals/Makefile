CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -Wpedantic

.PHONY: all clean

all: signal_demo

signal_demo: signal_demo.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) signal_demo
