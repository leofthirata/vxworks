#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <taskLib.h>
#include <semLib.h>
#include <msgQLib.h>

#include <sockLib.h>
#include <inetLib.h>

// #include <eventLibCommon.h>

// Issue 7 - event/notify example

MSG_Q_ID queue;
SEM_ID sem;
_Vx_event_t ev;

void task_to_notify()
{
    _Vx_event_t events;
    int ret = eventReceive(VXEV24, EVENTS_WAIT_ANY, WAIT_FOREVER, &events); // hangs waiting for the VXEV24 event to be received
    printf("Event received: %d event result: %d\n", events, ret);
    taskDelete(NULL);
}

int main()
{
    printf("ISSUE 7...\n");

    int task_id1 = taskSpawn(
        "notify",   // task name
        100,             // task priority, 100 is medium
        0,               // task options, 0 default
        2000,            // stack size
        (FUNCPTR)task_to_notify,  // task to be created
        0, 0, 0, 0, 0,   // task args (max 10)
        0, 0, 0, 0, 0);

    if (task_id1 == ERROR)
        perror("Failed to create task!\n");
    else
        printf("Task id: %d\n", task_id1);

    eventSend(task_id1, VXEV01);
    eventSend(task_id1, VXEV02);
    eventSend(task_id1, VXEV24);

    while (taskIdVerify(task_id1) == OK) { };
    
    printf("Tasks done, returning from application\n");

    return 0; 
}