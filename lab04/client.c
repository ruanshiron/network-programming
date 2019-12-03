#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define BUFF_SIZE 8192

void *doRecieving(void *sockID)
{

    int clientSocket = *((int *)sockID);

    while (1)
    {

        char data[1024];
        int read = recv(clientSocket, data, 1024, 0);
        data[read] = '\0';
        printf("SERVER | %s\n", data);
    }
}

void *doSending(void *sockID) 
{
    int bytes_sent, msg_len;
    int clientSocket = *((int *)sockID);

    while (1)
    {
        char buff[BUFF_SIZE];
        memset(buff, '\0', (strlen(buff) + 1));
        fgets(buff, BUFF_SIZE, stdin);
        msg_len = strlen(buff);
        if (strcmp(buff, "\0") == 0)
        {
            printf("Closing now\n");
            close(clientSocket);
            return 0;
            exit(1);
        }
        strtok(buff, "\n");
        bytes_sent = send(clientSocket, buff, strlen(buff), 0);

    }
}

int main(int argc, char const *argv[])
{

    int clientSocket = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        return 0;

    printf("Connection established ............\n");

    pthread_t thread_r, thread_s;
    pthread_create(&thread_r, NULL, doRecieving, (void *)&clientSocket);
    pthread_create(&thread_s, NULL, doSending, (void *)&clientSocket);

    pthread_join(thread_r, NULL);
    pthread_join(thread_s, NULL);
}