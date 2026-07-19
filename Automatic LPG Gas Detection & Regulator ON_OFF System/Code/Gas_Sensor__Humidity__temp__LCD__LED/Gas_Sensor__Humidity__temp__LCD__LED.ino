#include <LiquidCrystal.h>
#include <dht.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


#define lpg_sensor A1
#define ledPin 13
#define reLay 10
#define dht_apin A0 // Analog Pin sensor is connected to
dht DHT;



void setup() 
{
  Serial.begin(9600);
  pinMode(lpg_sensor, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(reLay,OUTPUT);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LPG Gas Detector");
  lcd.setCursor(1,1);
  lcd.print("& Auto ON/OFF");
  delay(5000);
  lcd.clear();
}

void loop() 
{
  DHT.read11(dht_apin);
   
  delay(50);//Wait 5 seconds before accessing sensor again.

  if(digitalRead(lpg_sensor))
  {
    digitalWrite(ledPin, HIGH);
    digitalWrite(reLay, HIGH);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("LPG Gas Leakage");
    lcd.setCursor(4, 1);
    lcd.print("     Alert     ");
    delay(500);    
  }  
  else 
  {
    digitalWrite(ledPin, LOW);
    digitalWrite(reLay, LOW);
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("No Leakage ");
    delay(3000);
    
    lcd.clear();
    
    lcd.setCursor(1,0);
    lcd.print("Temp = ");
    lcd.print(DHT.temperature);  //printing temperarture to the LCD display
    Serial.print("Temperature = ");
    Serial.println(DHT.temperature);
    lcd.print("'C");
    
    lcd.setCursor(0,1);
    lcd.print("Humi. = ");
    lcd.print(DHT.humidity);  //printing humidity to the LCD display
    Serial.print("Humidity = ");
    Serial.println(DHT.humidity);
    lcd.print(" %");
   
    delay(3000);
  }
}
