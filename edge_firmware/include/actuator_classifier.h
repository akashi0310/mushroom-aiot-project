// AUTO-GENERATED – DO NOT EDIT
// Model: Actuator Decision Tree
// Test accuracy: 87.1%
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
                        if (air_humidity <= 41.4505f) {
                            return ACTUATOR_IDLE;  // idle
                        } else {
                            return ACTUATOR_IDLE;  // idle
                        }
                    } else {
                        if (air_humidity <= 42.4243f) {
                            if (air_humidity <= 40.8690f) {
                                return ACTUATOR_PUMP_AND_FAN;  // pump_and_fan
                            } else {
                                if (temperature <= 22.5107f) {
                                    return ACTUATOR_PUMP_AND_FAN;  // pump_and_fan
                                } else {
                                    return ACTUATOR_FAN;  // fan
                                }
                            }
                        } else {
                            return ACTUATOR_PUMP_AND_FAN;  // pump_and_fan
                        }
                    }
                } else {
                    if (temperature <= 20.9257f) {
                        return ACTUATOR_FAN;  // fan
                    } else {
                        if (soil_moisture <= 35.0340f) {
                            return ACTUATOR_FAN;  // fan
                        } else {
                            return ACTUATOR_FAN;  // fan
                        }
                    }
                }
            } else {
                if (soil_moisture <= 14.1963f) {
                    return ACTUATOR_PUMP;  // pump
                } else {
                    if (soil_moisture <= 32.7690f) {
                        if (soil_moisture <= 30.2144f) {
                            if (soil_moisture <= 20.2037f) {
                                if (temperature <= 23.0293f) {
                                    return ACTUATOR_IDLE;  // idle
                                } else {
                                    return ACTUATOR_FAN;  // fan
                                }
                            } else {
                                if (soil_moisture <= 24.2227f) {
                                    return ACTUATOR_PUMP;  // pump
                                } else {
                                    return ACTUATOR_IDLE;  // idle
                                }
                            }
                        } else {
                            if (air_humidity <= 52.4517f) {
                                return ACTUATOR_PUMP;  // pump
                            } else {
                                return ACTUATOR_PUMP;  // pump
                            }
                        }
                    } else {
                        if (air_humidity <= 45.3435f) {
                            if (soil_moisture <= 37.5525f) {
                                return ACTUATOR_IDLE;  // idle
                            } else {
                                return ACTUATOR_FAN;  // fan
                            }
                        } else {
                            if (temperature <= 18.8558f) {
                                return ACTUATOR_IDLE;  // idle
                            } else {
                                return ACTUATOR_IDLE;  // idle
                            }
                        }
                    }
                }
            }
        } else {
            if (soil_moisture <= 14.2846f) {
                if (air_humidity <= 54.8897f) {
                    return ACTUATOR_PUMP_AND_FAN;  // pump_and_fan
                } else {
                    return ACTUATOR_PUMP_AND_FAN;  // pump_and_fan
                }
            } else {
                if (soil_moisture <= 32.9554f) {
                    if (soil_moisture <= 19.9733f) {
                        if (air_humidity <= 51.8955f) {
                            return ACTUATOR_FAN;  // fan
                        } else {
                            if (temperature <= 27.3787f) {
                                return ACTUATOR_IDLE;  // idle
                            } else {
                                return ACTUATOR_FAN;  // fan
                            }
                        }
                    } else {
                        if (soil_moisture <= 24.2381f) {
                            if (soil_moisture <= 22.7395f) {
                                if (air_humidity <= 49.9802f) {
                                    return ACTUATOR_PUMP_AND_FAN;  // pump_and_fan
                                } else {
                                    return ACTUATOR_PUMP;  // pump
                                }
                            } else {
                                if (soil_moisture <= 23.2730f) {
                                    return ACTUATOR_PUMP_AND_FAN;  // pump_and_fan
                                } else {
                                    return ACTUATOR_PUMP_AND_FAN;  // pump_and_fan
                                }
                            }
                        } else {
                            if (soil_moisture <= 29.9657f) {
                                if (air_humidity <= 54.9981f) {
                                    return ACTUATOR_FAN;  // fan
                                } else {
                                    return ACTUATOR_IDLE;  // idle
                                }
                            } else {
                                if (temperature <= 26.0808f) {
                                    return ACTUATOR_PUMP;  // pump
                                } else {
                                    return ACTUATOR_PUMP_AND_FAN;  // pump_and_fan
                                }
                            }
                        }
                    }
                } else {
                    if (temperature <= 24.0812f) {
                        return ACTUATOR_IDLE;  // idle
                    } else {
                        if (air_humidity <= 53.7192f) {
                            return ACTUATOR_FAN;  // fan
                        } else {
                            if (soil_moisture <= 37.8549f) {
                                return ACTUATOR_IDLE;  // idle
                            } else {
                                return ACTUATOR_FAN;  // fan
                            }
                        }
                    }
                }
            }
        }
    } else {
        if (soil_moisture <= 14.2087f) {
            if (soil_moisture <= 13.7086f) {
                return ACTUATOR_PUMP;  // pump
            } else {
                return ACTUATOR_PUMP;  // pump
            }
        } else {
            if (soil_moisture <= 32.9742f) {
                if (soil_moisture <= 20.0086f) {
                    if (temperature <= 29.3405f) {
                        if (temperature <= 28.3255f) {
                            return ACTUATOR_IDLE;  // idle
                        } else {
                            return ACTUATOR_IDLE;  // idle
                        }
                    } else {
                        return ACTUATOR_FAN;  // fan
                    }
                } else {
                    if (soil_moisture <= 24.2512f) {
                        if (temperature <= 19.3134f) {
                            return ACTUATOR_PUMP;  // pump
                        } else {
                            if (soil_moisture <= 23.2735f) {
                                return ACTUATOR_PUMP;  // pump
                            } else {
                                return ACTUATOR_PUMP;  // pump
                            }
                        }
                    } else {
                        if (soil_moisture <= 30.0152f) {
                            if (temperature <= 29.1227f) {
                                if (soil_moisture <= 24.6106f) {
                                    return ACTUATOR_IDLE;  // idle
                                } else {
                                    return ACTUATOR_IDLE;  // idle
                                }
                            } else {
                                return ACTUATOR_FAN;  // fan
                            }
                        } else {
                            if (temperature <= 28.4102f) {
                                if (temperature <= 25.2714f) {
                                    return ACTUATOR_PUMP;  // pump
                                } else {
                                    return ACTUATOR_PUMP;  // pump
                                }
                            } else {
                                return ACTUATOR_PUMP_AND_FAN;  // pump_and_fan
                            }
                        }
                    }
                }
            } else {
                if (temperature <= 27.7705f) {
                    if (temperature <= 18.9540f) {
                        return ACTUATOR_IDLE;  // idle
                    } else {
                        return ACTUATOR_IDLE;  // idle
                    }
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
