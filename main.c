#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

void hexDump(char *desc, void *addr, int len)
{
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Output description if given.
    if (desc != NULL)
        printf ("%s:\n", desc);

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf("  %s\n", buff);

            // Output the offset.
            printf("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf(" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
            buff[i % 16] = '.';
        } else {
            buff[i % 16] = pc[i];
        }

        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf("  %s\n", buff);
}

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
//    inet_pton(AF_INET6, "2001:4860:4860::8888", &ipv6_addr.sin6_addr);


//    long ipv4_icmp = sendto(sockfd_v4, data, sizeof icmp_hdr + 7, 0, (struct sockaddr_in *) &ipv4_addr,
//                            sizeof(struct sockaddr_in));
//    printf("output ipv4_icmp %ld\n", ipv4_icmp);
//
//    long ipv6_icmp = sendto(sockfd_v6, data_v6, sizeof icmp_hdr6 + 7, 0, (struct sockaddr_in6 *) &ipv6_addr,
//                            sizeof(struct sockaddr_in6));
//    printf("output ipv6_icmp %ld\n", ipv6_icmp);

//    int sockfd_v4_raw = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); //create socket
    int sockfd_v6_raw = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6); //create socket

    printf("sockfd_v6_raw : %d\n", sockfd_v6_raw);

    char *message = "message";
//    long ipv4_raw = sendto(sockfd_v4_raw, message, strlen(message) + 1, 0, (struct sockaddr_in *) &ipv4_addr,
//                           sizeof(struct sockaddr_in));
//    printf("output ipv4_raw %ld\n", ipv4_raw);


    hexDump(NULL, &ipv6_addr, sizeof(ipv6_addr));

    long ipv6_raw = sendto(sockfd_v6_raw, message, strlen(message) + 1, 0, (struct sockaddr_in6 *) &ipv6_addr,
                           sizeof(struct sockaddr_in6));
    printf("output ipv6_raw %ld\n", ipv6_raw);

    return 0;
}