#include <Wire.h>
#include <FastLED.h>
#include <PS4Controller.h>
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include <WiFi.h>

#define SerialOut Serial
int r = 255;
int g = 0;
int b = 0;
#define NUM_LEDS 25
#define DATA_PIN 27
CRGB leds[NUM_LEDS];
// MACROS
char MAC[18];


uint16_t yc;
uint16_t xc;
int accely2;
int accelx2;
int x = 0;
bool startup = 0;
volatile bool g_event_happened = false;
bool g_previously_connected = false;
bool hold = 0;
int dzL;
int dzR;
int dzFixLx;
int dzFixLy;
int dzFixRx;
int dzFixRy;
int M5Button = 39;
//Controller LED colors
#define white 1
#define red 2
#define blue 3
#define black 4
#define ltBlue 5


/*
M5 Atom
   _______
   |*****| Controller Connection
   |*****| Top of controller color
   |*****| Bottom of Controller color
   |*****|
   |*****| Team Color
   -------

*/
//Controller Colors See FastLED Keywords for more color names.
#define topColor Blue
#define bottomColor CornflowerBlue

//Team colors
#define team Black


void ps4_callback() {
  g_event_happened = true;
}



void setup() {
  Serial.begin(115200);
  pinMode(M5Button, INPUT);

  // lcd.setRGB(colorR, colorG, colorB);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(128);
  FastLED.clear();
  for (int i = 5; i < 10; i++) {  // see fastLED keywords for colors
    leds[i] = CRGB::topColor;     // top color of controller
  }
  for (int i = 10; i < 15; i++) {
    leds[i] = CRGB::bottomColor;  // bottom color of controller
  }

  for (int i = 0; i < 5; i++) {
    leds[i] = CRGB::Red;
  }

  FastLED.show();
  //
  while (!Serial && millis() < 5000)
    ;
  Serial.print("ESP Board MAC Address:  ");
  WiFi.macAddress().toCharArray(MAC, 18);
  Serial.println(MAC);

  PS4.begin();

  Serial.println("Waiting for Prizm.");

  delay(200);
  Serial.println("waiting for PS4!");

  PS4.attach(&ps4_callback);

  //ControllerDesplay(white,white); //Controller color top,bottom
  while (x = 0) {}
}

typedef union {
  uint32_t data;
  ps4_button_t button;
} button_data_t;


ps4_t ps4data_prev;
// BUGBUG we won't copy or look at latestpacket...
uint32_t ps4_data_size = sizeof(ps4_t) - sizeof(uint8_t*);

