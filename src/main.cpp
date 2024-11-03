/*
 * main.cpp
 * Binary Counter
 * Started: 02.11.2024
 * Edited:  03.11.2024
 * Copyright Tauno Erik 2024
 */
#include <Arduino.h>
#include <LedControl.h> // MAX7219

#define SERIAL_SPEED 9600
#define PRINT_INTERVAL 1000
#define MATRIX_INTERVAL 1000
#define DIN_PIN 11
#define CLK_PIN 13
#define CS_PIN 10
#define NUM_OF_MATRIXES 1
#define MATRIX_ADDR 0
#define MATRIX_BRIGHTNESS 8 // Set brightness level (0-15)

int green_leds[8] = {
    0b10101010, // Row 0
    0b01010101, // Row 1
    0b10101010, // Row 2
    0b01010101, // Row 3
    0b10101010, // Row 4
    0b01010101, // Row 5
    0b10101010, // Row 6
    0b01010101, // Row 7
};

int yellow_leds[8] = {
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

LedControl matrix = LedControl(DIN_PIN, CLK_PIN, CS_PIN, NUM_OF_MATRIXES);

/*************************************************************************
 * Function declarations:
 ************************************************************************/
int count();
void one_by_one_all();
void one_by_one_yellows();
void one_by_one_greens();

void setup()
{
  Serial.begin(SERIAL_SPEED);

  matrix.shutdown(MATRIX_ADDR, false); // Wake up the display
  matrix.brightness(MATRIX_ADDR, MATRIX_BRIGHTNESS);
  matrix.clearDisplay(MATRIX_ADDR); // Clear the display
}

void loop()
{
  static unsigned long old_time = 0;
  static unsigned long old_matrix_time = 0;
  static bool to_print = false;
  static bool change_matrix = false;

  unsigned long current_time = millis();

  // Serial Print time
  if (current_time - old_time >= PRINT_INTERVAL)
  {
    to_print = true;
    old_time = current_time;
  }

  // Matrix time
  if (current_time - old_matrix_time >= MATRIX_INTERVAL)
  {
    Serial.println("Matrix time");
    change_matrix = true;
    old_matrix_time = current_time;
  }

  // Print
  if (to_print)
  {
    int num = count();
    Serial.print(num);
    Serial.print("\n");
    to_print = false;
  }

  // Matrix
  if (change_matrix)
  {
    change_matrix = false;
    one_by_one_all();
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

  matrix.setRow(MATRIX_ADDR, row, bit);

  bit = bit << 1;
  col++;
}
