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
    struct sockaddr_in addr;
    int seq = 0;

    signal(SIGINT, handle_sigint);
    resolve_host(opts.host, &addr);
    gettimeofday(&stats.start_time, NULL);

    printf("PING %s (%s)\n", opts.host, inet_ntoa(addr.sin_addr));

    sockfd = create_raw_socket();

    while (running) {
        if (opts.count != -1 &&
            seq >= opts.count)
            break;

        struct timeval send_time;

        send_ping(sockfd, &addr, seq, &send_time);
        receive_ping(sockfd, seq, &send_time);
        seq++;
        usleep(opts.interval * 1000000);
    }
    
    print_statistics(opts.host);

    close(sockfd);

    return 0;
}