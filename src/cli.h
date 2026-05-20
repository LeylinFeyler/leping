#ifndef CLI_H
#define CLI_H

typedef struct {
    char *host;

    int count;
    double interval;
    int packet_size;
    int timeout;

} PingOptions;

int parse_args(
    int argc,
    char **argv,
    PingOptions *opts
);

void print_help(
    const char *prog
);

#endif