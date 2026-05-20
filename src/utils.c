#include "leping.h"

double time_diff(struct timeval start, struct timeval end) {

    // difference in seconds
    double sec = end.tv_sec - start.tv_sec;

    // difference in microseconds
    double usec = end.tv_usec - start.tv_usec;

    // convert result to milliseconds
    return sec * 1000.0 + usec / 1000.0;
}


void print_statistics(const char *host) {
    struct timeval end_time;
    gettimeofday(&end_time, NULL);

    double total_time = time_diff(stats.start_time, end_time);
    double avg = 0;
    double mdev = 0;

    if (stats.received > 0) {
        avg = stats.rtt_sum / stats.received;

        double mean_sq =
            stats.rtt_sum_sq / stats.received;

        mdev = sqrt(mean_sq - avg * avg);
    }

    int loss = 0;

    if (stats.transmitted > 0) {
        loss = (stats.transmitted - stats.received)
            * 100 / stats.transmitted;
    }

    printf("\n--- %s leping statistics ---\n", host);

    printf("%d packets transmitted, %d received, %d%% packet loss, time %.0fms\n",
        stats.transmitted,
        stats.received,
        loss,
        total_time
    );

    if (stats.received > 0) {
        printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
            stats.rtt_min,
            avg,
            stats.rtt_max,
            mdev
        );
    }
}