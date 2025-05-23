#include <stdio.h>
#include <stdbool.h>

#include <sysLib.h>
#include <taskLib.h>

#include <wdLib.h>

#include <subsys/gpio/vxbGpioLib.h>
#include <subsys/gpio/vxbGpioCommon.h>

#include <sockLib.h>
#include <inetLib.h>

#define DELAY_MS(ms)	(sysClkRateGet() / (1000 / ms))

#define LED_PIN			21
#define BUTTON_PIN		20
#define GPIO_MODE_OUTPUT 		1
#define GPIO_MODE_INPUT 		0

// socket applications dont work in DKM mode?

#define UDP_PORT		8080

bool control = false;
bool stop = false;
WDOG_ID timer;
TASK_ID timer_id;
int count = 0;

int led_config(int gpio);
int gpio_config(int gpio, int mode);
int gpio_toggle(int gpio);

void timer_handler()
{
	control = true;
}

int timer_config()
{
	timer = wdCreate();
	if (timer == NULL) {
		perror("Failed to create timer!\n");
		return -1;
	}

	wdStart(timer, DELAY_MS(500), (FUNCPTR)timer_handler, 0);

	return 0;
}

void timer_task()
{
	printf("Timer task started!\n");

	int ret = timer_config();
	if (ret < 0) {
		perror("timer_config fail\n");
	}

	int gpio = LED_PIN;
	ret = led_config(gpio);
	if (ret < 0) {
		perror("led_config fail\n");
	}

	// int count = 0;
	while (count < 6) {
	// while (!stop) {
		if (control) {
			// count++;
			// printf("Timer triggered!\n");
			control = false;
			gpio_toggle(gpio);
			wdStart(timer, DELAY_MS(1000), (FUNCPTR)timer_handler, 0);
		}
		taskDelay(DELAY_MS(1));
	}

	stop = true;

	taskDelete(NULL);
}

int led_config(int gpio)
{
	int ret = gpio_config(gpio, GPIO_MODE_OUTPUT);
	if (ret < 0) {
		perror("led config fail\n");
	}

	return ret;
}

int gpio_config(int gpio, int mode)
{
	int ret = vxbGpioSetDir(gpio, mode);
	if (ret < 0) {
		perror("vxbGpioSetDir fail\n");
	}

	return ret;
}
int gpio_get_level(int gpio)
{
	return vxbGpioGetValue(gpio);
}

int gpio_set_level(int gpio, int level)
{
	int val = vxbGpioGetValue(gpio);
	if (val == level)
		return 0;
	
	return vxbGpioSetValue(gpio, level);
}

int gpio_toggle(int gpio)
{
	int dir = vxbGpioGetDir(gpio);
	if (dir == 0) {
		perror("toggle fail, gpio mode input\n");
		return -1;
	}

	int level = vxbGpioGetValue(gpio);
	int ret = vxbGpioSetValue(gpio, level^1);
	return ret;
}

void button_handler()
{
	stop = true;
}

void button_task()
{
	printf("Button task started!\n");

	int gpio = BUTTON_PIN;
	int ret = gpio_config(gpio, GPIO_MODE_INPUT);
	if (ret < 0) {
		perror("Failed to config button\n");
	}

	int prev = 0;
	int curr = 0;
	while (count < 6) {
		curr = gpio_get_level(gpio);
		if (prev != curr && curr == 0) { // neg edge
			// button_handler();
			printf("Button pressed!\n");
			if (!taskIsSuspended(timer_id))
				taskSuspend(timer_id);
			else
				taskResume(timer_id);

			count++;
			stop = true;
		}
		prev = curr;
		// taskDelay(DELAY_MS(1));
	}

	taskDelete(NULL);
}

void udp_server_task()
{
	printf("UDP server task started\n");

	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		perror("Failed to create socket\n");
		stop = true;
	}

	struct sockaddr_in server;
	memset(&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(UDP_PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	int optval;
	int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(int));
    if (ret != 0)
    {
        close(sock);
        perror("setsockopt failed: ");
		stop = true;
    }

	ret = bind(sock, (struct sockaddr *)&server, sizeof(server));
	if (ret < 0) {
        close(sock);
		perror("Failed to bind server\n");
		stop = true;
	}

	while (!stop) {
		struct sockaddr client;
		memset(&client, 0, sizeof(struct sockaddr));
		socklen_t len = sizeof(client);
		char buf[100];
		long int rx_bytes = recvfrom(sock, buf, sizeof(buf), 0, &client, &len);
		printf("Received %d bytes\n", rx_bytes);

		if (rx_bytes > 0) {
			char msg[rx_bytes];
			snprintf(msg, rx_bytes, "%s", buf);
			printf("Received msg: %s\n", msg);
		}
	}

	close(sock);

	taskDelete(NULL);
}

int main()
{
	timer_id = taskSpawn("timer_task", 10, 0, 200, (FUNCPTR)timer_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	TASK_ID button_id = taskSpawn("button_task", 11, 0, 200, (FUNCPTR)button_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	// TASK_ID server_id = taskSpawn("udp_server_task", 12, 0, 500, (FUNCPTR)udp_server_task, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	// while (taskIdVerify(timer_id) == OK || taskIdVerify(button_id) == OK || taskIdVerify(server_id) == OK) {
	while (taskIdVerify(timer_id) == OK || taskIdVerify(button_id) == OK) {
			// taskDelay(DELAY_MS(500));
	}

	return 0;
}