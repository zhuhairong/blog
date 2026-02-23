#include "quaternion.h"
#include <math.h>

quat_t quat_add(quat_t a, quat_t b) {
    return (quat_t){a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z};
}

quat_t quat_mul(quat_t a, quat_t b) {
    return (quat_t){
        a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z,
        a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,
        a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,
        a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w
    };
}

quat_t quat_normalize(quat_t q) {
    double mag = sqrt(q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z);
    if (mag < 1e-9) return (quat_t){1, 0, 0, 0};
    return (quat_t){q.w/mag, q.x/mag, q.y/mag, q.z/mag};
}
