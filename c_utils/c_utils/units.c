#include "units.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

units_error_t units_create(units_ctx_t** ctx, const units_config_t* config) {
    if (!ctx) return UNITS_INVALID_PARAMS;
    
    units_ctx_t* c = (units_ctx_t*)malloc(sizeof(units_ctx_t));
    if (!c) return UNITS_CONVERSION_ERROR;
    
    if (config) {
        c->config = *config;
    } else {
        c->config.precision = 6;
        c->config.validate_range = false;
        c->config.min_value = -1e308;
        c->config.max_value = 1e308;
    }
    c->last_error = UNITS_OK;
    
    *ctx = c;
    return UNITS_OK;
}

void units_destroy(units_ctx_t* ctx) {
    if (ctx) free(ctx);
}

static units_error_t validate_value(units_ctx_t* ctx, double value) {
    if (!ctx) return UNITS_INVALID_PARAMS;
    
    if (ctx->config.validate_range) {
        if (value < ctx->config.min_value || value > ctx->config.max_value) {
            return UNITS_OUT_OF_RANGE;
        }
    }
    return UNITS_OK;
}

static double celsius_to_kelvin(double c) { return c + 273.15; }
static double kelvin_to_celsius(double k) { return k - 273.15; }
static double fahrenheit_to_rankine(double f) { return f + 459.67; }
static double rankine_to_fahrenheit(double r) { return r - 459.67; }

units_error_t units_convert_temperature(units_ctx_t* ctx, double value, 
                                        units_temp_unit_t from_unit, 
                                        units_temp_unit_t to_unit, 
                                        double* result) {
    if (!result) return UNITS_INVALID_PARAMS;
    
    units_error_t err = ctx ? validate_value(ctx, value) : UNITS_OK;
    if (err != UNITS_OK) return err;
    
    double celsius;
    switch (from_unit) {
        case UNITS_TEMP_CELSIUS: celsius = value; break;
        case UNITS_TEMP_FAHRENHEIT: celsius = units_fahrenheit_to_celsius(value); break;
        case UNITS_TEMP_KELVIN: celsius = kelvin_to_celsius(value); break;
        case UNITS_TEMP_RANKINE: celsius = kelvin_to_celsius(rankine_to_fahrenheit(value) - 459.67 + 273.15); break;
        default: return UNITS_INVALID_UNIT;
    }
    
    switch (to_unit) {
        case UNITS_TEMP_CELSIUS: *result = celsius; break;
        case UNITS_TEMP_FAHRENHEIT: *result = units_celsius_to_fahrenheit(celsius); break;
        case UNITS_TEMP_KELVIN: *result = celsius_to_kelvin(celsius); break;
        case UNITS_TEMP_RANKINE: *result = fahrenheit_to_rankine(units_celsius_to_fahrenheit(celsius)); break;
        default: return UNITS_INVALID_UNIT;
    }
    
    return UNITS_OK;
}

units_error_t units_convert_distance(units_ctx_t* ctx, double value,
                                     units_dist_unit_t from_unit,
                                     units_dist_unit_t to_unit,
                                     double* result) {
    if (!result) return UNITS_INVALID_PARAMS;
    
    units_error_t err = ctx ? validate_value(ctx, value) : UNITS_OK;
    if (err != UNITS_OK) return err;
    
    static const double to_meter[] = {
        [UNITS_DIST_KILOMETER] = 1000.0,
        [UNITS_DIST_MILE] = 1609.344,
        [UNITS_DIST_METER] = 1.0,
        [UNITS_DIST_FOOT] = 0.3048,
        [UNITS_DIST_INCH] = 0.0254,
        [UNITS_DIST_YARD] = 0.9144
    };
    
    if (from_unit < 0 || from_unit > UNITS_DIST_YARD ||
        to_unit < 0 || to_unit > UNITS_DIST_YARD) {
        return UNITS_INVALID_UNIT;
    }
    
    double meters = value * to_meter[from_unit];
    *result = meters / to_meter[to_unit];
    
    return UNITS_OK;
}

units_error_t units_convert_weight(units_ctx_t* ctx, double value,
                                   units_weight_unit_t from_unit,
                                   units_weight_unit_t to_unit,
                                   double* result) {
    if (!result) return UNITS_INVALID_PARAMS;
    
    units_error_t err = ctx ? validate_value(ctx, value) : UNITS_OK;
    if (err != UNITS_OK) return err;
    
    static const double to_kg[] = {
        [UNITS_WEIGHT_KILOGRAM] = 1.0,
        [UNITS_WEIGHT_POUND] = 0.45359237,
        [UNITS_WEIGHT_GRAM] = 0.001,
        [UNITS_WEIGHT_OUNCE] = 0.028349523125
    };
    
    if (from_unit < 0 || from_unit > UNITS_WEIGHT_OUNCE ||
        to_unit < 0 || to_unit > UNITS_WEIGHT_OUNCE) {
        return UNITS_INVALID_UNIT;
    }
    
    double kg = value * to_kg[from_unit];
    *result = kg / to_kg[to_unit];
    
    return UNITS_OK;
}

