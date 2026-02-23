#include "complex.h"
#include <math.h>
#include <float.h>

complex_t complex_add(complex_t a, complex_t b) {
    return (complex_t){a.real + b.real, a.imag + b.imag};
}

complex_t complex_sub(complex_t a, complex_t b) {
    return (complex_t){a.real - b.real, a.imag - b.imag};
}

complex_t complex_mul(complex_t a, complex_t b) {
    return (complex_t){
        a.real * b.real - a.imag * b.imag,
        a.real * b.imag + a.imag * b.real
    };
}

complex_t complex_div(complex_t a, complex_t b, complex_error_t *error) {
    double denom = b.real * b.real + b.imag * b.imag;
    if (denom < DBL_MIN) {
        if (error) *error = COMPLEX_ERROR_DIVISION_BY_ZERO;
        return (complex_t){0.0, 0.0};
    }
    if (error) *error = COMPLEX_OK;
    return (complex_t){
        (a.real * b.real + a.imag * b.imag) / denom,
        (a.imag * b.real - a.real * b.imag) / denom
    };
}

double complex_abs(complex_t a) {
    return sqrt(a.real * a.real + a.imag * a.imag);
}

complex_t complex_conj(complex_t a) {
    return (complex_t){a.real, -a.imag};
}

double complex_arg(complex_t a) {
    return atan2(a.imag, a.real);
}

complex_t complex_exp(complex_t a) {
    double r = exp(a.real);
    return (complex_t){r * cos(a.imag), r * sin(a.imag)};
}

complex_t complex_log(complex_t a, complex_error_t *error) {
    double abs_a = complex_abs(a);
    if (abs_a < DBL_MIN) {
        if (error) *error = COMPLEX_ERROR_DIVISION_BY_ZERO;
        return (complex_t){-INFINITY, 0.0};
    }
    if (error) *error = COMPLEX_OK;
    return (complex_t){log(abs_a), complex_arg(a)};
}

complex_t complex_pow(complex_t a, complex_t b, complex_error_t *error) {
    if (complex_abs(a) < DBL_MIN) {
        if (complex_abs(b) < DBL_MIN) {
            if (error) *error = COMPLEX_OK;
            return (complex_t){1.0, 0.0};
        }
        if (error) *error = COMPLEX_OK;
        return (complex_t){0.0, 0.0};
    }
    
    complex_t log_a = complex_log(a, error);
    if (error && *error != COMPLEX_OK) return (complex_t){0.0, 0.0};
    
    complex_t b_log_a = complex_mul(b, log_a);
    return complex_exp(b_log_a);
}

complex_t complex_sqrt(complex_t a) {
    double r = complex_abs(a);
    double theta = complex_arg(a);
    double sqrt_r = sqrt(r);
    return (complex_t){
        sqrt_r * cos(theta / 2.0),
        sqrt_r * sin(theta / 2.0)
    };
}

complex_t complex_sin(complex_t a) {
    return (complex_t){
        sin(a.real) * cosh(a.imag),
        cos(a.real) * sinh(a.imag)
    };
}

complex_t complex_cos(complex_t a) {
    return (complex_t){
        cos(a.real) * cosh(a.imag),
        -sin(a.real) * sinh(a.imag)
    };
}

complex_t complex_tan(complex_t a, complex_error_t *error) {
    complex_t s = complex_sin(a);
    complex_t c = complex_cos(a);
    return complex_div(s, c, error);
}

complex_t complex_polar(double r, double theta) {
    return (complex_t){r * cos(theta), r * sin(theta)};
}

bool complex_is_real(complex_t a) {
    return fabs(a.imag) < DBL_EPSILON;
}

bool complex_is_imag(complex_t a) {
    return fabs(a.real) < DBL_EPSILON && fabs(a.imag) >= DBL_EPSILON;
}

bool complex_equal(complex_t a, complex_t b, double epsilon) {
    if (epsilon <= 0.0) epsilon = DBL_EPSILON * 10.0;
    return fabs(a.real - b.real) < epsilon && fabs(a.imag - b.imag) < epsilon;
}

const char* complex_strerror(complex_error_t error) {
    switch (error) {
        case COMPLEX_OK: return "Success";
        case COMPLEX_ERROR_DIVISION_BY_ZERO: return "Division by zero";
        case COMPLEX_ERROR_INVALID_PARAM: return "Invalid parameter";
        case COMPLEX_ERROR_OVERFLOW: return "Overflow";
        default: return "Unknown error";
    }
}
