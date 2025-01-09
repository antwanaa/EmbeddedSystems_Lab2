#include <Arduino.h>
#include <U8x8lib.h>
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

#define buttonPin 4 // Pin for the button
#define potPin A0   // Pin for the potentiometer
#define buzzer 5    // Pin for the buzzer

// 0: Normal State
// 1: Change Clock
// 2: Change Alarm
// 3: Alarm
// 4: Snooze State
int state = 0; // variable that holds the current state. initialize to Normal State, or regular time
unsigned long previousMillis = 0; // holds the previous timer
unsigned long duration = 0;
bool buttonPrevState = false; // true if button was pressed right before, else false
int pressCounter = 0; //keeps track of how many times the button was pressed

// Time
int hours = 0;    // Variable to hold the hours
int minutes = 0;  // Variable to hold the minutes
int days = 0;     // variable to hold the days

// Change Time
int hoursTemp = 0;
int minutesTemp = 0;
int offset = 0;

// Alarm
int hoursAlarm = 7;     // Variable to hold the hours for the alarm
int minutesAlarm = 0;   // Variable to hold the minutes for the alarm
bool noPress = true;    // Boolean indicating that the button has not been pressed yet
unsigned long alarmStopped = 0; // storing the time at which the alarm has been stopped

// Snooze Alarm
bool font = false;    // boolean used for the screen flashing
unsigned long futureBlink = 0;  // time at which the screen last blinked

// Strings for screen display
String hoursString = "00";    // String to hold the hours that are displayed
String minutesString = "00";  // String to hold the minutes that are displayed


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  u8x8.begin(); // to start using the screen
  u8x8.setFlipMode(1);
  pinMode(buttonPin, INPUT);  // sets the Button Pin as an input
  pinMode(buzzer, OUTPUT);    // set the buzzer as an output

  u8x8.setFont(u8x8_font_profont29_2x3_f);
  u8x8.setCursor(0, 0); //defining the starting point for the cursor
}

void loop() {
  //check if alarm time == true and that it is not going to snooze
  if (hoursAlarm == hours && minutesAlarm == minutes && state != 4 && (millis() - alarmStopped > 60000)) {
    state = 3;
  }

  // Switch Case to choose which state to enter
  switch (state) {
    case 0: // Normal Time
      normalTime();
      break;
    case 1: // Change Clock
      changeClock();
      break;
    case 2: // Change Alarm
      changeAlarm();
      break;
    case 3: // Alarm State
      alarm();
      break;
    case 4: // Snooze State
      snooze();
      break;
  }

}

// Display the time and check for button presses specific to this state
void normalTime() {
  // State routine - Simply print the time to the display
  days = (millis() + offset*60000)/ 86400000;
  hours = (millis() + offset*60000)/ (3600000) - days * 24;
  minutes = (millis() + offset*60000) / (60000) - hours * 60 - days * 1440;
  // change the ints to Strings for printing, add a 0 if it is single digit
  if (hours < 10)
    hoursString = "0" + String(hours);
  else
    hoursString = String(hours);

  if (minutes < 10)
    minutesString = "0" + String(minutes);
  else
    minutesString = String(minutes);

  // print time
  u8x8.setFont(u8x8_font_profont29_2x3_f);
  u8x8.setCursor(3, 2); //defining the starting point for the cursor
  u8x8.print(hoursString + ":" + minutesString);


  // Check for possible button presses
  duration = 0;
  if (digitalRead(buttonPin) == HIGH) { // If button is pressed
    if (buttonPrevState == false) { // and the button was not pressed before
      buttonPrevState = true;       // button has been pressed
      previousMillis = millis();    // and save the current time.
    }
    duration = millis() - previousMillis;
    if(duration >= 1000){ // if the button has been pressed for more than 1s then 
      state = 1;  // go to state CHANGE CLOCK
      buttonPrevState = false;  // reset the prevState for the button
      return;
    }
  } else if (buttonPrevState == true) { // if the button is not pressed but was before:
    buttonPrevState = false;
    Serial.println("button press stopped");
    duration = millis() - previousMillis;
    Serial.println("Duration: " + duration);  // compute the duration of the press and choose a state accordingly

    if (duration >= 1000) // if the button press is greater than or equal to 1000ms
      state = 1;          // set STATE to be CHANGE CLOCK
    else                // otherwise
      state = 2;          // set STATE to be CHANGE ALARM
  }
  return;
}

