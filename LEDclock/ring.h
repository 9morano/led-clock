#include <FastLED.h>
#include <DS3231.h>


#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define PIN_NUM_MIN 5
#define PIN_NUM_HOUR 3
#define NUM_LED_HOUR 24
#define NUM_LED_MIN  60


extern CRGB HourLeds[NUM_LED_HOUR];
extern CRGB MinLeds[NUM_LED_MIN];


typedef struct myTime{
  int hour;
  int min;
  int sec;
};

typedef struct user_color{
  uint8_t compas;
  uint8_t hour_color;
  uint8_t min_color;
};

/*---------------CLASSES-----------------------------------------------------------------------------*/
class HourRing{
  public:
    uint8_t px_hou;

    //Constructor
    HourRing();

    //Functions for mode: lamp
    void setBlack();
    void setWhite();
    void setRGB(int pixel, uint8_t r, uint8_t g, uint8_t b);
    void setHSV(int pixel, uint8_t h, uint8_t s, uint8_t v);
    void setAllHSV(uint8_t h, uint8_t s, uint8_t v);
    void rainbow(uint8_t start_hue);
    void colorWipe(int hue, int wait, int start_px);

    //Functions for mode: clock
    void updateClockPosition(myTime t);
    void displayCompas(uint8_t hue);
    void displayCompasBig(uint8_t hue);
    void displayClockVariableColor(int color);
    void displayClockUserColor(user_color uc);
    void displayClockPredefinedColor(int fact); 
};


class MinRing{
  public:
    uint8_t px_min;
    uint8_t px_sec;
    uint8_t px_mil;

    //Constructor
    MinRing();

    //Functions for mode: lamp
    void setBlack();
    void setWhite();
    void setRGB(int pixel, uint8_t r, uint8_t g, uint8_t b);
    void setHSV(int pixel, uint8_t h, uint8_t s, uint8_t v);
    void setAllHSV(uint8_t h, uint8_t s, uint8_t v);
    void rainbow(uint8_t start_hue);
    void rainbowFaling (int hue);
 
    
    //With FastLED.show() in the function
    void colorWipe(int hue, int wait, int start_px);
    void quarterWipe(int hue, int wait);
    void rainbowCycle(int delay_time);
    void rainbowFillin(uint8_t start_hue);
    void birthDayShow(uint8_t hue);

    //Functions for mode: clock
    void updateClockPosition(myTime t);
    void displayCompas(uint8_t hue);
    void displayCompasBig(uint8_t hue);
    void displayClockUserColor(user_color uc);
    void displayClockPredefinedColor(int fact);
    void displayClockVariableColor(int color);
};


