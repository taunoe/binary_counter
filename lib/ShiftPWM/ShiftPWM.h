#ifndef ShiftPWM_h
#define ShiftPWM_h

#include <Arduino.h>

class ShiftPWM {
public:
    // Constructor
    ShiftPWM(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, uint8_t numOutputs, uint16_t pwmPeriod);

    // Methods
    void begin();
    void setPWM(uint8_t pin, uint8_t dutyCycle);
    void update();

private:
    uint8_t _dataPin, _clockPin, _latchPin;
    uint8_t _numOutputs;
    uint16_t _pwmPeriod;
    byte* _state;         // Array to hold the shift register states
    uint8_t* _dutyCycles; // Array to store duty cycles for each output
    unsigned long _lastUpdateTime;
};

#endif
