#include <Arduino.h>
#include <U8x8lib.h>
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

#define soundSensorPin A2 // defining the pin for the sound sensor
#define lightSensorPin A3 // defining the pin for the light sensor
#define ledPin 6  // defining the pin for the LED

// 0: Nothing State
// 1: light blinking, waiting to close gates
// 2: lowering gate
// 3: Wait for train to be done passing
// 4: raising gate
// 5: stop blinking
int state = 0; // variable that holds the current state. initialize to Normal State, or regular time
unsigned long previousMillis = 0; // holds the previous timer
unsigned long lightTimer = 0;     // holds the time at which the previous light blink was
unsigned long randomDuration = 20000; // variable used to hold the random durations computed

int lightBlink = 0; // 1 when LED blinks, 0 otherwise
int lightPrevStatus = 0;  // stores the previous status of the LED to enable blinking

void setup() {
  Serial.begin(9600); // Start serial communication
  u8x8.begin();       // to start using the screen
  u8x8.setFlipMode(1);
  pinMode(ledPin,OUTPUT); //Sets the pinMode to Output for the LED
}

void loop() {
  // Read and output the light sensor reading and the sound sensor reading
  int lightIntensity = analogRead(lightSensorPin);
  Serial.print("light intensity: ");
  Serial.println(lightIntensity);
  int soundIntensity = analogRead(soundSensorPin);
  Serial.print("sound intensity: ");
  Serial.println(soundIntensity);
  delay(200);

  if (state == 0 && lightIntensity < 600) { // if a train is NOT detected as incoming
    //    u8x8.clear();
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.setCursor(0, 4); //defining the starting point for the cursor
    u8x8.print("Wait for Train");   // Print "Wait for train" on the display
    state = 0;
  }
  if (state == 0 && lightIntensity >= 600) {  // if a train is detected as incoming
    u8x8.clear(); // clear the display
    previousMillis = millis();  // store the time
    lightBlink = 1; // set the LED to blinking
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.setCursor(0, 0); //defining the starting point for the cursor
    u8x8.print("Train is coming");  // Print that the train has been detected
    u8x8.setCursor(0, 1); //defining the starting point for the cursor
    u8x8.print("/ Blink");
    state = 1;  // change the state
  }
  // 5 seconds after the lights started flashing: close the gates
  if (state == 1 && (millis() - previousMillis >= 5000)) {  
    u8x8.clear(); // clear the display
    previousMillis = millis();  // store the time
    u8x8.setCursor(0, 0); //defining the starting point for the cursor
    u8x8.print("t>5");
    u8x8.setCursor(0, 1); //defining the starting point for the cursor
    u8x8.print("/ lower Gates");
    u8x8.setCursor(2, 3); //defining the starting point for the cursor
    u8x8.print("Waiting for");
    u8x8.setCursor(1, 4); //defining the starting point for the cursor
    u8x8.print("Gates to Lower"); // Lower the gates
    state = 2;  // change the state
    randomDuration = random(8000, 10000); // create a random time for the closing of the gates
  }
  // After the random time of gate closing, wait for the train
  if (state == 2 && (millis() - previousMillis >= randomDuration)) {
    previousMillis = millis();
//    u8x8.clear();
    u8x8.setCursor(2, 3); //defining the starting point for the cursor
    u8x8.print("Waiting for");
    u8x8.setCursor(1, 4); //defining the starting point for the cursor
    u8x8.print("Train to Pass ");
    state = 3;
  }
  // once the horn of the train has been detected or 4 min, raise gates.
  if (state == 3 && (soundIntensity >= 700 || (millis() - previousMillis >= 240000))) {
    u8x8.clear();
    previousMillis = millis();
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.setCursor(0, 0); //defining the starting point for the cursor
    u8x8.print("Train is leaving");
    u8x8.setCursor(0, 1); //defining the starting point for the cursor
    u8x8.print("/ raise");
    u8x8.setCursor(2, 3); //defining the starting point for the cursor
    u8x8.print("Raising Gate");
    state = 4;  // change to state 4
    randomDuration = random(10000, 20000); // create a random duration for the opening of the gates
  }
  if (state == 4 && (millis() - previousMillis >= randomDuration)) {  // once the gates have opened and wait 5s:
    previousMillis = millis();
    u8x8.clear();
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.setCursor(3, 3); //defining the starting point for the cursor
    u8x8.print("Gate raised");
    state = 5;
  }
  // wait 5 seconds after the gates have opened and stop the lights blinking
  if (state == 5 && (millis() - previousMillis >= 5000)) {
    u8x8.clear();
    previousMillis = millis();
    lightBlink = 0; // set the blinking to false
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.setCursor(0, 0); //defining the starting point for the cursor
    u8x8.print("t>5");
    u8x8.setCursor(0, 1); //defining the starting point for the cursor
    u8x8.print("/ stop");
    state = 0;
  }

  // if statements enabling the LED to blink while the rest is executing
  if (lightBlink == 1 && (millis() - lightTimer > 500)) {
    lightTimer = millis();
    if (lightPrevStatus == 0) {
      digitalWrite(ledPin, HIGH);
      lightPrevStatus = 1;
    } else {
      digitalWrite(ledPin, LOW);
      lightPrevStatus = 0;
    }
  }
  if (lightBlink == 0)
    digitalWrite(ledPin, LOW);
}
