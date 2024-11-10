/*
 * Tauno_shift_register.h
 * Started: 09.11.2024
 * Edited:  10.11.2024
 * Copyright Tauno Erik 2024
 */

#ifndef TAUNO_SHIFT_REGISTER_H_
#define TAUNO_SHIFT_REGISTER_H_



#include <Arduino.h>

class Tauno_shift_register
{
public:
    Tauno_shift_register(uint8_t data_pin,
                         uint8_t clock_pin,
                         uint8_t latch_pin,
                         uint8_t num_outputs,
                         uint16_t pwb_period);
    void begin();
    void set_PWM(uint8_t duty_cycle);
    void set_pin(uint8_t pin, bool state);   // Set pin LOW or HIGH
    void run();
    void pins_off();
    void write(uint8_t data);
    void shift_port_d(uint8_t data);
    void shift_out_fast(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);

private:
    uint8_t _data_pin;
    uint8_t _clock_pin;
    uint8_t _latch_pin;
    uint8_t _num_outputs;
    uint8_t _duty_cycle;

    uint8_t* _state;  // Array to hold the shift register states
    uint8_t* _pin_states;  // Per-pin control (LOW or HIGH)

    uint16_t _pwm_period;
    uint32_t _last_update;
};

#endif // TAUNO_SHIFT_REGISTER_H_ end