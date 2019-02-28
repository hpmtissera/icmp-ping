//
//  main.c
//  ping-cksum
//
//  Created by Brett Wooldridge on 10/4/17.
//  Copyright © 2017 Brett Wooldridge. All rights reserved.
//

#include <stdio.h>
#include <stddef.h>
#include <sys/param.h>        /* NB: we rely on this for <sys/types.h> */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip_var.h>

struct tv32 {
    u_int32_t tv32_sec;
    u_int32_t tv32_usec;
};

u_short
in_cksum(addr, len)
        u_short *addr;
        int len;
{
    int nleft, sum;
    u_short *w;
    union {
        u_short    us;
        u_char    uc[2];
    } last;
    u_short answer;

    nleft = len;
    sum = 0;
    w = addr;

    /*
     * Our algorithm is simple, using a 32 bit accumulator (sum), we add
     * sequential 16 bit words to it, and at the end, fold back all the
     * carry bits from the top 16 bits into the lower 16 bits.
     */
    while (nleft > 1)  {
        sum += *w++;
        nleft -= 2;
    }

    /* mop up an odd byte, if necessary */
    if (nleft == 1) {
        last.uc[0] = *(u_char *)w;
        last.uc[1] = 0;
        sum += last.us;
    }

    /* add back carry outs from top 16 bits to low 16 bits */
    sum = (sum >> 16) + (sum & 0xffff);    /* add hi 16 to low 16 */
    sum += (sum >> 16);            /* add carry */
    answer = ~sum;                /* truncate to 16 bits */
    return(answer);
}

int main(int argc, const char * argv[]) {
    u_char outpackhdr[64];

    for (int i = 0; i < 64; i++)
        outpackhdr[i] = i;

    printf("Checksum: %d\n", in_cksum((u_short *)&outpackhdr, 64));

    for (int i = 0; i < 64; i++)
        outpackhdr[i] = (255 - i);

    printf("Checksum: %d\n", in_cksum((u_short *)&outpackhdr, 64));

    printf("ICMP_MINLEN: %d\n", ICMP_MINLEN);

    printf("sizeOf(struct ip): %ld\n", sizeof(struct ip));

    printf("sizeOf(struct icmp): %ld\n", sizeof(struct icmp));

    printf("icmp.icmp_seq offset: %ld\n", offsetof(struct icmp, icmp_seq));

    printf("icmp.ira_data offset: %ld\n", offsetof(struct icmp, icmp_dun.id_data));

    printf("sizeOf(struct tv32): %ld\n", sizeof(struct tv32));

    printf("----------------------------------\n");
    printf("sizeOf(struct sockaddr_storage): %ld\n", sizeof(struct sockaddr_storage));
    printf("ss.ss_len offset: %ld\n", offsetof(struct sockaddr_storage, ss_len));
    printf("ss.ss_family offset: %ld\n", offsetof(struct sockaddr_storage, ss_family));
    printf("ss.__ss_pad1 offset: %ld\n", offsetof(struct sockaddr_storage, __ss_pad1));
    printf("ss.__ss_align offset: %ld\n", offsetof(struct sockaddr_storage, __ss_align));
    printf("ss.__ss_pad2 offset: %ld\n", offsetof(struct sockaddr_storage, __ss_pad2));
    printf("sizeOf(struct sockaddr_storage.__ss_pad2): %ld\n", sizeof(struct sockaddr_storage) - offsetof(struct sockaddr_storage, __ss_pad2));

    return 0;
}
