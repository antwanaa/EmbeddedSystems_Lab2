#include <Arduino.h>
#include <U8x8lib.h>

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

unsigned long previousMillis = 0;    // will store last time LED was updated

// constants won't change:
const long interval = 1000;          // interval at which to decrement the seconds

int minutes = 10;             // Variable to hold the number of minutes remaining
int seconds = 0;              // Variable to hold the number of seconds remaining
String minutesString = "10";  // String to hold the minutes that are displayed
String secondsString = "00";  // String to hold the seconds that are displayed


void setup() {
  u8x8.begin(); // to start using the screen
  u8x8.setFlipMode(1);

  u8x8.setFont(u8x8_font_profont29_2x3_f);
  u8x8.setCursor(0, 0); //defining the starting point for the cursor
  u8x8.print(minutesString + ":" + secondsString);
}

void loop() {
  
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time the screen was updated
    previousMillis = currentMillis;

    seconds = seconds - 1; // decrement the number of seconds by 1
    
    // if the seconds have reached below 0, change them to 59 and decrement the minutes
    if (seconds <= -1) {
      seconds = 59;
      minutes = minutes - 1;
    }
    // if the minutes have reached -1, then the timer is done
    if (minutes <= -1) {
      seconds = 0;
      minutes = 0;
    }

    // formatting what is about to be displayed, adding a 0 in front of the number of 
    // minutes or seconds if there is only one digit to follow the mm:ss format
    minutesString = String(minutes);
    secondsString = String(seconds);
    if(minutes <10)
      minutesString = "0" + String(minutes);
    if(seconds <10)
      secondsString = "0" + String(seconds);
    
    // updating the display
    u8x8.setFont(u8x8_font_profont29_2x3_f);
    u8x8.setCursor(0, 0); //defining the starting point for the cursor
    u8x8.print(minutesString + ":" + secondsString);
  }
}
