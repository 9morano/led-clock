#include "ring.h"


/*---------------HOUR RING FUNCTIONS-----------------------------------------------------------------*/
//Hour Ring constructor (init)
HourRing :: HourRing(){
  //Serial.println("INIT Minute Led Strip");
  FastLED.addLeds<LED_TYPE,PIN_NUM_HOUR,COLOR_ORDER>(HourLeds, NUM_LED_HOUR).setCorrection(TypicalLEDStrip);

  px_hou = 0;
}

void HourRing :: updateClockPosition(myTime t){
  int tmp = 0;
  tmp = t.hour % 12; //from 24h format to 12h
  px_hou = tmp;
}

void HourRing :: displayCompasBig(uint8_t hue){
  setHSV(0, hue, 200, 200);
  setHSV(1, hue, 200, 200);
  
  setHSV(6, hue, 200, 200);
  setHSV(7, hue, 200, 200);
  
  setHSV(12, hue, 200, 200);
  setHSV(13, hue, 200, 200);
  
  setHSV(18, hue, 200, 200);
  setHSV(19, hue, 200, 200);
}

void HourRing :: displayCompas(uint8_t hue){
  setHSV(0, hue, 250, 200);
  setHSV(1, hue, 250, 200);
  
  setHSV(6, hue, 250, 200);
  setHSV(7, hue, 250, 200);
  
  setHSV(12, hue, 250, 200);
  setHSV(13, hue, 250, 200);
  
  setHSV(18, hue, 250, 200);
  setHSV(19, hue, 250, 200);
}

void HourRing::displayClockVariableColor(int color){
  setBlack();

  //display compas of hours
  displayCompas(color);
    
  setHSV(px_hou*2, (color - 40), 255, 250);
  setHSV((px_hou*2)+1, (color - 40), 255, 250);
}

