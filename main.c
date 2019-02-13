#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

int main() {

    struct icmphdr {
        u_int8_t type;        /* message type */
        u_int8_t code;        /* type sub-code */
        u_int16_t checksum;
        union {
            struct {
                u_int16_t id;
                u_int16_t sequence;
            } echo;            /* echo datagram */
            u_int32_t gateway;    /* gateway address */
            struct {
                u_int16_t    __unused;
                u_int16_t mtu;
            } frag;            /* path mtu discovery */
        } un;
    };

    struct sockaddr_in6 ipv6_addr; //set up dest address info
    struct sockaddr_in ipv4_addr;

    struct icmphdr icmp_hdr;

    unsigned char data[2048];
    memset(&icmp_hdr, 0, sizeof icmp_hdr);
    icmp_hdr.type = 8; // ICMP_ECHO from #include <netinet/ip_icmp.h>
    icmp_hdr.un.echo.id = 1234;//arbitrary id
    memcpy(data, &icmp_hdr, sizeof icmp_hdr);
    memcpy(data + sizeof icmp_hdr, "message", 7); //icmp payload

    struct icmphdr icmp_hdr6;

    unsigned char data_v6[2048];
    memset(&icmp_hdr6, 0, sizeof icmp_hdr6);
    icmp_hdr6.type = 128; // ICMPV6_ECHO_REQUEST from #include icmpv6.h
    icmp_hdr6.un.echo.id = 1234;//arbitrary id
    memcpy(data_v6, &icmp_hdr6, sizeof icmp_hdr6);
    memcpy(data_v6 + sizeof icmp_hdr6, "message", 7); //icmp payload


    int sockfd_v4 = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP); //create socket
    int sockfd_v6 = socket(AF_INET6, SOCK_DGRAM, IPPROTO_ICMPV6); //create socket

    ipv4_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "8.8.8.8", &ipv4_addr.sin_addr);

    ipv6_addr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, "::1", &ipv6_addr.sin6_addr);


    long ipv4_icmp = sendto(sockfd_v4, data, sizeof icmp_hdr + 7, 0, (struct sockaddr_in *) &ipv4_addr,
                            sizeof(struct sockaddr_in));
    printf("output ipv4_icmp %ld\n", ipv4_icmp);

    long ipv6_icmp = sendto(sockfd_v6, data_v6, sizeof icmp_hdr6 + 7, 0, (struct sockaddr_in6 *) &ipv6_addr,
                            sizeof(struct sockaddr_in6));
    printf("output ipv6_icmp %ld\n", ipv6_icmp);

    int sockfd_v4_raw = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); //create socket
    int sockfd_v6_raw = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6); //create socket

    char *message = "hello5545";
    long ipv4_raw = sendto(sockfd_v4_raw, message, strlen(message) + 1, 0, (struct sockaddr_in *) &ipv4_addr,
                           sizeof(struct sockaddr_in));
    printf("output ipv4_raw %ld\n", ipv4_raw);

    long ipv6_raw = sendto(sockfd_v6_raw, message, strlen(message) + 1, 0, (struct sockaddr_in6 *) &ipv6_addr,
                           sizeof(struct sockaddr_in6));
    printf("output ipv6_raw %ld\n", ipv6_raw);

    return 0;
}