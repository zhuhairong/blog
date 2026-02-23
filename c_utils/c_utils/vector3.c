#include "vector3.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

vec3_error_t vec3_create(vec3_ctx_t** ctx, const vec3_config_t* config) {
    if (!ctx) return VEC3_INVALID_PARAMS;
    
    vec3_ctx_t* c = (vec3_ctx_t*)malloc(sizeof(vec3_ctx_t));
    if (!c) return VEC3_INVALID_PARAMS;
    
    if (config) {
        c->config = *config;
    } else {
        c->config.use_epsilon = true;
        c->config.epsilon = DBL_EPSILON * 10.0;
        c->config.check_div_zero = true;
        c->config.check_length_zero = true;
    }
    c->last_error = VEC3_OK;
    c->operation_count = 0;
    
    *ctx = c;
    return VEC3_OK;
}

void vec3_destroy(vec3_ctx_t* ctx) {
    if (ctx) free(ctx);
}

vec3_t vec3_add(vec3_t a, vec3_t b) {
    return (vec3_t){a.x + b.x, a.y + b.y, a.z + b.z};
}

vec3_t vec3_sub(vec3_t a, vec3_t b) {
    return (vec3_t){a.x - b.x, a.y - b.y, a.z - b.z};
}

double vec3_dot(vec3_t a, vec3_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
    return (vec3_t){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

double vec3_length(vec3_t v) {
    return sqrt(vec3_dot(v, v));
}

vec3_t vec3_normalize(vec3_t v) {
    double len = vec3_length(v);
    if (len < DBL_MIN) return (vec3_t){0, 0, 0};
    return (vec3_t){v.x / len, v.y / len, v.z / len};
}

vec3_t vec3_scale(vec3_t v, double scalar) {
    return (vec3_t){v.x * scalar, v.y * scalar, v.z * scalar};
}

vec3_error_t vec3_divide(vec3_ctx_t* ctx, vec3_t v, double scalar, vec3_t* out) {
    if (!out) return VEC3_INVALID_PARAMS;
    
    if (ctx && ctx->config.check_div_zero && fabs(scalar) < DBL_MIN) {
        ctx->last_error = VEC3_DIVISION_BY_ZERO;
        return VEC3_DIVISION_BY_ZERO;
    }
    
    if (fabs(scalar) < DBL_MIN) {
        return VEC3_DIVISION_BY_ZERO;
    }
    
    *out = (vec3_t){v.x / scalar, v.y / scalar, v.z / scalar};
    
    if (ctx) {
        ctx->last_error = VEC3_OK;
        ctx->operation_count++;
    }
    
    return VEC3_OK;
}

double vec3_distance(vec3_t a, vec3_t b) {
    return vec3_length(vec3_sub(a, b));
}

double vec3_distance_squared(vec3_t a, vec3_t b) {
    vec3_t diff = vec3_sub(a, b);
    return vec3_dot(diff, diff);
}

double vec3_angle(vec3_t a, vec3_t b) {
    double len_a = vec3_length(a);
    double len_b = vec3_length(b);
    
    if (len_a < DBL_MIN || len_b < DBL_MIN) return 0.0;
    
    double cos_angle = vec3_dot(a, b) / (len_a * len_b);
    
    if (cos_angle > 1.0) cos_angle = 1.0;
    if (cos_angle < -1.0) cos_angle = -1.0;
    
    return acos(cos_angle);
}

vec3_t vec3_lerp(vec3_t a, vec3_t b, double t) {
    return (vec3_t){
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t
    };
}

vec3_t vec3_slerp(vec3_t a, vec3_t b, double t) {
    double angle = vec3_angle(a, b);
    
    if (angle < DBL_MIN) {
        return vec3_lerp(a, b, t);
    }
    
    double sin_angle = sin(angle);
    double wa = sin((1.0 - t) * angle) / sin_angle;
    double wb = sin(t * angle) / sin_angle;
    
    return (vec3_t){
        a.x * wa + b.x * wb,
        a.y * wa + b.y * wb,
        a.z * wa + b.z * wb
    };
}

vec3_t vec3_reflect(vec3_t v, vec3_t normal) {
    vec3_t n = vec3_normalize(normal);
    double d = 2.0 * vec3_dot(v, n);
    return vec3_sub(v, vec3_scale(n, d));
}

vec3_error_t vec3_refract(vec3_ctx_t* ctx, vec3_t v, vec3_t normal, double eta, vec3_t* out) {
    if (!out) return VEC3_INVALID_PARAMS;
    
    vec3_t n = vec3_normalize(normal);
    vec3_t i = vec3_normalize(v);
    
    double cos_i = -vec3_dot(i, n);
    double sin_t2 = eta * eta * (1.0 - cos_i * cos_i);
    
    if (sin_t2 > 1.0) {
        if (ctx) ctx->last_error = VEC3_LENGTH_ZERO;
        return VEC3_LENGTH_ZERO;
    }
    
    double cos_t = sqrt(1.0 - sin_t2);
    vec3_t refracted = vec3_add(vec3_scale(i, eta), vec3_scale(n, eta * cos_i - cos_t));
    
    *out = refracted;
    
    if (ctx) {
        ctx->last_error = VEC3_OK;
        ctx->operation_count++;
    }
    
    return VEC3_OK;
}

bool vec3_equal(vec3_ctx_t* ctx, vec3_t a, vec3_t b) {
    if (ctx && ctx->config.use_epsilon) {
        double eps = ctx->config.epsilon;
        return fabs(a.x - b.x) < eps && fabs(a.y - b.y) < eps && fabs(a.z - b.z) < eps;
    }
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

vec3_error_t vec3_add_batch(vec3_ctx_t* ctx, const vec3_t* a, const vec3_t* b, vec3_t* out, size_t count) {
    if (!a || !b || !out) return VEC3_INVALID_PARAMS;
    
    for (size_t i = 0; i < count; i++) {
        out[i] = vec3_add(a[i], b[i]);
    }
    
    if (ctx) {
        ctx->last_error = VEC3_OK;
        ctx->operation_count += count;
    }
    
    return VEC3_OK;
}

vec3_error_t vec3_scale_batch(vec3_ctx_t* ctx, const vec3_t* v, double scalar, vec3_t* out, size_t count) {
    if (!v || !out) return VEC3_INVALID_PARAMS;
    
    for (size_t i = 0; i < count; i++) {
        out[i] = vec3_scale(v[i], scalar);
    }
    
    if (ctx) {
        ctx->last_error = VEC3_OK;
        ctx->operation_count += count;
    }
    
    return VEC3_OK;
}

vec3_t vec3_set(double x, double y, double z) {
    return (vec3_t){x, y, z};
}

vec3_t vec3_zero(void) {
    return (vec3_t){0, 0, 0};
}

vec3_t vec3_unit_x(void) {
    return (vec3_t){1, 0, 0};
}

vec3_t vec3_unit_y(void) {
    return (vec3_t){0, 1, 0};
}

vec3_t vec3_unit_z(void) {
    return (vec3_t){0, 0, 1};
}

vec3_error_t vec3_get_last_error(vec3_ctx_t* ctx) {
    return ctx ? ctx->last_error : VEC3_INVALID_PARAMS;
}

const char* vec3_strerror(vec3_error_t error) {
    switch (error) {
        case VEC3_OK: return "Success";
        case VEC3_INVALID_PARAMS: return "Invalid parameters";
        case VEC3_DIVISION_BY_ZERO: return "Division by zero";
        case VEC3_LENGTH_ZERO: return "Vector length is zero";
        case VEC3_OVERFLOW: return "Overflow";
        case VEC3_UNDERFLOW: return "Underflow";
        default: return "Unknown error";
    }
}