// Change the HOURS then the MINUTES
void changeClock() {
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setCursor(0, 0); //defining the starting point for the cursor
  u8x8.print("CHANGE HOURS  ");

  // buffer to disregard the button if it is pressed when entering the state
  while (digitalRead(buttonPin) == HIGH){
    // wait until button is depressed before going into changing mode
  }
  // while the button is not pressed again, change the hours based on the potentiometer
  while (digitalRead(buttonPin) == LOW) {
    days = millis() / 86400000;
    hours = millis() / (3600000) - days * 24 + offset / 60;
    hoursTemp = int(analogRead(potPin) * 0.0234375);
    minutes = millis() / (60000) - hours * 60 - days * 1440 + offset;
    if (hoursTemp < 10)
      hoursString = "0" + String(hoursTemp);
    else
      hoursString = String(hoursTemp);

    if (minutes < 10)
      minutesString = "0" + String(minutes);
    else
      minutesString = String(minutes);

    // display the time
    u8x8.setFont(u8x8_font_profont29_2x3_f);
    u8x8.setCursor(3, 2); //defining the starting point for the cursor
    u8x8.print(hoursString + ":" + minutesString);
  }
  // save the new time
  offset = offset + hoursTemp * 60 - hours * 60;

  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setCursor(0, 0); //defining the starting point for the cursor
  u8x8.print("CHANGE MINUTES");
  delay(500);

  // change the minutes
  while (digitalRead(buttonPin) == LOW) {
    days = millis() / 86400000;
    hours = millis() / (3600000) - days * 24 + offset / 60;
    minutes = millis() / (60000) - hours * 60 - days * 1440 + offset;
    minutesTemp = int(analogRead(potPin) * 0.05859375);
    if (hours < 10)
      hoursString = "0" + String(hours);
    else
      hoursString = String(hours);

    if (minutesTemp < 10)
      minutesString = "0" + String(minutesTemp);
    else
      minutesString = String(minutesTemp);

    // display the time
    u8x8.setFont(u8x8_font_profont29_2x3_f);
    u8x8.setCursor(3, 2); //defining the starting point for the cursor
    u8x8.print(hoursString + ":" + minutesString);
  }
  while(digitalRead(buttonPin) == HIGH){
    //wait until button is depressed
  }
  // save the new time and display it
  offset = offset + minutesTemp - minutes;
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setCursor(0, 0);         //defining the starting point for the cursor
  u8x8.print("              "); // Clear the top of the display
  state = 0;  // return to the time display
  return;
}

// change the alarm time
void changeAlarm() {
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setCursor(0, 0); //defining the starting point for the cursor
  u8x8.print("ALARM HOUR    ");

  // set the hours of the alarm
  while (digitalRead(buttonPin) == LOW) {
    hoursTemp = int(analogRead(potPin) * 0.0234375);

    if (hoursTemp < 10)
      hoursString = "0" + String(hoursTemp);
    else
      hoursString = String(hoursTemp);

    if (minutesAlarm < 10)
      minutesString = "0" + String(minutesAlarm);
    else
      minutesString = String(minutesAlarm);

    // display the new alarm time
    u8x8.setFont(u8x8_font_profont29_2x3_f);
    u8x8.setCursor(3, 2); //defining the starting point for the cursor
    u8x8.print(hoursString + ":" + minutesString);
  }
  // save the new time
  hoursAlarm = hoursTemp;

  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setCursor(0, 0); //defining the starting point for the cursor
  u8x8.print("ALARM MINUTES ");
  delay(500);

  // change the alarm minutes
  while (digitalRead(buttonPin) == LOW) {
    minutesTemp = int(analogRead(potPin) * 0.05859375);
    if (hoursAlarm < 10)
      hoursString = "0" + String(hoursAlarm);
    else
      hoursString = String(hoursAlarm);

    if (minutesTemp < 10)
      minutesString = "0" + String(minutesTemp);
    else
      minutesString = String(minutesTemp);

    // display the new alarm time
    u8x8.setFont(u8x8_font_profont29_2x3_f);
    u8x8.setCursor(3, 2); //defining the starting point for the cursor
    u8x8.print(hoursString + ":" + minutesString);
  }
  while(digitalRead(buttonPin) == HIGH){
    //wait until button is depressed
  }
  // save the new alarm time
  minutesAlarm = minutesTemp;
  state = 0;  // return to time display
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setCursor(0, 0); //defining the starting point for the cursor
  u8x8.print("              "); // Clear the top of the display
  return;
}

