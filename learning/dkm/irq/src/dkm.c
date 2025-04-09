#include <stdio.h>
#include <stdbool.h>

#include <taskLib.h>
#include <intLib.h>
#include <sysLib.h>

#include <iv.h>

#define USER_IRQ		50

#define millis()		((tickGet() / sysClkRateGet()) * 1000)
#define DELAY_MS(ms)	(sysClkRateGet() / (1000 / ms))

bool button_pressed = false;
uint32_t timeout = 1000;

void button_handler(int arg);

// not working

void button_handler(int arg)
{
	button_pressed = true;
}

int main()
{
	printf("DKM app 4\n");

	uint8_t arg = 0x10;
	int ret = intConnect((VOIDFUNCPTR*)INUM_TO_IVEC(8), (VOIDFUNCPTR)button_handler, arg);
	if (ret < 0) {
		perror("Failed to intConnect\n");
		return -1;
	}
	ret = intEnable(8);
	// sysIntEnable(0xFF);
	if (ret < 0) {
		perror("Failed to Enable\n");
		return -1;
	}

	int count = 0;
	while (count < 20) {
		if (button_pressed) {
			printf("loop count %d\n", count);
			button_pressed = false;
		}
		count += 1;
		taskDelay(DELAY_MS(500));
	}

	printf("Application done\n");
	return 0;
}