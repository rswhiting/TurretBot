#include <SoftwareSerial.h>
#include <Servo.h> 
#include <AFMotor.h>

Servo servoP;
Servo servoY;

AF_DCMotor flywheel(3);
AF_DCMotor fire(4);

void setup()  
{
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    servoP.attach(10);
    servoY.attach(9);
    // Set controls to full speed
    flywheel.setSpeed(255);
    fire.setSpeed(255);
    // Set all controls to release
    flywheel.run(RELEASE);
    fire.run(RELEASE);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for Leonardo only
    }


    Serial.println("Connection established");
}

void loop() // run over and over
{
    read();
}

void read(){ 
    static int v = 0;

    if ( Serial.available()) {
        char ch = Serial.read();

        switch(ch) {
            case '0'...'9':// nums for servos only
                v = v * 10 + ch - '0';
                break;
            case 'p':// pitch
                Serial.print('p');
                Serial.println(v);
                servoP.write(v);
                v = 0;
                break;
            case 'y':// yaw
                Serial.print('y');
                Serial.println(v);
                servoY.write(v);
                v = 0;
                break;
            case 's':// start flywheel
                Serial.println("starting flywheel");
                flywheel.run(FORWARD); 
                v = 0;
                break;
            case 'h':// halt flywheel
                Serial.println("halting flywheel");
                flywheel.run(RELEASE); 
                v = 0;
                break;
            case 'f':// fire
                Serial.println("firing turret");
                fire.run(FORWARD); 
                delay(7500); // hold for 1/2 second
                fire.run(RELEASE); 
                v = 0;
                break;
        }
    }
}
