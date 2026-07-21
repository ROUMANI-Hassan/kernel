#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * A handler can interrupt the main program at almost any instruction.  Use
 * sig_atomic_t because reading and writing it cannot be torn by that
 * interruption.  volatile prevents the compiler from assuming that these
 * values cannot change unexpectedly.
 */
static volatile sig_atomic_t usr1_pending;
static volatile sig_atomic_t stop_requested;

static void signal_handler(int signo)
{
	/*
	 * Keep the handler minimal.  Functions such as printf() and malloc() are
	 * not async-signal-safe, so the handler records the event and lets the
	 * normal control flow do the real work.
	 */
	if (signo == SIGUSR1)
		usr1_pending = 1;
	else if (signo == SIGINT || signo == SIGTERM)
		stop_requested = 1;
}

static int install_handler(int signo)
{
	struct sigaction action;

	/* Start with a fully initialized structure rather than indeterminate data. */
	memset(&action, 0, sizeof(action));
	action.sa_handler = signal_handler;

	/* Do not block any additional signals while this handler is executing. */
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	/* sigaction() asks the kernel to remember this signal disposition. */
	return sigaction(signo, &action, NULL);
}

int main(void)
{
	/* SIGUSR1 is our event; SIGINT and SIGTERM request orderly shutdown. */
	if (install_handler(SIGUSR1) == -1 ||
	    install_handler(SIGINT) == -1 ||
	    install_handler(SIGTERM) == -1) {
		perror("sigaction");
		return EXIT_FAILURE;
	}

	printf("signal_demo PID: %ld\n", (long)getpid());
	puts("Send SIGUSR1 to produce an event.");
	puts("Send SIGINT or SIGTERM to stop cleanly.");

	while (!stop_requested) {
		/*
		 * Sleep in the kernel instead of polling and consuming CPU.  A
		 * delivered signal interrupts pause(), runs signal_handler(), and
		 * then returns control here.
		 */
		while (!usr1_pending && !stop_requested)
			pause();

		/* Library I/O is safe here because this is normal program context. */
		if (usr1_pending) {
			usr1_pending = 0;
			puts("main loop: handled the SIGUSR1 event safely");
		}
	}

	puts("main loop: clean shutdown requested");
	return EXIT_SUCCESS;
}
