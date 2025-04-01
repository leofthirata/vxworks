#include <vxWorks.h>
#include <taskLib.h>
#include <stdio.h>

#define TASK_DELAY  100

// Issue 2 - periodic tasks
// two tasks log periodically
void log(const char *fmt, ...) 
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void task_hello_world()
{
    int count = 0;
    while (count < 5) {
        log("Hello, World!\n");
        taskDelay(TASK_DELAY);
        count += 1;
    }

    taskClose(NULL);
}

void task_hello_vxworks()
{
    int count = 0;
    while (count < 5) {
        log("Hello from VxWorks!\n");
        taskDelay(TASK_DELAY);
        count += 1;
    }

    taskClose(NULL);
}

int main()
{
    printf("Starting application...\n");

    int task_id1 = taskSpawn(
        "hello_world",   // task name
        100,             // task priority, 100 is medium
        0,               // task options, 0 default
        2000,            // stack size
        (FUNCPTR)task_hello_world,  // task to be created
        0, 0, 0, 0, 0,   // task args (max 10)
        0, 0, 0, 0, 0);

    if (task_id1 == ERROR)
        log("Failed to create task!\n");
    else
        log("Task id: %d\n", task_id1);

    int task_id2 = taskSpawn(
        "hello_vxworks",   // task name
        100,             // task priority, 100 is medium
        0,               // task options, 0 default
        2000,            // stack size
        (FUNCPTR)task_hello_vxworks,  // task to be created
        0, 0, 0, 0, 0,   // task args (max 10)
        0, 0, 0, 0, 0);

    if (task_id2 == ERROR)
        log("Failed to create task!\n");
    else
        log("Task id: %d\n", task_id2);

    while (taskIdVerify(task_id1) == OK || taskIdVerify(task_id2) == OK) {};
    
    printf("Tasks done, returning from application\n");

    return 0; 
}