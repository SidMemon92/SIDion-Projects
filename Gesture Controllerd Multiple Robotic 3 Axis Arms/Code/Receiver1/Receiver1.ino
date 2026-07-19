#include <Servo.h>    // Servo motor library

#define Base 4        // Motor 1 (x_axis)
#define Arm 5         // Motor 2 (y_axis)
#define Finger 6      // Motor 3 (Flex)

void Motor1();
void Motor2();
void Motor3();

String M1;    
String M2;
String Flex;

Servo myServo;        // Library Variable for Motor 1
Servo myServo1;       // Library Variable for Motor 2
Servo myServo2;       // Library Variable for Motor 3

void setup() 
{
  Serial.begin(9600);           // Serial Monitor Baud Rate setting
  myServo.attach(Base);         // Connect Motor 1 to pin no. 4
  myServo1.attach(Arm);         // Connect Motor 2 to pin no. 5
  myServo2.attach(Finger);      // Connect Motor 3 to pin no. 6
}

void loop() 
{
  if( Serial.available( )>0)  
  {  
    
     M1 = Serial.readStringUntil('.');      // Read the received value
      
     M2 = Serial.readStringUntil(';');     // Read the received value
      
     Flex = Serial.readStringUntil(',');   // Read the received value

     Motor1();
     delay(100);
     Motor2();
     delay(100);
     Motor3();
     delay(100);
  }
}

void Motor1()
{
  Serial.print("M1 = ");                
  Serial.println(M1);
  myServo.write(M1.toInt());
}

void Motor2()
{
  Serial.print("M2 = ");
  Serial.println(M2);
  myServo1.write(M2.toInt());
}

void Motor3()
{
  Serial.print("Flex = ");
  Serial.println(Flex);
  myServo2.write(Flex.toInt()); 
}
