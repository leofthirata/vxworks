#include <vxWorks.h>
#include <taskLib.h>
#include <stdio.h>

void log(const char *fmt, ...) 
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void task_hello_world()
{
    log("Hello, World!\n");
}

int main()
{
    printf("Starting application...\n");

    int task_id = taskSpawn(
        "hello_world",   // task name
        100,             // task priority, 100 is medium
        0,               // task options, 0 default
        2000,            // stack size
        (FUNCPTR)task_hello_world,  // task to be created
        0, 0, 0, 0, 0,   // task args (max 10)
        0, 0, 0, 0, 0);

    // Verifica se a task foi criada com sucesso 
    if (task_id == ERROR)
        log("Failed to create task!\n");
    else
        log("Task id: %d\n", task_id);
    
    taskClose(task_id);

    return 0; 
}