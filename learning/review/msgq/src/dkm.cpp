#include <iostream>
#include <string>

#include <sysLib.h>
#include <taskLib.h>
#include <msgQLib.h>

#define DELAY_MS(ms)	(sysClkRateGet() / (1000 / ms))
MSG_Q_ID msgq;

void task1()
{
	std::cout << "task1 init" << std::endl; 

	int count = 0;
	while (count < 10) {
		std::string msg = "msg to task2 num: ";
		msg += std::to_string(count);
		const char *test = "Hello from task1";
		
		// int ret = msgQSend(msgq, const_cast<char *>(msg.c_str()), msg.length(), WAIT_FOREVER, 10);
		int ret = msgQSend(msgq, (char *)test, strlen(test), WAIT_FOREVER, 0);
		if (ret != OK) {
			std::cout << "Failed to send" << std::endl;
		}

		count += 1;
		
		taskDelay(DELAY_MS(500));
	}

	taskDelete(nullptr);
}

void task2()
{
	std::cout << "task2 init" << std::endl; 

	int msg_rcv_count = 0;
	while (msg_rcv_count < 10) {
		char buf[100];
		ssize_t ret = msgQReceive(msgq, buf, sizeof(buf), WAIT_FOREVER);

		std::string msg = buf;
		msg = msg.substr(0, ret);
		std::cout << "Received from msgQ: " << msg << std::endl;

		ssize_t q_size = msgQNumMsgs(msgq);
		std::cout << "amount of messages in msgQ: " << q_size << std::endl;

		msg_rcv_count += 1;

		taskDelay(DELAY_MS(3000));
	}
	
	taskDelete(nullptr);
}

int main()
{
	msgq = msgQCreate(10, 100, 0);

	TASK_ID task_id1 = taskSpawn(const_cast<char *>("task1"), 10, 0, 200, (FUNCPTR)task1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	TASK_ID task_id2 = taskSpawn(const_cast<char *>("task2"), 9, 0, 200, (FUNCPTR)task2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	while (taskIdVerify(task_id1) == 0 || taskIdVerify(task_id2) == 0) {}

	std::cout << "app done" << std::endl;

	return 0;
}