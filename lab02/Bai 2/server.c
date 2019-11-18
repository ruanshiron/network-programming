#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <netdb.h>
#include <ctype.h>

#define MAXLINE 1024


int isNumber(const char * str) {
    int i;
    for (i=0; i<strlen(str); i++) {
        if (!isdigit(str[i])) return 0;
    }

    return 1;
}

//use this to check if string is IP address
int isValidIpAddress(char ipAddress[])
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    if (result != 0)
        return 1;
    
    char buffer[256];
    strcpy(buffer, ipAddress);

    const char dot[2] = ".";
    char *token;

    token = strtok(buffer, dot);

    while (token != NULL) {

        if (!isNumber(token)) return 0;
        else if (atoi(token)<0 || atoi(token)>255) return -1; 

        token = strtok(NULL, dot);
    }

    return 1;
}

//convert from IP to address
char *fromIpToAddress(const char* ip)
{
    int i;
    struct hostent *host_entry; 
    struct in_addr ipv4addr;
    host_entry = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);

    char *message = "Official name: ";
    char firstloglocation[MAXLINE];
    inet_pton(AF_INET, ip, &ipv4addr);

    if (host_entry == NULL)
    {
        herror("Error");
        return "Not found information\n";
    }
    else
    {
        strcpy(firstloglocation, message);
        message = strcat(firstloglocation, host_entry->h_name);
        strcat(message, "\nAlias name:\n");
        for (i = 0; host_entry->h_aliases[i] != NULL; i++)
        {
            strcpy(firstloglocation, message);
            message = strcat(firstloglocation, host_entry->h_aliases[i]);
            strcat(message, "\n");
        }
        return message;
    }
}

//convert form address to ip
char *fromAddressToIp(const char* address)
{
    int i;
    struct hostent *host_entry; 
    struct in_addr ipv4addr;
    host_entry = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);

    char *message = "Official IP: \n";
    char firstloglocation[MAXLINE];
    char secondloglocation[MAXLINE];
    if ((host_entry = gethostbyname(address)) == NULL)
    {
        herror("Error");
        return "Not found information\n";
    }
    else
    {
        for (i = 0; host_entry->h_addr_list[i] != NULL; i++)
        {
            if (i == 1) {
                strcat(message, "Alias IP address: \n");
            }

            strcpy(firstloglocation, message);
            message = strcat(firstloglocation, inet_ntoa(*(struct in_addr *)host_entry->h_addr_list[i]));
            strcat(message, "\n");
        }

        return message;
    }
}

int main(int argc, char const *argv[])
{
    int i;

    if (argc != 2)
    {
        printf("error, too many or too few arguments\n");
        return 1;
    }
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(atoi(argv[1]));
    //servaddr.sin_port = htons(PORT);
    bzero(&(servaddr.sin_zero), 8); /* zero the rest of the structure */

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr,
             sizeof(struct sockaddr)) == -1)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        int len, n;
        len = sizeof(struct sockaddr_in);
        //receive request form client, save in buffer
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&cliaddr,
                     &len);
        if (n < 0)
        {
            perror("Error: ");
            close(sockfd);
            return 0;
        }
        buffer[n - 1] = '\0';
        printf("Recieve form Client : %s\n", buffer);
        //check if requested string is a ip address
        if (isValidIpAddress(buffer) == 1)
        {
            printf("IP to address: Processing ...\n");
            n = sendto(sockfd, (const char *)fromIpToAddress(buffer),
                       strlen(fromIpToAddress(buffer)),
                       MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                       len);
        }
        else if (isValidIpAddress(buffer) == 0)
        {
            printf("Address to IP: Processing ...\n");
            n = sendto(sockfd, (const char *)fromAddressToIp(buffer),
                       strlen(fromAddressToIp(buffer)),
                       MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                       len);
        } else if (isValidIpAddress(buffer) == -1) {
            const char * invalidIPstr = "IP Address is invalid";
            printf("%s\n", invalidIPstr);
            n = sendto(sockfd, invalidIPstr, strlen(invalidIPstr),
                        MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
                       len);
        }
        if (n < 0)
        {
            perror("Error: ");
            close(sockfd);
            return 0;
        }
        printf("Result sent.\n");
    }
    close(sockfd);
    return 0;
}