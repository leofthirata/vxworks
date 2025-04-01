#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <taskLib.h>
#include <sysLib.h>
#include <syscall.h>

// Issue 9 - print every 1 second

#define MACRO_DELAY_MS(ms)  (sysClkRateGet() / (1000 / ms))

void task_print()
{
    int count = 0;
    uint32_t delay_ms = 1000;
    while (count < 10) {
        printf("Count = %d\n", count);
        count += 1;
        taskDelay(MACRO_DELAY_MS(delay_ms));
    }

    taskDelete(NULL);
}

int main()
{
    printf("ISSUE 7...\n");

    int task_id1 = taskSpawn(
        "print",   // task name
        100,             // task priority, 100 is medium
        0,               // task options, 0 default
        2000,            // stack size
        (FUNCPTR)task_print,  // task to be created
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