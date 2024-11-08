/*
 Copyright 2024 Tauno Erik
 Started 02.03.2024
 Edited  06.11.2024
*/
#ifndef TAUNO_DEBUG_H_
#define TAUNO_DEBUG_H_

// Uncomment the line below to enable debugging
// #define DEBUG_ENABLED 1

#if DEBUG_ENABLED == 1
  #define DEBUG_BEGIN(baud) Serial.begin(baud)
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINT_BIN(x) Serial.print(x, BIN)
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_PRINTF(fmt, ...) Serial.printf(fmt, __VA_ARGS__)
#else
  #define DEBUG_BEGIN(baud) // Do nothing
  #define DEBUG_PRINT(x)    // Do nothing
  #define DEBUG_PRINTLN(x)  // Do nothing
  #define DEBUG_PRINTF(fmt, ...) // Do nothing
#endif

#endif  // TAUNO_DEBUG_H_
