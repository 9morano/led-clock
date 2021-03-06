/* --------------------------------------------------------------------------------------------------
 * TODO
 * 
 * Ver.:  8.1
 * Auth.: Grega Morano
 * Date.: 7.6.2020
 * 
 * --------------------------------------------------------------------------------------------------*/

#include "ring.h"


/* ----------------------------------------------------------------------------------------------------*/
/*--------------- HOUR RING CLASS ---------------------------------------------------------------------*/
/* ----------------------------------------------------------------------------------------------------*/

//Hour Ring constructor (init)
HourRing :: HourRing()
{
	//Serial.println("INIT Minute Led Strip");
	FastLED.addLeds<LED_TYPE,PIN_RING_HOUR,COLOR_ORDER>(HourLeds, NUM_LED_HOUR).setCorrection(TypicalLEDStrip);

	px_hou = 0;
}


/* ----------------------------------------------------------------------------------------------------*/
void HourRing :: updateClockPosition(myTime t)
{
	uint8_t tmp = 0;
	tmp = t.hour % 12; //from 24h format to 12h
	px_hou = tmp;
}


/* ----------------------------------------------------------------------------------------------------*/
void HourRing :: displayCompas(uint8_t hue)
{
	setHSV(0, hue, 250, 200);
	setHSV(1, hue, 250, 200);
	
	setHSV(6, hue, 250, 200);
	setHSV(7, hue, 250, 200);
	
	setHSV(12, hue, 250, 200);
	setHSV(13, hue, 250, 200);
	
	setHSV(18, hue, 250, 200);
	setHSV(19, hue, 250, 200);

	last_compas_hue = hue;
}

uint8_t HourRing :: getCompasHue(void)
{
	return last_compas_hue;
}

/* ----------------------------------------------------------------------------------------------------*/
void HourRing::displayClockVariableColor(int color)
{
	setBlack();

	//display compas of hours
	displayCompas(color);
		
	setHSV(px_hou*2, (color - 40), 255, 250);
	setHSV((px_hou*2)+1, (color - 40), 255, 250);
}


/* ----------------------------------------------------------------------------------------------------*/
void HourRing :: displayClockPredefinedColor(int fact)
{
	nscale8(HourLeds,NUM_LED_HOUR,253); 

	fact = fact/43;   // faktorji med 0 in 5

	switch(fact){
	//red 0
		case 0:
		displayCompas(0);//red
		setHSV(px_hou*2, 20, 255, 250);//orange
		setHSV((px_hou*2)+1, 20, 255, 250);//orange
		break;

	//yellow 42
	case 1:
		displayCompas(42);//yellow
		setHSV(px_hou*2, 160, 255, 250);//blue
		setHSV((px_hou*2)+1, 160, 255, 250);//blue
		break;

	//green 85
	case 2:
		displayCompas(82);//green
		setHSV(px_hou*2, 128, 255, 250);//aqua
		setHSV((px_hou*2)+1, 128, 255, 250);//aqua
		break;

	//aqua 128
	case 3:
		displayCompas(171);//blue
		setHSV(px_hou*2, 128, 255, 250);//aqua
		setHSV((px_hou*2)+1, 128, 255, 250);//aqua
		break;

	//blue 171
	case 4:
		displayCompas(171);//blue
		setHSV(px_hou*2, 208, 255, 255);//purple
		setHSV((px_hou*2)+1, 208, 255, 255);//purple
		break;

	//purple 213
	case 5:
		displayCompas(213);//purple
		setHSV(px_hou*2, 171, 255, 250);//blue
		setHSV((px_hou*2)+1, 171, 255, 250);//blue
		break;  
  }
}


/* ----------------------------------------------------------------------------------------------------*/
void HourRing :: displayClockUserColor(user_color uc)
{
	setBlack();

	//display compas of hours
	displayCompas(uc.compas);

	setHSV(px_hou*2, uc.hour_color, 255, 250);
	setHSV((px_hou*2)+1, uc.hour_color, 255, 250);
}


/* ----------------------------------------------------------------------------------------------------*/
//Set all leds to black - off
void HourRing :: setBlack(void)
{
	for(int i=0; i<NUM_LED_HOUR; i++){
		HourLeds[i] = CRGB :: Black;
	}
}


/* ----------------------------------------------------------------------------------------------------*/
void HourRing :: setWhite(void)
{
	for(int i=0; i<NUM_LED_HOUR; i++){
		HourLeds[i] = CRGB :: White;
	}
}


/* ----------------------------------------------------------------------------------------------------*/
//Set leds to same color in RGB order
void HourRing :: setRGB(int pixel, uint8_t r, uint8_t g, uint8_t b)
{
	HourLeds[pixel].setRGB(r, g, b);
}


