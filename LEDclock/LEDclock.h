/* ---------------------------------------------------------------------------------------------------
 * TODO
 * 
 * Ver.:  8.1
 * Auth.: Grega Morano
 * Date.: 7.6.2020
 * 
 * ---------------------------------------------------------------------------------------------------*/

#include <FastLED.h>
#include "DS3231.h"


/* ----------------------------------------------------------------------------------------------------*/
/*--------------- WIRING ------------------------------------------------------------------------------*/
/* ----------------------------------------------------------------------------------------------------*/
// See schematics for more information about wiring

#define PIN_RING_HOUR   3   // WS2812 (24 LEDs) DIN pin --> Arduino digital pin D3
#define PIN_RING_MIN    5   // WS2812 (60 LEDs) DIN pin --> Arduino digital pin D5
#define PIN_BTN1        7   // Button 1                 --> Arduino digital pin D7
#define PIN_BTN2        9   // Button 2                 --> Arduino digital pin D9
#define PIN_POT         A1  // Potentiometer            --> Arduino analog pin A1
#define PIN_PHOTO       A0  // Photo resistor           --> Arduino analog pin A0

// To use the hardware I2C interface of the Arduino (Uno or Nano) you must connect
// 								DS3231 SDA pin  		--> Arduino Analog A4 or the dedicated SDA pin
//         				DS3231 SCL pin   		--> Arduino Analog A5 or the dedicated SCL pin
//

/* ----------------------------------------------------------------------------------------------------*/
/*--------------- CLOCK CONFIGURATION -----------------------------------------------------------------*/
/* ----------------------------------------------------------------------------------------------------*/

// If you want to use photo resistor to automatically dim the LEDs durring night, set macro to 1
#define USE_PHOTO_RESISTOR		(1)

// Configure this number by your photo resistor - when is dark enough to turn the Clock off
// My values are: 0 when bright and 1000 when dark. I want to turn the LEDs off when photoresistor is  
// more than 860. You can use SERIAL_DEBUG (or Serial Monitor) to find your preffered value.
#define CLOCK_PHOTO_TRESHOLD	(860)


// Number of modes and defines that clock can have
#define NUM_OF_CLOCK_MODES		(5)
#define NUM_OF_CLOCK_TYPES		(4)


// If you want to see some debug messages in Serial Monitor, set DEBUG macro to 1
#define DEBUG               	(1)

#if DEBUG
    #define STR(s) (#s)
	#define SERIAL_DEBUG(s, x) Serial.print(STR(s)); \
							Serial.print(" "); \
							Serial.println((x));
#else
	#define SERIAL_DEBUG(s, x)
#endif



/* ----------------------------------------------------------------------------------------------------*/
/*--------------- DEFINES -----------------------------------------------------------------------------*/
/* ----------------------------------------------------------------------------------------------------*/

// Macros needed for FastLED library
#define LED_TYPE            WS2812
#define COLOR_ORDER         GRB
#define START_BRIGHTNESS    100
#define NUM_LED_HOUR        24
#define NUM_LED_MIN         60

// Default hour set at powerupp - you can change it
#define CLOCK_DEFAULT_HOUR	1
#define CLOCK_DEFAULT_MIN	20
#define CLOCK_DEFAULT_SEC	5
