#include "ShiftPWM.h"

// Constructor
ShiftPWM::ShiftPWM(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin, uint8_t numOutputs, uint16_t pwmPeriod)
    : _dataPin(dataPin), _clockPin(clockPin), _latchPin(latchPin), _numOutputs(numOutputs), _pwmPeriod(pwmPeriod) {
    _state = new byte[_numOutputs / 8 + (_numOutputs % 8 != 0)];
    _dutyCycles = new uint8_t[_numOutputs];
    memset(_state, 0, _numOutputs / 8 + (_numOutputs % 8 != 0));
    memset(_dutyCycles, 0, _numOutputs);
    _lastUpdateTime = 0;
}

// Initialize pins
void ShiftPWM::begin() {
    pinMode(_dataPin, OUTPUT);
    pinMode(_clockPin, OUTPUT);
    pinMode(_latchPin, OUTPUT);
}

// Set the duty cycle for a specific pin
void ShiftPWM::setPWM(uint8_t pin, uint8_t dutyCycle) {
    if (pin < _numOutputs) {
        _dutyCycles[pin] = dutyCycle;
    }
}

// Update the shift register outputs
void ShiftPWM::update() {
    unsigned long currentMillis = millis();
    if (currentMillis - _lastUpdateTime >= _pwmPeriod) {
        _lastUpdateTime = currentMillis;
    }

    unsigned long phaseMillis = currentMillis - _lastUpdateTime;


    // Update each pin's state
    for (uint8_t i = 0; i < _numOutputs; i++) {
        if (phaseMillis < _dutyCycles[i] * (_pwmPeriod / 255.0)) {
            _state[i / 8] |= (1 << (i % 8)); // Set the bit
            Serial.print("+");
        } else {
            _state[i / 8] &= ~(1 << (i % 8)); // Clear the bit
            Serial.print(".");
        }
    }

    // Shift out the data
    digitalWrite(_latchPin, LOW); // Disable latch
    for (int i = (_numOutputs / 8 + (_numOutputs % 8 != 0)) - 1; i >= 0; i--) {
        shiftOut(_dataPin, _clockPin, MSBFIRST, _state[i]);
    }
    digitalWrite(_latchPin, HIGH); // Enable latch
}
