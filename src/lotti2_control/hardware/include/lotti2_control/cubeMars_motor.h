#ifndef CUBEMARS_MOTOR_H
#define CUBEMARS_MOTOR_H

#include <stdint.h>

struct motorState {
    uint8_t motor_id;
    double velocity;    // -320000 to +320000 eRPM
    double current;     // -60 to +60 A
    double motor_temp;  // -20 to +127 °C
    int error_code;     // 0 = no error, 1 = over temperature, 2 = over current, 3 = over voltage, 4 = under voltage, 5 = encoder error, 6 = phase current unbalance (The hardware may be damaged)
};

struct motorCommand {
    uint8_t motor_id;
    int32_t speed;  // +- 50000 eRPM
};

struct motorParams {
    float P_MIN        = -12.5;
    float P_MAX        = 12.5;
    float V_MIN        = -50;
    float V_MAX        = 50;
    float T_MIN        = -65.0;
    float T_MAX        = 65.0;
    float KP_MIN       = 0.0;
    float KP_MAX       = 500.0;
    float KD_MIN       = 0;
    float KD_MAX       = 5;
    int AXIS_DIRECTION = 1;
};
#endif  // LOTTI2_CONTROL__CUBEMARS_MOTOR_H