void HourRing :: displayClockPredefinedColor(int fact){
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

void HourRing :: displayClockUserColor(user_color uc)
{
  setBlack();

  //display compas of hours
  displayCompasBig(uc.compas);
    
  setHSV(px_hou*2, uc.hour_color, 255, 250);
  setHSV((px_hou*2)+1, uc.hour_color, 255, 250);
}

//Set all leds to black - off
void HourRing :: setBlack(void){
  for(int i=0; i<NUM_LED_HOUR; i++){
    HourLeds[i] = CRGB :: Black;
  }
}
void HourRing :: setWhite(void){
  for(int i=0; i<NUM_LED_HOUR; i++){
    HourLeds[i] = CRGB :: White;
  }
}

//Set leds to same color in RGB order
void HourRing :: setRGB(int pixel, uint8_t r, uint8_t g, uint8_t b){
  HourLeds[pixel].setRGB(r, g, b);
}

//Set leds to same color in HSV order
void HourRing :: setHSV(int pixel, uint8_t h, uint8_t s, uint8_t v){
  HourLeds[pixel].setHSV(h, s, v);
}

//Set all leds to same color in HSV order
void HourRing :: setAllHSV(uint8_t h, uint8_t s, uint8_t v){
  
  for(int i=0; i<NUM_LED_HOUR; i++){
    HourLeds[i].setHSV(h, s, v);
  }
 }

//All leds assamble a rainbow - variable says with which colour
//to start the rainbow
void HourRing :: rainbow(uint8_t start_hue){
  //this one looks better
  fill_gradient(HourLeds, NUM_LED_HOUR, CHSV(start_hue,255,220), CHSV(start_hue+1,255,220), LONGEST_HUES);
  //fill_rainbow(HourLeds, NUM_LED_HOUR, start_hue, 4);
}

void HourRing :: colorWipe(int hue, int wait, int start_px){

//TO DO: popravi tle pozicijo - start_px je prau, sam uro dej x2
  
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









/*---------------MINUTE RING FUNCTIONS---------------------------------------------------------------*/
//Minute Ring constructor (init)
MinRing :: MinRing(){
  //Serial.println("INIT Minute Led Strip");
  FastLED.addLeds<LED_TYPE,PIN_NUM_MIN ,COLOR_ORDER>(MinLeds,  NUM_LED_MIN ).setCorrection(TypicalLEDStrip);

  px_min = 0;
  px_sec = 0;
}

void MinRing :: updateClockPosition(myTime t){
  px_sec = t.sec;
  px_min = t.min;
}

void MinRing :: displayCompasBig(uint8_t hue){
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

void MinRing :: displayCompas(uint8_t hue){
  setHSV(57, hue, 200, 80);
  setHSV(58, hue, 250, 200);
  setHSV(59, hue, 200, 80);

  setHSV(12, hue, 200, 80);
  setHSV(13, hue, 250, 200);
  setHSV(14, hue, 200, 80);

  setHSV(27, hue, 200, 80);
  setHSV(28, hue, 250, 200);
  setHSV(29, hue, 200, 80);

  setHSV(42, hue, 200, 80);
  setHSV(43, hue, 250, 200);
  setHSV(44, hue, 200, 80);
}


void MinRing :: displayClockVariableColor(int color){
  //set all leds to black colour
  nscale8(MinLeds,NUM_LED_MIN,200); 

  //display compas of minutes
  displayCompas(color);
    
  setHSV(px_min, (color - 40), 255, 250);
  setHSV(px_sec, (color + 40), 255, 250);
}

void MinRing :: displayClockPredefinedColor(int fact){

  nscale8(MinLeds,NUM_LED_MIN,253); 
  
  fact = fact/43;   // factors between 0 and 5

  switch(fact){
    //red 0
    case 0:
      displayCompas(0);//red
      setHSV(px_min, 20, 255, 250);//orange
      setHSV(px_sec, 42, 255, 250);//yellow
    break;

    //yellow 42
    case 1:
      displayCompas(42);//yellow
      setHSV(px_min, 160, 255, 250);//blue
      setHSV(px_sec, 20, 255, 250);//orange
    break;

    //green 85
    case 2:
      displayCompas(82);//green
      setHSV(px_min, 128, 255, 250);//aqua
      setHSV(px_sec, 171,255, 250);//blue
    break;

    //aqua 128
    case 3:
      displayCompas(171);//blue
      setHSV(px_min, 128, 255, 250);//aqua
      setHSV(px_sec, 171, 50, 250);//white
    break;

    //blue 171
    case 4:
      displayCompas(171);//blue
      setHSV(px_min, 208, 255, 255);//purple -> bolj temno vijola
      setHSV(px_sec, 230, 255, 250);//pink -> bolj roza    
    break;

    //purple 213
    case 5:
      displayCompas(213);//purple
      setHSV(px_min, 171, 255, 250);//blue
      setHSV(px_sec, 25, 255, 250);//yellow
    break;  
  }
}

void MinRing :: displayClockUserColor(user_color uc)
{
  //set all leds to black colour
  nscale8(MinLeds,NUM_LED_MIN,200);

  //display compas of minutes
  displayCompasBig(uc.compas);
    
  setHSV(px_min, uc.min_color, 255, 250);
  //setHSV(px_sec, (color + 40), 255, 250);
}



//Set all leds to black - off
void MinRing :: setBlack(void)
{
  for(int i=0; i<NUM_LED_MIN; i++){
    MinLeds[i] = CRGB :: Black;
  }
}
void MinRing :: setWhite(void)
{
  for(int i=0; i<NUM_LED_MIN; i++){
    MinLeds[i] = CRGB :: White;
  }
}

//Set leds to same color in RGB order
void MinRing :: setRGB(int pixel, uint8_t r, uint8_t g, uint8_t b)
{
  MinLeds[pixel].setRGB(r, g, b);
}

//Set leds to same color in HSV order
void MinRing :: setHSV(int pixel, uint8_t h, uint8_t s, uint8_t v)
{
  MinLeds[pixel].setHSV(h, s, v);
}

//Set all leds to same color in HSV order
void MinRing :: setAllHSV(uint8_t h, uint8_t s, uint8_t v)
{
  for(int i=0; i<NUM_LED_MIN; i++){
    MinLeds[i].setHSV(h, s, v);
  }
}

//All leds assamble a rainbow - variable says with which colour to start the rainbow
void MinRing :: rainbow(uint8_t start_hue)
{
	//this one looks better
	fill_gradient(MinLeds, NUM_LED_MIN, CHSV(start_hue,255,220), CHSV(start_hue+1,255,220), LONGEST_HUES);
	//fill_rainbow(MinLeds, NUM_LED_MIN, start_hue, 4);
}




//Rainbow circuling around the ring - variable says how quick
void MinRing :: rainbowCycle(int delay_time)
{
	for(int i=0; i<255; i++){
		fill_rainbow(MinLeds, NUM_LED_MIN, i, 4);
		FastLED.show();
		FastLED.delay(delay_time);
	}
}

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



void MinRing :: birthDayShow(uint8_t hue)
{
	//setHSV(35)
}

  
