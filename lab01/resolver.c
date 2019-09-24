// C program to display hostname 
// and IP address 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <string.h>
#include <ctype.h>
  
// Returns host information corresponding to host name 
void checkHostEntry(struct hostent * hostentry) 
{ 
    if (hostentry == NULL) 
    { 
        printf("Not found information\n");
        exit(1); 
    } 
} 

int isNumber(const char * str) {
    int i;
    for (i=0; i<strlen(str); i++) {
        if (!isdigit(str[i])) return 0;
    }

    return 1;
}

int isIP(const char * IPBuffer) 
{
    char buffer[256];
    strcpy(buffer, IPBuffer);

    const char dot[2] = ".";
    char *token;

    token = strtok(buffer, dot);

    while (token != NULL) {

        if (!isNumber(token)) return 0;
        else if (atoi(token)<0 || atoi(token)>255) return 0; 

        token = strtok(NULL, dot);
    }

    return 1;
}

int main(int argc, char const *argv[])
{
    if (argc < 2) {
        perror("no input string");
        return 1;
    }

    int i;

    struct hostent *host_entry; 
    struct in_addr **addr_list;

    if (!isIP(argv[1])) {
        host_entry = gethostbyname(argv[1]); 
        checkHostEntry(host_entry); 

        addr_list = (struct in_addr **)host_entry->h_addr_list;
        for(i = 0; addr_list[i] != NULL; i++) {
            if (i==0) printf("Official IP: ");
            else printf("Alias: \n");
            printf("\t%s\n", inet_ntoa(*addr_list[i]));
        }
        printf("\n");

        return 0;
    } 

    struct in_addr ipv4addr;

    inet_pton(AF_INET, argv[1], &ipv4addr);
    host_entry = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);

    checkHostEntry(host_entry);

    printf("Official name: %s\n", host_entry->h_name);

    char ** aliases = host_entry->h_aliases;
    for(i = 0; aliases[i] != NULL; i++) {
        printf("Alias name: \n");
        printf("\t%s\n", aliases[i]);
    }
    printf("\n");

    // struct in6_addr ipv6addr;
    // inet_pton(AF_INET6, "2001:db8:63b3:1::beef", &ipv6addr);
    // host_entry = gethostbyaddr(&ipv6addr, sizeof ipv6addr, AF_INET6);
    // if (host_entry!=NULL) printf("Host name: %s\n", host_entry->h_name);
  
    return 0;

    
}
