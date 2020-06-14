/* --------------------------------------------------------------------------------------------------
 * LED CLOCK
 * Program for controling clock - 2 rings with addressable led strips
 * 
 * Ver.:  8.1
 * Auth.: Grega Morano
 * Date.: 7.6.2020
 * 
 * --------------------------------------------------------------------------------------------------
 * User can change modes of the clock with BTN 1, types with BTN 2
 * 4 Modes with different types:
 * 	- MODE 1: (default) clock where user can define colors and brightness. User can circle through 
 * 	  different types with button 2 - for type. Values are changed with potenciometter. 
 * 			TYPE 1: Set brightness of the clock
 * 			TYPE 2: Set compas color
 * 			TYPE 3: Set minute indicator color
 * 			TYPE 4: Set hour indicator color
 * 
 * - MODE 2: Display clock with indicator for seconds. Here colors are predefined, but user can change
 * 	 all of them together. It can be done same as before, with BNT2 for type in circular order. Values 
 *   are changed with potenciometter. 
 * 			TYPE 1: Set brightness of the clock
 * 			TYPE 2: Set color of the clock
 * 
 * - MODE 3: Some colors with LED-S
 * 			TYPE 1: Rainbow circling around - user can change its speed with pot.
 * 			TYPE 2: Sinelon back and forward - user can change its color with pot.
 * 			TYPE 3: Chasing dots - user can just observe dots chasing each other.
 * 			TYPE 4: Fireworks - user must observe all 3 rockets - it can't be changed, but it will
 * 					automatically shifted back to rainbow (type 1) after 3 rockets.
 * 
 * - MODE 4: Thermometer 
 * 
 * If both buttons are pressed at the same time, user can change the time of the clock
 * If both buttons are pressed before startup, clock will go in the mantainance mode - it will display
 * compas (for mounting purpose) and later all effect between hours (demonstration purpose)
 *-----------------------------------------------------------------------------------------------------*/



// TODO: testjrej BD show zdej ku si spremenu ku nej vč for zanke...tudi mantainance mode je zdej drgačn
// TODO: Fix compiler warnings

/* ----------------------------------------------------------------------------------------------------*/
#include "LEDclock.h"
#include "ring.h"

/* ----------------------------------------------------------------------------------------------------*/
/*-------------- GLOBAL VARIABLES ---------------------------------------------------------------------*/
/* ----------------------------------------------------------------------------------------------------*/

// DS3231 clock module
DS3231  Clock;

// FastLED array for led manipulation
CRGB HourLeds[NUM_LED_HOUR];
CRGB MinLeds[NUM_LED_MIN];

// Ring class defines
MinRing  mRing;
HourRing hRing;


// Variable for clock modes
uint8_t clock_mode = 0;

// Variable for different types of same mode
uint8_t clock_type = 0;

// Value of photo resistor
int photo_val = 1023;

// Variable for color of the clock
user_color clock_color;

/* ----------------------------------------------------------------------------------------------------*/
/*--------------- FUNCTION DECLARATIONS ---------------------------------------------------------------*/
/* ----------------------------------------------------------------------------------------------------*/
void CLOCK_FadeToBlack			(void);
void CLOCK_SetBrightness		(void);

void CLOCK_DisplayTime			(uint8_t mode, uint8_t val, uint8_t type);
void CLOCK_DisplayLamp			(uint8_t val, uint8_t type);
void CLOCK_DisplayTemperature	(void);

void CLOCK_DisplayFullHour		(uint8_t hue);
void CLOCK_DisplayHalfHour		(uint8_t hue);
void CLOCK_DisplayQuarter		(uint8_t hue, myTime t);
void CLOCK_DisplayTriQuarter	(uint8_t hue, myTime t);
void CLOCK_DisplayFirework		(void);

void CLOCK_SetTime				(void);
void CLOCK_MaintainanceMode		(void);


