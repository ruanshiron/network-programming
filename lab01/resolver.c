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
  
// Returns hostname for the local computer 
void checkHostName(int hostname) 
{ 
    if (hostname == -1) 
    { 
        perror("gethostname"); 
        exit(1); 
    } 
} 
  
// Returns host information corresponding to host name 
void checkHostEntry(struct hostent * hostentry) 
{ 
    if (hostentry == NULL) 
    { 
        perror("gethostbyname"); 
        exit(1); 
    } 
} 
  
// Converts space-delimited IPv4 addresses 
// to dotted-decimal format 
void checkIPbuffer(char *IPbuffer) 
{ 
    if (NULL == IPbuffer) 
    { 
        perror("inet_ntoa"); 
        exit(1); 
    } 
} 
  
// Driver code 
int main() 
{ 
    int i;
    char hostbuffer[256]; 
    char *IPbuffer; 
    struct hostent *host_entry; 
    struct in_addr **addr_list;
    int hostname; 

    host_entry = gethostbyname("vietnamnet.vn"); 
    checkHostEntry(host_entry); 
  
    IPbuffer = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])); 
  
    printf("Hostname: %s\n", hostbuffer); 
    printf("Host IP: %s\n", IPbuffer); 

    addr_list = (struct in_addr **)host_entry->h_addr_list;
    for(i = 0; addr_list[i] != NULL; i++) {
        printf("\t%s ", inet_ntoa(*addr_list[i]));
    }
    printf("\n");


    struct hostent *he;
    struct in_addr ipv4addr;
    struct in6_addr ipv6addr;

    inet_pton(AF_INET, IPbuffer, &ipv4addr);
    he = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
    if (he!=NULL) printf("Host name: %s\n", he->h_name);

    inet_pton(AF_INET6, "2001:db8:63b3:1::beef", &ipv6addr);
    he = gethostbyaddr(&ipv6addr, sizeof ipv6addr, AF_INET6);
    if (he!=NULL) printf("Host name: %s\n", he->h_name);
  
    return 0; 
} 