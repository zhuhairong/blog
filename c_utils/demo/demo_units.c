/**
 * 单位转换演示程序
 *
 * 功能：
 * - 温度转换
 * - 距离转换
 * - 重量转换
 * - 时间转换
 * - 体积转换
 * - 面积转换
 * - 速度转换
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../c_utils/units.h"

// 演示 1: 温度转换
static void demo_temperature_conversion(void) {
    printf("\n=== 演示 1: 温度转换 ===\n");

    units_ctx_t *ctx = NULL;
    units_config_t config = {2, true, -273.15, 1000.0};
    units_create(&ctx, &config);

    double value = 25.0; // 25摄氏度
    double result;

    // 摄氏度转华氏度
    units_error_t error = units_convert_temperature(ctx, value, UNITS_TEMP_CELSIUS, UNITS_TEMP_FAHRENHEIT, &result);
    if (error == UNITS_OK) {
        printf("%.2f 摄氏度 = %.2f 华氏度\n", value, result);
    }

    // 摄氏度转开尔文
    error = units_convert_temperature(ctx, value, UNITS_TEMP_CELSIUS, UNITS_TEMP_KELVIN, &result);
    if (error == UNITS_OK) {
        printf("%.2f 摄氏度 = %.2f 开尔文\n", value, result);
    }

    // 华氏度转摄氏度
    value = 77.0; // 77华氏度
    error = units_convert_temperature(ctx, value, UNITS_TEMP_FAHRENHEIT, UNITS_TEMP_CELSIUS, &result);
    if (error == UNITS_OK) {
        printf("%.2f 华氏度 = %.2f 摄氏度\n", value, result);
    }

    units_destroy(ctx);
}

// 演示 2: 距离转换
static void demo_distance_conversion(void) {
    printf("\n=== 演示 2: 距离转换 ===\n");

    units_ctx_t *ctx = NULL;
    units_config_t config = {2, true, 0.0, 1000000.0};
    units_create(&ctx, &config);

    double value = 10.0; // 10公里
    double result;

    // 公里转英里
    units_error_t error = units_convert_distance(ctx, value, UNITS_DIST_KILOMETER, UNITS_DIST_MILE, &result);
    if (error == UNITS_OK) {
        printf("%.2f 公里 = %.2f 英里\n", value, result);
    }

    // 米转英尺
    value = 100.0; // 100米
    error = units_convert_distance(ctx, value, UNITS_DIST_METER, UNITS_DIST_FOOT, &result);
    if (error == UNITS_OK) {
        printf("%.2f 米 = %.2f 英尺\n", value, result);
    }

    // 英寸转厘米
    value = 12.0; // 12英寸
    error = units_convert_distance(ctx, value, UNITS_DIST_INCH, UNITS_DIST_METER, &result);
    if (error == UNITS_OK) {
        printf("%.2f 英寸 = %.2f 米\n", value, result * 100);
    }

    units_destroy(ctx);
}

// 演示 3: 重量转换
static void demo_weight_conversion(void) {
    printf("\n=== 演示 3: 重量转换 ===\n");

    units_ctx_t *ctx = NULL;
    units_config_t config = {2, true, 0.0, 10000.0};
    units_create(&ctx, &config);

    double value = 5.0; // 5公斤
    double result;

    // 公斤转磅
    units_error_t error = units_convert_weight(ctx, value, UNITS_WEIGHT_KILOGRAM, UNITS_WEIGHT_POUND, &result);
    if (error == UNITS_OK) {
        printf("%.2f 公斤 = %.2f 磅\n", value, result);
    }

    // 克转盎司
    value = 100.0; // 100克
    error = units_convert_weight(ctx, value, UNITS_WEIGHT_GRAM, UNITS_WEIGHT_OUNCE, &result);
    if (error == UNITS_OK) {
        printf("%.2f 克 = %.2f 盎司\n", value, result);
    }

    units_destroy(ctx);
}

// 演示 4: 时间转换
static void demo_time_conversion(void) {
    printf("\n=== 演示 4: 时间转换 ===\n");

    units_ctx_t *ctx = NULL;
    units_config_t config = {2, true, 0.0, 1000000.0};
    units_create(&ctx, &config);

    double value = 2.5; // 2.5小时
    double result;

    // 小时转分钟
    units_error_t error = units_convert_time(ctx, value, UNITS_TIME_HOUR, UNITS_TIME_MINUTE, &result);
    if (error == UNITS_OK) {
        printf("%.2f 小时 = %.2f 分钟\n", value, result);
    }

    // 天转秒
    value = 1.0; // 1天
    error = units_convert_time(ctx, value, UNITS_TIME_DAY, UNITS_TIME_SECOND, &result);
    if (error == UNITS_OK) {
        printf("%.2f 天 = %.2f 秒\n", value, result);
    }

    // 周转小时
    value = 1.0; // 1周
    error = units_convert_time(ctx, value, UNITS_TIME_WEEK, UNITS_TIME_HOUR, &result);
    if (error == UNITS_OK) {
        printf("%.2f 周 = %.2f 小时\n", value, result);
    }

    units_destroy(ctx);
}

// 演示 5: 体积转换
static void demo_volume_conversion(void) {
    printf("\n=== 演示 5: 体积转换 ===\n");

    units_ctx_t *ctx = NULL;
    units_config_t config = {2, true, 0.0, 10000.0};
    units_create(&ctx, &config);

    double value = 1.0; // 1升
    double result;

    // 升转加仑(美国)
    units_error_t error = units_convert_volume(ctx, value, UNITS_VOL_LITER, UNITS_VOL_GALLON_US, &result);
    if (error == UNITS_OK) {
        printf("%.2f 升 = %.2f 美国加仑\n", value, result);
    }

    // 毫升转品脱
    value = 500.0; // 500毫升
    error = units_convert_volume(ctx, value, UNITS_VOL_MILLILITER, UNITS_VOL_PINT, &result);
    if (error == UNITS_OK) {
        printf("%.2f 毫升 = %.2f 品脱\n", value, result);
    }

    units_destroy(ctx);
}

// 演示 6: 面积转换
static void demo_area_conversion(void) {
    printf("\n=== 演示 6: 面积转换 ===\n");

    units_ctx_t *ctx = NULL;
    units_config_t config = {2, true, 0.0, 1000000.0};
    units_create(&ctx, &config);

    double value = 1000.0; // 1000平方米
    double result;

    // 平方米转公顷
    units_error_t error = units_convert_area(ctx, value, UNITS_AREA_SQUARE_METER, UNITS_AREA_HECTARE, &result);
    if (error == UNITS_OK) {
        printf("%.2f 平方米 = %.2f 公顷\n", value, result);
    }

    // 平方公里转英亩
    value = 1.0; // 1平方公里
    error = units_convert_area(ctx, value, UNITS_AREA_SQUARE_KILOMETER, UNITS_AREA_ACRE, &result);
    if (error == UNITS_OK) {
        printf("%.2f 平方公里 = %.2f 英亩\n", value, result);
    }

    units_destroy(ctx);
}

// 演示 7: 速度转换
static void demo_speed_conversion(void) {
    printf("\n=== 演示 7: 速度转换 ===\n");

    units_ctx_t *ctx = NULL;
    units_config_t config = {2, true, 0.0, 1000.0};
    units_create(&ctx, &config);

    double value = 100.0; // 100公里/小时
    double result;

    // 公里/小时转米/秒
    units_error_t error = units_convert_speed(ctx, value, UNITS_SPEED_KMH, UNITS_SPEED_MPS, &result);
    if (error == UNITS_OK) {
        printf("%.2f 公里/小时 = %.2f 米/秒\n", value, result);
    }

    // 英里/小时转节
    value = 60.0; // 60英里/小时
    error = units_convert_speed(ctx, value, UNITS_SPEED_MPH, UNITS_SPEED_KNOT, &result);
    if (error == UNITS_OK) {
        printf("%.2f 英里/小时 = %.2f 节\n", value, result);
    }

    units_destroy(ctx);
}

// 演示 8: 传统转换函数
static void demo_traditional_functions(void) {
    printf("\n=== 演示 8: 传统转换函数 ===\n");

    // 温度转换
    double celsius = 25.0;
    double fahrenheit = units_celsius_to_fahrenheit(celsius);
    printf("%.2f 摄氏度 = %.2f 华氏度 (传统函数)\n", celsius, fahrenheit);

    // 距离转换
    double km = 10.0;
    double miles = units_km_to_miles(km);
    printf("%.2f 公里 = %.2f 英里 (传统函数)\n", km, miles);

    double meters = 100.0;
    double feet = units_meters_to_feet(meters);
    printf("%.2f 米 = %.2f 英尺 (传统函数)\n", meters, feet);
}

// 演示 9: 错误处理
static void demo_error_handling(void) {
    printf("\n=== 演示 9: 错误处理 ===\n");

    units_ctx_t *ctx = NULL;
    units_config_t config = {2, true, -273.15, 1000.0};
    units_create(&ctx, &config);

    double value = -300.0; // 低于绝对零度
    double result;

    // 测试温度范围错误
    units_error_t error = units_convert_temperature(ctx, value, UNITS_TEMP_CELSIUS, UNITS_TEMP_FAHRENHEIT, &result);
    if (error != UNITS_OK) {
        printf("温度转换错误: %s\n", units_strerror(error));
    }

    // 测试无效单位
    error = units_convert_distance(ctx, 10.0, (units_dist_unit_t)999, UNITS_DIST_MILE, &result);
    if (error != UNITS_OK) {
        printf("距离转换错误: %s\n", units_strerror(error));
    }

    units_destroy(ctx);
}

// 演示 10: 配置管理
static void demo_config_management(void) {
    printf("\n=== 演示 10: 配置管理 ===\n");

    // 测试不同精度
    units_ctx_t *ctx1 = NULL;
    units_config_t config1 = {0, true, 0.0, 1000.0}; // 0位小数
    units_create(&ctx1, &config1);

    units_ctx_t *ctx2 = NULL;
    units_config_t config2 = {4, true, 0.0, 1000.0}; // 4位小数
    units_create(&ctx2, &config2);

    double value = 10.0;
    double result1, result2;

    units_convert_distance(ctx1, value, UNITS_DIST_KILOMETER, UNITS_DIST_MILE, &result1);
    units_convert_distance(ctx2, value, UNITS_DIST_KILOMETER, UNITS_DIST_MILE, &result2);

    printf("10公里转英里 (0位小数): %.0f\n", result1);
    printf("10公里转英里 (4位小数): %.4f\n", result2);

    units_destroy(ctx1);
    units_destroy(ctx2);
}

int main(void) {
    printf("========================================\n");
    printf("    单位转换演示\n");
    printf("========================================\n");

    demo_temperature_conversion();
    demo_distance_conversion();
    demo_weight_conversion();
    demo_time_conversion();
    demo_volume_conversion();
    demo_area_conversion();
    demo_speed_conversion();
    demo_traditional_functions();
    demo_error_handling();
    demo_config_management();

    printf("\n========================================\n");
    printf("演示完成!\n");

    return 0;
}