/* ----------------------------------------------------------------------------------------------------*/
/*--------------- SETUP -------------------------------------------------------------------------------*/
/* ----------------------------------------------------------------------------------------------------*/
void setup() {
	delay(100);

#if DEBUG
	// Initialize serial connection - debugging purposes  
	Serial.begin(115200);

	Serial.println("Debugging state is on!");

	SERIAL_DEBUG(Number of clock modes:, NUM_OF_CLOCK_MODES);
	SERIAL_DEBUG(Number of clock types:, NUM_OF_CLOCK_TYPES);
	SERIAL_DEBUG(Will clock use photo resistor?, USE_PHOTO_RESISTOR);
#endif

	// Initialize GPIO pins for buttons
	pinMode(PIN_BTN1, INPUT_PULLUP);
	pinMode(PIN_BTN2, INPUT_PULLUP);

	// Begin with I2C connection (for DS3231 module)
	Wire.begin();

	// Configure clock module
	Clock.setClockMode(true);  // set to 12h format

	Clock.setYear(2020);
	Clock.setMonth(6);
	Clock.setDate(1);
	Clock.setHour(CLOCK_DEFAULT_HOUR);
	Clock.setMinute(CLOCK_DEFAULT_MIN);
	Clock.setSecond(CLOCK_DEFAULT_SEC);

	// Set initial values of the user clock color
	clock_color.compas = 255;
	clock_color.hour_color = 200;
	clock_color.min_color = 200;

	// If both BTN are pressed durring startup, go to maintainance mode
	if(!digitalRead(PIN_BTN1) && !digitalRead(PIN_BTN2))
	{
		CLOCK_MaintainanceMode();
	}  

	FastLED.setBrightness(START_BRIGHTNESS);
}


/* ----------------------------------------------------------------------------------------------------*/
/*--------------- MAIN LOOP ---------------------------------------------------------------------------*/
/* ----------------------------------------------------------------------------------------------------*/
void loop() {

	/*---------VARIABLE DECLARATION -------------------------------------------------------------------*/
	// Potentiometer variables
	static int pot_val[5];
	static uint8_t pot_count = 0;
	static int avg_val = 0;
	static uint8_t analog_val = 0;

	// Buttons variables
	static uint8_t btn1_last_state = 1;
	static uint8_t btn2_last_state = 1;
	static uint8_t btn1_state = 1;
	static uint8_t btn2_state = 1;
	static unsigned long btn1_debounce_time;
	static unsigned long btn2_debounce_time;


	/*---------BUTTON STATE CHECK------------------------------*/
	// Source: https://learn.adafruit.com/make-it-switch/debouncing 

	uint8_t btn1_read = digitalRead(PIN_BTN1);
	uint8_t btn2_read = digitalRead(PIN_BTN2);

	// Check if the new state of the button is different from the last state
	if(btn1_read != btn1_last_state)
	{
		btn1_debounce_time = millis();
	}
	if(btn2_read != btn2_last_state)
	{
		btn2_debounce_time = millis();
	}

	// If BTN 1 is pressed (time between state changes must be greather than 50ms)
	if((millis() - btn1_debounce_time) > 50)
	{
		if(btn1_read != btn1_state)
		{
			btn1_state = btn1_read;
			if(btn1_state == LOW)
			{	
				//If later BTN 2 is also pressed
				if(btn2_state == LOW){
					CLOCK_SetTime();
				}
				else{
					// Change mode in circular order
					clock_mode ++;
					if(clock_mode >= NUM_OF_CLOCK_MODES){
						clock_mode = 0;
					}
					// Reset type counter so it allways starts from begining
					clock_type = 0; 

					// Turn the leds off durring mode transition
					CLOCK_FadeToBlack();

					SERIAL_DEBUG(Mode:, clock_mode);
				}   
			}
		}
	}
	//If BTN 2 is pressed (time between state changes must be greather than 50ms)
	if((millis() - btn2_debounce_time) > 50)
	{
		if(btn2_read != btn2_state)
		{
			btn2_state = btn2_read;
			if(btn2_state == LOW)
			{
				//If later BTN 1 is also pressed
				if(btn1_state == LOW){
					CLOCK_SetTime();
				}
				else{
					//change type in circular order
					clock_type ++;
					if(clock_type >= NUM_OF_CLOCK_TYPES){
						clock_type = 0;
					}
					SERIAL_DEBUG(Type:, clock_type);
				}        
			}
		}  
	} 
	btn1_last_state = btn1_read;
	btn2_last_state = btn2_read;

	/*---------GET POTENTIOMETER VALUE------------------------------*/
	// Average reading of 5 potentiometer values, because potentiometer varies up/down

	pot_val[pot_count] = analogRead(PIN_POT);
	pot_count++;

	if(pot_count == 5){
		avg_val = 0;
		for (int i=0; i<5; i++){
			avg_val = avg_val + pot_val[i];
		}
		avg_val = avg_val/5;
		pot_count = 0;
	}

	analog_val = map(avg_val,0,1023,0,255);
  
	//SERIAL_DEBUG(Pot value:, analog_val);

	/*---------SET BRIGHTNESS ACORDING TO PHOTO RESISTOR--------------*/
#if USE_PHOTO_RESISTOR
	photo_val = analogRead(PIN_PHOTO);

	//SERIAL_DEBUG(Photo value:, photo_val);

	photo_val = map(photo_val,0,1023,0,255);
	FastLED.setBrightness(photo_val);
	if(photo_val < 100){
		FastLED.setBrightness(0);
	}

	/*
	if(photo_val < 120){
		FastLED.setBrightness(0);
	}
	else if(photo_val > 900){
		FastLED.setBrightness(200);
	}
	else{
		FastLED.setBrightness(100);
	}
	*/
#endif

	/*---------DISPLAY LEDS IN GIVEN MODE-------------------------------*/
	switch(clock_mode){
		case 0:
			CLOCK_DisplayTime(0, analog_val, clock_type);
			break;

		case 1:
			CLOCK_DisplayTime(1, analog_val, clock_type);
			break;

		case 2:
			CLOCK_DisplayTime(2, analog_val, clock_type);
			break;

		case 3:
			CLOCK_DisplayLamp(analog_val, clock_type);
			break;  

		case 4:
			CLOCK_DisplayTemperature();
			break;

		default:
			CLOCK_DisplayLamp(analog_val, clock_type);
			break;  
	}  

	FastLED.show();
}


