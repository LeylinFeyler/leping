#include "leping.h"

uint16_t icmp_checksum(void *buf, int len) {
    uint16_t *data = buf;
    unsigned long sum = 0;

    // checksum is calculated as a 16-bit one's complement sum
    while (len > 1) {
        sum += *data++;
        len -= 2;
    }

    // handle odd byte at the end
    if (len == 1)
        sum += *(uint8_t*)data;

    // fold 32-bit sum into 16 bits
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    return (uint16_t)(~sum);
}

void build_icmp_packet(struct icmp *pkt, int seq) {

    // icmp echo request packet
    pkt->icmp_type = ICMP_ECHO;
    pkt->icmp_code = 0;

    // process id is commonly used to identify ping sessions
    pkt->icmp_id = getpid();
    pkt->icmp_seq = seq;

    // payload area (not required but typical for ping)
    memset(pkt->icmp_data, 0, PING_DATA_SIZE);

    // checksum must be calculated with checksum field set to zero
    pkt->icmp_cksum = 0;
    pkt->icmp_cksum = icmp_checksum(pkt, PACKET_SIZE);
}

int send_ping(int sockfd, struct sockaddr_in *addr, int seq, struct timeval *send_time) {
    
    // interpret raw buffer as icmp structure
    char packet[PACKET_SIZE];
    struct icmp *icmp_hdr = (struct icmp*)packet;

    build_icmp_packet(icmp_hdr, seq);

    // record send time to later compute rtt
    gettimeofday(send_time, NULL);

    // count sent packages
    stats.transmitted++;

    int sent = sendto(sockfd, packet, PACKET_SIZE, 0, (struct sockaddr*)addr, sizeof(*addr));

    if (sent <= 0) {
        perror("sendto");
        return -1;
    }

    return 0;
}

int receive_ping(int sockfd, int seq, struct timeval *send_time) {
    char buffer[1024];

    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    int n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addr_len);

    if (n <= 0) {
        perror("recvfrom");
        return -1;
    }

    // timestamp of received reply
    struct timeval recv_time;
    gettimeofday(&recv_time, NULL);

    // received packet contains ip header followed by icmp payload
    struct ip *ip_hdr = (struct ip*)buffer;

    // ip_hl is stored in 32-bit words, convert to bytes
    int ip_header_len = ip_hdr->ip_hl << 2;

    // move pointer past ip header to reach icmp header
    struct icmp *icmp_hdr = (struct icmp*)(buffer + ip_header_len);

    // verify this is the reply to our echo request
    if (icmp_hdr->icmp_type == ICMP_ECHOREPLY &&
        icmp_hdr->icmp_id == getpid() &&
        icmp_hdr->icmp_seq == seq)
    {
        double rtt = time_diff(*send_time, recv_time);

        // count received packages
        stats.received++;

        if (rtt < stats.rtt_min)
            stats.rtt_min = rtt;

        if (rtt > stats.rtt_max)
            stats.rtt_max = rtt;

        stats.rtt_sum += rtt;
        stats.rtt_sum_sq += rtt * rtt;

        printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n",
            n - ip_header_len,
            inet_ntoa(addr.sin_addr),
            seq,
            ip_hdr->ip_ttl,
            rtt
        );
    }

    return 0;
}