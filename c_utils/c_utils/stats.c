#include "stats.h"
#include <math.h>
#include <float.h>

stats_t stats_compute(const double *data, size_t n) {
    stats_t s = {DBL_MAX, -DBL_MAX, 0, 0, 0};
    if (n == 0) return s;
    double sum = 0;
    for (size_t i = 0; i < n; i++) {
        if (data[i] < s.min) s.min = data[i];
        if (data[i] > s.max) s.max = data[i];
        sum += data[i];
    }
    s.mean = sum / n;
    double sum_sq_diff = 0;
    for (size_t i = 0; i < n; i++) {
        double diff = data[i] - s.mean;
        sum_sq_diff += diff * diff;
    }
    s.variance = sum_sq_diff / n;
    s.stddev = sqrt(s.variance);
    return s;
}
