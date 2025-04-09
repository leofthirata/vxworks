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

void timer_handler()
{
	control = true;
}

int timer_config()
{
	timer = wdCreate();
	if (timer == NULL) {
		perror("Failed to create timer\n");
		return -1;
	}

	wdStart(timer, DELAY_MS(1000), (FUNCPTR)timer_handler, 0);

	return 0;
}

void timer_task()
{
	printf("Timer task started!\n");

	timer_config();

	int count = 0;
	while (count < 20) {
		if (control) {
			count++;
			printf("Timer triggered\n");
			control = false;
			wdStart(timer, DELAY_MS(1000), (FUNCPTR)timer_handler, 0);
		}
		taskDelay(DELAY_MS(1));
	}

	taskDelete(NULL);
}

int main()
{
	printf("DKM WD Timer\n");

	TASK_ID timer_task_id = taskSpawn("timer_task", 10, 0, 200, (FUNCPTR)timer_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	while (taskIdVerify(timer_task_id) == OK) {
		taskDelay(DELAY_MS(500));
	}

	return 0;
}