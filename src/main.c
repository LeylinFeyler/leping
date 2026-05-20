#include "leping.h"

volatile int running = 1;

ping_stats stats = {
    .transmitted = 0,
    .received = 0,
    .rtt_min = 1e9,
    .rtt_max = 0,
    .rtt_sum = 0,
    .rtt_sum_sq = 0
};

void handle_sigint(int sig) {
    (void)sig;
    running = 0;
}

int main(int argc, char **argv)
{
    PingOptions opts;

    if (parse_args(argc, argv, &opts) != 0)
        return 1;

    int sockfd;
    struct sockaddr_storage addr;
    socklen_t addr_len;
    int seq = 0;

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);

    if (resolve_host(opts.host, &addr, &addr_len) != 0)
        return 1;
    gettimeofday(&stats.start_time, NULL);

    char ip[INET6_ADDRSTRLEN];
    void *raw_addr;
    if (addr.ss_family == AF_INET) {
        struct sockaddr_in *ipv4 = (struct sockaddr_in*)&addr;
        raw_addr = &ipv4->sin_addr;
    } else {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)&addr;
        raw_addr = &ipv6->sin6_addr;
    }
    inet_ntop(addr.ss_family, raw_addr, ip, sizeof(ip));

    printf("LEPING %s (%s)\n",
        reverse_dns((struct sockaddr*)&addr, addr_len),
        ip
    );

    sockfd = create_raw_socket(addr.ss_family);
    if (sockfd < 0)
        return 1;

    while (running) {
        if (opts.count != -1 &&
            seq >= opts.count)
            break;

        struct timeval send_time;

        if (send_ping(sockfd, (struct sockaddr*)&addr, addr_len,
                        seq, opts.packet_size, &send_time) == 0)
            receive_ping(sockfd, seq, opts.timeout, &send_time);

        seq++;
        usleep(opts.interval * 1000000);
    }

    print_statistics(opts.host);

    close(sockfd);

    return 0;
}