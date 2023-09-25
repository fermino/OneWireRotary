/*
 * Rotary encoder handler for arduino.
 *
 * Copyright 2011 Ben Buxton.
 * Licenced under the GNU GPL Version 3.
 *
 * Rewritten and adapted by Fermín Olaiz (@fermino) to handle an encoder with
 * only one pin using a voltage divider.
 *
 * Contact:
 *  bb@cactii.net
 *  ferminolaiz@gmail.com
 */

#ifndef OneWireRotary_h
#define OneWireRotary_h

#include "Arduino.h"

// Enable this to emit codes twice per step.
// #define HALF_STEP

#define ROTARY_PRESSED  (true)
#define ROTARY_RELEASED (false)

#define ROTARY_DIR_CW (0x10)
#define ROTARY_DIR_CCW (0x20)


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

    uint16_t getPosition();
    void resetPosition();
};

#endif
