#include "leping.h"

int create_raw_socket(int family) {
    int protocol;

    if (family == AF_INET6) {
        protocol = IPPROTO_ICMPV6;
    } else {
        protocol = IPPROTO_ICMP;
    }

    int sockfd = socket(family, SOCK_RAW, protocol);

    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    return sockfd;
}

int resolve_host(const char *host, struct sockaddr_storage *addr, socklen_t *addr_len) {
    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_RAW;

    int status = getaddrinfo(host, NULL, &hints, &result);

    if (status != 0) {
        fprintf(stderr, "Cannot resolve %s: %s\n", host, gai_strerror(status));

        return -1;
    }

    memcpy(addr, result->ai_addr, result->ai_addrlen);
    *addr_len = result->ai_addrlen;

    freeaddrinfo(result);

    return 0;
}

const char *reverse_dns(struct sockaddr *addr, socklen_t addr_len) {
    static char host[NI_MAXHOST];

    int status = getnameinfo(addr, addr_len, host, sizeof(host), NULL, 0, NI_NAMEREQD);

    if (status != 0) {
        return "unknown";
    }

    return host;
}