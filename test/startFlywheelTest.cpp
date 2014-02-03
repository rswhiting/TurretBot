// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include <AFMotor.h>

AF_DCMotor motor(4);

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Motor test!");

  // turn on motor
  motor.setSpeed(255);
 
  motor.run(RELEASE);
}

void loop() {
  
  Serial.println("on");
  motor.run(FORWARD); 
  delay(5000);
  
  Serial.println("off");
  motor.run(RELEASE);
  delay(5000);
 
}
