#include <stdio.h>
#include <stdbool.h>

#include <subsys/gpio/vxbGpioLib.h>
#include <subsys/int/vxbIntLib.h>

#include <timerLib.h>
#include <taskLib.h>
#include <sysLib.h>
#include <tickLib.h>

#include <intLib.h>

#include <iv.h>

#define GPIO_MODE_OUTPUT	1
#define GPIO_MODE_INPUT		0

#define USER_LED_PIN		21
#define USER_BUTTON_PIN		20
// #define USER_BUTTON_PIN		1

#define millis()		((tickGet() / sysClkRateGet()) * 1000)
#define DELAY_MS(ms)	(sysClkRateGet() / (1000 / ms))

bool button_pressed = false;
uint32_t timeout = 1000;

// void button_handler(uint32_t val);
// void button_handler(void *arg);
void button_handler(int arg);

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

int gpio_set_isr(uint32_t gpio)
{
	// int ret = vxbGpioFree(gpio);
	// if (ret < 0) {
	// 	perror("Failed to vxbGpioFree\n");
	// 	return -1;
	// }

	// ret = vxbGpioPinRelease(gpio);
	// if (ret < 0) {
	// 	perror("Failed to vxbGpioPinRelease\n");
	// 	return -1;
	// }

	// ret = vxbGpioAlloc(gpio);
	// if (ret < 0) {
	// 	perror("Failed to vxbGpioAlloc\n");
	// 	return -1;
	// }

	// vxbIntCommon.h
	int ret = vxbGpioIntConfig(gpio, INTR_TRIGGER_EDGE, INTR_POLARITY_LOW);
	if (ret < 0) {
		perror("Failed to vxbGpioIntConfig\n");
		return -1;
	}

	ret = vxbGpioIntConnect(gpio, (VOIDFUNCPTR)button_handler, NULL);
	if (ret < 0) {
		perror("Failed to vxbGpioIntConnect\n");
		return -1;
	}

	ret = vxbGpioIntEnable(gpio, (VOIDFUNCPTR)button_handler, NULL);
	if (ret < 0) {
		perror("Failed to vxbGpioIntEnable\n");
		return -1;
	}
	
	return 0;
}

void gpio_toggle(uint32_t gpio)
{
	int val = vxbGpioGetValue(gpio);
	vxbGpioSetValue(gpio, val^1);
}

void button_handler(int arg)
{
	button_pressed = true;
}

int main()
{
	printf("DKM app 3\n");

	// vxbGpioLibInit();
	// gpio_config(USER_BUTTON_PIN, GPIO_MODE_INPUT);
	// gpio_set_isr(USER_BUTTON_PIN);

	uint8_t arg = 0x10;
	int ret = intConnect((VOIDFUNCPTR*)INUM_TO_IVEC(50), (VOIDFUNCPTR)button_handler, arg);
	if (ret < 0) {
		perror("Failed to intConnect\n");
		return -1;
	}
	ret = intEnable(50);
	// sysIntEnable(0xFF);
	if (ret < 0) {
		perror("Failed to Enable\n");
		return -1;
	}

	int count = 0;
	while (count < 20) {
		if (button_pressed) {
			printf("loop count %d\n", count);
			button_pressed = false;
		}
		count += 1;
		taskDelay(DELAY_MS(500));
	}

	printf("Application done\n");
	return 0;
}