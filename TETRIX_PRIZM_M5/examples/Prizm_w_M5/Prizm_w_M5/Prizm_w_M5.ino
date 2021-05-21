
//DigitalWrite to all pins - done
//digitalRead from all pins - done
//analogWrite to corret pins - done
// pin 8 start button - done
// Green LED - done
// Red LED - done
//analogRead from A# pins-done

//Sensors
//UltraSonic sensors
//Linefinder

#include <Wire.h>
#include "Ultrasonic.h"
int x;
Ultrasonic ultrasonic(3);
void setup()
{
  Wire.begin(9);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent);
  Serial.begin(115200);           // start serial for output
  pinMode(8, INPUT);
  //digitalWrite(8, LOW);

}
void requestEvent() {
  Serial.print("request= ");

  // int x = Wire.read();    // receive byte as an integer

  if ((x >= 20) && (x <= 29)) { // Digital read of all pins
    x = x - 20;
    pinMode(x, INPUT_PULLUP);
    bool pin = digitalRead(x);
    //pin=pin,HEX;
    Wire.write(pin);
    x = 0;


  }

  /* // need to figure out 2 byte transmisison
    if ((x >= 40)&&(x<=49)) {// analog read of all pins
    x=x-40;
    pinMode(x,INPUT_PULLUP);
    bool pin=analogRead(x);
    pin=pin,HEX;
    Wire.write(pin);
    x=0;


    }*/
  if (x == 83) { // Read battery voltage from Prizm
    byte Bvoltage = (analogRead(0) * 2) / 10;
    Serial.print("voltage = ");
    Serial.println(Bvoltage);
    Wire.write(Bvoltage);
    x = 0;
  }
  if ((x >= 41) && (x <= 43)) {
    x = x - 40;
    int aRead = analogRead(x);

    byte highbyte = aRead >> 8; //shift right 8 bits, leaving only the 8 high bits.
    byte lowbyte = aRead & 0xFF; //bitwise AND with 0xFF

    Wire.write(highbyte);
    Wire.write(lowbyte);
    delay(10);

  }
  if (x == 60) {
    int s = Wire.read();
 //Ultrasonic ultrasonic(s);

  byte duration = ultrasonic.MeasureInInches();
  Wire.write(duration); 
  Serial.print("US = ");
  Serial.println(duration);
  }

  if (x == 61) {
    int s = Wire.read();
    UltrasonicCm(s);
  }
}
// Setup byte variable in the correct size
/* byte response[ANSWERSIZE];

  // Format answer as array
  for (byte i=0;i<ANSWERSIZE;i++) {
   response[i] = (byte)answer.charAt(i);
  }

  // Send response back to Master
  Wire.write(response,sizeof(response));

  // Print to Serial Monitor
  Serial.println("Request event");*/



void loop()
{
  delay(10);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()

void receiveEvent(int howMany)
{
  /*  while(1 < Wire.available()) // loop through all but the last
     {
      c =+ Wire.read(); // receive byte as a character

     }
        //int  incoming= atoi(c);
          Serial.println(c);         // print the character

  */
  x = Wire.read();    // receive byte as an integer
  Serial.print(x);
  Serial.print("  ");
  if (x == 100) {
    Wire.begin(9);                // join i2c bus with address #4
    Wire.onReceive(receiveEvent); // register event
    Wire.onRequest(requestEvent);

  }

  if ((x >= 2) && (x <= 17 )) { //block out i2c
    int s = Wire.read();    // receive byte as an integer

    Serial.println(s);         // print the integer
    setPin(x, s);
  }
  if ((x == 23) || (x == 25) || (x == 26) || (x == 29) || (x == 30) || (x == 31)) { //analog pins available on
    int s = Wire.read();

    Serial.println(s);
    setAPin(x, s);
  }


}

// Reset


// Outoputs / sensors


void setPin (int pin, int state) { // digitalWrite of any pins
  pinMode(pin, OUTPUT); //===== Green LED is on pin 7 red on 6
  digitalWrite(pin, state);

}
void setAPin (int pin, int state) { // analogWrite of any pins
  pinMode(pin, OUTPUT);
  analogWrite(pin, state);

}





int UltrasonicIn(byte pin) {
 
}
int UltrasonicCm(byte pin) {
  Ultrasonic ultrasonic(pin);
  delayMicroseconds(500);  // added in version 2 to help with reading accuracy, can't read sonic sensors very fast
  int duration;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delayMicroseconds(2);
  digitalWrite(pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pin, LOW);
  pinMode(pin, INPUT);
  duration = pulseIn(pin, HIGH);
  Wire.write( duration / 29 / 2); // convert time of echo to centimeters distance
}