/* ----------------------------------------------------------------------------------------------------*/
/*--------------- FUNCTION DEFINITIONS ----------------------------------------------------------------*/
/* ----------------------------------------------------------------------------------------------------*/


// Slowly fade away
void CLOCK_FadeToBlack(void)
{
	for(uint8_t i = 0; i < 50; i++)
	{
		for(uint8_t j = 0; j < NUM_LED_MIN; j++)
		{
			MinLeds[j].fadeToBlackBy(16);
		}
		for(uint8_t k = 0; k < NUM_LED_HOUR; k++)
		{
			HourLeds[k].fadeToBlackBy(16);
		}
		FastLED.delay(10);
	}
	mRing.setBlack();
	hRing.setBlack();
}


/* ----------------------------------------------------------------------------------------------------*/
// MODE 0: Predefined colors - with milliseconds
// MODE 1: Variable colors -  with seconds
// MODE 2: User colors - without seconds
void CLOCK_DisplayTime(uint8_t mode, uint8_t val, uint8_t type)
{

	static int color = 0;
	static int brightness = START_BRIGHTNESS;
	bool h12 = false;
	myTime t;

	// Get precise time from DS3231 module
	t.hour = Clock.getHour(h12,h12);
	t.min = Clock.getMinute();
	t.sec = Clock.getSecond();

	// SERIAL_DEBUG("Time is:", t.hour);

	// Update position of clock pixels
	hRing.updateClockPosition(t);
	mRing.updateClockPosition(t);

	switch(mode){
		case 0:
			// Toggle between Color and Brightness settings - depends on BTN2
			switch(type){
				case 0:
				case 2:
					color = val;
					break;
				
				case 1:
				case 3:
					brightness = val;
					break;
				
				default:
					break;
			}

			// Prepare colors for clock
			mRing.displayClockPredefinedColor(color);
			hRing.displayClockPredefinedColor(color);// Prepare colors

			// Adjust brightness
			FastLED.setBrightness(brightness);
			break;
			
		case 1:
			// Toggle between Color and Brightness settings - depends on BTN2
			switch(type){
				case 0:
				case 2:
					color = val;
					break;
				
				case 1:
				case 3:
					brightness = val;
					break;
				
				default:
					break;
			}

			mRing.displayClockVariableColor(color);
			hRing.displayClockVariableColor(color);
			
			// Adjust brightness
			FastLED.setBrightness(brightness);
			break;

		case 2:

			// Get user colors - depending on type button (BTN2)
			switch(type){
				case 0:
					clock_color.compas = val;
					break;

				case 1:
					clock_color.min_color = val;
					break;

				case 2:
					clock_color.hour_color = val;
					break;
				
				case 3:
					brightness = val;
					break;

				default:
					break;
			}

			// Prepare colors for clock
			mRing.displayClockUserColor(clock_color);
			hRing.displayClockUserColor(clock_color);

			// Adjust brightness
			FastLED.setBrightness(brightness);
			break;
	}


	// Disable special effects durring night
	// So it doesn't wake up whole neighbour-hood 
	if(photo_val > 100 && (t.hour > 8 && t.hour < 23))
	{
		// Every 15min display one fo the effects
		if((t.min == 0) && (t.sec == 5))
		{
			CLOCK_DisplayFullHour(hRing.getCompasHue());
		}
		else if((t.min == 15) && (t.sec == 5))
		{
			CLOCK_DisplayQuarter(hRing.getCompasHue(), t);
		}
		else if((t.min == 30) && (t.sec == 5))
		{
			CLOCK_DisplayHalfHour(hRing.getCompasHue());
		}
		else if((t.min == 45) && (t.sec == 5))
		{
			CLOCK_DisplayTriQuarter(hRing.getCompasHue(), t);
		}
		else if((t.hour == 13) && (t.min == 20) && (t.sec == 5))
		{	
			for(uint8_t i=0; i<7; i++){
				CLOCK_DisplayFirework();
			}
		}
	}

	// For testing purpose
	#if DEBUG
		if((t.sec == 5) || (t.sec == 35)){

		}
	#endif
}


