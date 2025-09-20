#include <iostream>

#include <taskLib.h>
#include <sysLib.h>
#include <semLib.h>

#define DELAY_MS(ms)	(sysClkRateGet() / (1000 / ms))

SEM_ID sem;

void producer_task()
{
	semTake(sem, WAIT_FOREVER);
	std::cout << "producer doing work" << std::endl;
	semGive(sem);

	taskDelete(nullptr);
}

void consumer_task()
{
	taskDelay(DELAY_MS(1000));
	semTake(sem, WAIT_FOREVER);
	std::cout << "consumer doing work" << std::endl;
	semGive(sem);

	taskDelete(nullptr);
}

int main()
{
	sem = semBCreate(0, SEM_EMPTY);
	semGive(sem);

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