/* ----------------------------------------------------------------------------------------------------*/
//Set leds to same color in HSV order
void HourRing :: setHSV(int pixel, uint8_t h, uint8_t s, uint8_t v)
{
	HourLeds[pixel].setHSV(h, s, v);
}


/* ----------------------------------------------------------------------------------------------------*/
//Set all leds to same color in HSV order
void HourRing :: setAllHSV(uint8_t h, uint8_t s, uint8_t v)
{ 
	for(int i=0; i<NUM_LED_HOUR; i++){
		HourLeds[i].setHSV(h, s, v);
	}
 }


/* ----------------------------------------------------------------------------------------------------*/
//All leds assamble a rainbow - variable says with which colour
//to start the rainbow
void HourRing :: rainbow(uint8_t start_hue)
{
	//this one looks better
	fill_gradient(HourLeds, NUM_LED_HOUR, CHSV(start_hue,255,220), CHSV(start_hue+1,255,220), LONGEST_HUES);
	//fill_rainbow(HourLeds, NUM_LED_HOUR, start_hue, 4);
}


/* ----------------------------------------------------------------------------------------------------*/
void HourRing :: colorWipe(int hue, int wait, int start_px)
{

	//TODO: popravi tle pozicijo - start_px je prau, sam uro dej x2

	if(hue == -1){
		for (int i=(start_px*2); i<NUM_LED_HOUR; i++){
			setHSV(i, 20, 255, 0);
			FastLED.show();
			FastLED.delay(wait);
		}
		for (int j=0; j<(start_px*2); j++){
			setHSV(j, 20, 255, 0);
			FastLED.show();
			FastLED.delay(wait);
		}
	}
	else
	{
		for (int i=(start_px*2); i<NUM_LED_HOUR; i++){
			setHSV(i, hue, 255, 255);
			FastLED.show();
			FastLED.delay(wait);
		}
		for (int j=0; j<(start_px*2); j++){
			setHSV(j, hue, 255, 255);
			FastLED.show();
			FastLED.delay(wait);
		}
	}

}



/* ----------------------------------------------------------------------------------------------------*/
/*--------------- MIN RING CLASS ----------------------------------------------------------------------*/
/* ----------------------------------------------------------------------------------------------------*/

//Minute Ring constructor (init)
MinRing :: MinRing()
{
	SERIAL_DEBUG(INIT Minute Led Strip, 1);
	FastLED.addLeds<LED_TYPE,PIN_RING_MIN ,COLOR_ORDER>(MinLeds,  NUM_LED_MIN ).setCorrection(TypicalLEDStrip);

	px_min = 0;
	px_sec = 0;
}


/* ----------------------------------------------------------------------------------------------------*/
void MinRing :: updateClockPosition(myTime t)
{
	px_sec = t.sec;
	px_min = t.min;
	px_mil =  map( (millis() % 1000), 0 ,1000, 0, 60);
}


/* ----------------------------------------------------------------------------------------------------*/
void MinRing :: displayCompasBig(uint8_t hue)
{
	setHSV(59, hue, 120, 60);
	setHSV(0, hue, 200, 200);
	setHSV(1, hue, 120, 60);

	setHSV(5, hue, 120, 60);
	setHSV(10, hue, 120, 60);

	setHSV(14, hue, 120, 60);
	setHSV(15, hue, 200, 200);
	setHSV(16, hue, 120, 60);

	setHSV(20, hue, 120, 60);
	setHSV(25, hue, 120, 60);

	setHSV(29, hue, 120, 60);
	setHSV(30, hue, 200, 200);
	setHSV(31, hue, 120, 60);

	setHSV(35, hue, 120, 60);
	setHSV(40, hue, 120, 60);

	setHSV(44, hue, 120, 60);
	setHSV(45, hue, 200, 200);
	setHSV(46, hue, 120, 60);

	setHSV(50, hue, 120, 60);
	setHSV(55, hue, 120, 60);
}

/* ----------------------------------------------------------------------------------------------------*/
void MinRing :: displayCompas(uint8_t hue)
{
	setHSV(59, hue, 200, 80);
	setHSV(0, hue, 250, 200);
	setHSV(1, hue, 200, 80);

	setHSV(14, hue, 200, 80);
	setHSV(15, hue, 250, 200);
	setHSV(16, hue, 200, 80);

	setHSV(29, hue, 200, 80);
	setHSV(30, hue, 250, 200);
	setHSV(31, hue, 200, 80);

	setHSV(44, hue, 200, 80);
	setHSV(45, hue, 250, 200);
	setHSV(46, hue, 200, 80);
}

/* ----------------------------------------------------------------------------------------------------*/
void MinRing :: displayCompasSmall(uint8_t hue)
{
	setHSV(0, hue, 250, 200);

	setHSV(15, hue, 250, 200);

	setHSV(30, hue, 250, 200);

	setHSV(45, hue, 250, 200);
}


