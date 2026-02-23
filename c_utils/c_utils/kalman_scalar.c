#include "kalman_scalar.h"
#include <math.h>
#include <string.h>
#include <stdbool.h>

static bool is_valid_covariance(double value) {
    return !isnan(value) && !isinf(value) && value >= 0.0;
}

static bool is_valid_value(double value) {
    return !isnan(value) && !isinf(value);
}

kalman_scalar_error_t kalman_scalar_init(kalman_scalar_t *kf, double initial_value, double initial_P, double Q, double R) {
    if (!kf) {
        return KALMAN_SCALAR_INVALID_INPUT;
    }

    if (!is_valid_value(initial_value)) {
        return KALMAN_SCALAR_INVALID_INPUT;
    }

    if (!is_valid_covariance(initial_P) || !is_valid_covariance(Q) || !is_valid_covariance(R)) {
        return KALMAN_SCALAR_INVALID_COVARIANCE;
    }

    kf->x = initial_value;
    kf->P = initial_P;
    kf->Q = Q;
    kf->R = R;
    kf->K = 0.0;
    kf->has_error = false;
    kf->error = KALMAN_SCALAR_OK;
    kf->is_adaptive = false;
    kf->last_measurement = initial_value;
    kf->measurement_variance = R;
    memset(kf->error_msg, 0, sizeof(kf->error_msg));

    return KALMAN_SCALAR_OK;
}

kalman_scalar_error_t kalman_scalar_init_with_config(kalman_scalar_t *kf, const kalman_scalar_config_t *config) {
    if (!kf || !config) {
        return KALMAN_SCALAR_INVALID_INPUT;
    }

    if (!is_valid_value(config->initial_value)) {
        return KALMAN_SCALAR_INVALID_INPUT;
    }

    if (!is_valid_covariance(config->initial_P) || !is_valid_covariance(config->Q) || !is_valid_covariance(config->R)) {
        return KALMAN_SCALAR_INVALID_COVARIANCE;
    }

    kf->x = config->initial_value;
    kf->P = config->initial_P;
    kf->Q = config->Q;
    kf->R = config->R;
    kf->K = 0.0;
    kf->has_error = false;
    kf->error = KALMAN_SCALAR_OK;
    kf->is_adaptive = config->enable_adaptive_noise;
    kf->last_measurement = config->initial_value;
    kf->measurement_variance = config->R;
    memset(kf->error_msg, 0, sizeof(kf->error_msg));

    return KALMAN_SCALAR_OK;
}

double kalman_scalar_update(kalman_scalar_t *kf, double measurement) {
    if (!kf) {
        return 0.0;
    }

    if (!is_valid_value(measurement)) {
        kf->has_error = true;
        kf->error = KALMAN_SCALAR_INVALID_INPUT;
        snprintf(kf->error_msg, sizeof(kf->error_msg), "Invalid measurement value");
        return kf->x;
    }

    // Prediction update
    double x_pred = kf->x;
    double P_pred = kf->P + kf->Q;

    if (!is_valid_value(P_pred)) {
        kf->has_error = true;
        kf->error = KALMAN_SCALAR_NAN_ERROR;
        snprintf(kf->error_msg, sizeof(kf->error_msg), "Prediction covariance error");
        return kf->x;
    }

    // Measurement update
    kf->K = P_pred / (P_pred + kf->R);
    kf->x = x_pred + kf->K * (measurement - x_pred);
    kf->P = (1.0 - kf->K) * P_pred;

    if (!is_valid_value(kf->x) || !is_valid_value(kf->P)) {
        kf->has_error = true;
        kf->error = KALMAN_SCALAR_NAN_ERROR;
        snprintf(kf->error_msg, sizeof(kf->error_msg), "Filter update error");
        return x_pred;
    }

    // Adaptive noise adjustment
    if (kf->is_adaptive) {
        double measurement_error = fabs(measurement - x_pred);
        kf->measurement_variance = 0.9 * kf->measurement_variance + 0.1 * measurement_error * measurement_error;
        kf->R = 0.9 * kf->R + 0.1 * kf->measurement_variance;
    }

    // Check for divergence
    if (kf->P > 1e10) {
        kf->has_error = true;
        kf->error = KALMAN_SCALAR_DIVERGED;
        snprintf(kf->error_msg, sizeof(kf->error_msg), "Filter diverged");
    }

    kf->last_measurement = measurement;
    return kf->x;
}

double kalman_scalar_predict(kalman_scalar_t *kf) {
    if (!kf) {
        return 0.0;
    }

    // For a static system (A=1, B=0), x_pred = x_prev
    // P_pred = P_prev + Q
    double P_pred = kf->P + kf->Q;

    if (!is_valid_value(P_pred)) {
        kf->has_error = true;
        kf->error = KALMAN_SCALAR_NAN_ERROR;
        snprintf(kf->error_msg, sizeof(kf->error_msg), "Prediction error");
        return kf->x;
    }

    return kf->x;
}

kalman_scalar_error_t kalman_scalar_reset(kalman_scalar_t *kf, double initial_value) {
    if (!kf) {
        return KALMAN_SCALAR_INVALID_INPUT;
    }

    if (!is_valid_value(initial_value)) {
        return KALMAN_SCALAR_INVALID_INPUT;
    }

    kf->x = initial_value;
    kf->K = 0.0;
    kf->has_error = false;
    kf->error = KALMAN_SCALAR_OK;
    kf->last_measurement = initial_value;
    memset(kf->error_msg, 0, sizeof(kf->error_msg));

    return KALMAN_SCALAR_OK;
}

kalman_scalar_error_t kalman_scalar_set_Q(kalman_scalar_t *kf, double Q) {
    if (!kf) {
        return KALMAN_SCALAR_INVALID_INPUT;
    }

    if (!is_valid_covariance(Q)) {
        return KALMAN_SCALAR_INVALID_COVARIANCE;
    }

    kf->Q = Q;
    return KALMAN_SCALAR_OK;
}

kalman_scalar_error_t kalman_scalar_set_R(kalman_scalar_t *kf, double R) {
    if (!kf) {
        return KALMAN_SCALAR_INVALID_INPUT;
    }

    if (!is_valid_covariance(R)) {
        return KALMAN_SCALAR_INVALID_COVARIANCE;
    }

    kf->R = R;
    kf->measurement_variance = R;
    return KALMAN_SCALAR_OK;
}

double kalman_scalar_get_P(const kalman_scalar_t *kf) {
    return kf ? kf->P : 0.0;
}

double kalman_scalar_get_K(const kalman_scalar_t *kf) {
    return kf ? kf->K : 0.0;
}

bool kalman_scalar_is_diverged(const kalman_scalar_t *kf) {
    return kf && kf->error == KALMAN_SCALAR_DIVERGED;
}

void kalman_scalar_get_default_config(kalman_scalar_config_t *config) {
    if (config) {
        config->initial_value = 0.0;
        config->initial_P = 1.0;
        config->Q = 0.01;
        config->R = 0.1;
        config->enable_validation = true;
        config->min_covariance = 1e-10;
        config->max_covariance = 1e10;
        config->enable_adaptive_noise = false;
        config->noise_adaptation_factor = 0.1;
    }
}