units_error_t units_convert_time(units_ctx_t* ctx, double value,
                                 units_time_unit_t from_unit,
                                 units_time_unit_t to_unit,
                                 double* result) {
    if (!result) return UNITS_INVALID_PARAMS;
    
    units_error_t err = ctx ? validate_value(ctx, value) : UNITS_OK;
    if (err != UNITS_OK) return err;
    
    static const double to_second[] = {
        [UNITS_TIME_SECOND] = 1.0,
        [UNITS_TIME_MINUTE] = 60.0,
        [UNITS_TIME_HOUR] = 3600.0,
        [UNITS_TIME_DAY] = 86400.0,
        [UNITS_TIME_WEEK] = 604800.0
    };
    
    if (from_unit < 0 || from_unit > UNITS_TIME_WEEK ||
        to_unit < 0 || to_unit > UNITS_TIME_WEEK) {
        return UNITS_INVALID_UNIT;
    }
    
    double seconds = value * to_second[from_unit];
    *result = seconds / to_second[to_unit];
    
    return UNITS_OK;
}

units_error_t units_convert_volume(units_ctx_t* ctx, double value,
                                   units_vol_unit_t from_unit,
                                   units_vol_unit_t to_unit,
                                   double* result) {
    if (!result) return UNITS_INVALID_PARAMS;
    
    units_error_t err = ctx ? validate_value(ctx, value) : UNITS_OK;
    if (err != UNITS_OK) return err;
    
    static const double to_liter[] = {
        [UNITS_VOL_MILLILITER] = 0.001,
        [UNITS_VOL_LITER] = 1.0,
        [UNITS_VOL_GALLON_US] = 3.785411784,
        [UNITS_VOL_GALLON_IMPERIAL] = 4.54609,
        [UNITS_VOL_QUART] = 0.946352946,
        [UNITS_VOL_PINT] = 0.473176473
    };
    
    if (from_unit < 0 || from_unit > UNITS_VOL_PINT ||
        to_unit < 0 || to_unit > UNITS_VOL_PINT) {
        return UNITS_INVALID_UNIT;
    }
    
    double liters = value * to_liter[from_unit];
    *result = liters / to_liter[to_unit];
    
    return UNITS_OK;
}

units_error_t units_convert_area(units_ctx_t* ctx, double value,
                                 units_area_unit_t from_unit,
                                 units_area_unit_t to_unit,
                                 double* result) {
    if (!result) return UNITS_INVALID_PARAMS;
    
    units_error_t err = ctx ? validate_value(ctx, value) : UNITS_OK;
    if (err != UNITS_OK) return err;
    
    static const double to_sqmeter[] = {
        [UNITS_AREA_SQUARE_METER] = 1.0,
        [UNITS_AREA_SQUARE_KILOMETER] = 1000000.0,
        [UNITS_AREA_SQUARE_MILE] = 2589988.110336,
        [UNITS_AREA_ACRE] = 4046.8564224,
        [UNITS_AREA_HECTARE] = 10000.0
    };
    
    if (from_unit < 0 || from_unit > UNITS_AREA_HECTARE ||
        to_unit < 0 || to_unit > UNITS_AREA_HECTARE) {
        return UNITS_INVALID_UNIT;
    }
    
    double sqmeters = value * to_sqmeter[from_unit];
    *result = sqmeters / to_sqmeter[to_unit];
    
    return UNITS_OK;
}

units_error_t units_convert_speed(units_ctx_t* ctx, double value,
                                  units_speed_unit_t from_unit,
                                  units_speed_unit_t to_unit,
                                  double* result) {
    if (!result) return UNITS_INVALID_PARAMS;
    
    units_error_t err = ctx ? validate_value(ctx, value) : UNITS_OK;
    if (err != UNITS_OK) return err;
    
    static const double to_mps[] = {
        [UNITS_SPEED_MPS] = 1.0,
        [UNITS_SPEED_KMH] = 0.2777777777777778,
        [UNITS_SPEED_MPH] = 0.44704,
        [UNITS_SPEED_KNOT] = 0.5144444444444444
    };
    
    if (from_unit < 0 || from_unit > UNITS_SPEED_KNOT ||
        to_unit < 0 || to_unit > UNITS_SPEED_KNOT) {
        return UNITS_INVALID_UNIT;
    }
    
    double mps = value * to_mps[from_unit];
    *result = mps / to_mps[to_unit];
    
    return UNITS_OK;
}

double units_celsius_to_fahrenheit(double c) { return c * 9.0 / 5.0 + 32.0; }
double units_fahrenheit_to_celsius(double f) { return (f - 32.0) * 5.0 / 9.0; }
double units_celsius_to_kelvin(double c) { return c + 273.15; }
double units_kelvin_to_celsius(double k) { return k - 273.15; }
double units_km_to_miles(double km) { return km * 0.621371; }
double units_miles_to_km(double m) { return m / 0.621371; }
double units_meters_to_feet(double meters) { return meters * 3.28084; }
double units_feet_to_meters(double feet) { return feet / 3.28084; }

units_error_t units_get_last_error(units_ctx_t* ctx) {
    return ctx ? ctx->last_error : UNITS_INVALID_PARAMS;
}

const char* units_strerror(units_error_t error) {
    switch (error) {
        case UNITS_OK: return "Success";
        case UNITS_INVALID_PARAMS: return "Invalid parameters";
        case UNITS_INVALID_UNIT: return "Invalid unit";
        case UNITS_OUT_OF_RANGE: return "Value out of range";
        case UNITS_CONVERSION_ERROR: return "Conversion error";
        default: return "Unknown error";
    }
}
