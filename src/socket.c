#include "leping.h"

int create_raw_socket() {
    int sockfd;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    return sockfd;
}

int resolve_host(const char *host, struct sockaddr_in *addr) {
    struct hostent *he;

    he = gethostbyname(host);

    if (!he)
    {
        fprintf(stderr, "Cannot resolve %s\n", host);
        exit(1);
    }

    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    memcpy(&addr->sin_addr, he->h_addr, he->h_length);

    return 0;
}