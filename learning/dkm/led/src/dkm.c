#include <stdio.h>
#include <subsys/gpio/vxbGpioLib.h>
#include <taskLib.h>
#include <sysLib.h>

#define GPIO_MODE_OUTPUT	1
#define GPIO_MODE_INPUT		0

#define USER_LED_PIN		21

#define DELAY_MS(ms)		(sysClkRateGet() / (1000 / ms))

int gpio_config(uint32_t gpio, uint8_t mode)
{
	if (mode != GPIO_MODE_INPUT && mode != GPIO_MODE_OUTPUT) {
		perror("Failed to config gpio: Mode val out of range!\n");
		return -1;
	}

	int dir = vxbGpioGetDir(gpio);
	if (dir != mode)
		vxbGpioSetDir(gpio, mode);
	
	return 0;
}

void gpio_toggle(uint32_t gpio)
{
	int val = vxbGpioGetValue(gpio);
	vxbGpioSetValue(gpio, val^1);
}

void led_task()
{
	printf("Led task started!\n");

	gpio_config(USER_LED_PIN, GPIO_MODE_OUTPUT);

	int count = 0;
	while (count < 10) {
		gpio_toggle(USER_LED_PIN);
		count += 1;

		taskDelay(DELAY_MS(1000));
	}

	taskDelete(NULL);
}

int main()
{
	printf("DKM app 1\n");

	TASK_ID led_task_id = taskSpawn("led_task", 10, 0, 200, (FUNCPTR)led_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if (led_task_id == NULL) {
		perror("Failed to create task!\n");
		return -1;
	}

	while (taskIdVerify(led_task_id) == OK) {}

	printf("Application done\n");
	return 0;
}