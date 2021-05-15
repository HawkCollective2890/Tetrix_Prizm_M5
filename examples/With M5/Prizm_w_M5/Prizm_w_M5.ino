5

// pin 8 start button


#include <Wire.h>
 int x;
void setup()
{
  Wire.begin(9);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
    Wire.onRequest(requestEvent);
  Serial.begin(115200);           // start serial for output

}
void requestEvent() {
      Serial.print("request= ");

  // int x = Wire.read();    // receive byte as an integer
 if (x == 8) {
  pinMode(8,INPUT_PULLUP);
  bool pin8=digitalRead(8);
  //pin8=pin8+1;
  Wire.write(pin8);
  x=0;

  
 }
   if (x == 83) {
    int Bvoltage = (analogRead(0) * 2)/10;
   
    Serial.println(Bvoltage);
    Wire.write(Bvoltage);}
//    return(b);

 // }
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
}

void loop()
{
  delay(100);
}
String c;
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
  if ((x == 7) || (x == 6)) {
    int s = Wire.read();    // receive byte as an integer

    Serial.println(s);         // print the integer
    setLED(x, s);
  }
  //else if(x==8){
 ///   rBit=8
  }





void setLED (int pin, int state) {
  pinMode(pin, OUTPUT); //===== Green LED is on pin 7 red on 6
  digitalWrite(pin, state);

}
