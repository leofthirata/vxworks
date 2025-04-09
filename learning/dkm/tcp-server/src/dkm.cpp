#include <iostream>
#include <string>

#include <sysLib.h>
#include <taskLib.h>

#include <sockLib.h>
#include <inetLib.h>

#define DELAY_MS(ms)	(sysClkRateGet() / (1000 / ms))
#define TCP_PORT 		8003

uint32_t timeout = 1000;
bool control = false;

void tcp_task()
{
	std::cout << "TCP TASK BEGIN\n" << std::endl;

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		std::cout << "Failed to open sock" << std::endl;
		taskDelete(nullptr);
		return;
	}

	int optval;
	int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&optval), sizeof(int));
	if (ret != 0) {
		std::cout << "Failed to setsockopt" << std::endl;
		taskDelete(nullptr);
		return;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(TCP_PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	ret = bind(sock, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr));
	if (ret != 0) {
		std::cout << "Failed to bind sock" << std::endl;
		taskDelete(nullptr);
		return;
	}

	ret = listen(sock, 10);
	if (ret != 0) {
		std::cout << "Failed to listen" << std::endl;
		taskDelete(nullptr);
		return;
	}

	while (!control) {
		struct sockaddr client;
		socklen_t len;
		ret = accept(sock, &client, &len);
		if (ret != -1) {
			char buf[100];
			auto rcv = read(sock, buf, sizeof(buf));
			if (rcv > 0) {
				std::cout << "Received: " << buf << std::endl;
			}
		}
	}

	taskDelete(nullptr);
}

void timeout_task()
{
	std::cout << "Timeout task started!" << std::endl;

	int count = 60;

	while (count > 0) {
		count--;
		taskDelay(DELAY_MS(1000));
	}

	control = true;

	taskDelete(nullptr);
}

int main()
{
	std::cout << "DKM TCP SERVER" << std::endl;

	// std::string tcp_name = "tcp_task";
	TASK_ID tcp_task_id = taskSpawn(const_cast<char *>("tcp_task"), 10, 0, 200, (FUNCPTR)tcp_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if (tcp_task_id == nullptr) {
		perror("Failed to create tcp server task\n");
		return -1;
	}

	TASK_ID timeout_task_id = taskSpawn(const_cast<char *>("timeout_task"), 9, 0, 100, (FUNCPTR)timeout_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if (timeout_task_id == nullptr) {
		perror("Failed to create timeout task\n");
		return -1;
	}

	while (taskIdVerify(tcp_task_id) == OK || taskIdVerify(timeout_task_id) == OK) {}

	std::cout << "Application done\n" << std::endl;
	return 0;
}