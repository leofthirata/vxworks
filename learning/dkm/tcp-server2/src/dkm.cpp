#include <iostream>
#include <string>

#include <taskLib.h>
#include <sysLib.h>

#include <inetLib.h>
#include <sockLib.h>

#define DELAY_MS(ms)	(sysClkRateGet() / (1000 / ms))
#define TCP_PORT 		8003

// GET / HTTP1.1\r\nHost: 192.168.0.12:8003\r\n\r\n
std::string handle_get(std::string req)
{
	std::string tmp = req;
	auto path_begin_idx = tmp.find("/");
	tmp = tmp.substr(path_begin_idx);
	std::cout << tmp << std::endl;
	
	auto path_end_idx = tmp.find(0x20);
	std::string path = tmp.substr(0, path_end_idx);

	std::string ret = "HTTP/1.1 404 Not Found\r\n\r\n";

	std::cout << path << std::endl;

	if (path == "/") {
		ret = "HTTP/1.1 200 OK\r\n\r\n";
	}

	return ret;
}

std::string tcp_handle_request(std::string req)
{
	if (req.find("GET") != std::string::npos) {
		// return handle_get();
		return handle_get(req);
	}
	else if (req.find("POST") != std::string::npos) {
		// return handle_post();
	}
	else {
		// return handle_invalid_request();
	}

	return "";
}

int tcp_handle_client(int client, std::string msg)
{
	// std::cout << "Received from client: " << msg.lenth() << " bytes" << std::endl;
	std::cout << "Received (" << msg.length() << " bytes)" << ": " << msg << std::endl;

	std::string resp = tcp_handle_request(msg);
	ssize_t ret = send(client, resp.c_str(), resp.length(), 0);
	if (ret < 0) {
		std::cerr << "Failed to send response" << std::endl;	
		return -1;
	}

	close(client);

	return 0;
}

void server_task()
{
	std::cout << "server task init" << std::endl;
	
	int server = socket(AF_INET, SOCK_STREAM, 0);
	if (server < 0) {
		std::cout << "failed to create socket" << std::endl;
		taskDelete(nullptr);
		return;
	}

	int optval = 1;
	int ret = setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(TCP_PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	ret = bind(server, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr));
	if (ret < 0) {
		std::cout << "failed to bind socket" << std::endl;
		taskDelete(nullptr);
		return;
	}

	ret = listen(server, 10);
	if (ret < 0) {
		std::cout << "failed to listen" << std::endl;
		taskDelete(nullptr);
		return;
	}

	struct sockaddr client_addr;
	socklen_t len = sizeof(client_addr);

	int client = accept(server, &client_addr, &len);
	if (client < 0) {
		std::cout << "failed to listen" << std::endl;
		taskDelete(nullptr);
		return;
	}

	char buf[100];
	ssize_t rcv_bytes = read(client, buf, sizeof(buf));
	if (rcv_bytes > 0) {
		std::string msg = buf;
		msg = msg.substr(0, rcv_bytes);
		tcp_handle_client(client, msg);
	}

	// int count = 0;
	// while (count < 10) {
	// 	count += 1;
	// 	std::cout << "server task loop" << std::endl;
	// 	taskDelay(DELAY_MS(1000));
	// }

	close(server);

	std::cout << "server task delete" << std::endl;
	taskDelete(nullptr);
}

int main()
{
	TASK_ID server_task_id = taskSpawn(const_cast<char *>("server_task"), 10, 0, 2000, (FUNCPTR)server_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	if (server_task_id == nullptr)
		std::cout << "failed to create task" << std::endl;

	while (taskIdVerify(server_task_id) == OK) {}

	std::cout << "app done" << std::endl;

	return 0;
}