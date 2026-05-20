#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cli.h"

static void init_options(PingOptions *opts) {
    memset(opts, 0, sizeof(*opts));

    opts->count       = -1;
    opts->interval    = 1.0;
    opts->packet_size = 56;
    opts->timeout     = 1;
}

int parse_args(int argc, char **argv, PingOptions *opts) {
    init_options(opts);

    int opt;

    while ((opt = getopt(argc, argv, "c:i:s:t:h")) != -1) {
        switch (opt) {

        case 'c':
            opts->count = atoi(optarg);
            break;

        case 'i':
            opts->interval = atof(optarg);
            break;

        case 's':
            opts->packet_size = atoi(optarg);
            break;

        case 't':
            opts->timeout = atoi(optarg);
            break;

        case 'h':
            print_help(argv[0]);
            exit(0);

        default:
            return -1;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Host required\n");
        return -1;
    }

    opts->host = argv[optind];

    return 0;
}

void print_help(const char *prog) {
    printf("Usage: %s [options] <host>\n", prog);
    printf("Options:\n");
    printf("  -c <count>      number of packets\n");
    printf("  -i <interval>   delay between packets\n");
    printf("  -s <size>       packet size\n");
    printf("  -t <timeout>    receive timeout\n");
    printf("  -h              help\n");
}