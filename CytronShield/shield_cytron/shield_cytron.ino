#include <Encoder.h>
#include "CytronMotorDriver.h"

// Cytron + Moteur Polulu + Encodeur
// Entrée interruption sur arduino : 2 ET 3


Encoder myEnc(2, 3); // 
CytronMD motor(PWM_DIR, 5, 4);  // PWM = Pin 5, DIR = Pin 4


void setup() {
  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");
}

long oldPosition  = -999;
float kp = 10;
float consigne = 3200. ;  
float epsilon = 3200.; 
float mesure = 0.;
float cmot ;
void loop() {
  
  long newPosition = myEnc.read();
  mesure = (float) newPosition ;
  epsilon = consigne - mesure ;
  cmot = epsilon * kp;
  if (cmot > 255) {
    cmot = 255;
  }
  if (cmot < -255) {
    cmot = -255;
  }
  
  motor.setSpeed(cmot);  // Run forward at 50% speed.
  Serial.print(newPosition);
  Serial.print(",");
  Serial.print(consigne);
  Serial.print(",");
  Serial.print(kp*epsilon);
  Serial.print(",");
  Serial.print(cmot);
  Serial.print("\n");

  
  /*
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.println(newPosition);
  }

  motor.setSpeed(128);  // Run forward at 50% speed.
  delay(1000);
  
  motor.setSpeed(255);  // Run forward at full speed.
  delay(1000);

  motor.setSpeed(0);    // Stop.
  delay(1000);

  motor.setSpeed(-128);  // Run backward at 50% speed.
  delay(1000);
  
  motor.setSpeed(-255);  // Run backward at full speed.
  delay(1000);

  motor.setSpeed(0);    // Stop.
  delay(1000);*/

}