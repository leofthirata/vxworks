#include <iostream>

#include <taskLib.h>
#include <sysLib.h>
#include <wdLib.h>

#define DELAY_MS(ms)	(sysClkRateGet() / (1000 / ms))

bool timeout = false;

void timer_callback()
{
	timeout = true;
}

void producer_task()
{
	std::cout << "timer task" << std::endl;

	WDOG_ID timer = wdCreate();

	int ret = wdStart(timer, 1, (FUNCPTR)timer_callback, 0);
	if (ret != OK) {
		std::cerr << "Failed to start wtdg" << std::endl;
		taskDelete(nullptr);
		return;
	}

	int count = 0;
	while (count < 10) {
		if (timeout) {
			ret = wdStart(timer, 1, (FUNCPTR)timer_callback, 0);
			if (ret != OK) {
				std::cerr << "Failed to start wtdg" << std::endl;
				break;
			}

			count += 1;
		}
	}

	wdDelete(timer);

	taskDelete(nullptr);
}


void consumer_task()
{
	std::cout << "consumer task" << std::endl;

	int count = 0;
	while (count < 10) {
		if (timeout) {
			std::cout << "consuming" << std::endl;
			count += 1;
		}
	}

	taskDelete(nullptr);
}

int main()
{
	TASK_ID producer_id = taskSpawn(const_cast<char *>("producer task"), 10, 0, 200, (FUNCPTR)producer_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if (producer_id == nullptr) {
		std::cerr << "Failed to create producer task" << std::endl;
	}

	TASK_ID consumer_id = taskSpawn(const_cast<char *>("consumer task"), 10, 0, 200, (FUNCPTR)consumer_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if (consumer_id == nullptr) {
		std::cerr << "Failed to create consumer task" << std::endl;
	}

	while (taskIdVerify(producer_id) == OK || taskIdVerify(consumer_id) == OK) {}

	std::cout << "App done" << std::endl;

	return 0;
}