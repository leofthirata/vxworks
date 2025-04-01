#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <taskLib.h>
#include <sysLib.h>
#include <syscall.h>

#define GPIO_SYSCALL
#define GPIO_GROUP 7
#define GPIO_SET_DIR SYSCALL_NUMBER(GPIO_GROUP, 2)
#define GPIO_GET_DIR SYSCALL_NUMBER(GPIO_GROUP, 3)

// Issue 8 - gpio example doesnt work

void task_led()
{
    // while (1) {

    // }

    taskDelete(NULL);
}

void gpio_set_dir(uint32_t gpio)
{
    syscall(gpio, 1, 1, 1, 1, 1, 1, 1, GPIO_SET_DIR);
}

uint32_t gpio_get_dir(uint32_t gpio)
{
    uint32_t ret = 0;
    syscall(gpio, ret, 0, 0, 0, 0, 0, 0, GPIO_GET_DIR);
    return ret;
}

int main()
{
    printf("ISSUE 7...\n");

    uint32_t freq = sysClkRateGet();
    
    printf("freq = %u\n", freq);

    gpio_set_dir(21);

    int task_id1 = taskSpawn(
        "led",   // task name
        100,             // task priority, 100 is medium
        0,               // task options, 0 default
        2000,            // stack size
        (FUNCPTR)task_led,  // task to be created
        0, 0, 0, 0, 0,   // task args (max 10)
        0, 0, 0, 0, 0);

    if (task_id1 == ERROR)
        perror("Failed to create task!\n");
    else
        printf("Task id: %d\n", task_id1);

    while (taskIdVerify(task_id1) == OK) { };
    
    printf("Tasks done, returning from application\n");

    return 0; 
}