// alarm state
void alarm() {
  
  // wait for button press(es)
  pressCounter = 0; // make sure the button presses are set to 0
  duration = 0;
  while (true) {
    if (digitalRead(buttonPin) == HIGH) { // If button is pressed
      noPress = false;                    // save that a button has been pressed
      if (buttonPrevState == false) {     // and if the button was not pressed before
        buttonPrevState = true;           // save that the previous button state was pressed
        if(pressCounter == 0)   // only start the 5s clock if the first press is detected
          previousMillis = millis();  // then save the current time.
      }
    } else if (buttonPrevState == true) { // if the button is not pressed but was before:
      buttonPrevState = false;
      pressCounter++;   // Count the number of button presses
      Serial.println("button press stopped");
      Serial.println("Duration: " + duration);
      if (duration >= 1000) // if the button press is greater than or equal to 1000ms
        state = 1;          // set STATE to be change clock
      else                // otherwise
        state = 2;          // set STATE to be change alarm
    }
    
    duration = millis() - previousMillis;
    if(pressCounter >=3 && duration <= 5000){ // if the button was pressed 3 times in 5s
      state = 0;  // return to normal time state
      u8x8.clear(); // clear the display
      buttonPrevState = false;  // reset the previous button state
      pressCounter = 0; // make sure the button presses are set to 0
      duration = 0; // clear the duration
      alarmStopped = millis();  // save the time at which the alarm is stopped
      break;
    }
    // if the button was pressed less than 3 times in 5s:
    if(duration > 5000 && (pressCounter == 1 || pressCounter == 2)){  
      state = 4;  // go to snooze state
      u8x8.clear(); // clear the display
      buttonPrevState = false;
      pressCounter = 0; // make sure the button presses are set to 0
      duration = 0; // reset the duration
      break;
    }
    // blink the display
    if(millis() >= futureBlink){
      futureBlink = millis() + 1000;
      font = !font;
      u8x8.setFont(u8x8_font_chroma48medium8_r);
      u8x8.setInverseFont(font);
      u8x8.drawString(0, 0, "A               ");
      u8x8.drawString(0, 1, "                ");
      u8x8.drawString(0, 2, "                ");
      u8x8.drawString(0, 3, "                ");
      u8x8.drawString(0, 4, "                ");
      u8x8.drawString(0, 5, "                ");
      u8x8.drawString(0, 6, "                ");
      u8x8.drawString(0, 7, "                ");  
      if(font && noPress)
        tone(buzzer, 1000); // start the tone
      else
        noTone(buzzer); // stop the tone
    }
  }
  noTone(buzzer); // if leaving the alarm state, stop tone
  noPress = true;
  return;
}

// snooze state, only blink the display and wait for 3 button presses in 5s
void snooze() {
    
  while (true) {
    if (digitalRead(buttonPin) == HIGH) { // If button is pressed
      Serial.println("BUTTON IS PRESSED");
      if (buttonPrevState == false) { // and the button was not pressed before
        Serial.println("button press started");
        buttonPrevState = true;
        if(pressCounter == 0) // only start the 5s clock if the first press is detected
          previousMillis = millis();  // then save the current time.
      }
    } else if (buttonPrevState == true) {
      buttonPrevState = false;
      pressCounter++;   // Count the number of button presses
      Serial.println("button press stopped");
      Serial.println("Duration: " + duration);
      if (duration >= 1000) // if the button press is greater than or equal to 1000ms
        state = 1;          // set STATE to be change clock
      else                // otherwise
        state = 2;          // set STATE to be change alarm
    }
    
    duration = millis() - previousMillis;
    if(pressCounter >=3 && duration <= 5000){
      state = 0;
      u8x8.clear();
      buttonPrevState = false;
      pressCounter = 0; // make sure the button presses are set to 0
      duration = 0;
      alarmStopped = millis();
      break;
    }
    if(duration > 5000){
      buttonPrevState = false;
      pressCounter = 0; // make sure the button presses are set to 0
      duration = 0;
    }

    // blink the display
    if(millis() >= futureBlink){
      futureBlink = millis() + 1000;
      font = !font;
      u8x8.setFont(u8x8_font_chroma48medium8_r);
      u8x8.setInverseFont(font);
      u8x8.drawString(0, 0, "S               ");
      u8x8.drawString(0, 1, "                ");
      u8x8.drawString(0, 2, "                ");
      u8x8.drawString(0, 3, "                ");
      u8x8.drawString(0, 4, "                ");
      u8x8.drawString(0, 5, "                ");
      u8x8.drawString(0, 6, "                ");
      u8x8.drawString(0, 7, "                ");  
    }
  }
  
  state = 0;  // return to normal time
  u8x8.setInverseFont(0);
  u8x8.clear();
  return;
}
