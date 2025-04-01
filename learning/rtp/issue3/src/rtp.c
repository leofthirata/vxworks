#include <stdio.h>

#include <taskLib.h>
#include <semLib.h>

// Issue 3 - periodic tasks with semaphore
// two tasks log periodically with no delay

SEM_ID semaphore;

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
        semTake(semaphore, WAIT_FOREVER);
        log("Hello World task!\n");
        count += 1;
        semGive(semaphore);
    }

    taskDelete(NULL);
}

void task_vxworks()
{
    int count = 0;
    while (count < 5) {
        semTake(semaphore, WAIT_FOREVER);
        log("VxWorks task!\n");
        count += 1;
        semGive(semaphore);
    }

    taskDelete(NULL);
}

int main()
{
    printf("ISSUE 3...\n");

    semaphore = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);

    semGive(semaphore);

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
        "vxworks",   // task name
        100,             // task priority, 100 is medium
        0,               // task options, 0 default
        2000,            // stack size
        (FUNCPTR)task_vxworks,  // task to be created
        0, 0, 0, 0, 0,   // task args (max 10)
        0, 0, 0, 0, 0);

    if (task_id2 == ERROR)
        log("Failed to create task!\n");
    else
        log("Task id: %d\n", task_id2);

    while (taskIdVerify(task_id1) == OK || taskIdVerify(task_id2) == OK) { };
    
    printf("Tasks done, returning from application\n");

    semDelete(semaphore);

    return 0; 
}