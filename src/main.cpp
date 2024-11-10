/*
 * main.cpp
 * Binary Counter
 * Started: 02.11.2024
 * Edited:  09.11.2024
 * Copyright Tauno Erik 2024
 */
#include <Arduino.h>
#include <LedControl.h> // MAX7219
#include <FastLED.h>
#include "Tauno_PIR.h"
#define DEBUG_ENABLED 1
#include "tauno_debug.h"
#include "Tauno_shift_register.h"

#define SERIAL_SPEED 9600

#define PRINT_INTERVAL 1000
#define MATRIX_INTERVAL 500

#define MATRIX_DIN_PIN 11  // PB3
#define MATRIX_CLK_PIN 13  // PB5
#define MATRIX_CS_PIN 10  // PB2
#define NUM_OF_MATRIXES 1
#define MATRIX_ADDR 0
#define MATRIX_BRIGHTNESS 8 // Set brightness level (0-15)
#define LEFT_PIR_PIN 12  // PB4
#define RIGHT_PIR_PIN 9  // PB1
#define NUM_RGB_LEDS 12
#define RGB_DATA_PIN 4  // PD4

#define SHIFT_DATA_PIN 5   // PD5
#define SHIFT_CLOCK_PIN 7  // PD7
#define SHIFT_LATCH_PIN 6  // PD6
#define SHIFT_NUM_OUTPUTS 8
#define SHIFT_PWM_PERIOD 10 //100Hz
#define OFF false
#define ON true

uint8_t green_leds[8] = {
    0b10101010, // Row 0
    0b01010101, // Row 1
    0b10101010, // Row 2
    0b01010101, // Row 3
    0b10101010, // Row 4
    0b01010101, // Row 5
    0b10101010, // Row 6
    0b01010101, // Row 7
};

uint8_t yellow_leds[8] = {
    0b01010101, // Row 0
    0b10101010, // Row 1
    0b01010101, // Row 2
    0b10101010, // Row 3
    0b01010101, // Row 4
    0b10101010, // Row 5
    0b01010101, // Row 6
    0b10101010, // Row 7
};

int row = 0;
int col = 1;
uint8_t bit = 0b00000001;

LedControl matrix = LedControl(MATRIX_DIN_PIN, MATRIX_CLK_PIN, MATRIX_CS_PIN, NUM_OF_MATRIXES);

Tauno_PIR left_pir(LEFT_PIR_PIN);
Tauno_PIR right_pir(RIGHT_PIR_PIN);

bool all_greens_on = false;
bool all_yellows_on = false;

CRGB rgbleds[NUM_RGB_LEDS];

Tauno_shift_register flower(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, SHIFT_LATCH_PIN, SHIFT_NUM_OUTPUTS, SHIFT_PWM_PERIOD);

//#include <ShiftPWM.h>
//ShiftPWM pwm(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, SHIFT_LATCH_PIN, SHIFT_NUM_OUTPUTS, SHIFT_PWM_PERIOD);

/*************************************************************************
 * Function declarations:
 ************************************************************************/
int count();
void one_by_one_all();
void one_by_one_yellows();
void one_by_one_greens();
bool is_bit_high(uint8_t bitMask, uint8_t arr[], int size);
void all_yellows();
void all_greens();
void clear_display();

void setup()
{
  DEBUG_BEGIN(SERIAL_SPEED);

  matrix.shutdown(MATRIX_ADDR, false); // Wake up the display
  matrix.brightness(MATRIX_ADDR, MATRIX_BRIGHTNESS);
  matrix.clearDisplay(MATRIX_ADDR); // Clear the display

  left_pir.begin();
  right_pir.begin();

  FastLED.addLeds<NEOPIXEL, RGB_DATA_PIN>(rgbleds, NUM_RGB_LEDS);
  FastLED.setBrightness(50);
  FastLED.clear();
  FastLED.show();

  flower.begin();
  flower.set_PWM(128); // 50% brightness

  flower.set_pin(0, OFF);
  flower.set_pin(1, ON);
  flower.set_pin(2, OFF);
  flower.set_pin(3, ON);
  flower.set_pin(4, ON);
}

static uint8_t d = 0b10000001;

