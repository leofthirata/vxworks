#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <taskLib.h>
#include <semLib.h>
#include <msgQLib.h>

#include <sockLib.h>
#include <inetLib.h>

// Issue 6 - udp server with loging task
// logging task deletes tasks after receiving stop msg but cant close socket without its descriptor
MSG_Q_ID queue;
#define SERVER_IP       "192.168.0.18"
#define SERVER_PORT     8000

#define UDP_SERVER      1
// #define TCP_SERVER      1

#define BUF_SIZE        100
#define QUEUE_SIZE      5
#define QUEUE_MSG_SIZE  100

static bool stop_udp = false;
static bool stop_log = false;

#ifdef TCP_SERVER
void receive_handle(int client, struct sockaddr *client_addr)
{
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    long int rx_bytes = recv(client, buf, sizeof(buf), 0);

    if (rx_bytes > 0) {
        printf("Received %d bytes\n", rx_bytes);

        char *rcv = (char *)malloc(rx_bytes);
        strncpy(rcv, buf, rx_bytes);
        printf("Received from client: %s\n", rcv);
    }
}
#endif

void task_log(int id)
{
    printf("Log task started!\n");

    int task_id_server = id;

    char buf[QUEUE_MSG_SIZE];
    const char *stop_msg = "stop";
    while (!stop_log) {
        // timeout param (third) can be WAIT_FOREVER or NO_WAIT
        long int rx_bytes = msgQReceive(queue, buf, sizeof(buf), WAIT_FOREVER);
        if (rx_bytes > 0) {
            char *msg = (char *)malloc(sizeof(rx_bytes));
            strncpy(msg, buf, rx_bytes);
            printf("Received from queue: %s\n", msg);

            if (strncmp(msg, stop_msg, rx_bytes) == 0) {
                stop_udp = true;
                stop_log = true;
                printf("Task self delete: %d\n", taskIdSelf());

                taskDelete(task_id_server);
                taskDelete(NULL);
                return;
            }
        }
    }

    printf("Task done: %d\n", taskIdSelf());

    taskDelete(NULL);
}

void task_server()
{
    printf("UDP server task started!\n");

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Failed to open socket\n");
        taskDelete(NULL);
        return;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int optval;
    int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(int));
    if (ret != 0)
    {
        close(sock);
        perror("setsockopt failed: ");
        taskDelete(NULL);
        return;   
    }

    ret = bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        close(sock);
        perror("bind failed\n");
        taskDelete(NULL);
        return;
    }

#ifdef TCP_SERVER
    ret = listen(sock, 5);
    if (ret < 0) {
        close(sock);
        perror("listen failed\n");
        taskDelete(NULL);
        return;
    }
#endif

    struct sockaddr client_addr;
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    socklen_t client_addr_len = sizeof(client_addr);
    char msg[BUF_SIZE];
    sprintf(msg, "Ack from UDP server\n");

    while (!stop_udp) {
#ifdef TCP_SERVER
        int client = accept(sock, &client_addr, &client_addr_len);

        receive_handle(client, &client_addr);
#elif UDP_SERVER
        char buf[BUF_SIZE];
        memset(buf, 0, BUF_SIZE);
        long int rx_bytes = recvfrom(sock, buf, sizeof(buf), 0, &client_addr, &client_addr_len);
        printf("recvfrom returned: %d\n", rx_bytes);

        if (rx_bytes > 0) {
            char *rcv = (char *)malloc(rx_bytes);
            strncpy(rcv, buf, rx_bytes);
            sendto(sock, msg, strlen(msg), 0, &client_addr, client_addr_len);
            msgQSend(queue, rcv, rx_bytes, WAIT_FOREVER, MSG_PRI_NORMAL);
        }
        else {
            close(sock);
            perror("recvfrom failed\n");
            printf("Task self delete: %d\n", taskIdSelf());
            taskDelete(NULL);
            return;
        }
#endif
    }

    close(sock);

    printf("Task done: %d\n", taskIdSelf());

    taskDelete(NULL);
}

int main()
{
    printf("ISSUE 10...\n");

    // third param can be MSG_Q_FIFO or MSG_Q_PRIORITY
    queue = msgQCreate(QUEUE_SIZE, QUEUE_MSG_SIZE, MSG_Q_FIFO);
    if (queue == NULL) {
        perror("Failed to create queue");
    }

    int task_id1 = taskSpawn(
        "server",   // task name
        100,             // task priority, 100 is medium
        0,               // task options, 0 default
        2000,            // stack size
        (FUNCPTR)task_server,  // task to be created
        0, 0, 0, 0, 0,   // task args (max 10)
        0, 0, 0, 0, 0);

    if (task_id1 == ERROR)
        perror("Failed to create task!\n");
    else
        printf("Task id: %d\n", task_id1);

    int task_id2 = taskSpawn(
        "log",   // task name
        100,             // task priority, 100 is medium
        0,               // task options, 0 default
        2000,            // stack size
        (FUNCPTR)task_log,  // task to be created
        task_id1, 0, 0, 0, 0,   // task args (max 10)
        0, 0, 0, 0, 0);

    if (task_id2 == ERROR)
        perror("Failed to create task!\n");
    else
        printf("Task id: %d\n", task_id2);

    while (taskIdVerify(task_id1) == OK || taskIdVerify(task_id2) == OK) { };
    
    printf("Tasks done, returning from application\n");

    return 0; 
}