#ifndef OneWireRotary_h
#define OneWireRotary_h

/**
 * Wiring for the rotary encoder:
 *       ┌─────────┐
 * GND───┤         ├───10k───GND
 *       │         │              ┌───10k───VCC
 *       │         ├──────────────┤
 *       │         │              └───ADC
 * ADC───┤         ├───22k───GND
 *       └─────────┘
 */

#include "Arduino.h"

#define ROTARY_DIR_CW   (0x10)
#define ROTARY_DIR_CCW  (0x20)

#define ROTARY_PRESSED  (true)
#define ROTARY_RELEASED (false)

// Half-step table
#define R_START         (0x0)
#define R_CCW_BEGIN     (0x1)
#define R_CW_BEGIN      (0x2)
#define R_START_M       (0x3)
#define R_CW_BEGIN_M    (0x4)
#define R_CCW_BEGIN_M   (0x5)

class OneWireRotary
{
private:
    uint8_t input_pin;
    uint16_t expected_a_value;
    uint16_t expected_b_value;
    uint16_t variance;
    uint16_t expected_ab_value;

    uint8_t state;
    int16_t position;
public:
    OneWireRotary(uint8_t input_pin, uint16_t expected_a_value, uint16_t expected_b_value, uint16_t variance);
    void begin();

    bool poll();

    int16_t getPosition();
    void resetPosition();
};

#endif
