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
    int sockfd;
    struct sockaddr_in addr;
    int seq = 0;

    if (argc != 2) {
        printf("Usage: %s <host>\n", argv[0]);
        return 1;
    }

    signal(SIGINT, handle_sigint);
    resolve_host(argv[1], &addr);
    gettimeofday(&stats.start_time, NULL);
    printf("PING %s (%s)\n", argv[1], inet_ntoa(addr.sin_addr));
    sockfd = create_raw_socket();

    while (running)
    {
        struct timeval send_time;
        send_ping(sockfd, &addr, seq, &send_time);
        receive_ping(sockfd, seq, &send_time);
        seq++;
        sleep(1);
    }

    print_statistics(argv[1]);
    close(sockfd);

    return 0;
}