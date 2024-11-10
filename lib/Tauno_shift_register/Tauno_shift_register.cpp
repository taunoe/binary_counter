/*
 * Tauno_shift_register.cpp
 * Started: 09.11.2024
 * Edited:  10.11.2024
 * Copyright Tauno Erik 2024
 */

#include "Tauno_shift_register.h"

Tauno_shift_register::Tauno_shift_register (uint8_t data_pin,
                                            uint8_t clock_pin,
                                            uint8_t latch_pin,
                                            uint8_t num_outputs,
                                            uint16_t pwm_period)
    : _data_pin(data_pin),
      _clock_pin(clock_pin),
      _latch_pin(latch_pin),
      _num_outputs(num_outputs),
      _pwm_period(pwm_period)
{
    _state = new uint8_t[(_num_outputs + 7 ) / 8];
    _pin_states = new uint8_t[(_num_outputs + 7) / 8]();
    _duty_cycle = 0;
    _last_update = 0;
}

void Tauno_shift_register::begin()
{
    pinMode(_data_pin, OUTPUT);
    pinMode(_clock_pin, OUTPUT);
    pinMode(_latch_pin, OUTPUT);

    digitalWrite(_data_pin, LOW);
    digitalWrite(_clock_pin, LOW);
    digitalWrite(_latch_pin, LOW);
}


// Set the duty cycle for a specific pin
void Tauno_shift_register::set_PWM(uint8_t duty_cycle)
{
    _duty_cycle = constrain(duty_cycle, 0, 255); // Ensure dutyCycle is within range
}


void Tauno_shift_register::set_pin(uint8_t pin, bool state)
{
    if (pin >= _num_outputs) return;

    if (state)
    {
        _pin_states[pin / 8] |= (1 << (pin % 8)); // Set bit (HIGH and follows PWM)
    }
    else
    {
        _pin_states[pin / 8] &= ~(1 << (pin % 8)); // Clear bit (LOW)
    }
}


void Tauno_shift_register::run()
{
    uint32_t now = millis();

    //if (current_millis - _last_update >= _pwm_period) {
    //    _last_update = current_millis;
    //}

    uint16_t phase_millis = now % _pwm_period;  // Current phase within the PWM period
    uint16_t on_time = (_duty_cycle * _pwm_period) / 255.0;  // ON time in milliseconds

    // Update each pin's state
    for (uint8_t i = 0; i < _num_outputs; i++)
    {
        if (_pin_states[i / 8] & (1 << (i % 8))) {
            // Pin is HIGH and follows PWM
            if (phase_millis < on_time) {
                _state[i / 8] |= (1 << (i % 8)); // Set bit
            } else {
                _state[i / 8] &= ~(1 << (i % 8)); // Clear bit
            }
        } else {
            // Pin is LOW
            _state[i / 8] &= ~(1 << (i % 8)); // Clear bit
        }
    }

    // Shift out the data
    
    PORTD &= ~(1<<_latch_pin);// digitalWrite(_latch_pin, LOW); // Disable latch

    for (int i = (_num_outputs - 1); i >= 0; i--) {
        shift_port_d(_state[i]);
        //uint8_t bit = (_state[i / 8] & (1 << (i % 8))) ? HIGH : LOW;
        //shift_port_d(bit);
    }
    
    PORTD |= (1<<_latch_pin);//digitalWrite(_latch_pin, HIGH); // Enable latch
}


void Tauno_shift_register::pins_off()
{
    for (int i = (_num_outputs - 1); i >= 0; i--)
    {
        set_pin(i, false);
    }
}



void Tauno_shift_register::write(uint8_t data)
{
    PORTD &= ~(1<<_latch_pin);  //digitalWrite(_latch_pin, LOW);

    for (uint8_t i = 0; i < 8; i++) {
      uint8_t bit = (data & 128) != 0;
      if (bit == 0) {
        PORTD &= ~(1 << _data_pin);
      } else {
        PORTD |= (1 << _data_pin);
      }
      data <<= 1;

      PORTD |= (1 << _clock_pin);   // Set to HIGH
      PORTD &= ~(1 << _clock_pin);  // Set to LOW
    }

    PORTD |= (1<<_latch_pin);  //digitalWrite(_latch_pin, HIGH);
}

void Tauno_shift_register::shift_port_d(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++) {
      uint8_t bit = (data & 128) != 0;
      if (bit == 0) {
        PORTD &= ~(1 << _data_pin);
      } else {
        PORTD |= (1 << _data_pin);
      }
      data <<= 1;

      PORTD |= (1 << _clock_pin);   // Set to HIGH
      PORTD &= ~(1 << _clock_pin);  // Set to LOW
    }
}


void Tauno_shift_register::shift_out_fast(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val) {
    uint8_t dataBit;
    volatile uint8_t *dataPort = portOutputRegister(digitalPinToPort(dataPin));
    volatile uint8_t *clockPort = portOutputRegister(digitalPinToPort(clockPin));
    uint8_t dataMask = digitalPinToBitMask(dataPin);
    uint8_t clockMask = digitalPinToBitMask(clockPin);

    for (uint8_t i = 0; i < 8; i++) {
        if (bitOrder == LSBFIRST) {
            dataBit = (val & (1 << i)) ? dataMask : 0;
        } else {
            dataBit = (val & (1 << (7 - i))) ? dataMask : 0;
        }

        *dataPort = (*dataPort & ~dataMask) | dataBit; // Set data pin
        *clockPort |= clockMask;                      // Set clock HIGH
        *clockPort &= ~clockMask;                     // Set clock LOW
    }
}