void loop()
{
  static unsigned long old_time = 0;
  static unsigned long old_matrix_time = 0;

  static bool to_print = false;
  static bool change_matrix = false;

  unsigned long current_time = millis();

  flower.run();

  // RGB LEDS
  for(int i = 0; i < NUM_RGB_LEDS; i++)
  {
    rgbleds[i] = CRGB::Yellow;
  }

  FastLED.show();

  // MOTION
  if (left_pir.is_motion())
  {
    DEBUG_PRINTLN("Left PIR");
    clear_display();
    all_yellows();
  }

  if (right_pir.is_motion())
  {
    DEBUG_PRINTLN("Right PIR");
    clear_display();
    all_greens();
  }

  // Serial Print time
  if (current_time - old_time >= PRINT_INTERVAL)
  {
    to_print = true;
    old_time = current_time;
  }

  // Matrix time
  if (current_time - old_matrix_time >= MATRIX_INTERVAL)
  {
    // Serial.println("Matrix time");
    change_matrix = true;
    old_matrix_time = current_time;
  }

  // Print
  if (to_print)
  {
    int num = count();
    // Serial.print(num);
    // Serial.print("\n");
    uint8_t pin = num % 10;
    flower.pins_off();
    flower.set_pin(pin, ON);
    to_print = false;
  }

  // Matrix
  if (change_matrix)
  {
    change_matrix = false;
    // one_by_one_all();
    // one_by_one_yellows();
    // one_by_one_greens();
    //all_yellows();
  }
  
}

/*************************************************************************
 * Fnction definitions:
 ************************************************************************/

int count()
{
  static int counter = 0;
  counter++;
  return counter;
}

void one_by_one_all()
{
  static int row = 0;
  static int col = 1;
  static uint8_t bit = 0b00000001;

  if (col > 8) // End of row
  {
    matrix.setRow(MATRIX_ADDR, row, 0b00000000); // All off
    col = 1;
    row++;
    bit = 0b00000001;
  }

  if (row >= 8)
  {
    row = 0;
  }

  matrix.setRow(MATRIX_ADDR, row, bit);

  bit = bit << 1;
  col++;
}

void one_by_one_yellows()
{
  static int row = 0;
  static int col = 1;
  static uint8_t bit = 0b00000001;

  if (col > 8) // End of row
  {
    matrix.setRow(MATRIX_ADDR, row, 0b00000000); // All off
    col = 1;
    row++;
    bit = 0b00000001;
  }

  if (row >= 8)
  {
    row = 0;
  }

  bool is_yellow = is_bit_high(bit, yellow_leds, row);

  if (is_yellow)
  {
    matrix.setRow(MATRIX_ADDR, row, bit);
  }
  // else time jump?!

  bit = 1 << col;
  col++;
}


void one_by_one_greens()
{
  static int row = 0;
  static int col = 1;
  static uint8_t bit = 0b00000001;

  if (col > 8) // End of row
  {
    matrix.setRow(MATRIX_ADDR, row, 0b00000000); // All off
    col = 1;
    row++;
    bit = 0b00000001;
  }

  if (row >= 8)
  {
    row = 0;
  }

  bool is_green = is_bit_high(bit, green_leds, row);

  if (is_green)
  {
    matrix.setRow(MATRIX_ADDR, row, bit);
  }
  // else time jump?!

  bit = 1 << col;
  col++;
}


/*
 * 
 */
bool is_bit_high(uint8_t bitMask, uint8_t arr[], int row)
{
  DEBUG_PRINT("arr ");
  DEBUG_PRINT_BIN(arr[row]);
  DEBUG_PRINT(" mask ");
  DEBUG_PRINT_BIN(bitMask);
  if (arr[row] & bitMask)
  { // Check if the specific bit is high in this element
    DEBUG_PRINTLN(" true");
    return true; // Bit is high in at least one array element
  }
  DEBUG_PRINTLN(" false");
  return false; // Bit is not high in any array element
}


/*
 * All yellowa ON
 */
void all_yellows() {
  if (!all_yellows_on)
  {
    DEBUG_PRINT("Set all yellows ON");
    all_yellows_on = true;
    for (int row = 0; row < 8; row++){
      matrix.setRow(MATRIX_ADDR, row, yellow_leds[row]);
    }
  }
}


/*
 * All greens ON
 */
void all_greens() {
  if (!all_greens_on)
  {
    DEBUG_PRINTLN("Set all greens ON");
    all_greens_on = true;
    for (int row = 0; row < 8; row++){
      matrix.setRow(MATRIX_ADDR, row, green_leds[row]);
    }
  }
}

/*
 * Turn display off
 */
void clear_display() {
  DEBUG_PRINTLN("Clear display");
  matrix.clearDisplay(MATRIX_ADDR);
  all_greens_on = false;
  all_yellows_on = false;
}