/* ----------------------------------------------------------------------------------------------------*/
void MinRing :: displayClockUserColor(user_color uc)
{
	// Scale down the brightness of leds - smooth pixel transition
	nscale8(MinLeds,NUM_LED_MIN,253);

	// Set compas of minute ring
	displayCompasSmall(uc.compas);

	// Set minutes
	setHSV(px_min, uc.min_color, 255, 250);
}

/* ----------------------------------------------------------------------------------------------------*/
void MinRing :: displayClockVariableColor(int color)
{
	// Scale down the brightness of leds - smooth pixel transition
	nscale8(MinLeds,NUM_LED_MIN,253); 

	// Set compas of minute ring
	displayCompasSmall(color);

	// Set minutes and seconds
	setHSV(px_min, (color - 40), 255, 250);
	setHSV(px_sec, (color + 40), 255, 250);
}


/* ----------------------------------------------------------------------------------------------------*/
void MinRing :: displayClockPredefinedColor(int factor)
{
	// Set leds to black
	nscale8(MinLeds,NUM_LED_MIN,100); 

	// Get factors between 0 and 5
	factor = factor/43;   

	// Set minutes, seconds an milli seconds
	switch(factor){
		//red 0
		case 0:
			displayCompasSmall(0);
			setHSV(px_min, 20, 255, 250);//orange
			setHSV(px_sec, 42, 255, 250);//yellow
			setHSV(px_mil, 42, 255, 200);//yellow
			break;

		//yellow 42
		case 1:
			displayCompasSmall(42);
			setHSV(px_min, 160, 255, 250);//blue
			setHSV(px_sec, 20, 255, 250);//orange
			setHSV(px_mil, 20, 255, 200);//orange
			break;

		//green 85
		case 2:
			displayCompasSmall(82);
			setHSV(px_min, 128, 255, 250);//aqua
			setHSV(px_sec, 171,255, 250);//blue
			setHSV(px_mil, 171,255, 200);//blue
			break;

		//aqua 128
		case 3:
			displayCompasSmall(171);
			setHSV(px_min, 128, 255, 250);//aqua
			setHSV(px_sec, 171, 50, 250);//white
			setHSV(px_mil, 171, 50, 200);//white
			break;

		//blue 171
		case 4:
			displayCompasSmall(171);
			setHSV(px_min, 200, 255, 255);//purple
			setHSV(px_sec, 230, 255, 250);//pink
			setHSV(px_mil, 230, 255, 200);//pink
			break;

		//purple 213
		case 5:
			displayCompasSmall(213);
			setHSV(px_min, 171, 255, 250);//blue
			setHSV(px_sec, 25, 255, 250);//yellow
			setHSV(px_mil, 25, 255, 200);//yellow
			break;  
	}
}


/* ----------------------------------------------------------------------------------------------------*/
//Set all leds to black - off
void MinRing :: setBlack(void)
{
	for(int i=0; i<NUM_LED_MIN; i++){
		MinLeds[i] = CRGB :: Black;
	}
}


/* ----------------------------------------------------------------------------------------------------*/
void MinRing :: setWhite(void)
{
	for(int i=0; i<NUM_LED_MIN; i++){
		MinLeds[i] = CRGB :: White;
	}
}


/* ----------------------------------------------------------------------------------------------------*/
//Set leds to same color in RGB order
void MinRing :: setRGB(int pixel, uint8_t r, uint8_t g, uint8_t b)
{
  	MinLeds[pixel].setRGB(r, g, b);
}


/* ----------------------------------------------------------------------------------------------------*/
//Set leds to same color in HSV order
void MinRing :: setHSV(int pixel, uint8_t h, uint8_t s, uint8_t v)
{
 	 MinLeds[pixel].setHSV(h, s, v);
}


/* ----------------------------------------------------------------------------------------------------*/
//Set all leds to same color in HSV order
void MinRing :: setAllHSV(uint8_t h, uint8_t s, uint8_t v)
{
	for(int i=0; i<NUM_LED_MIN; i++){
		MinLeds[i].setHSV(h, s, v);
	}
}


/* ----------------------------------------------------------------------------------------------------*/
//All leds assamble a rainbow - variable says with which colour to start the rainbow
void MinRing :: rainbow(uint8_t start_hue)
{
	//this one looks better
	fill_gradient(MinLeds, NUM_LED_MIN, CHSV(start_hue,255,220), CHSV(start_hue+1,255,220), LONGEST_HUES);
	//fill_rainbow(MinLeds, NUM_LED_MIN, start_hue, 4);
}


