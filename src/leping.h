#ifndef LEPING_H
#define LEPING_H

#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/icmp6.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "cli.h"

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

int create_raw_socket(int family);
int resolve_host(const char *host, struct sockaddr_storage *addr, socklen_t *addr_len);
const char *reverse_dns(struct sockaddr *addr, socklen_t addr_len);

uint16_t icmp_checksum(void *buf, int len);
void build_icmp_packet(struct icmp *pkt, int seq, int packet_size);

int send_ping(int sockfd, struct sockaddr *addr, socklen_t addr_len, int seq, int packet_size,
              struct timeval *send_time);
int receive_ping(int sockfd, int seq, int timeout, struct timeval *send_time);

double time_diff(struct timeval start, struct timeval end);
void print_statistics(const char *host);

#endif