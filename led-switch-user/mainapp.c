#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "led_control.h"
#include "switch_control.h"

void catcher(int signum) {
	printf("terminated...\n");
}


int main()
{
	struct sigaction act;

	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = catcher;
	sigaction(SIGINT, &act, NULL);

	printf("Start....\n");
	led_init();
	switch_init();
	pause();
	led_uninit();
	switch_uninit();
	printf("End....\n");

	return 0;
}