/* ----------------------------------------------------------------------------------------------------*/
//Rainbow circuling around the ring - variable says how quick
void MinRing :: rainbowCycle(int delay_time)
{
	for(int i=0; i<255; i++){
		fill_rainbow(MinLeds, NUM_LED_MIN, i, 4);
		FastLED.show();
		FastLED.delay(delay_time);
	}
}


/* ----------------------------------------------------------------------------------------------------*/
// Set all leds into shifted color one by one - wipe
void MinRing :: colorWipe(int hue, int wait, int start_px)
{	
	uint8_t hUe = hue;
	// For turning off the leds, set hue to -1
	if(hue == -1){
		for(uint8_t i=start_px; i<NUM_LED_MIN; i++){
			setHSV(i, 20, 255, 0);
			FastLED.show();
			FastLED.delay(wait);
		}
		for(uint8_t j=0; j<start_px; j++){
			setHSV(j, 20, 255, 0);
			FastLED.show();
			FastLED.delay(wait);
		}
	}
	else{
		for(uint8_t i=start_px; i < NUM_LED_MIN; i++){
			setHSV(i, hUe, 255, 255);
			FastLED.show();
			FastLED.delay(wait);
			if(i <= 45){
				hUe = hUe + 2;
			}
			else{
				hUe = hUe - 2;
			}
		}
		
		for(uint8_t j=0; j < start_px; j++){
			setHSV(j, hUe, 255, 255);
			FastLED.show();
			FastLED.delay(wait);
			hUe = hUe - 2;
		}
	}
}


/* ----------------------------------------------------------------------------------------------------*/
// Wipe leds but with 4 beginings together 
void MinRing :: quarterWipe(int hue, int wait)
{
	uint8_t pos, hUe = hue;
	if(hue == -1){
		for(uint8_t j = 0; j < 15; j++)
		{
			for(uint8_t i = 0; i < 4; i++)
			{
				pos = 15 * i + j;
				setHSV(pos, hue, 255, 0);
			}
			
			FastLED.show();
			FastLED.delay(wait);
		}
	}
	else {
		for(uint8_t j = 0; j < 15; j++)
		{
			for(uint8_t i = 0; i < 4; i++)
			{
				pos = 15 * i + j;
				setHSV(pos, hUe, 255, 250);
			}
			hUe = hUe + 5;
			FastLED.show();
			FastLED.delay(wait);
		}
	}
}


/* ----------------------------------------------------------------------------------------------------*/
// Rainbow dots are falling from both sides
void MinRing :: rainbowFaling (int hue)
{
	// Turn on first three leds on the bottom
	setHSV(31, hue, 255, 250);
	setHSV(30, hue-9, 255, 250);
	setHSV(29, hue, 255, 250);

	for(int k=0; k <= 28; k++){
		// Right side first
		for (int i=1; i<(29-k); i++){

			setHSV(i, (hue+k*9), 255, 250);
			FastLED.show();
			FastLED.delay(20);

			// If not yet on top of the stack, turn it back off
			if (i != 28-k){
				setHSV(i, 0, 255, 0);  
			}
		}
		// Do the same on the other side
		for (int j=59; j>(31+k); j--){

			setHSV(j, (hue+k*9), 255, 250);
			FastLED.show();
			FastLED.delay(20);

			if (j != 32+k){
				setHSV(j, 0, 255, 0);
			} 
		}
	}

	// Turn on the last one after small delay
	FastLED.delay(500);
	setHSV(0, hue+243, 255, 250);

	FastLED.delay(500);
}


/* ----------------------------------------------------------------------------------------------------*/
//Pixels start to assamble a rainbow -faling down and filling the ring
//variable sets the starting pixel - from where to fall 
void MinRing :: rainbowFillin(uint8_t start_hue)
{
	int dilej = 10;
	int pos = 0;
	int hue = start_hue;

	setHSV(59, hue, 255, 255);
	FastLED.show();

	for (int i=0; i<NUM_LED_MIN; i++){
		for(int pixel = 59; pixel >= 0; pixel--){

			if(pixel > pos){
				setHSV(pixel, hue, 255, 255);
				FastLED.show();
				FastLED.delay(dilej);
				setHSV(pixel, hue, 255, 0);
			}   
			else{
				setHSV(pixel, hue, 255, 255);
				FastLED.show();
				FastLED.delay(dilej);
				pos++;
				break;
			}   
		}

		//In the begining go fast, then more and more slowly
		if(i%10 == 0){
		dilej += (i/10);
		}
		hue=hue+4;
	}

	FastLED.delay(300);

	// Turn them off
	for (int j = 59; j >= 0; j--){
		setHSV(j, 255, 255, 0);
		FastLED.show();
		FastLED.delay(dilej);
	}
	setHSV(59, 255, 255, 0);
	FastLED.show();

	FastLED.delay(500);
}
