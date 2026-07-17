CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2

all: kernel_message_client

kernel_message_client: kernel_message_client.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f kernel_message_client

.PHONY: all clean
