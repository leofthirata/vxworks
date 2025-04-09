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

#define LED_PIN			21
#define LED_PIN2			16
#define BUTTON_PIN		20
#define GPIO_MODE_OUTPUT 		1
#define GPIO_MODE_INPUT 		0

bool control = false;
WDOG_ID timer;
MSG_Q_ID q_id;
int count = 0;

int led_config(int gpio);
int gpio_config(int gpio, int mode);
int gpio_toggle(int gpio);

void timer_handler()
{
	control = true;
}

int timer_config()
{
	timer = wdCreate();
	if (timer == NULL) {
		perror("Failed to create timer!\n");
		return -1;
	}

	wdStart(timer, DELAY_MS(500), (FUNCPTR)timer_handler, 0);

	return 0;
}

void timer_task()
{
	printf("Timer task started!\n");

	int ret = timer_config();
	if (ret < 0) {
		perror("timer_config fail\n");
	}

	int gpio = LED_PIN;
	ret = led_config(gpio);
	if (ret < 0) {
		perror("led_config fail\n");
	}

	while (count < 6) {
		if (control) {
			control = false;
			gpio_toggle(gpio);
			wdStart(timer, DELAY_MS(1000), (FUNCPTR)timer_handler, 0);
		}
		taskDelay(DELAY_MS(1));
	}

	taskDelete(NULL);
}

void led_task()
{
	printf("Led task started!\n");

	int gpio = LED_PIN2;
	int ret = led_config(gpio);
	if (ret < 0) {
		perror("led_config fail\n");
	}

	while (count < 6) {
		char buf[100];
		long int rx_bytes = msgQReceive(q_id, buf, sizeof(buf), WAIT_FOREVER);
		if (rx_bytes > 0) {
			gpio_toggle(gpio);
			printf("Led 2 triggered\n");
		}
	}

	taskDelete(NULL);
}

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

void button_task()
{
	printf("Button task started!\n");

	int gpio = BUTTON_PIN;
	int ret = gpio_config(gpio, GPIO_MODE_INPUT);
	if (ret < 0) {
		perror("Failed to config button\n");
	}

	int prev = 0;
	int curr = 0;
	while (count < 6) {
		curr = gpio_get_level(gpio);
		if (prev != curr && curr == 0) { // neg edge
			printf("Button pressed!\n");
			const char *msg = "TEST MSG";
			ret = msgQSend(q_id, (char *)msg, strlen(msg), WAIT_FOREVER, 0);
			if (ret < 0) {
				perror("Failed to send Q msg\n");
			}
			count++;
		}
		prev = curr;
	}

	taskDelete(NULL);
}

// void producer_task()
// {

// }

// void consumer_task()
// {

// }

int main()
{
	q_id = msgQCreate(10, 50, 0);
	TASK_ID timer_id = taskSpawn("timer_task", 10, 0, 200, (FUNCPTR)timer_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	TASK_ID led_id = taskSpawn("led_task", 9, 0, 200, (FUNCPTR)led_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	TASK_ID button_id = taskSpawn("button_task", 11, 0, 200, (FUNCPTR)button_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	while (taskIdVerify(timer_id) == OK || taskIdVerify(button_id) == OK || taskIdVerify(led_id) == OK) {
			// taskDelay(DELAY_MS(500));
	}

	return 0;
}