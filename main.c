#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <net/if.h>

void hexDump(char *desc, void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char *) addr;

    // Output description if given.
    if (desc != NULL)
        printf("%s:\n", desc);

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

    struct icmp6_hdr {
        u_int8_t icmp6_type;    /* type field */
        u_int8_t icmp6_code;    /* code field */
        u_int16_t icmp6_cksum;    /* checksum field */
        union {
            u_int32_t icmp6_un_data32[1]; /* type-specific field */
            u_int16_t icmp6_un_data16[2]; /* type-specific field */
            u_int8_t icmp6_un_data8[4];  /* type-specific field */
        } icmp6_dataun;
    } __attribute__((__packed__));

    struct icmp6_nodeinfo {
        struct icmp6_hdr icmp6_ni_hdr;
        u_int8_t icmp6_ni_nonce[8];
        /* could be followed by reply data */
    }__attribute__((__packed__));

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
//    ipv6_addr.sin6_scope_id = 0;
    ipv6_addr.sin6_scope_id = if_nametoindex("en0");

    printf("Interface id : %d\n", if_nametoindex("en0"));

    inet_pton(AF_INET6, "::1", &ipv6_addr.sin6_addr);
//    inet_pton(AF_INET6, "2001:4860:4860::8888", &ipv6_addr.sin6_addr);


//    long ipv4_icmp = sendto(sockfd_v4, data, sizeof icmp_hdr + 7, 0, (struct sockaddr_in *) &ipv4_addr,
//                            sizeof(struct sockaddr_in));
//    printf("output ipv4_icmp %ld\n", ipv4_icmp);
//
    long ipv6_icmp = sendto(sockfd_v6, data_v6, sizeof icmp_hdr6 + 7, 0, (struct sockaddr_in6 *) &ipv6_addr,
                            sizeof(struct sockaddr_in6));
    printf("output ipv6_icmp : %ld\n", ipv6_icmp);

    struct msghdr m;
    struct iovec iov[2];
    struct cmsghdr *cm = NULL;

# define CONTROLLEN    10240

    m.msg_name = (caddr_t) &ipv6_addr;
    m.msg_namelen = sizeof(ipv6_addr);
    memset(&iov, 0, sizeof(iov));
    iov[0].iov_base = (caddr_t) data_v6;
    iov[0].iov_len = sizeof(data_v6);
    m.msg_iov = iov;
    m.msg_iovlen = 1;

//    memset(cm, 0, CONTROLLEN);

    m.msg_control = (void *) cm;
//    m.msg_controllen = CONTROLLEN;

    long cc = recvmsg(sockfd_v6, &m, 0);
    printf("cc : %ld\n", cc);

    struct sockaddr *from;
    int fromlen;
    struct icmp6_hdr *icp;
    struct icmp6_nodeinfo *ni;
    u_int16_t seq;

    if (cc > 0) {
        from = (struct sockaddr *) (&m)->msg_name;
        fromlen = (&m)->msg_namelen;
        icp = (struct icmp6_hdr *) data_v6;
        ni = (struct icmp6_nodeinfo *) data_v6;
        seq = ntohs(*(u_int16_t *)ni->icmp6_ni_nonce);

        printf("Icmp6 type : %d\n", icp->icmp6_type);
        printf("Icmp6 seq : %d\n", seq);

    }

//    int sockfd_v4_raw = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); //create socket
//    int sockfd_v6_raw = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6); //create socket

//    int bindResult = bind(sockfd_v6_raw, (struct sockaddr *)&ipv6_addr, sizeof(ipv6_addr));
//    printf("bind result v6-raw : %d\n", bindResult);

//    printf("sockfd_v6_raw : %d\n", sockfd_v6_raw);

//    char *message = "message";
//    long ipv4_raw = sendto(sockfd_v4_raw, message, strlen(message) + 1, 0, (struct sockaddr_in *) &ipv4_addr,
//                           sizeof(struct sockaddr_in));
//    printf("output ipv4_raw %ld\n", ipv4_raw);


//    hexDump(NULL, &ipv6_addr, sizeof(ipv6_addr));
//
//    long ipv6_raw = sendto(sockfd_v6_raw, data_v6, sizeof icmp_hdr6 + 7, 0, (struct sockaddr_in6 *) &ipv6_addr,
//                           sizeof(struct sockaddr_in6));
//    printf("output ipv6_raw %ld\n", ipv6_raw);

    return 0;
}