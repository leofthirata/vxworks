#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <sysLib.h>
#include <taskLib.h>
#include <msgQLib.h>

#include <wdLib.h>

#include <subsys/gpio/vxbGpioLib.h>
#include <subsys/gpio/vxbGpioCommon.h>

#define DELAY_MS(ms)	(sysClkRateGet() / (1000 / ms))

bool control = false;
MSG_Q_ID q_id;
int count = 0;

void producer_task()
{
	printf("Producer task\n");

	while (count < 10) {
		const char *msg = "Hello from producer";
		// sprintf(buf, "Hello from producer\n");
		int ret = msgQSend(q_id, (char *)msg, strlen(msg) + 1, WAIT_FOREVER, 0);
		if (ret < 0) {
			perror("Failed to send Q msg\n");
		}
		taskDelay(DELAY_MS(1000));
	}
	taskDelete(NULL);
}

void consumer_task()
{
	printf("Consumer task\n");

	while (count < 10) {
		char buf[100];
		long int rx_bytes = msgQReceive(q_id, buf, sizeof(buf), WAIT_FOREVER);
		if (rx_bytes > 0) {
			printf("Received %d bytes\n", rx_bytes);
			char msg[rx_bytes];
			snprintf(msg, rx_bytes, buf);
			printf("%s\n", msg);
		}
		count++;
	}
	taskDelete(NULL);
}

int main()
{
	q_id = msgQCreate(10, 50, 0);
	TASK_ID producer_id = taskSpawn("producer_task", 10, 0, 200, (FUNCPTR)producer_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	TASK_ID consumer_id = taskSpawn("consumer_task", 9, 0, 200, (FUNCPTR)consumer_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	while (taskIdVerify(producer_id) == OK || taskIdVerify(consumer_id) == OK) {
			// taskDelay(DELAY_MS(500));
	}

	return 0;
}