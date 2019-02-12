#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <stdio.h>
#include <arpa/inet.h>

main()
{
    char *message = "hello5545";
    int dest_port = 5000;
    struct sockaddr_in6 dest_addr; //set up dest address info

    int sockfd = socket(PF_INET6, SOCK_RAW, IPPROTO_ICMP); //create socket

    dest_addr.sin6_family = AF_INET6;
    inet_pton (AF_INET6, "::1",  &dest_addr.sin6_addr);

    long out = sendto (sockfd, message, strlen(message)+1, 0 , (struct sockaddr_in6 *)&dest_addr, sizeof(struct sockaddr_in6));
    printf("output %ld\n", out);
}

// now get it back and print it
//    inet_ntop(AF_INET6, &(sa.sin6_addr), str, INET6_ADDRSTRLEN);
//    printf("%s\n", str); // prints "2001:db8:8714:3a90::12"