#include <stdio.h>
#include <stdbool.h>

#include <sysLib.h>
#include <taskLib.h>

#include <wdLib.h>

#define DELAY_MS(ms)	(sysClkRateGet() / (1000 / ms))
#define TCP_PORT 		8008

uint32_t timeout = 1000;
bool control = false;
WDOG_ID timer;

void timer_handler(int param)
{
	// printf("Timer triggered: param %d\n", param);
	control = true;
	wdStart(timer, DELAY_MS(1000), (FUNCPTR)timer_handler, param);
}

int timer_config()
{
	timer = wdCreate();
	if (timer == NULL) {
		perror("Failed to create timer\n");
		return -1;
	}

	wdStart(timer, DELAY_MS(1000), (FUNCPTR)timer_handler, 0x100);

	return 0;
}

int main()
{
	printf("DKM WD Timer\n");

	timer_config();

	int count = 0;
	while (count < 20) {
		count++;
		printf("main while...\n");
		if (control) {
			printf("Timer triggered\n");
			control = false;
		}
		taskDelay(DELAY_MS(500));
	}

	return 0;
}