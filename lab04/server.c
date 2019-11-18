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
#define FILEN "account.txt"

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

struct account
{
	char id[255];
	char password[255];
	int status;
	struct account *next;
};

struct account *head = NULL;
struct account Account[1024];
struct client Client[1024];
pthread_t thread[1024];

void updateAccountData()
{
	FILE *f;
	f = fopen(FILEN, "w+");

	struct account *c = head;

	while (c != NULL)
	{
		fprintf(f, "%s\t%s\t%d\n", c->id, c->password, c->status);
		c = c->next;
	}

	fclose(f);
}

struct account *existID(char *id)
{
	struct account *c = head;

	while (c != NULL)
	{
		if (!strcmp(c->id, id))
			return c;
		c = c->next;
	}
}

int verifyPassword(struct account *acc, char *password)
{
	if (!strcmp(acc->password, password))
		return 1;
	return 0;
}

void doAuthenticated(int clientSocket)
{
	while (1)
	{
		char recv_data[BUFF_SIZE];
		int bytes_sent, bytes_received;
		//receives message from client
		bytes_received = recv(clientSocket, recv_data, BUFF_SIZE - 1, 0); //blocking
		if (bytes_received <= 0)
		{
			printf("\nConnection closed");
			return;
		}
		else
		{
			recv_data[bytes_received] = '\0';
			printf("\nReceive: %s ", recv_data);
		}

		//echo to client
		bytes_sent = send(clientSocket, recv_data, bytes_received, 0); /* send to the client welcome message */
	}
}

int authentication(int max_auth_error, int clientSocket)
{
	char recv_data[BUFF_SIZE];
	int bytes_received, bytes_sent;

	while (1)
	{
		//receives username from client
		bytes_received = recv(clientSocket, recv_data, BUFF_SIZE - 1, 0); //blocking
		if (bytes_received <= 0)
		{
			printf("\nConnection closed");
			return 0;
		}

		recv_data[bytes_received] = '\0';

		struct account *current_account = existID(recv_data);

		if (current_account)
		{
			//echo to client
			bytes_sent = send(clientSocket, "User existed\n", sizeof "User existed\n", 0); /* send to the client welcome message */

			int error_time = 0;
			while (1)
			{
				if (error_time >= max_auth_error)
				{
					//echo to client
					bytes_sent = send(clientSocket, "status -1\n", sizeof "status -1\n", 0); /* send to the client welcome message */
					current_account->status = -1;
					updateAccountData();
					break;
				}
				//receives password from client
				bytes_received = recv(clientSocket, recv_data, BUFF_SIZE - 1, 0); //blocking
				if (bytes_received <= 0)
				{
					printf("\nConnection closed");
					return 0;
				}

				recv_data[bytes_received] = '\0';

				if (verifyPassword(current_account, recv_data))
				{
					//echo to client
					bytes_sent = send(clientSocket, "Logged In\n", sizeof "Logged In\n", 0); /* send to the client welcome message */
					doAuthenticated(clientSocket);
					break;
				}
				else
				{
					bytes_sent = send(clientSocket, "Wrong Password\n", sizeof "Wrong Password\n", 0); /* send to the client welcome message */
					error_time++;
				}
			}
		}
		else
		{
			bytes_sent = send(clientSocket, "User not existed\n", sizeof "User existed\n", 0); /* send to the client welcome message */
		}
	}
}

void *doNetworking(void *ClientDetail)
{

	struct client *clientDetail = (struct client *)ClientDetail;
	int index = clientDetail->index;
	int clientSocket = clientDetail->sockID;

	char recv_data[BUFF_SIZE];
	int bytes_sent, bytes_received;

	const int max_auth_error = 3;

	printf("Client %d connected.\n", index + 1);

	while (1)
	{
		//authentication
		if (!authentication(max_auth_error, clientSocket))
			break;
	}

	return NULL;
}

void initAccountData()
{
	FILE *f;
	f = fopen(FILEN, "r+");

	int temp_status;
	char temp_password[255], temp_id[255];

	while (1)
	{
		if (fscanf(f, "%s\t%s\t%d\n", temp_id, temp_password, &temp_status) == EOF)
			break;

		//add account to list
		struct account *link = (struct account *)malloc(sizeof(struct account));
		strcpy(link->id, temp_id);
		strcpy(link->password, temp_password);
		link->status = temp_status;
		link->next = head;
		head = link;
	}

	fclose(f);
}

int main()
{
	initAccountData();

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