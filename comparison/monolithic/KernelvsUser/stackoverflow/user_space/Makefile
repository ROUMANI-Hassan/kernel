CC ?= cc
CFLAGS ?= -std=gnu11 -Wall -Wextra -Werror -O0

all: user_stack_overflow

user_stack_overflow: stack_overflow.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f user_stack_overflow
