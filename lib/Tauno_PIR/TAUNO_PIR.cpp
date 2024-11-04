/*
 * TAUNO_PIR.cpp
 * Started: 04.11.2024
 * Edited:  04.11.2024
 * Copyright Tauno Erik 2024
 */

#include "Tauno_PIR.h"

Tauno_PIR::Tauno_PIR(int pin)
{
    _pin = pin;
    _debounce_time = 500;
    _last_read_time = 0;
}

void Tauno_PIR::begin()
{
    pinMode(_pin, INPUT);
}

bool Tauno_PIR::is_motion()
{
    unsigned long current_millis = millis();

    if (current_millis - _last_read_time >= _debounce_time)
    {
        _last_read_time = current_millis;
        return digitalRead(_pin) == HIGH; // Return true if motion is detected
    }
    return false;
}

void Tauno_PIR::set_debounce(unsigned long debounce_time)
{
    _debounce_time = debounce_time;
}
