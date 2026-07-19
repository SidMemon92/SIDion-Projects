#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>

SoftwareSerial GP(3, 2);

SoftwareSerial GM(1,0);
LiquidCrystal lcd(13,12,11,10,9,8);

TinyGPS gps;
 
int temp = A0;
int pul = A1;
const int trig = 6;
const int echo = 5;
int SW1 = 4;
int Buzzer = 7;

float lat = 0.0;
float lon = 0.0;

float temp_adc_val;
float temp_val;
int sms_count = 0;
int BPM;
long duration;
long int distance;

void setup()
{
  pinMode(temp, INPUT);
  pinMode(pul, INPUT);
  pinMode(SW1, INPUT);
  pinMode (Buzzer, OUTPUT);
  
  Serial.begin(9600);
  GM.begin(9600);
  lcd.begin(20,4);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Women & Child Safety");
  lcd.setCursor(0,2);
  lcd.print("Initializing GSM &");
  lcd.setCursor(5,3);
  lcd.print("GPS Module");

 
  

  delay(1000);

 
}

void loop()
{
  

  int Swi = digitalRead(SW1);
  
  
  if(Swi == HIGH)
  {
    tem();
  pulse();
  dist();
     
    
  if((temp_val<88 || temp_val>100)&&(BPM > 130 || BPM < 60))
  {
    while(sms_count<3)
   {
      SendTextMessage();
   }
  }

  else if((BPM > 130 || BPM < 60) && distance<50)
  {
    while(sms_count<3)
   {
      SendTextMessage();
   }
  }

  else if((temp_val<88 || temp_val>100) && distance<50)
  {
    while(sms_count<3)
   {
      SendTextMessage();
   }
  }

  else if((temp_val<88 || temp_val>100) && distance<50 && (BPM > 130 || BPM < 60))
  {
    while(sms_count<3)
   {
      SendTextMessage();
   }
  }

  else
  {
    lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp  : ");
  lcd.print(temp_val);
  lcd.print("'F");
  lcd.setCursor(0,1);
  lcd.print("Beats : ");
  lcd.print(BPM);
  lcd.print(" BPM");

 
  lcd.setCursor(0,2);
  lcd.print("Dist. : ");
  lcd.print(distance);
  lcd.print(" cm");
  }

  
 delay(500); 
  }

  else if(Swi == LOW)
  {
    dist();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Blind Person Safety");
    lcd.setCursor(8,1);
    lcd.print("MODE");
    lcd.setCursor(4,2);
    lcd.print("Dist. : ");
    lcd.print(distance);
    lcd.print(" cm");
    
    if(distance < 100)
    {
      
      lcd.setCursor(2,3);
      lcd.print("Obstacle Detected");

      digitalWrite(Buzzer, HIGH);

      
      
    }
    else
    {
      digitalWrite(Buzzer, LOW);
    }
    delay(500);
  }
  
 if((temp_val<100 || temp_val>88)||(BPM < 130 || BPM > 60) || distance>50)
 {
  sms_count = 0;
 }
 
}

void tem()
{
  temp_adc_val = (analogRead(temp))/2.048;
  temp_val = (((temp_adc_val)*9.0)/5.0)+32.0;
}

void pulse()
{
  BPM = (analogRead(pul)*4*88)/250;
  delay(20);
}

void dist()
{
  digitalWrite(trig, HIGH);
  delay(1);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = (duration/2)/59.1;
}

void SendTextMessage()
{
  tem();
  pulse();
  dist();

  GP.listen();
  while(GP.available())
  {
    int c = GP.read();
    if(gps.encode(c))
    {
      gps.f_get_position(&lat, &lon);
    }
  }

   lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp  : ");
  lcd.print(temp_val);
  lcd.print("'F");
  lcd.setCursor(0,1);
  lcd.print("Beats : ");
  lcd.print(BPM);
  lcd.print(" BPM");

 
  lcd.setCursor(0,2);
  lcd.print("Dist. : ");
  lcd.print(distance);
  lcd.print(" cm");

  
  GM.println("AT+CMGF=1");    //To send SMS in Text Mode
  delay(200);
  GM.println("AT+CMGS=\"+917057727898\"\r");  // change to the phone number you using
  GM.println("AT+CMGS=\"+919822097859\"\r");  
  delay(200);
  GM.println("Temperature : " + String(temp_val) + " *F");
  GM.println("Beats : " + String(BPM) +  " BPM");
  GM.println("Obstacle : " + String(distance) +  " cm");
  delay(200);
  
  GM.println("Myself ABC...");// The SMS text you want to send
  GM.println("I'm in Danger. I want help.");
  GM.println("My location is ");
  GM.print("Latitude : ");
  GM.println(lat, 6);//Latitude Coordinate
  GM.print("Longitude : ");
  GM.println(lon, 6);//Longitude Coordinate
  delay(100);

  GM.println((char)26);//the stopping character
  delay(500);
  sms_count++;
  
}
