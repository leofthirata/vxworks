#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <taskLib.h>
#include <timerLib.h>
#include <semLib.h>

// Issue 11 - Timer triggers and give sem to end the application
timer_t timer_id;
bool triggered = false;
SEM_ID sem;

void timer_handler(timer_t id, _Vx_usr_arg_t args)
{
    static int count = 0;

    if (count == 10)
        semGive(sem);

    printf("Timer triggered!\n");
    printf("id: %d\n", id);
    printf("args: %d\n", args);
    printf("count: %d\n", count);
    count += 1;
}

int main()
{
    printf("ISSUE 11...\n");

    sem = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);

    struct timespec tp_gettime, tp_settime;
    struct itimerspec time_val;

    tp_settime.tv_sec = 0; // clock resolution
    tp_settime.tv_nsec = 1;
    clock_settime(CLOCK_REALTIME, &tp_settime);

    // clockLib.h
    int ret = timer_create(CLOCK_REALTIME, NULL, &timer_id);
    if (ret == ERROR) {
        printf("Failed to create timer\n");
    }

    ret = timer_connect(timer_id, timer_handler, 666);
    if (ret == ERROR) {
        printf("Failed to connect timer\n");
    }

    time_val.it_interval.tv_sec = 1;
    time_val.it_interval.tv_nsec = 0;
    time_val.it_value.tv_sec = 1;
    time_val.it_value.tv_nsec = 0;

    ret = timer_settime(timer_id, CLOCK_REALTIME, &time_val, NULL);
    if (ret == ERROR) {
        printf("Failed to set timer\n");
    }

    clock_gettime(CLOCK_REALTIME, &tp_gettime) ;

    printf("tp_gettime.tv_sec == %d\n", tp_gettime.tv_sec);
    printf("tp_gettime.tv_nsec == %d\n", tp_gettime.tv_nsec);

    ret = timer_gettime(timer_id, &time_val);
    if (ret == ERROR) {
        printf("Failed to get timer\n");
    }

    printf("time_val.it_interval.tv_sec == %d\n", time_val.it_interval.tv_sec);
    printf("time_val.it_interval.tv_nsec == %d\n", time_val.it_interval.tv_nsec);
    printf("time_val.it_value.tv_sec == %d\n", time_val.it_value.tv_sec);
    printf("time_val.it_value.tv_nsec == %d\n", time_val.it_value.tv_nsec);

    semTake(sem, WAIT_FOREVER);

    printf("Application done!\n");

    return 0; 
}