#include <stdio.h>
#include <stdbool.h>

#include <subsys/gpio/vxbGpioLib.h>

#include <timerLib.h>
#include <taskLib.h>
#include <sysLib.h>
#include <tickLib.h>

// https://comp.os.vxworks.narkive.com/Pdbc5O8e/system-time-in-milliseconds

#define GPIO_MODE_OUTPUT	1
#define GPIO_MODE_INPUT		0

#define USER_LED_PIN		21
#define USER_BUTTON_PIN		20

#define millis()		((tickGet() / sysClkRateGet()) * 1000)

timer_t timer_id;
SEM_ID my_sem;
bool button_pressed = false;
uint32_t timeout = 1000;

int gpio_config(uint32_t gpio, uint8_t mode)
{
	if (mode != GPIO_MODE_INPUT && mode != GPIO_MODE_OUTPUT) {
		perror("Failed to config gpio: Mode val out of range!\n");
		return -1;
	}

	int dir = vxbGpioGetDir(gpio);
	printf("gpio: %d, curr mode: %d\n", gpio, dir);
	if (dir != mode)
		vxbGpioSetDir(gpio, mode);
	
	dir = vxbGpioGetDir(gpio);
	printf("gpio: %d, curr mode: %d\n", gpio, dir);

	return 0;
}

void gpio_toggle(uint32_t gpio)
{
	int val = vxbGpioGetValue(gpio);
	vxbGpioSetValue(gpio, val^1);
}

void button_handler(uint32_t val)
{
	static uint32_t timestamp = 0;
	if ((millis() - timestamp) > timeout) {
		button_pressed = true;
		timestamp = millis();
	}
}

void button_task()
{
	printf("Button task started!\n");

	gpio_config(USER_LED_PIN, GPIO_MODE_OUTPUT);
	gpio_config(USER_BUTTON_PIN, GPIO_MODE_INPUT);
	uint32_t val = 0;
	uint32_t prev_val = 0;
	uint8_t count = 0;

	while (count < 10) {
		val = vxbGpioGetValue(USER_BUTTON_PIN);
		if (prev_val != val)
			// button_pressed = true;
			button_handler(val);
		
		prev_val = val;

		if (button_pressed) {
			count += 1;
			gpio_toggle(USER_LED_PIN);
			button_pressed = false;
		}
	}

	taskDelete(NULL);
}

int main()
{
	printf("DKM app 1\n");

	TASK_ID task_id = taskSpawn("button_task", 10, 0, 200, (FUNCPTR)button_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if (task_id == NULL) {
		perror("Failed to create task!\n");
		return -1;
	}

	while (taskIdVerify(task_id) == OK) {}

	printf("Application done\n");
	return 0;
}