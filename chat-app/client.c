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
        printf("%s\n", data);
    }
}

int main()
{

    int clientSocket = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        return 0;

    printf("Connection established ............\n");

    pthread_t thread;
    pthread_create(&thread, NULL, doRecieving, (void *)&clientSocket);

    while (1)
    {
        int msg_len, bytes_sent, bytes_received;
        char buff[BUFF_SIZE];
        //send message
        printf("\nInsert string to send:");
        memset(buff, '\0', (strlen(buff) + 1));
        fgets(buff, BUFF_SIZE, stdin);
        msg_len = strlen(buff);
        if (strcmp(buff, "\n") == 0)
        {
            printf("Closing\n");
            close(clientSocket);
            return 0;
        }

        bytes_sent = send(clientSocket, buff, msg_len, 0);

        if (bytes_sent <= 0)
        {
            printf("\nConnection closed!\n");
            break;
        }

        //receive echo reply
        bytes_received = recv(clientSocket, buff, BUFF_SIZE - 1, 0);
        if (bytes_received <= 0)
        {
            printf("\nError!Cannot receive data from sever!\n");
            break;
        }

        buff[bytes_received] = '\0';
        printf("Reply from server: %s", buff);
    }
}