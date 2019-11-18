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
#include <pthread.h>

#define BUFF_SIZE 8192

int clientCount = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct client
{

	int index;
	int sockID;
	struct sockaddr_in clientAddr;
	int len;
};

struct client Client[1024];
pthread_t thread[1024];

void *doNetworking(void *ClientDetail)
{

	struct client *clientDetail = (struct client *)ClientDetail;
	int index = clientDetail->index;
	int clientSocket = clientDetail->sockID;

	printf("Client %d connected.\n", index + 1);

	while (1)
	{
		char recv_data[BUFF_SIZE];
		int bytes_sent, bytes_received;
		//receives message from client
		bytes_received = recv(clientSocket, recv_data, BUFF_SIZE - 1, 0); //blocking
		if (bytes_received <= 0)
		{
			printf("\nConnection closed");
			break;
		}
		else
		{
			recv_data[bytes_received] = '\0';
			printf("\nReceive: %s ", recv_data);
		}

		//echo to client
		bytes_sent = send(clientSocket, recv_data, bytes_received, 0); /* send to the client welcome message */

	}

	return NULL;
}

int main()
{

	int serverSocket = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8080);
	serverAddr.sin_addr.s_addr = htons(INADDR_ANY);

	if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
		return 0;

	if (listen(serverSocket, 1024) == -1)
		return 0;

	printf("Server started listenting on port 8080 ...........\n");

	while (1)
	{

		Client[clientCount].sockID = accept(serverSocket, (struct sockaddr *)&Client[clientCount].clientAddr, &Client[clientCount].len);
		Client[clientCount].index = clientCount;

		pthread_create(&thread[clientCount], NULL, doNetworking, (void *)&Client[clientCount]);

		clientCount++;
	}

	for (int i = 0; i < clientCount; i++)
		pthread_join(thread[i], NULL);
}