#include <stdio.h>
#include <stdbool.h>

#include <sysLib.h>
#include <taskLib.h>

#include <sockLib.h>
#include <inetLib.h>

#define DELAY_MS(ms)	(sysClkRateGet() / (1000 / ms))
#define TCP_PORT 		8008

uint32_t timeout = 1000;
bool control = false;

void tcp_task()
{
	printf("TCP TASK BEGIN\n");

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		printf("Failed to open sock");
		taskDelete(NULL);
		return;
	}

	int optval;
	int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(int));
	if (ret != 0) {
		printf("Failed to setsockopt");
		taskDelete(NULL);
		return;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(TCP_PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	ret = bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret != 0) {
		printf("Failed to bind sock");
		taskDelete(NULL);
		return;
	}

	ret = listen(sock, 10);
	if (ret != 0) {
		printf("Failed to listen");
		taskDelete(NULL);
		return;
	}

	while (1) {
		struct sockaddr client;
		socklen_t len = sizeof(client);
		ret = accept(sock, &client, &len);
		if (ret != -1) {
			char buf[100];
			long int rcv = recv(sock, buf, sizeof(buf), 0);
			if (rcv > 0) {
				printf("Received: %s\n", buf);
			}
		}
	}

	taskDelete(NULL);
}

void timeout_task()
{
	printf("Timeout task started!\n");

	int count = 60;

	while (count > 0) {
		count--;
		taskDelay(DELAY_MS(1000));
	}

	control = true;

	taskDelete(NULL);
}

int main()
{
	printf("DKM TCP SERVER\n");

	// std::string tcp_name = "tcp_task";
	TASK_ID tcp_task_id = taskSpawn("tcp_task", 10, 0, 200, (FUNCPTR)tcp_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if (tcp_task_id == NULL) {
		perror("Failed to create tcp server task\n");
		return -1;
	}

	// TASK_ID timeout_task_id = taskSpawn("timeout_task", 9, 0, 100, (FUNCPTR)timeout_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	// if (timeout_task_id == NULL) {
	// 	perror("Failed to create timeout task\n");
	// 	return -1;
	// }

	while (taskIdVerify(tcp_task_id) == OK) {}

	printf("Application done\n");
	return 0;
}