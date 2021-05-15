#include <PS4Controller.h>
#include<Wire.h>
#include <PRIZM5.h>    // include the PRIZM library in the sketch
#include <FastLED.h>
PRIZM5 prizm;
EXPANSION expan;
int r = 255;
int g = 0;
int b = 0;
#define NUM_LEDS 1
#define DATA_PIN 27
CRGB leds[NUM_LEDS];
void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  Wire.begin(26, 32);
  delay(100);
  Serial.begin(115200);

  leds[0] = CRGB::Green;
  FastLED.show();
  prizm.PrizmBegin();   // Initialize the PRIZM controller

  // Replace the "03:03:03:03:03:03" with the MAC address
  // the controller wants to pair to

  Serial.println("PS4?");
  PS4.begin();

  while (PS4.isConnected()==0) {
    leds[0] = CRGB::Red;
    FastLED.show();
  }
  leds[0] = CRGB::Black;
  FastLED.show();

}

void loop() {
  // Below has all accessible outputs from the controller
  if (PS4.isConnected()) {
    // Serial.println(prizm.readStartButton());
    if (PS4.Right()) Serial.println("Right Button");
    if (PS4.Down()) Serial.println("Down Button");
    if (PS4.Up()) Serial.println("Up Button");
    if (PS4.Left()) Serial.println("Left Button");
    Serial.println(prizm.readAnalogPin(8));
    if (PS4.Square()) {
      Serial.println("Square Button");
      prizm.setRedLED(1);
    }
    if (PS4.Cross()) {
      Serial.println("Cross Button");
      prizm.setGreenLED(0);
    }
    if (PS4.Circle()) {
      Serial.println("Circle Button");
      prizm.setRedLED(0);
    }
    if (PS4.Triangle()) {
      Serial.println("Triangle Button");
      prizm.setGreenLED(1);
    }

    if (PS4.UpRight()) Serial.println("Up Right");
    if (PS4.DownRight()) Serial.println("Down Right");
    if (PS4.UpLeft()) Serial.println("Up Left");
    if (PS4.DownLeft()) Serial.println("Down Left");

    if (PS4.L1()) {
      Serial.println("L1 Button");
      float vBat = expan.readBatteryVoltage(9);
      vBat = (vBat / 10); // set up voltage to 1 decimal point
      Serial.println(vBat, 1);
    }
    if (PS4.R1()) Serial.println("R1 Button");

    if (PS4.Share()) Serial.println("Share Button");
    if (PS4.Options()) Serial.println("Options Button");
    if (PS4.L3()) Serial.println("L3 Button");
    if (PS4.R3()) Serial.println("R3 Button");

    if (PS4.PSButton()) Serial.println("PS Button");
    if (PS4.Touchpad()) Serial.println("Touch Pad Button");

    if (PS4.L2()) {
      Serial.printf("L2 button at %d\n", PS4.L2Value());
      if (PS4.Up()) {
        int Motor1 = map(PS4.L2Value(), 0, 255, 0, 100);
        prizm.setMotorPower(1, Motor1);   // spin Motor 1 CW at 25% power
      }
      else if (PS4.Down()) {
        int Motor1 = map(PS4.L2Value(), 0, 255, 0, -100);
        prizm.setMotorPower(1, Motor1);   // spin Motor 1 CW at 25% power
      }
    }
    else {
      prizm.setMotorPower(1, 0);  // spin Motor 1 CW at 25% power
    }
    if (PS4.R2()) {
      Serial.printf("R2 button at %d\n", PS4.R2Value());
    }
    /*
        if (PS4.LStickX()) {
          Serial.printf("Left Stick x at %d\n", PS4.LStickX());
        }
        if (PS4.LStickY()) {
          Serial.printf("Left Stick y at %d\n", PS4.LStickY());
        }
        if (PS4.RStickX()) {
          Serial.printf("Right Stick x at %d\n", PS4.RStickX());
        }
        if (PS4.RStickY()) {
          Serial.printf("Right Stick y at %d\n", PS4.RStickY());
        }
    */
    if (PS4.Charging()) Serial.println("The controller is charging");
    if (PS4.Audio()) Serial.println("The controller has headphones attached");
    if (PS4.Mic()) Serial.println("The controller has a mic attached");

    //  Serial.printf("Battery Level : %d\n", PS4.Battery());

    Serial.println();
    // This delay is to make the output more human readable
    // Remove it when you're not trying to see the output
    delay(100);
  }
}