/* ----------------------------------------------------------------------------------------------------*/
void CLOCK_DisplayFullHour(uint8_t hue)
{
	CLOCK_FadeToBlack();
	mRing.rainbowFillin(hue);
}


/* ----------------------------------------------------------------------------------------------------*/
void CLOCK_DisplayQuarter(uint8_t hue, myTime t)
{
	int start_px_hour = (t.hour%12);
	int start_px_min  = 15;

	//same as CLOCK_FadeToBlack() but keep 15min compas
	for(int i=0; i<50; i++){
		for(int j=0; j<NUM_LED_MIN; j++){
			MinLeds[j].fadeToBlackBy(16);
			mRing.setHSV(14, hue, 250, 150);
			mRing.setHSV(15, hue, 250, 255);
			mRing.setHSV(16, hue, 250, 150);
		}
		for(int k=0; k<NUM_LED_HOUR; k++){
			HourLeds[k].fadeToBlackBy(16);
			hRing.setHSV(start_px_hour, hue, 250, 250);
			hRing.setHSV(start_px_hour+1, hue, 250, 250);
		}
		FastLED.delay(10);
	}


	mRing.colorWipe(hue, 40, start_px_min);
	hRing.colorWipe(hue, 40, start_px_hour);

	FastLED.delay(300);

	hRing.colorWipe(-1, 30, start_px_hour);
	mRing.colorWipe(-1, 30, start_px_min);
}


/* ----------------------------------------------------------------------------------------------------*/
void CLOCK_DisplayHalfHour(uint8_t hue)
{
	//CLOCK_FadeToBlack() but keep half hour compas
	for(int i=0; i<50; i++){
		for(int j=0; j<NUM_LED_MIN; j++){
			MinLeds[j].fadeToBlackBy(16);

			mRing.setHSV(31, hue, 255, 250);
			mRing.setHSV(30, hue-9, 255, 250);
			mRing.setHSV(29, hue, 255, 250);
		}
		for(int k=0; k<NUM_LED_HOUR; k++){
			HourLeds[k].fadeToBlackBy(16);
		}
	FastLED.delay(10);
	}

	mRing.rainbowFaling(hue);
	CLOCK_FadeToBlack();  
}


