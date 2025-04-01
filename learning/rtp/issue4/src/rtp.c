#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <taskLib.h>
#include <semLib.h>

#include <sockLib.h>
#include <inetLib.h>

// Issue 4 - tcp client
SEM_ID semaphore;
#define SERVER_IP       "192.168.0.15"
#define SERVER_PORT     8080

void task_client()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == ERROR) {
        printf("Failed to open socket\n");
        taskDelete(NULL);
        return;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    int ret = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        printf("Failed to connect to server\n");
        taskDelete(NULL);
        return;
    }

    int count = 0;
    while (count < 10) {
        char buf[100];
        sprintf(buf, "Client from task %d message count %d\n", taskIdSelf(), count);

        semTake(semaphore, WAIT_FOREVER);

        // size_t bytes = sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        long int bytes = send(sock, buf, strlen(buf), 0);

        printf("Sent %d bytes\n", bytes);

        memset(buf, 0, 100);
        long int rx_bytes = recv(sock, buf, 100, 0);
        if (rx_bytes > 0) {
            printf("Received %d bytes\n", rx_bytes);

            char *rcv = (char *)malloc(rx_bytes);
            strncpy(rcv, buf, rx_bytes);
            printf("Received from tcp server: %s\n", rcv);
        }
        semGive(semaphore);
        count += 1;
    }

    close(sock);

    printf("Task done %d\n", taskIdSelf());

    taskDelete(NULL);
}

int main()
{
    printf("ISSUE 4...\n");

    semaphore = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);

    semGive(semaphore);

    int task_id1 = taskSpawn(
        "client 1",   // task name
        100,             // task priority, 100 is medium
        0,               // task options, 0 default
        2000,            // stack size
        (FUNCPTR)task_client,  // task to be created
        0, 0, 0, 0, 0,   // task args (max 10)
        0, 0, 0, 0, 0);

    if (task_id1 == ERROR)
        printf("Failed to create task!\n");
    else
        printf("Task id: %d\n", task_id1);

    int task_id2 = taskSpawn(
        "client 2",   // task name
        100,             // task priority, 100 is medium
        0,               // task options, 0 default
        2000,            // stack size
        (FUNCPTR)task_client,  // task to be created
        0, 0, 0, 0, 0,   // task args (max 10)
        0, 0, 0, 0, 0);

    if (task_id2 == ERROR)
        printf("Failed to create task!\n");
    else
        printf("Task id: %d\n", task_id2);

    while (taskIdVerify(task_id1) == OK || taskIdVerify(task_id2) == OK) { };
    
    printf("Tasks done, returning from application\n");

    semDelete(semaphore);

    return 0; 
}