void loop() {
  //button on Atom --- Press to find controller.--
  if (digitalRead(M5Button) == 0) {
    //rumble for 5 seconds
    Serial.print("button");
    for (int R = 0; R < 5; R++) {
      PS4.setRumble(0, 255);
      PS4.sendToController();
      delay(500);
      PS4.setRumble(255, 0);
      PS4.sendToController();
      delay(500);
    }
    PS4.setRumble(0, 0);
    PS4.sendToController();
  }
  bool isConnected = PS4.isConnected();
  if (g_previously_connected != isConnected) {
    if (isConnected) {
      for (int i = 0; i < 5; i++) {
        leds[i] = CRGB::Green;
      }
      FastLED.show();
      PS4.setLed(0, 128, 0);
      PS4.sendToController();

      Serial.println("*** Connected ***");


    } else {
      Serial.println("*** DisConnected ***");

      PS4.setLed(128, 0, 0);
      PS4.sendToController();
      for (int i = 0; i < 5; i++) {
        leds[i] = CRGB::Red;
      }
      FastLED.show();
    }
    g_previously_connected = isConnected;
  }
  /* At this point, the Esp32 is ready and waiting for the PS4 remote to connect, it will connect automatically once the PS4 remote is turned on using the PS4 button
        There are some buttons/events available in the "PS4Controller.h" library that are unused in this case*/
  if (g_event_happened && isConnected) {
    if (memcmp(&PS4.data, &ps4data_prev, ps4_data_size)) {
      memcpy(&ps4data_prev, &PS4.data, ps4_data_size);
      if (startup == 0) {  // do only once after startup

        startup = 1;

        Wire.begin(8, 21, 25, 100000);
        Wire.onReceive(receiveEvent);  // register event
        Wire.onRequest(requestEvent);
      }

      button_data_t button_data;
      g_event_happened = false;
      button_data.button = PS4.data.button;
      //   PS4.setRumble(128, 128);
      /* Serial.printf("%08X %3d - 0%4d %4d %4d %4d %4d %4d",
                    button_data.data, PS4.Battery(),
                    PS4.data.analog.stick.lx,
                    PS4.data.analog.stick.ly,
                    PS4.data.analog.stick.rx,
                    PS4.data.analog.stick.ry,
                    PS4.data.analog.button.l2,
                    PS4.data.analog.button.r2);*/

      // Trackpad touch 1: id, active, x, y
      xc = ((PS4.data.latestPacket[37 + 12] & 0x0f) << 8) | PS4.data.latestPacket[36 + 12];
      yc = PS4.data.latestPacket[38 + 12] << 4 | ((PS4.data.latestPacket[37 + 12] & 0xf0) >> 4);
      uint8_t isTouched = (PS4.data.latestPacket[35 + 12] & 0x80) == 0;
      if (!isTouched) xc = yc = 0;
      //Serial.printf(" - %4u %4u", xc, yc);

      // Accel info
      /* not the best choice for data menipulaton should use something like this:
pitch = (atan2f(accely, accelz) + PI) * RAD_TO_DEG;
but I could not get that to work. 
found therer is a "flat spot" in with R2 corner pointing up*/

      int accelx = (int16_t)(PS4.data.latestPacket[12 + 20] << 8) | PS4.data.latestPacket[12 + 19];
      int accely = (int16_t)(PS4.data.latestPacket[12 + 22] << 8) | PS4.data.latestPacket[12 + 21];
      int accelz = (int16_t)(PS4.data.latestPacket[12 + 24] << 8) | PS4.data.latestPacket[12 + 23];

      float ax = (float)accelx / 8192;
      float ay = (float)accely / 8192;
      float az = (float)accelz / 8192;
      accely2 = accely;

      //float accely2 = (atan2(accely, accelz) + PI) * RAD_TO_DEG;
      if (accely > 0 && accelz > 0) {
        accely2 = map(accely, 8400, 0, 180, 270);
      } else if (accely > 0 && accelz < 0) {
        accely2 = map(accely, 0, 8400, 90, 180);
      } else if (accely < 0 && accelz < 0) {
        accely2 = map(accely, 0, -8000, 90, 1);
      } else if (accely < 0 && accelz > 0) {
        accely2 = map(accely, 0, -8000, 270, 360);
      }
      if (accely2 < 1) { accely2 = 1; }


      accelx2 = accelx;
      //float accely2 = (atan2(accely, accelz) + PI) * RAD_TO_DEG;
      if (accelx < 0 && accely > 0) {
        accelx2 = map(accelx, 0, -8200, 180, 270);
      } else if (accelx > 0 && accely > 0) {
        accelx2 = map(accelx, 0, 8200, 180, 90);
      } else if (accelx > 0 && accely < 0) {
        accelx2 = map(accelx, 0, 8200, 1, 90);
      } else if (accelx < 0 && accely < 0) {
        accelx2 = map(accelx, 1, -8200, 360, 270);
      }
      if (accelx2 < 1) { accelx2 = 1; }
      // Serial.printf(" - %4d(%3.2f) %4d(%3.2f) %4d(%3.2f)", accelx, ax, accely, ay, accelz, az);


      // Gyro info
      int gyroy = (int16_t)(PS4.data.latestPacket[12 + 14] << 8) | PS4.data.latestPacket[12 + 13];
      int gyroz = (int16_t)(PS4.data.latestPacket[12 + 16] << 8) | PS4.data.latestPacket[12 + 15];
      int gyrox = (int16_t)(PS4.data.latestPacket[12 + 18] << 8) | PS4.data.latestPacket[12 + 17];

      float gx = (float)gyrox * RAD_TO_DEG / 1024;
      float gy = (float)gyroy * RAD_TO_DEG / 1024;
      float gz = (float)gyroz * RAD_TO_DEG / 1024;
      // Serial.printf(" - %4d(%3.2f) %4d(%3.2f) %4d(%3.2f)\n", gyrox, gx, gyroy, gy, gyroz, gz);

      //Deadzoning analog sticks
      dzFixLx = (int16_t)(PS4.data.latestPacket[13]);
      dzFixLy = (int16_t)(PS4.data.latestPacket[14]);
      dzFixRx = (int16_t)(PS4.data.latestPacket[15]);
      dzFixRy = (int16_t)(PS4.data.latestPacket[16]);
      if ((dzFixLx > 128 - (.5 * dzL)) && (dzFixLx < (.5 * dzL) + 128)) {
        dzFixLx = 128;
      }
      if ((dzFixLy > 128 - (.5 * dzL)) && (dzFixLy < (.5 * dzL) + 128)) {
        dzFixLy = 128;
      }
      if ((dzFixRx > 128 - (.5 * dzR)) && (dzFixRx < (.5 * dzR) + 128)) {
        dzFixRx = 128;
      }
      if ((dzFixRy > 128 - (.5 * dzR)) && (dzFixRy < (.5 * dzR) + 128)) {
        dzFixRy = 128;
      }

      Serial.print(dzFixLx);
      Serial.print("  ");
      Serial.print(dzFixLy);
      Serial.print("  ");
      Serial.print(dzFixRx);
      Serial.print("  ");
      Serial.println(dzFixRy);
    }
  }
}


