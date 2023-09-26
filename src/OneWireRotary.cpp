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

#include "OneWireRotary.h"

/*
 * The below state table has, for each state (row), the new state
 * to set based on the next encoder output. From left to right in,
 * the table, the encoder outputs are 00, 01, 10, 11, and the value
 * in that position is the new state to set.
 */

#define R_START 0x0

#ifdef HALF_STEP
// Use the half-step state table (emits a code at 00 and 11)
#define R_CCW_BEGIN 0x1
#define R_CW_BEGIN 0x2
#define R_START_M 0x3
#define R_CW_BEGIN_M 0x4
#define R_CCW_BEGIN_M 0x5
const unsigned char rotary_ttable[6][4] = {
  // R_START (00)
  {R_START_M,            R_CW_BEGIN,     R_CCW_BEGIN,  R_START},
  // R_CCW_BEGIN
  {R_START_M | DIR_CCW, R_START,        R_CCW_BEGIN,  R_START},
  // R_CW_BEGIN
  {R_START_M | DIR_CW,  R_CW_BEGIN,     R_START,      R_START},
  // R_START_M (11)
  {R_START_M,            R_CCW_BEGIN_M,  R_CW_BEGIN_M, R_START},
  // R_CW_BEGIN_M
  {R_START_M,            R_START_M,      R_CW_BEGIN_M, R_START | DIR_CW},
  // R_CCW_BEGIN_M
  {R_START_M,            R_CCW_BEGIN_M,  R_START_M,    R_START | DIR_CCW},
};
#else
// Use the full-step state table (emits a code at 00 only)
#define R_CW_FINAL 0x1
#define R_CW_BEGIN 0x2
#define R_CW_NEXT 0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT 0x6

const unsigned char rotary_ttable[7][4] = {
  // R_START
  {R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START},
  // R_CW_FINAL
  {R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | ROTARY_DIR_CW},
  // R_CW_BEGIN
  {R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START},
  // R_CW_NEXT
  {R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START},
  // R_CCW_BEGIN
  {R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START},
  // R_CCW_FINAL
  {R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | ROTARY_DIR_CCW},
  // R_CCW_NEXT
  {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},
};
#endif

OneWireRotary::OneWireRotary(uint8_t input_pin, uint16_t expected_a_value, uint16_t expected_b_value, uint16_t variance) {
    this->input_pin = input_pin;
    this->expected_a_value = expected_a_value;
    this->expected_b_value = expected_b_value;
    this->variance = variance;

    /**
     * We will calculate the output when AB are in contact based on the
     * expected values for A and B, and a assumed R1 of 1. Since we use only
     * relative values it shouldn't matter :)
     */
    double Ra = 1.f / (1023.f / (double)expected_a_value) - 1.f;
    double Rb = 1.f / (1023.f / (double)expected_b_value) - 1.f;
    double Rab = 1.f / (1.f / Ra + 1.f / Rb);
    this->expected_ab_value = Rab / (1.f + Rab) * 1023.f;

    this->state = R_START;
    this->resetPosition();
}

void OneWireRotary::begin() {
    pinMode(this->input_pin, INPUT);
}

bool OneWireRotary::poll() {
    uint16_t reading = analogRead(this->input_pin);

    if (reading <= 0 + variance) {
        return ROTARY_PRESSED;
    }

    uint8_t ab;
    if (reading > expected_ab_value - this->variance && reading < expected_ab_value + this->variance) {
        ab = 0b11;
    } else if (reading > expected_a_value - this->variance && reading < expected_a_value + this->variance) {
        ab = 0b10;
    } else if (reading > expected_b_value - this->variance && reading < expected_b_value + this->variance) {
        ab = 0b01;
    } else {
        ab = 0b00;
    }

    this->state = rotary_ttable[this->state & 0xf][ab];

    switch (this->state & 0x30) {
        case ROTARY_DIR_CW:
            position++;
        case ROTARY_DIR_CCW:
            position--;
    }

    return ROTARY_RELEASED;
}

uint16_t OneWireRotary::getPosition() {
    return this->position;
};

void OneWireRotary::resetPosition() {
    this->position = 0;
}

