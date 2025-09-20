#include <iostream>

#include <subsys/gpio/vxbGpioLib.h>
#include <subsys/gpio/vxbGpioCommon.h>
#include <sysLib.h>
#include <taskLib.h>

#define GPIO_MODE_OUTPUT	1
#define GPIO_MODE_INPUT		0
#define GPIO_LEVEL_HIGH		1
#define GPIO_LEVEL_LOW		0

#define BUTTON_GPIO			20

#define DELAY_MS(ms)	(sysClkRateGet() / (1000 / ms))

bool button_pressed = false;

void button_callback()
{
	button_pressed = true;
}

void button_task()
{
	// std::cout << "Button task init" << std::endl;

	while (button_pressed) {
		if (button_pressed) {
			button_pressed = false;
			std::cout << "Button pressed" << std::endl;
		}
	}

	taskDelete(nullptr);
}

void timer_task()
{
	// std::cout << "Timer task init" << std::endl;
	
	int count = 0;
	while (count < 10) {
		count += 1;
		taskDelay(DELAY_MS(1000));
	}
	button_pressed = true;

	taskDelete(nullptr);
}

int main()
{
	TASK_ID button_task_id = taskSpawn(const_cast<char *>("button_task"), 10, 0, 200, (FUNCPTR)button_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	TASK_ID timer = taskSpawn(const_cast<char *>("timer"), 10, 0, 200, (FUNCPTR)timer_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	int gpio = BUTTON_GPIO;

	// int ret = vxbGpioLibInit();
	// if (ret != OK) {
	// 	std::cerr << "Failed to set gpio init" << std::endl;
	// 	return -1;
	// }

	int ret = vxbGpioSetDir(gpio, GPIO_MODE_INPUT);
	if (ret != OK) {
		std::cerr << "Failed to set gpio dir" << std::endl;
		return -1;
	}

	ret = vxbGpioIntConfig(gpio, INTR_TRIGGER_EDGE, INTR_POLARITY_BOTH);
	if (ret != OK) {
		std::cerr << "Failed to set gpio dir" << std::endl;
		return -1;
	}

	// ret = vxbGpioPinRequest(gpio);
	// if (ret != OK) {
	// 	std::cerr << "Failed to set request gpio" << std::endl;
	// 	return -1;
	// }

	ret = vxbGpioIntConnect(gpio, (VOIDFUNCPTR)button_callback, nullptr);
	if (ret != OK) {
		std::cerr << "Failed to set connect int" << std::endl;
		return -1;
	}

	ret = vxbGpioIntEnable(gpio, (VOIDFUNCPTR)button_callback, nullptr);
	if (ret != OK) {
		std::cerr << "Failed to set enable int" << std::endl;
		return -1;
	}

	while (taskIdVerify(button_task_id) == OK || taskIdVerify(timer) == OK) {}

	std::cout << "app done" << std::endl;

	return 0;
}