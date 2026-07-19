/* This example Arduino Sketch controls the complete rotation of
 *  SG995 Servo motor by using its PWM and Pulse width modulation technique
 */

#include <Servo.h> // include servo library to use its related functions
#define M1 4 // A descriptive name for D6 pin of Arduino to provide PWM signal
#define M2 5
#define M3 6
void Motor1();
void Motor2();
void Motor3();

char b;

Servo Servo1;  // Define an instance of of Servo with the name of "MG995_Servo"
Servo Servo2;
Servo Servo3;

void setup() 
{
  Serial.begin(9600); // Initialize UART with 9600 Baud rate
  Servo1.attach(M1);  // Connect D6 of Arduino with PWM signal pin of servo motor
  Servo2.attach(M2);
  Servo3.attach(M3);
}

void loop() 
{
  if(Serial.available()>0)
  {
    b = Serial.read();
    //Serial.println(b);
  }


  if(b=='A')
  {
    Serial.println("B");
    
      Motor1();
      delay(200);
    Motor2();
    delay(200);
    Motor3();
    delay(200);

    b='C';
  }

}

void Motor1()
{
  Servo1.write(0); //Turn clockwise at high speed
  delay(500);
  Serial.println("0");//Turn left high speed
  Servo1.write(180);
  delay(500);
  Serial.println("180");
}

void Motor2()
{
  Servo2.write(0); //Turn clockwise at high speed
  delay(500);
  Serial.println("0");//Turn left high speed
  Servo2.write(180);
  delay(500);
  Serial.println("180");
}

void Motor3()
{
  Servo3.write(0); //Turn clockwise at high speed
  delay(500);
  Serial.println("0");//Turn left high speed
  Servo3.write(180);
  delay(500);
  Serial.println("180");
}
