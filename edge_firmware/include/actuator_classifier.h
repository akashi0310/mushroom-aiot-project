// AUTO-GENERATED – DO NOT EDIT
// Model: Actuator Decision Tree (max_depth=5)
// Test accuracy: 75.4%
// Features: temperature (°C), air_humidity (%), soil_moisture (%)
// Labels: 0=idle  1=pump  2=fan  3=pump_and_fan
#pragma once
#include <stdint.h>

#define N_ACTUATOR_CLASSES 4

enum ActuatorAction : uint8_t {
    ACTUATOR_IDLE         = 0,
    ACTUATOR_PUMP         = 1,
    ACTUATOR_FAN          = 2,
    ACTUATOR_PUMP_AND_FAN = 3,
};

static const char* const ACTUATOR_NAMES[] = {
    "idle", "pump", "fan", "pump_and_fan"
};

inline ActuatorAction classifyActuator(
    float temperature,
    float air_humidity,
    float soil_moisture
) {
    if (air_humidity <= 56.9768f) {
        if (temperature <= 23.5748f) {
            if (air_humidity <= 43.0167f) {
                if (soil_moisture <= 32.3057f) {
                    if (temperature <= 20.3524f) {
                        return ACTUATOR_IDLE;  // idle
                    } else {
                        return ACTUATOR_PUMP_AND_FAN;  // pump_and_fan
                    }
                } else {
                    return ACTUATOR_FAN;  // fan
                }
            } else {
                if (soil_moisture <= 14.1963f) {
                    return ACTUATOR_PUMP;  // pump
                } else {
                    return ACTUATOR_IDLE;  // idle
                }
            }
        } else {
            if (soil_moisture <= 14.2846f) {
                return ACTUATOR_PUMP_AND_FAN;  // pump_and_fan
            } else {
                if (soil_moisture <= 32.9554f) {
                    if (soil_moisture <= 19.9733f) {
                        return ACTUATOR_FAN;  // fan
                    } else {
                        return ACTUATOR_PUMP_AND_FAN;  // pump_and_fan
                    }
                } else {
                    if (temperature <= 24.0812f) {
                        return ACTUATOR_IDLE;  // idle
                    } else {
                        return ACTUATOR_FAN;  // fan
                    }
                }
            }
        }
    } else {
        if (soil_moisture <= 14.2087f) {
            return ACTUATOR_PUMP;  // pump
        } else {
            if (soil_moisture <= 32.9742f) {
                if (soil_moisture <= 20.0086f) {
                    if (temperature <= 29.3405f) {
                        return ACTUATOR_IDLE;  // idle
                    } else {
                        return ACTUATOR_FAN;  // fan
                    }
                } else {
                    if (soil_moisture <= 24.2512f) {
                        return ACTUATOR_PUMP;  // pump
                    } else {
                        return ACTUATOR_IDLE;  // idle
                    }
                }
            } else {
                if (temperature <= 27.7705f) {
                    return ACTUATOR_IDLE;  // idle
                } else {
                    if (air_humidity <= 61.8026f) {
                        return ACTUATOR_FAN;  // fan
                    } else {
                        return ACTUATOR_IDLE;  // idle
                    }
                }
            }
        }
    }
}
