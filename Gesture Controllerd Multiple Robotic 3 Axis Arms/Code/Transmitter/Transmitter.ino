#define Base A0     // x_axis
#define Arm A1      // y_axis
#define Finger A2   // Flex

int M1;
int M2;
int Flex;

void setup() 
{
  Serial.begin(9600);         // Serial Monitor Baud Rate setting
  pinMode(Base, INPUT);       // Input/Output pin define
  pinMode(Arm, INPUT);        // Input/Output pin define
  pinMode (Finger, INPUT);    // Input/Output pin define
}

void loop() 
{
  M1 = analogRead(Base);              // Analog pin value read
  M1 = map(M1,265,415,0,180);         // conversion 0 - 180 degree
  
  M2 = analogRead(Arm);               // Analog pin value read
  M2 = map(M2, 250, 400, 30,150);      // conversion 0 - 180 degree

  Flex = analogRead(Finger);          // Analog pin value read
  Flex = map(Flex, 600, 850, 60,11 \0);   // conversion 0 - 180 degree

  Serial.print(M1);                   // Display on Serial Monitor
  Serial.print(".");
  Serial.print(M2);
  Serial.print(";");
  Serial.print(Flex);
  Serial.println(",");

  delay(1000);               // Time Delay
}
