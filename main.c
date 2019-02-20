#include <string.h>
#include <stdio.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <zconf.h>

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

    #define icmp6_data16	icmp6_dataun.icmp6_un_data16
    struct icmp6_hdr icmp_hdr6;
    #define icmp6_id	icmp6_data16[0]		/* echo request/reply */
    #define icmp6_seq	icmp6_data16[1]		/* echo request/reply */

    unsigned char data_v6[2048];
    memset(&icmp_hdr6, 0, sizeof icmp_hdr6);
    (&icmp_hdr6)->icmp6_type = 128; // ICMPV6_ECHO_REQUEST from #include icmpv6.h
    (&icmp_hdr6)->icmp6_code = 0;
    memcpy(data_v6, &icmp_hdr6, sizeof icmp_hdr6);
    memcpy(data_v6 + sizeof icmp_hdr6, "message", 7); //icmp payload

    int ident;
    ident = getpid() & 0xFFFF;
    (&icmp_hdr6)->icmp6_id = htons(ident);
    (&icmp_hdr6)->icmp6_seq = ntohs(47806);


    int sockfd_v6 = socket(AF_INET6, SOCK_DGRAM, IPPROTO_ICMPV6); //create socket

    ipv6_addr.sin6_family = AF_INET6;
//    ipv6_addr.sin6_scope_id = 0;
    ipv6_addr.sin6_scope_id = if_nametoindex("en0");


    struct iovec iov[2];
    memset(&iov, 0, sizeof(iov));
    iov[0].iov_base = (caddr_t)data_v6;

    printf("Interface id : %d\n", if_nametoindex("en0"));

//    inet_pton(AF_INET6, "::1", &ipv6_addr.sin6_addr);
    inet_pton(AF_INET6, "fe80::18e1:317:51c:5db0", &ipv6_addr.sin6_addr);

    long ipv6_icmp = sendto(sockfd_v6, data_v6, sizeof icmp_hdr6 + 7, 0, (struct sockaddr_in6 *) &ipv6_addr,
                            sizeof(struct sockaddr_in6));
    printf("output ipv6_icmp : %ld\n", ipv6_icmp);

    struct msghdr m;

    m.msg_name = (caddr_t) &ipv6_addr;
    m.msg_namelen = sizeof(ipv6_addr);
    memset(&iov, 0, sizeof(iov));
    iov[0].iov_base = (caddr_t) data_v6;
    iov[0].iov_len = sizeof(data_v6);
    m.msg_iov = iov;
    m.msg_iovlen = 1;

    long cc = recvmsg(sockfd_v6, &m, 0);
    printf("cc : %ld\n", cc);

    struct icmp6_hdr *icp;
    struct icmp6_nodeinfo *ni;
    u_int16_t seq;

    if (cc > 0) {
        icp = (struct icmp6_hdr *) data_v6;
        ni = (struct icmp6_nodeinfo *) data_v6;
        seq = ntohs(*(u_int16_t *)ni->icmp6_ni_nonce);

        printf("Icmp6 type : %d\n", icp->icmp6_type);
        printf("Icmp6 seq : %d\n", seq);
    }

    return 0;
}