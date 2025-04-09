#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <sysLib.h>
#include <taskLib.h>
#include <semLib.h>

#include <subsys/gpio/vxbGpioLib.h>
#include <subsys/gpio/vxbGpioCommon.h>

#define DELAY_MS(ms)	(sysClkRateGet() / (1000 / ms))

#define LED_PIN			21
#define GPIO_MODE_OUTPUT 		1
#define GPIO_MODE_INPUT 		0

int count = 0;
SEM_ID sem;

int led_config(int gpio);
int gpio_config(int gpio, int mode);
int gpio_toggle(int gpio);

int led_config(int gpio)
{
	int ret = gpio_config(gpio, GPIO_MODE_OUTPUT);
	if (ret < 0) {
		perror("led config fail\n");
	}

	return ret;
}

int gpio_config(int gpio, int mode)
{
	int ret = vxbGpioSetDir(gpio, mode);
	if (ret < 0) {
		perror("vxbGpioSetDir fail\n");
	}

	return ret;
}
int gpio_get_level(int gpio)
{
	return vxbGpioGetValue(gpio);
}

int gpio_set_level(int gpio, int level)
{
	int val = vxbGpioGetValue(gpio);
	if (val == level)
		return 0;
	
	return vxbGpioSetValue(gpio, level);
}

int gpio_toggle(int gpio)
{
	int dir = vxbGpioGetDir(gpio);
	if (dir == 0) {
		perror("toggle fail, gpio mode input\n");
		return -1;
	}

	int level = vxbGpioGetValue(gpio);
	int ret = vxbGpioSetValue(gpio, level^1);
	return ret;
}

void led_task1()
{
	printf("LED task1\n");

	while (count < 20) {
		semTake(sem, WAIT_FOREVER);
		gpio_toggle(LED_PIN);
		taskDelay(DELAY_MS(500));
		semGive(sem);
	}
	taskDelete(NULL);
}

void led_task2()
{
	printf("LED task2\n");

	while (count < 20) {
		semTake(sem, WAIT_FOREVER);
		gpio_toggle(LED_PIN);
		taskDelay(DELAY_MS(500));
		semGive(sem);
	}
	taskDelete(NULL);
}

void timeout_task()
{
	printf("Timeout task\n");

	while (count < 20) {
		printf("count: %d\n", count);
		count++;
		taskDelay(DELAY_MS(1000));
	}
	taskDelete(NULL);
}

int main()
{
	led_config(LED_PIN);

	sem = semBCreate(0, SEM_EMPTY);
	semGive(sem);
	TASK_ID led_id1 = taskSpawn("led_task1", 10, 0, 200, (FUNCPTR)led_task1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	TASK_ID led_id2 = taskSpawn("led_task2", 10, 0, 200, (FUNCPTR)led_task2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	TASK_ID timeout_id = taskSpawn("timeout_task", 11, 0, 200, (FUNCPTR)timeout_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	while (taskIdVerify(led_id1) == OK || taskIdVerify(led_id2) == OK || taskIdVerify(timeout_id) == OK) {
			// taskDelay(DELAY_MS(500));
	}

	semDelete(sem);

	return 0;
}