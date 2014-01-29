#include <SoftwareSerial.h>
#include <Servo.h> 

Servo servoP;
Servo servoY;

void setup()  
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  servoP.attach(10);
  servoY.attach(9);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.println("Connection established");
}

void loop() // run over and over
{
  read();
  //if (Serial.available()){
    //int packet = Serial.read();
    //Serial.println(packet);
  //}
}

void read(){
 static int v = 0;

  if ( Serial.available()) {
    char ch = Serial.read();

    switch(ch) {
      case '0'...'9':
        v = v * 10 + ch - '0';
        break;
      case 'p':
        Serial.print('p');
        Serial.println(v);
        servoP.write(v);
        v = 0;
        break;
      case 'y':
        Serial.print('y');
        Serial.println(v);
        servoY.write(v);
        v = 0;
        break;
      case 's':
        Serial.print('f');
        Serial.println(v);
        v = 0;
        break;
      case 'f':
        Serial.print('f');
        Serial.println(v);
        v = 0;
        break;
    }
  }
}
