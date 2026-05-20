#include "leping.h"

uint16_t icmp_checksum(void *buf, int len) {
    uint16_t *data    = buf;
    unsigned long sum = 0;

    // checksum is calculated as a 16-bit one's complement sum
    while (len > 1) {
        sum += *data++;
        len -= 2;
    }

    // handle odd byte at the end
    if (len == 1) {
        sum += *(uint8_t *)data;
    }

    // fold 32-bit sum into 16 bits
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    return (uint16_t)(~sum);
}

void build_icmp_packet(struct icmp *pkt, int seq, int packet_size) {
    pkt->icmp_type = ICMP_ECHO;
    pkt->icmp_code = 0;

    pkt->icmp_id  = getpid();
    pkt->icmp_seq = seq;

    memset(pkt->icmp_data, 0, packet_size);

    pkt->icmp_cksum = 0;
    pkt->icmp_cksum = icmp_checksum(pkt, sizeof(struct icmp) + packet_size);
}

int send_ping(int sockfd, struct sockaddr *addr, socklen_t addr_len, int seq, int packet_size,
              struct timeval *send_time) {
    int total_size = sizeof(struct icmp) + packet_size;

    char packet[1500];
    struct icmp *icmp_hdr = (struct icmp *)packet;

    build_icmp_packet(icmp_hdr, seq, packet_size);

    gettimeofday(send_time, NULL);

    stats.transmitted++;

    int sent = sendto(sockfd, packet, total_size, 0, addr, addr_len);

    if (sent <= 0) {
        perror("sendto");
        return -1;
    }

    return 0;
}

int receive_ping(int sockfd, int seq, int timeout, struct timeval *send_time) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

    struct timeval tv;
    tv.tv_sec  = timeout;
    tv.tv_usec = 0;

    int ready = select(sockfd + 1, &readfds, NULL, NULL, &tv);

    if (ready == 0) {
        printf("Request timeout for icmp_seq %d\n", seq);
        stats.transmitted++;
        return -1;
    }

    if (ready < 0) {
        perror("select");
        return -1;
    }

    char buffer[1024];

    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, &addr_len);

    if (n <= 0) {
        perror("recvfrom");
        return -1;
    }

    // timestamp of received reply
    struct timeval recv_time;
    gettimeofday(&recv_time, NULL);

    // received packet contains ip header followed by icmp payload
    struct ip *ip_hdr = (struct ip *)buffer;

    // ip_hl is stored in 32-bit words, convert to bytes
    int ip_header_len = ip_hdr->ip_hl << 2;

    // move pointer past ip header to reach icmp header
    struct icmp *icmp_hdr = (struct icmp *)(buffer + ip_header_len);

    if (icmp_hdr->icmp_type == ICMP_DEST_UNREACH) {
        printf("From %s icmp_seq=%d Destination Host Unreachable\n", inet_ntoa(addr.sin_addr), seq);

        return -1;
    }

    // verify this is the reply to our echo request
    if (icmp_hdr->icmp_type == ICMP_ECHOREPLY && icmp_hdr->icmp_id == getpid() &&
        icmp_hdr->icmp_seq == seq) {

        double rtt = time_diff(*send_time, recv_time);

        // count received packages
        stats.received++;

        if (rtt < stats.rtt_min) {
            stats.rtt_min = rtt;
        }

        if (rtt > stats.rtt_max) {
            stats.rtt_max = rtt;
        }

        stats.rtt_sum += rtt;
        stats.rtt_sum_sq += rtt * rtt;

        char ip[INET6_ADDRSTRLEN];
        void *raw_addr;

        if (addr.sin_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)&addr;
            raw_addr                 = &ipv4->sin_addr;
        } else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)&addr;
            raw_addr                  = &ipv6->sin6_addr;
        }

        inet_ntop(addr.sin_family, raw_addr, ip, sizeof(ip));

        printf("%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%.2f ms\n", n - ip_header_len,
               reverse_dns((struct sockaddr *)&addr, addr_len), inet_ntoa(addr.sin_addr), seq,
               ip_hdr->ip_ttl, rtt);
    }

    return 0;
}