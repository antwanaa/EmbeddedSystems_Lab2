#include <Arduino.h>
#include <U8x8lib.h>
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

#define soundSensorPin A2
#define lightSensorPin A3
#define ledPin 6

// 0: Nothing State
// 1: light blinking, waiting to close gates
// 2: lowering gate
// 3: Wait for train to be done passing
// 4: raising gate
int state = 0; // variable that holds the current state. initialize to Normal State, or regular time
unsigned long previousMillis = 0; // holds the previous timer
unsigned long lightTimer = 0; // holds the previous timer

int lightBlink = 0;
int gateClosed = 0;
int lightPrevStatus = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  u8x8.begin(); // to start using the screen
  u8x8.setFlipMode(1);
  pinMode(ledPin,OUTPUT);    //Sets the pinMode to Output 
}

void loop() {
  // put your main code here, to run repeatedly:
  int lightIntensity = analogRead(lightSensorPin);
  Serial.print("light intensity: ");
  Serial.println(lightIntensity);
  int soundIntensity = analogRead(soundSensorPin);
  Serial.print("sound intensity: ");
  Serial.println(soundIntensity);

  delay(200);
  if (state == 0 && lightIntensity < 600) {
    //    u8x8.clear();
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.setCursor(0, 4); //defining the starting point for the cursor
    u8x8.print("Wait for Train");
    state = 0;
  }
  if (state == 0 && lightIntensity >= 600) {
    u8x8.clear();
    previousMillis = millis();
    lightBlink = 1;
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.setCursor(0, 0); //defining the starting point for the cursor
    u8x8.print("Train is coming");
    u8x8.setCursor(0, 1); //defining the starting point for the cursor
    u8x8.print("/ Blink");
    state = 1;
  }
  if (state == 1 && (millis() - previousMillis >= 5000)) {
    u8x8.clear();
    previousMillis = millis();
    u8x8.setCursor(0, 0); //defining the starting point for the cursor
    u8x8.print("t > 5");
    u8x8.setCursor(0, 1); //defining the starting point for the cursor
    u8x8.print("/ lower Gates");
    u8x8.setCursor(2, 3); //defining the starting point for the cursor
    u8x8.print("Waiting for");
    u8x8.setCursor(1, 4); //defining the starting point for the cursor
    u8x8.print("train to pass");
    state = 3;
  }
  if (state == 3 && soundIntensity >= 800) {
    u8x8.clear();
    previousMillis = millis();
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.setCursor(0, 0); //defining the starting point for the cursor
    u8x8.print("Train is leaving");
    u8x8.setCursor(0, 1); //defining the starting point for the cursor
    u8x8.print("/ raise");
    state = 4;
  }
  if (state == 4 && (millis() - previousMillis >= 5000)) {
    u8x8.clear();
    previousMillis = millis();
    lightBlink = 0;
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.setCursor(0, 0); //defining the starting point for the cursor
    u8x8.print("t > 5");
    u8x8.setCursor(0, 1); //defining the starting point for the cursor
    u8x8.print("/ stop");
    state = 0;
  }
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
