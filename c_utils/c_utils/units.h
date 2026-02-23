#ifndef C_UTILS_UNITS_H
#define C_UTILS_UNITS_H

#include <stddef.h>
#include <stdbool.h>

// 单位转换错误码
typedef enum {
    UNITS_OK = 0,
    UNITS_INVALID_PARAMS = -1,
    UNITS_INVALID_UNIT = -2,
    UNITS_OUT_OF_RANGE = -3,
    UNITS_CONVERSION_ERROR = -4
} units_error_t;

// 单位转换配置选项
typedef struct {
    int precision;         // 精度（小数位数）
    bool validate_range;   // 是否验证输入范围
    double min_value;      // 最小值
    double max_value;      // 最大值
} units_config_t;

// 温度单位
typedef enum {
    UNITS_TEMP_CELSIUS,
    UNITS_TEMP_FAHRENHEIT,
    UNITS_TEMP_KELVIN,
    UNITS_TEMP_RANKINE
} units_temp_unit_t;

// 距离单位
typedef enum {
    UNITS_DIST_KILOMETER,
    UNITS_DIST_MILE,
    UNITS_DIST_METER,
    UNITS_DIST_FOOT,
    UNITS_DIST_INCH,
    UNITS_DIST_YARD
} units_dist_unit_t;

// 重量单位
typedef enum {
    UNITS_WEIGHT_KILOGRAM,
    UNITS_WEIGHT_POUND,
    UNITS_WEIGHT_GRAM,
    UNITS_WEIGHT_OUNCE
} units_weight_unit_t;

// 时间单位
typedef enum {
    UNITS_TIME_SECOND,
    UNITS_TIME_MINUTE,
    UNITS_TIME_HOUR,
    UNITS_TIME_DAY,
    UNITS_TIME_WEEK
} units_time_unit_t;

// 体积单位
typedef enum {
    UNITS_VOL_MILLILITER,
    UNITS_VOL_LITER,
    UNITS_VOL_GALLON_US,
    UNITS_VOL_GALLON_IMPERIAL,
    UNITS_VOL_QUART,
    UNITS_VOL_PINT
} units_vol_unit_t;

// 面积单位
typedef enum {
    UNITS_AREA_SQUARE_METER,
    UNITS_AREA_SQUARE_KILOMETER,
    UNITS_AREA_SQUARE_MILE,
    UNITS_AREA_ACRE,
    UNITS_AREA_HECTARE
} units_area_unit_t;

// 速度单位
typedef enum {
    UNITS_SPEED_MPS,       // 米/秒
    UNITS_SPEED_KMH,       // 公里/小时
    UNITS_SPEED_MPH,       // 英里/小时
    UNITS_SPEED_KNOT       // 节
} units_speed_unit_t;

// 单位转换上下文
typedef struct {
    units_config_t config;
    units_error_t last_error;
} units_ctx_t;

// 创建单位转换上下文
units_error_t units_create(units_ctx_t** ctx, const units_config_t* config);

// 销毁单位转换上下文
void units_destroy(units_ctx_t* ctx);

// 温度转换
// 返回 UNITS_OK 表示成功，其他值表示错误
units_error_t units_convert_temperature(units_ctx_t* ctx, double value, units_temp_unit_t from_unit, units_temp_unit_t to_unit, double* result);

// 距离转换
// 返回 UNITS_OK 表示成功，其他值表示错误
units_error_t units_convert_distance(units_ctx_t* ctx, double value, units_dist_unit_t from_unit, units_dist_unit_t to_unit, double* result);

// 重量转换
// 返回 UNITS_OK 表示成功，其他值表示错误
units_error_t units_convert_weight(units_ctx_t* ctx, double value, units_weight_unit_t from_unit, units_weight_unit_t to_unit, double* result);

// 时间转换
// 返回 UNITS_OK 表示成功，其他值表示错误
units_error_t units_convert_time(units_ctx_t* ctx, double value, units_time_unit_t from_unit, units_time_unit_t to_unit, double* result);

// 体积转换
// 返回 UNITS_OK 表示成功，其他值表示错误
units_error_t units_convert_volume(units_ctx_t* ctx, double value, units_vol_unit_t from_unit, units_vol_unit_t to_unit, double* result);

// 面积转换
// 返回 UNITS_OK 表示成功，其他值表示错误
units_error_t units_convert_area(units_ctx_t* ctx, double value, units_area_unit_t from_unit, units_area_unit_t to_unit, double* result);

// 速度转换
// 返回 UNITS_OK 表示成功，其他值表示错误
units_error_t units_convert_speed(units_ctx_t* ctx, double value, units_speed_unit_t from_unit, units_speed_unit_t to_unit, double* result);

// 传统温度转换函数（向后兼容）
double units_celsius_to_fahrenheit(double c);
double units_fahrenheit_to_celsius(double f);
double units_celsius_to_kelvin(double c);
double units_kelvin_to_celsius(double k);

// 传统距离转换函数（向后兼容）
double units_km_to_miles(double km);
double units_miles_to_km(double m);
double units_meters_to_feet(double meters);
double units_feet_to_meters(double feet);

// 获取最后一次错误
units_error_t units_get_last_error(units_ctx_t* ctx);

// 获取错误信息
const char* units_strerror(units_error_t error);

#endif // C_UTILS_UNITS_H
