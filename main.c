#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <stdio.h>
#include <arpa/inet.h>

int main() {
    struct sockaddr_in6 sa;
    char str[INET6_ADDRSTRLEN];
    char* msg = "Hello World";

    int sockfd = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMP);

// store this IP address in sa:
    inet_pton(AF_INET6, "::1", &(sa.sin6_addr));
    sa.sin6_family = AF_INET6;

// now get it back and print it
//    inet_ntop(AF_INET6, &(sa.sin6_addr), str, INET6_ADDRSTRLEN);



    long out = sendto(sockfd, msg, strlen(msg) + 1, 0, (struct sockaddr *) &sa, sizeof(struct sockaddr));
    printf("output %ld\n", out);

//    printf("%s\n", str); // prints "2001:db8:8714:3a90::12"
    return 0;
}