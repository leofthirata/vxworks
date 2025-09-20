#include <iostream>
#include <string>

#include <logLib.h>
#include <taskLib.h>
#include <sysLib.h>

#define DELAY_MS(ms)	(sysClkRateGet() / (1000 / ms))

void print(const char *msg)
{
	logMsg(const_cast<char *>(msg), 0, 0, 0, 0, 0, 0);
}

void printing_task()
{
	print("printing task init\r\n");

	int count = 0;
	while (count < 10) {
		std::string msg = "counting = ";
		msg += std::to_string(count);
		msg += "\r\n";
		
		print(msg.c_str());
		
		count += 1;

		taskDelay(DELAY_MS(100));
	}

	taskDelete(nullptr);
}

void printing_task2()
{
	print("printing task2 init\r\n");

	int count = 0;
	while (count < 10) {
		std::string msg = "counting = ";
		msg += std::to_string(count);
		msg += "\r\n";
		
		print(msg.c_str());

		count += 1;

		taskDelay(DELAY_MS(100));
	}

	taskDelete(nullptr);
}

int main()
{
	print("logLib application starting\r\n");
	
	TASK_ID task_id = taskSpawn(const_cast<char *>("printing task"), 10, 0, 200, (FUNCPTR)printing_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	TASK_ID task_id2 = taskSpawn(const_cast<char *>("printing task2"), 10, 0, 200, (FUNCPTR)printing_task2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	while (taskIdVerify(task_id) == OK || taskIdVerify(task_id2) == OK) {}

	return 0;
}