#include "OneWireRotary.h"

/*
 * The below state table has, for each state (row), the new state
 * to set based on the next encoder output. From left to right in,
 * the table, the encoder outputs are 00, 01, 10, 11, and the value
 * in that position is the new state to set.
 */
const unsigned char rotary_state_table[6][4] = {
  // R_START (00)
  {R_START_M,                   R_CW_BEGIN,     R_CCW_BEGIN,    R_START},
  // R_CCW_BEGIN
  {R_START_M | ROTARY_DIR_CCW,  R_START,        R_CCW_BEGIN,    R_START},
  // R_CW_BEGIN
  {R_START_M | ROTARY_DIR_CW,   R_CW_BEGIN,     R_START,        R_START},
  // R_START_M (11)
  {R_START_M,                   R_CCW_BEGIN_M,  R_CW_BEGIN_M,   R_START},
  // R_CW_BEGIN_M
  {R_START_M,                   R_START_M,      R_CW_BEGIN_M,   R_START | ROTARY_DIR_CW},
  // R_CCW_BEGIN_M
  {R_START_M,                   R_CCW_BEGIN_M,  R_START_M,      R_START | ROTARY_DIR_CCW},
};

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
    double R2a = ((double)expected_a_value * 1.f) / (1023.f - (double)expected_a_value);
    double R2b = ((double)expected_b_value * 1.f) / (1023.f - (double)expected_b_value);
    double R2ab = 1.f / (1.f / R2a + 1.f / R2b);
    this->expected_ab_value = R2ab / (1.f + R2ab) * 1023.f;

    this->rotary_state = R_START;
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

    this->state = rotary_state_table[this->state & 0xf][ab];

    if ((this->state & 0x30) == ROTARY_DIR_CW) {
        position++;
    } else if ((this->state & 0x30) == ROTARY_DIR_CCW) {
        position--;
    }

    return ROTARY_RELEASED;
}

int16_t OneWireRotary::getPosition() {
    return this->position;
};

void OneWireRotary::resetPosition() {
    this->position = 0;
}

