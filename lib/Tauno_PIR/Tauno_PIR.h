/*
 * TAUNO_PIR.cpp
 * Started: 04.11.2024
 * Edited:  04.11.2024
 * Copyright Tauno Erik 2024
 */

#ifndef TAUNO_PIR_H_
#define TAUNO_PIR_H_

#include <Arduino.h>

class Tauno_PIR
{
public:
    Tauno_PIR(int pin);
    void begin();
    bool is_motion();
    void set_debounce(unsigned long debounce_time);

private:
    int _pin;
    unsigned long _debounce_time;
    unsigned long _last_read_time;
};

#endif // TAUNO_PIR_H_ end