/* ----------------------------------------------------------------------------------------------------*/
void CLOCK_DisplayTriQuarter(uint8_t hue, myTime t)
{	
	int start_px_hour = (t.hour%12);

	CLOCK_FadeToBlack();  

	mRing.quarterWipe(hue, 60);
	FastLED.delay(50);
	mRing.quarterWipe(-1, 60);

	hRing.colorWipe(hue, 40, start_px_hour);
	FastLED.delay(50);
	mRing.quarterWipe(hue, 60);
	hRing.colorWipe(-1, 30, start_px_hour);
	mRing.quarterWipe(-1, 60);

	CLOCK_FadeToBlack(); 
}


/* ----------------------------------------------------------------------------------------------------*/
void CLOCK_DisplayLamp(uint8_t val, uint8_t type){
	static int rainbow_val = 0;
	static int rainbow_count = 0;

	static uint8_t pos;
	uint8_t hour_br;

	byte dothue = 0;

	switch(type){
		// Rainbow cycling in a circle 
		case 0:
			mRing.rainbow(rainbow_val);
			hRing.rainbow(rainbow_val);

			// Costum Delay for rainbow to cycle
			rainbow_count++;
			if (rainbow_count > (val/10)+1 ){
				rainbow_val+=2;
				rainbow_count = 0;
			}
			if (rainbow_val >= 255){
				rainbow_val = 0;
			}
			break;


		// Sinelon left and right whit
		case 1:
			// Minute ring
			pos = beatsin8(15, 0, NUM_LED_MIN-1);
			MinLeds[pos] = CHSV(val, 200, 255);
			nscale8(MinLeds,NUM_LED_MIN,249); 

			hour_br = map(pos, 0, 59, 0, 255);
			// Low quality LEDs go funky at low brightness
			if(hour_br < 25){
				hour_br = 0;
			}

			hRing.setAllHSV(val, 240, hour_br);
			//nscale8(HourLeds,NUM_LED_HOUR,252);
			break;


		// Chasing dots
		case 2:   
			nscale8(MinLeds,NUM_LED_MIN,150); 
			for(int i=0; i<8; i++){
				MinLeds[beatsin16(i+7,0,NUM_LED_MIN-1)] |= CHSV(dothue,200,255);
				dothue += 32;
			}
			hRing.setBlack();
			break;
			

		// Firework (one time)
		case 3:
			CLOCK_DisplayFirework();
			// After firework, go back to rainbow
			clock_type = 0;
			FastLED.delay(100);
			break;	
			

		/*All leds glow in one colour - depends on potenciometer state*/
		default:
			if(val > 253){
				mRing.setWhite();
				hRing.setWhite();
			}
			else{
				mRing.setAllHSV(val,255,220);
				hRing.setAllHSV(val,255,220);
			}
			break;
	}
}


/* ----------------------------------------------------------------------------------------------------*/
void CLOCK_DisplayTemperature(void){

	// Get temperature from DS3213 module
	int temp = Clock.getTemperature();

	// Set all LEDs to black
	hRing.setBlack();
	mRing.setBlack();

	// Display temperature on minute ring
	if(temp < 18){
		fill_gradient(MinLeds, temp, CHSV(175,255,255), CHSV(171,255,255), SHORTEST_HUES);
		hRing.setAllHSV(175, 255, 255);
	}
	else if(temp > 26){
		fill_gradient(MinLeds, temp, CHSV(175,255,255), CHSV(0,255,255), LONGEST_HUES);
		hRing.setAllHSV(0, 255, 255);

	}
	else{
		switch(temp){
			case 18:
				fill_gradient(MinLeds, temp, CHSV(175,255,255), CHSV(169,255,255), SHORTEST_HUES);
				hRing.setAllHSV(171, 255, 255);
				break;

			case 19:
				fill_gradient(MinLeds, temp, CHSV(175,255,255), CHSV(150,255,255), SHORTEST_HUES);
				hRing.setAllHSV(150, 255, 255);
				break;

			case 20:
				fill_gradient(MinLeds, temp, CHSV(175,255,255), CHSV(128,255,255), SHORTEST_HUES);
				hRing.setAllHSV(128, 255, 255);
				break;
			
			case 21:
				fill_gradient(MinLeds, temp, CHSV(175,255,255), CHSV(103,255,255), SHORTEST_HUES);
				hRing.setAllHSV(103, 255, 255);
				break;
			
			case 22:
				fill_gradient(MinLeds, temp, CHSV(175,255,255), CHSV(80,255,255), SHORTEST_HUES);
				hRing.setAllHSV(80, 255, 255);
				break;
			
			case 23:
				fill_gradient(MinLeds, temp, CHSV(175,255,255), CHSV(60,255,255), SHORTEST_HUES);
				hRing.setAllHSV(60, 255, 255);
				break;
			
			case 24:
				fill_gradient(MinLeds, temp, CHSV(175,255,255), CHSV(42,255,255), LONGEST_HUES);
				hRing.setAllHSV(42, 255, 255);
				break;
			
			case 25:
				fill_gradient(MinLeds, temp, CHSV(175,255,255), CHSV(20,255,255), LONGEST_HUES);
				hRing.setAllHSV(20, 255, 255);
				break;
			
			case 26:
				fill_gradient(MinLeds, temp, CHSV(175,255,255), CHSV(1,255,255), LONGEST_HUES);
				hRing.setAllHSV(1, 255, 255);
				break;
			
			default:
				break;
		}
	}
}


