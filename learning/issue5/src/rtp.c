#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <taskLib.h>
#include <semLib.h>

#include <sockLib.h>
#include <inetLib.h>

// Issue 5 - udp server
// cant listen in udp mode
#define SERVER_IP       "192.168.0.18"
#define SERVER_PORT     8000

#define UDP_SERVER      1
// #define TCP_SERVER      1

#define BUF_SIZE        100

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

void task_server()
{
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
    int count = 0;
    while (count < 10) {
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
            printf("Received (%d) bytes from client: %s\n", rx_bytes, rcv);
            count += 1;
        }
        else {
            close(sock);
            perror("recvfrom failed\n");
            taskDelete(NULL);
            return;
        }
#endif
        // printf("Accepted connection from: %s\n", );
    }

    close(sock);

    printf("Task done %d\n", taskIdSelf());

    taskDelete(NULL);
}

int main()
{
    printf("ISSUE 5...\n");

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

    while (taskIdVerify(task_id1) == OK) { };
    
    printf("Tasks done, returning from application\n");

    return 0; 
}