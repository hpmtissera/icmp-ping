#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <inttypes.h>
#include <stdio.h>
#include <arpa/inet.h>

int main()
{
    char *message = "hello5545";

    struct icmphdr
    {
        u_int8_t type;		/* message type */
        u_int8_t code;		/* type sub-code */
        u_int16_t checksum;
        union
        {
            struct
            {
                u_int16_t	id;
                u_int16_t	sequence;
            } echo;			/* echo datagram */
            u_int32_t	gateway;	/* gateway address */
            struct
            {
                u_int16_t	__unused;
                u_int16_t	mtu;
            } frag;			/* path mtu discovery */
        } un;
    };

    struct sockaddr_in6 ipv6_addr; //set up dest address info
    struct sockaddr_in ipv4_addr;
    struct icmphdr icmp_hdr;

    memset(&icmp_hdr, 0, sizeof icmp_hdr);
    icmp_hdr.type = 8;
    icmp_hdr.un.echo.id = 1234;//arbitrary id
    unsigned char data[2048];
    memcpy(data, &icmp_hdr, sizeof icmp_hdr);
    memcpy(data + sizeof icmp_hdr, "message", 7); //icmp payload


    int sockfd_v4 = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP); //create socket
    int sockfd_v6 = socket(AF_INET6, SOCK_DGRAM, IPPROTO_ICMPV6); //create socket

    printf("sockfd_v4 : %ld\n", sockfd_v4);

    ipv4_addr.sin_family = AF_INET;
    inet_pton (AF_INET, "8.8.8.8",  &ipv4_addr.sin_addr);

    ipv6_addr.sin6_family = AF_INET6;
    inet_pton (AF_INET6, "::1",  &ipv6_addr.sin6_addr);

//    char str[INET_ADDRSTRLEN];
//    inet_ntop(AF_INET, &(ipv4_addr.sin_addr), str, INET_ADDRSTRLEN);
//    printf("%s\n", str);

//    char str[INET6_ADDRSTRLEN];
//    inet_ntop(AF_INET6, &(dest_addr.sin6_addr), str, INET6_ADDRSTRLEN);
//    printf("%s\n", str);

    long ipv4_icmp = sendto (sockfd_v4, data, sizeof icmp_hdr + 7, 0 , (struct sockaddr_in *)&ipv4_addr, sizeof(struct sockaddr_in));
    printf("output ipv4_icmp %ld\n", ipv4_icmp);

    long ipv4 = sendto (sockfd_v4, message, strlen(message)+1, 0 , (struct sockaddr_in *)&ipv4_addr, sizeof(struct sockaddr_in));
    printf("output ipv4 %ld\n", ipv4);

    long ipv6 = sendto (sockfd_v6, message, strlen(message)+1, 0 , (struct sockaddr_in6 *)&ipv6_addr, sizeof(struct sockaddr_in6));
    printf("output ipv6 %ld\n", ipv6);



    return 0;
}