/* ----------------------------------------------------------------------------------------------------*/
void CLOCK_SetTime(void){
	int pot_hour;
	int pot_min;

	CLOCK_FadeToBlack();

	/*---------HOUR SETUP -----------------------------------------------------------------------------*/
	hRing.displayCompas(128); //aqua compas
	FastLED.show();

	// Wait until the button is released 
	// user still holds it couple of seconds after compas is displayed
	while(!digitalRead(PIN_BTN1)){}

	FastLED.delay(300);

	// Loop until one of the button is pressed
	while(digitalRead(PIN_BTN1) == HIGH && digitalRead(PIN_BTN2) == HIGH){

		// Get average value of 5 readings and map it
		pot_hour = 0;
		for(uint8_t i = 0; i < 5; i++){
			// I have lousy potentiometer...high value readings don't happen often
			// Thats why manually add 10 to the reading, so the value of 1023 appears more often
			pot_hour += (analogRead(PIN_POT) + 10);	
		}
		pot_hour /= 5;
		pot_hour = map(pot_hour, 0, 1023, 0, 11);

		SERIAL_DEBUG("Hour pot", pot_hour);

		nscale8(HourLeds,NUM_LED_HOUR,250);

		hRing.displayCompas(128);
		hRing.setHSV(pot_hour*2, 40, 255, 255);   
		hRing.setHSV(pot_hour*2+1, 40, 255, 255);

		FastLED.show();
	}

	FastLED.delay(500);

	/*---------MINUTE SETUP ---------------------------------------------------------------------------*/
	mRing.displayCompasBig(128);
	FastLED.delay(500);

	// Loop until one of the button is pressed
	while(digitalRead(PIN_BTN1) == HIGH  && digitalRead(PIN_BTN2) == HIGH){

		// Get average value of 5 readings and map it
		pot_min = 0;
		for(uint8_t i = 0; i < 5; i++){
			pot_min += (analogRead(PIN_POT) + 10); 
		}
		pot_min /= 5;
		pot_min = map(pot_min, 0, 1023, 0, NUM_LED_MIN-1);

		SERIAL_DEBUG("Min pos", pot_min);

		// Dim the lights for smooth effect
		nscale8(MinLeds, NUM_LED_MIN, 250);

		// Show position and compas
		mRing.displayCompasBig(128);
		mRing.setHSV(pot_min, 40, 255, 255);
		FastLED.show();
	}

	// Save the clock setting to the DS3231 module
	Clock.setHour(pot_hour);
	Clock.setMinute(pot_min);
	Clock.setSecond(0);

	// Reset clock mode and type
	clock_type = 0;
	clock_mode = 0;

	CLOCK_FadeToBlack();
}


