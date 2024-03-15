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

#define ROTARY_NONE     (0x00)
#define ROTARY_PRESSED  (0x01)
#define ROTARY_CW       (0x02)
#define ROTARY_CCW      (0x03)

#ifdef ROTARY_HALF_STEP
    // Half-step table
    #define R_START         (0x0)
    #define R_CCW_BEGIN     (0x1)
    #define R_CW_BEGIN      (0x2)
    #define R_START_M       (0x3)
    #define R_CW_BEGIN_M    (0x4)
    #define R_CCW_BEGIN_M   (0x5)
#else
    // Use the full-step state table (emits a code at 00 only)
    #define R_START     (0x0)
    #define R_CW_FINAL  (0x1)
    #define R_CW_BEGIN  (0x2)
    #define R_CW_NEXT   (0x3)
    #define R_CCW_BEGIN (0x4)
    #define R_CCW_FINAL (0x5)
    #define R_CCW_NEXT  (0x6)
#endif

// Internal codes
#define R_DIR_CW   (0x10)
#define R_DIR_CCW  (0x20)

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

    bool button_pressed;
    bool button_handled;
public:
    OneWireRotary(uint8_t input_pin, uint16_t expected_a_value, uint16_t expected_b_value, uint16_t variance);
    void begin();
    void poll();

    uint8_t handle();
};

#endif
