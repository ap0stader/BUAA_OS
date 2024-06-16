#include <lib.h>

void sigint_handler(int sig) {
	debugf("capture SIGINT!!!\n");
	exit();
}

int main() {
	debugf("Enter sigtst.\n");
	struct sigaction sa;
	sa.sa_handler = sigint_handler;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	debugf("Sending SIGINT to myself\n");
	kill(0, SIGINT);
	debugf("ERROR: SIGINT not handled correctly\n");
	while (1) {
		;
	}
	return 0;
}
