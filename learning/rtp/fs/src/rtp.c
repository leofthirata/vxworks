#include <stdio.h>
#include <string.h>

#include <ioLib.h>

#define DELAY_MS(ms)	(sysClkRateGet() / (1000 / ms))

int main()
{
	int fd = open("test.txt", O_RDWR | O_CREAT);
	if (fd < 0) {
		perror("Failed to create test.txt\n");
		return -1;
	}

	const char *msg = "Something to write";
	long int ret = write(fd, msg, strlen(msg)+1);
	if (ret != strlen(msg)+1) {
		perror("Failed to write to test.txt\n");
		return -1;
	}

	char buf[50];
	ret = read(fd, buf, sizeof(buf));
	if (ret < 0) {
		perror("Failed to read test.txt\n");
		return -1;
	}

	char buf2[ret];
	snprintf(buf2, ret, buf);
	printf("Read: %s\n", buf2);

	close(fd);

	return 0;
}