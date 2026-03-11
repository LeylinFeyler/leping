#ifndef LEPING_H
#define LEPING_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <math.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <sys/time.h>
#include <netdb.h>

#define PING_DATA_SIZE 56
#define PACKET_SIZE 64

typedef struct {
    int transmitted;
    int received;

    double rtt_min;
    double rtt_max;
    double rtt_sum;
    double rtt_sum_sq;

    struct timeval start_time;
} ping_stats;

extern ping_stats stats;

int create_raw_socket();
int resolve_host(const char *host, struct sockaddr_in *addr);

uint16_t icmp_checksum(void *buf, int len);
void build_icmp_packet(struct icmp *pkt, int seq);

int send_ping(int sockfd, struct sockaddr_in *addr, int seq, struct timeval *send_time);
int receive_ping(int sockfd, int seq, struct timeval *send_time);

double time_diff(struct timeval start, struct timeval end);
void print_statistics(const char *host);

#endif