/* ----------------------------------------------------------------------------------------------------*/
void CLOCK_MaintainanceMode(void)
{
	mRing.setBlack();
	hRing.setBlack();
	
	// Turn on the compas - for displaying position / orientation of LED strip
	mRing.displayCompasBig(255);
	hRing.displayCompas(255);

	FastLED.show();

	// Wait until the button is released (user still holds it couple of seconds after compas is displayed)
	while(!digitalRead(PIN_BTN1)){}

	FastLED.delay(300);

	// Wait until the button is pressed again
	while(digitalRead(PIN_BTN1)){}

	CLOCK_FadeToBlack();  

	// Display some of clock patterns
	myTime t;
	t.hour = 12;
	t.min = 15;
	CLOCK_DisplayQuarter(255, t);
	CLOCK_FadeToBlack();  
	if(digitalRead(PIN_BTN1) == 0){
		return;
	}

	CLOCK_DisplayHalfHour(150);
	CLOCK_FadeToBlack();  
	if(digitalRead(PIN_BTN1) == 0){
		return;
	}

	t.min = 45;
	CLOCK_DisplayTriQuarter(255, t);
	CLOCK_FadeToBlack();  
	if(digitalRead(PIN_BTN1) == 0){
		return;
	}

	CLOCK_DisplayFullHour(130);
	if(digitalRead(PIN_BTN1) == 0){
		return;
	}

	while(1){
		CLOCK_DisplayFirework();

		if(digitalRead(PIN_BTN1) == 0){
			break;
		}
	}	
}


/* ----------------------------------------------------------------------------------------------------*/
void CLOCK_DisplayFirework(void)
{
	CLOCK_FadeToBlack();  

	uint8_t racket_pos, racket_go = 0, racket_color = 35, racket_direction;

	//CLOCK_FadeToBlack(); 

	racket_direction = random8(2);

	FastLED.delay(300);

	mRing.setHSV(30, racket_color, 255, 255);
	FastLED.show();

	if(racket_direction){
		// Go right
		while(1){
			racket_pos = beatsin8(15, 0, 30);

			if(racket_pos == 30){
				racket_go = 1;
				FastLED.delay(100);
			}

			if(racket_go){
				mRing.setBlack();
				mRing.setHSV(racket_pos, racket_color, 255, 255);
				FastLED.show();

				if(racket_pos == 0){
					break;
				}
			}
		}
	} else{
		// Go left
		while(1)
		{
			racket_pos = beatsin8(15, 30, 59);

			if(racket_pos == 30){
				racket_go = 1;
				//FastLED.delay(100);
			}

			if(racket_go){
				mRing.setBlack();
				mRing.setHSV(racket_pos, racket_color, 255, 255);
				FastLED.show();

				if(racket_pos == 59){
					FastLED.delay(80);
					mRing.setBlack();
					mRing.setHSV(0, racket_color, 255, 255);
					FastLED.show();
					break;
				}
			}
		}
	}

	// Rocket on the top
	FastLED.delay(100);

	for(uint8_t i=0; i<20; i++){
		MinLeds[0].fadeToBlackBy(16);
		FastLED.delay(10);
	}

	// Explosion with hour ring
	hRing.setAllHSV(20,180,120);

	// Fade to black
	for(uint8_t i=0; i<40; i++){
		MinLeds[0].fadeToBlackBy(16);
		for(int k=0; k<NUM_LED_HOUR; k++){
			HourLeds[k].fadeToBlackBy(16);
		}
		FastLED.delay(30);
	}


	// Show the sparks
	uint8_t spark[6], spark_color[6];
	
	spark[0] = random8(1, 5);
	spark[1] = random8(55, 59);
	spark[2] = random8(5, 10);
	spark[3] = random8(50, 55);
	spark[4] = random8(10, 15);
	spark[5] = random8(45, 50);

	for(uint8_t i=0; i<6; i++){
		spark_color[i] = random8(0, 255);
	}

	for(uint8_t i = 0; i < 6; i++)
	{
		MinLeds[spark[i]].setHSV(spark_color[i], 250, 250);
		FastLED.show();
		FastLED.delay(random8(30,100));
	}
	

	// Drop them down
	uint8_t dilej = 50, sajn = 250;

	for(uint8_t down = 1; down < 14; down++)
	{
		fadeToBlackBy(MinLeds, NUM_LED_MIN, 200);
		for(uint8_t j = 0; j < 6; j++){
			if(j%2 == 0){
				spark[j]++;
			}
			else{
				spark[j]--;
			}	

			MinLeds[spark[j]].setHSV(spark_color[j], 250, sajn);
		}
		FastLED.show();
		FastLED.delay(dilej);
		dilej += 7;
		sajn -= 20;
	}
	mRing.setBlack();
	FastLED.show();
}