void requestEvent() {

  Wire.write(PS4.data.latestPacket[17]);  //buttons_1
  Wire.write(PS4.data.latestPacket[19]);  //buttons_2
  Wire.write(PS4.Touchpad());             //TouchB
  Wire.write(PS4.data.latestPacket[19]);  //PBb
  //Wire.write(PS4.data.latestPacket[13]);  //LxStick
  Wire.write(dzFixLx);                    //LxStick dz Fixed
                                          // Wire.write(PS4.data.latestPacket[14]);  //LyStick
  Wire.write(dzFixLy);                    //LyStick dz Fixed
                                          // Wire.write(PS4.data.latestPacket[15]);  //RxStick
  Wire.write(dzFixRx);                    //RxStick dz Fixed
                                          // Wire.write(PS4.data.latestPacket[16]);  //RyStick
  Wire.write(dzFixRy);                    //RyStick dz Fixed
  Wire.write(PS4.data.latestPacket[20]);  //L2Trigger
  Wire.write(PS4.data.latestPacket[21]);  // R2Trigger
  Wire.write(0b0);                        //LxMotor
  Wire.write(0b0);                        //LyMotor
  Wire.write(0b0);                        //RxMotor
  Wire.write(0b0);                        //RyMotor
  Wire.write(0b0);                        //LxServo
  Wire.write(0b0);                        //LyServo
  Wire.write(0b0);                        //RxServo
  Wire.write(0b0);                        //RyServo
  Wire.write(0b0);                        //LtMotor
  Wire.write(0b0);                        //RtMotor
  Wire.write(0b0);                        //LtServo
  Wire.write(0b0);                        //RtServo

  Wire.write(accely2 % 256);  //Pitch_LO
  Wire.write(accely2 / 256);  //Pitch_HI
  Wire.write(accelx2 % 256);  //Roll_LO
  Wire.write(accelx2 / 256);  //Roll_HI

  Wire.write(xc % 256);  //TouchX_LO
  Wire.write(xc / 256);  //TouchX_HI

  Wire.write(yc % 256);  //TouchY_LO
  Wire.write(yc / 256);  //TouchY_HI

  Wire.write(PS4.isConnected());  //Status
  Wire.write(0b0);                //inRange
}

void receiveEvent(int howMany) {

  x = Wire.read();  // receive byte as an integer
                    // Serial.print("    recieve bit = ");
  Serial.println(x);
  Serial.print("  ");

  if (x == 0x21) {
    // Wire.end();
    Wire.begin(8);
  }
  if (x == 0x20) {  //32
    int color = Wire.read();
    Serial.println(color);
    if (color == 1) {
      Serial.println("red");
      PS4.setLed(255, 0, 0);
      PS4.sendToController();
      for (int i = 20; i < 25; i++) {
        leds[i] = CRGB::Red;  // bottom color of controller
        FastLED.show();
      }
    }
    
    if (color == 2) {
      Serial.println("Blue");
      PS4.setLed(0, 0, 255);
      PS4.sendToController();
      for (int i = 20; i < 25; i++) {
        leds[i] = CRGB::Blue;  // bottom color of controller
        FastLED.show();
      }
    }
      if (color == 3) {
        Serial.println("Yellow");
        PS4.setLed(255, 255, 0);
        PS4.sendToController();
        for (int i = 20; i < 25; i++) {
          leds[i] = CRGB::Yellow;  // bottom color of controller
          FastLED.show();
        }
      }
    

        if (color == 4) {
          Serial.println("Green");
          PS4.setLed(0, 255, 0);
          PS4.sendToController();
          for (int i = 20; i < 25; i++) {
            leds[i] = CRGB::Green;  // bottom color of controller
            FastLED.show();
          }
        }




        if (x == 0x1F) {
          int rumble;
          int read = Wire.read();  // receive byte as an integer
          Serial.println(read);
          if (read == 0) { rumble = 0; }
          if (read == 1) { rumble = 128; }
          if (read == 2) { rumble = 255; }
          PS4.setRumble(rumble, rumble);
          PS4.sendToController();
        }
        if (x == 0x22) {      //deadzone right
          dzR = Wire.read();  // receive byte as an integer
        }

        if (x == 0x23) {      //deadzone left
          dzL = Wire.read();  // receive byte as an integer
        }

        x = 0;
      }
    }
