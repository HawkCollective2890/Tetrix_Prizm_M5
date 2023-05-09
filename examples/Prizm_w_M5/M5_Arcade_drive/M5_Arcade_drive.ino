//added commands



// use ESP32 1.0.4 board defs.
#include <PS4Controller.h>
#include <Wire.h>
#include <PRIZM5.h>  // include the PRIZM library in the sketch
#include <FastLED.h>
PRIZM5 prizm;
EXPANSION expan;
int r = 255;
int g = 0;
int b = 0;
int deadZone = 5;  //Used for Arcade drive  ( 2 motors 1 analog Stick )


//servo invert
bool S1 = 0;
bool S2 = 0;
bool CS1 = 0;
bool CS2 = 0;

bool ds = 0;
int Left_motor;
int Right_motor;

int invertSelect = 0;
#define NUM_LEDS 25
#define DATA_PIN 27
CRGB leds[NUM_LEDS];
void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  Wire.begin(26, 32);
  delay(100);
  Serial.begin(115200);

  leds[0] = CRGB::White;
  FastLED.show();
  prizm.PrizmBegin();  // Initialize the PRIZM controller

  // Replace the "03:03:03:03:03:03" with the MAC address
  // the controller wants to pair to

  Serial.println("PS4?");
  PS4.begin();

  while (PS4.isConnected() == 0) {
    leds[0] = CRGB::Red;
    FastLED.show();
    
  }
  PS4.setLed(0, 128, 0);
  PS4.sendToController();
  leds[0] = CRGB::Green;
  FastLED.show();
}

void loop() {
  // Below has all accessible outputs from the controller

  if (PS4.isConnected()) {
    // Inversion();
    driveSelect();
    int motorDebug = 1;          //Set to 1 to see output on serial monitor
    int Deadzone = 5;            // area at the middle of the stick movement that causes not motor movement.
    int coordY = PS4.LStickY();  // Pick left or right stick
    int coordX = PS4.LStickX();
    //  coordY = PS4.RStickX();
    // coordX = PS4.RStickY();
    if (ds == 1) {  // tank drive
      //motor commands
      Right_motor = map(PS4.RStickY(), -127, 127, -100, 100);
      Left_motor = map(PS4.LStickY(), -127, 127, -100, 100);
      //servo commands
      prizm.setServoPosition(1, map(PS4.L2Value(), 0, 255, 0, 180));
      prizm.setServoPosition(2, map(PS4.R2Value(), 0, 255, 0, 180));
    }
    if (ds == 0) {                                           //convert xy into Arcade style drive
      int coordXmapped = map(coordX, -128, 128, 100, -100);  //Swap stick direction by moving - to other pair
      int coordYmapped = map(coordY, -128, 128, 100, -100);  //maps from 0-1023 POT range to -100 - 100 range of formula
      int coordXinvert = (coordXmapped * -1);
      int v = ((100 - abs(coordXinvert)) * (coordYmapped / 100) + coordYmapped);
      int w = ((100 - abs(coordYmapped)) * (coordXinvert / 100) + coordXinvert);
      int rightwheel = ((w - v) / 1);
      int leftwheel = ((v + w) / 1);



      Left_motor = abs(leftwheel) < Deadzone ? 0 : leftwheel;     // add dead zone from Joystick
      Right_motor = abs(rightwheel) < Deadzone ? 0 : rightwheel;  //  add dead zone from Joystick
      Left_motor = constrain(Left_motor, -100, 100);              // find a way to remove these
      Right_motor = constrain(Right_motor, -100, 100);
      prizm.setServoPosition(1, map(PS4.RStickX(), -127, 127, 0, 180));
      prizm.setServoPosition(2, map(PS4.RStickY(), -127, 127, 0, 180));
    }

    if (motorDebug == 1) {
      Serial.print("Mortor Speeds L= ");
      Serial.print(Left_motor);
      Serial.print("    R= ");
      Serial.print(Right_motor);

      Serial.print("  Stick data Y= ");
      Serial.print(coordY);
      Serial.print("   X= ");
      Serial.print(coordX);
      Serial.print("   ds= ");
      Serial.println(ds);
      delay(100);
    }

    prizm.setMotorPower(1, Right_motor);  // spin Motor 1 CW at 25% power

    prizm.setMotorPower(2, Left_motor);  // spin Motor 1 CW at 25% power




    /* Serial.println(prizm.readStartButton());
      if (PS4.Right()) Serial.println("Right Button");
      if (PS4.Down()) Serial.println("Down Button");
      if (PS4.Up()) Serial.println("Up Button");
      if (PS4.Left()) Serial.println("Left Button");
      Serial.println(prizm.readDigitalPin(8));
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
       float vBat = prizm.readBatteryVoltage();
       vBat = (vBat / 10.0); // set up voltage to 1 decimal point
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
         // prizm.setMotorPower(1, Motor1);   // spin Motor 1 CW at 25% power
       }
       else if (PS4.Down()) {
         int Motor1 = map(PS4.L2Value(), 0, 255, 0, -100);
         //prizm.setMotorPower(1, Motor1);   // spin Motor 1 CW at 25% power
       }
      }
      else {
       prizm.setMotorPower(1, 0);  // spin Motor 1 CW at 25% power
      }
      if (PS4.R2()) {
       Serial.printf("R2 button at %d\n", PS4.R2Value());
      }
      //prizm.arcadeDrive(1, deadZone); //uncomment and update Sticks and Deadzone for Arcade drive
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

      if (PS4.Charging()) Serial.println("The controller is charging");
      if (PS4.Audio()) Serial.println("The controller has headphones attached");
      if (PS4.Mic()) Serial.println("The controller has a mic attached");

      //  Serial.printf("Battery Level : %d\n", PS4.Battery());

      Serial.println();
      // This delay is to make the output more human readable
      // Remove it when you're not trying to see the output
      delay(100);*/
  }
}
void Inversion() {
  Serial.println("invert Mode");
  if (S1 == 0) {
    leds[20] = CRGB::Green;
  }
  if (S2 == 0) {
    leds[21] = CRGB::Green;
  }
  if (CS1 == 0) {
    leds[22] = CRGB::Green;
  }
  if (CS2 == 0) {
    leds[23] = CRGB::Green;
  }
  FastLED.show();
  if (PS4.Options()) {

    invertSelect++;

    /*  while (PS4.Options()) {
      PS4.Options();
      Serial.println("Set Invert ");
      Serial.println(invertSelect);
    }*/
  }
  while (invertSelect != 0) {
    Serial.println(invertSelect);
    if (invertSelect == 1) {
      leds[15] = CRGB::White;
      if (S1 == 0)
        leds[20] = CRGB::Green;


      if (S1 == 1) {
        leds[20] = CRGB::Red;
      }
      if (PS4.Options()) {
        S1++;
        //  while (PS4.Share()) {}
      }
      FastLED.show();
    }

    if (PS4.Options()) {
      invertSelect++;
      if (invertSelect > 3) {
        invertSelect = 0;
      }
    }
  }
}

void driveSelect() {
  Serial.println("DS");
  if (PS4.PSButton()) {

    if (ds == 0) {
      ds = 1;
      leds[2] = CRGB::Blue;
    } else {
      ds = 0;
      leds[2] = CRGB::Yellow;
    }
    FastLED.show();
    delay(500);
  }
}