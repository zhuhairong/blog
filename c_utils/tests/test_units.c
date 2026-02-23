#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../c_utils/utest.h"
#include "../c_utils/units.h"

void test_units_celsius_fahrenheit() {
    TEST(Units_CelsiusFahrenheit);
    double f = units_celsius_to_fahrenheit(0);
    EXPECT_TRUE(fabs(f - 32.0) < 0.001);
    
    f = units_celsius_to_fahrenheit(100);
    EXPECT_TRUE(fabs(f - 212.0) < 0.001);
    
    double c = units_fahrenheit_to_celsius(32);
    EXPECT_TRUE(fabs(c - 0.0) < 0.001);
    
    c = units_fahrenheit_to_celsius(212);
    EXPECT_TRUE(fabs(c - 100.0) < 0.001);
}

void test_units_celsius_kelvin() {
    TEST(Units_CelsiusKelvin);
    double k = units_celsius_to_kelvin(0);
    EXPECT_TRUE(fabs(k - 273.15) < 0.001);
    
    double c = units_kelvin_to_celsius(273.15);
    EXPECT_TRUE(fabs(c - 0.0) < 0.001);
}

void test_units_km_miles() {
    TEST(Units_KmMiles);
    double miles = units_km_to_miles(1);
    EXPECT_TRUE(fabs(miles - 0.621371) < 0.001);
    
    double km = units_miles_to_km(1);
    EXPECT_TRUE(fabs(km - 1.60934) < 0.001);
}

void test_units_meters_feet() {
    TEST(Units_MetersFeet);
    double feet = units_meters_to_feet(1);
    EXPECT_TRUE(fabs(feet - 3.28084) < 0.001);
    
    double meters = units_feet_to_meters(1);
    EXPECT_TRUE(fabs(meters - 0.3048) < 0.001);
}

void test_units_create_destroy() {
    TEST(Units_CreateDestroy);
    units_ctx_t* ctx = NULL;
    units_error_t error = units_create(&ctx, NULL);
    
    EXPECT_EQ(error, UNITS_OK);
    EXPECT_TRUE(ctx != NULL);
    
    units_destroy(ctx);
}

void test_units_convert_temperature() {
    TEST(Units_ConvertTemperature);
    units_ctx_t* ctx = NULL;
    units_create(&ctx, NULL);
    
    double result;
    units_error_t error = units_convert_temperature(ctx, 0, UNITS_TEMP_CELSIUS, UNITS_TEMP_FAHRENHEIT, &result);
    EXPECT_EQ(error, UNITS_OK);
    EXPECT_TRUE(fabs(result - 32.0) < 0.001);
    
    units_destroy(ctx);
}

void test_units_convert_distance() {
    TEST(Units_ConvertDistance);
    units_ctx_t* ctx = NULL;
    units_create(&ctx, NULL);
    
    double result;
    units_error_t error = units_convert_distance(ctx, 1, UNITS_DIST_KILOMETER, UNITS_DIST_MILE, &result);
    EXPECT_EQ(error, UNITS_OK);
    EXPECT_TRUE(fabs(result - 0.621371) < 0.001);
    
    units_destroy(ctx);
}

void test_units_convert_weight() {
    TEST(Units_ConvertWeight);
    units_ctx_t* ctx = NULL;
    units_create(&ctx, NULL);
    
    double result;
    units_error_t error = units_convert_weight(ctx, 1, UNITS_WEIGHT_KILOGRAM, UNITS_WEIGHT_POUND, &result);
    EXPECT_EQ(error, UNITS_OK);
    EXPECT_TRUE(fabs(result - 2.20462) < 0.001);
    
    units_destroy(ctx);
}

void test_units_convert_time() {
    TEST(Units_ConvertTime);
    units_ctx_t* ctx = NULL;
    units_create(&ctx, NULL);
    
    double result;
    units_error_t error = units_convert_time(ctx, 1, UNITS_TIME_HOUR, UNITS_TIME_MINUTE, &result);
    EXPECT_EQ(error, UNITS_OK);
    EXPECT_TRUE(fabs(result - 60.0) < 0.001);
    
    units_destroy(ctx);
}

void test_units_strerror() {
    TEST(Units_Strerror);
    const char* msg = units_strerror(UNITS_OK);
    EXPECT_TRUE(msg != NULL);
    
    msg = units_strerror(UNITS_INVALID_PARAMS);
    EXPECT_TRUE(msg != NULL);
}

int main() {
    test_units_celsius_fahrenheit();
    test_units_celsius_kelvin();
    test_units_km_miles();
    test_units_meters_feet();
    test_units_create_destroy();
    test_units_convert_temperature();
    test_units_convert_distance();
    test_units_convert_weight();
    test_units_convert_time();
    test_units_strerror();

    return 0;
}
