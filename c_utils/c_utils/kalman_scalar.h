#ifndef C_UTILS_KALMAN_SCALAR_H
#define C_UTILS_KALMAN_SCALAR_H

#include <stdbool.h>

// 卡尔曼滤波器错误码
typedef enum {
    KALMAN_SCALAR_OK = 0,
    KALMAN_SCALAR_INVALID_INPUT = -1,
    KALMAN_SCALAR_INVALID_COVARIANCE = -2,
    KALMAN_SCALAR_NAN_ERROR = -3,
    KALMAN_SCALAR_INFINITY_ERROR = -4,
    KALMAN_SCALAR_DIVERGED = -5
} kalman_scalar_error_t;

// 卡尔曼滤波器配置
typedef struct {
    double initial_value;
    double initial_P;
    double Q;
    double R;
    bool enable_validation;
    double min_covariance;
    double max_covariance;
    bool enable_adaptive_noise;
    double noise_adaptation_factor;
} kalman_scalar_config_t;

typedef struct {
    double x; // State estimate
    double P; // Estimation covariance
    double Q; // Process noise covariance
    double R; // Measurement noise covariance
    double K; // Kalman gain
    bool has_error;
    kalman_scalar_error_t error;
    char error_msg[256];
    bool is_adaptive;
    double last_measurement;
    double measurement_variance;
} kalman_scalar_t;

/**
 * Initialize the scalar Kalman filter.
 * 
 * @param kf Pointer to the filter structure.
 * @param initial_value Initial estimate of the state.
 * @param initial_P Initial estimation error covariance.
 * @param Q Process noise covariance (trust in the model).
 * @param R Measurement noise covariance (trust in the measurement).
 * @return Error code.
 */
kalman_scalar_error_t kalman_scalar_init(kalman_scalar_t *kf, double initial_value, double initial_P, double Q, double R);

/**
 * Initialize the scalar Kalman filter with configuration.
 * 
 * @param kf Pointer to the filter structure.
 * @param config Configuration parameters.
 * @return Error code.
 */
kalman_scalar_error_t kalman_scalar_init_with_config(kalman_scalar_t *kf, const kalman_scalar_config_t *config);

/**
 * Update the filter with a new measurement.
 * 
 * @param kf Pointer to the filter structure.
 * @param measurement The new measurement value.
 * @return The updated state estimate.
 */
double kalman_scalar_update(kalman_scalar_t *kf, double measurement);

/**
 * Predict the next state without measurement.
 * 
 * @param kf Pointer to the filter structure.
 * @return The predicted state estimate.
 */
double kalman_scalar_predict(kalman_scalar_t *kf);

/**
 * Reset the filter to initial state.
 * 
 * @param kf Pointer to the filter structure.
 * @param initial_value Initial estimate of the state.
 * @return Error code.
 */
kalman_scalar_error_t kalman_scalar_reset(kalman_scalar_t *kf, double initial_value);

/**
 * Update the process noise covariance.
 * 
 * @param kf Pointer to the filter structure.
 * @param Q New process noise covariance.
 * @return Error code.
 */
kalman_scalar_error_t kalman_scalar_set_Q(kalman_scalar_t *kf, double Q);

/**
 * Update the measurement noise covariance.
 * 
 * @param kf Pointer to the filter structure.
 * @param R New measurement noise covariance.
 * @return Error code.
 */
kalman_scalar_error_t kalman_scalar_set_R(kalman_scalar_t *kf, double R);

/**
 * Get the current estimation covariance.
 * 
 * @param kf Pointer to the filter structure.
 * @return Estimation covariance.
 */
double kalman_scalar_get_P(const kalman_scalar_t *kf);

/**
 * Get the current Kalman gain.
 * 
 * @param kf Pointer to the filter structure.
 * @return Kalman gain.
 */
double kalman_scalar_get_K(const kalman_scalar_t *kf);

/**
 * Check if the filter is diverged.
 * 
 * @param kf Pointer to the filter structure.
 * @return True if diverged, false otherwise.
 */
bool kalman_scalar_is_diverged(const kalman_scalar_t *kf);

/**
 * Get default configuration.
 * 
 * @param config Pointer to configuration structure.
 */
void kalman_scalar_get_default_config(kalman_scalar_config_t *config);

#endif // C_UTILS_KALMAN_SCALAR_H
