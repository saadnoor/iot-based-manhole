#include <OneWire.h> 
#include <DallasTemperature.h>
#include <Wire.h>
#include <SPI.h>
//#include <NewPing.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SoftwareSerial.h>
SoftwareSerial esp8266(11,10);
#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11 
#define BMP_CS 10

Adafruit_BMP280 bmp;
#define ONE_WIRE_BUS 3 
#define MAX_TEMP 30

const int trigPin = 5;
const int echoPin = 6;
// defines variables
long duration;
int distance;

OneWire oneWire(ONE_WIRE_BUS); 
// NewPing sonar(31, 33, 1234);
DallasTemperature sensors(&oneWire);
int flag,gss;
int lpgGasFlag=0;
int coGasFlag=0;
int overFlowFlag=0;
int lidFlag=1;
int LED = 8;
int msensor = 4;              // the pin that the sensor is atteched to
int mstate = LOW;             // by default, no motion detected
int mval = 0;   
int tmp,co,lid,lpgg,ov;

void setup() {
sensors.begin(); 
flag=0;
pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input
Serial.begin(9600); // Starts the serial communication
}
void loop() {
// Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);
// Calculating the distance
distance= duration*0.034/2;
// Prints the distance on the Serial Monitor
//Serial.print("Distance: ");
//Serial.println(distance);

int lpgAnalogal=analogRead(5);
  if(lpgAnalogal>=400)lpgGasFlag=1;
  else lpgGasFlag=0;
//  Serial.println(lpgAnalogal);
  delay(100);
  int co=analogRead(A7);
  Serial.println(co);
  if(co>=305)coGasFlag=1;
  else coGasFlag=0;
  int sensorReading = analogRead(A1);
  
   int aValue =analogRead(A8);
  // Serial.println(aValue);
   gss = map(aValue, 102, 922 , -640, 640); 
   Serial.println("gauss value");
   Serial.println(gss);
   int range = map(sensorReading, 0, 1024, 0, 3);
     switch (range) {
 case 0:    
    overFlowFlag=2;
   // Serial.println("Overflow");
    break;
 case 1:    
    overFlowFlag=1;
    //Serial.println("Overflow Warning");
    break;
 case 2:    
    overFlowFlag=0;
    //Serial.println("Not Overflow");
    break;
  }

//Serial.println(gss);
  if(abs(gss)<=100) {
      
      lidFlag=0;
     // Serial.print("NO LID ");
    //Serial.println(gss); 
    if(distance>7 && distance<=35)tone(7,1000);
    else noTone(7);
Serial.print("Distance : ");
Serial.println(distance);
// Prints the distance on the Serial Monitor
/*if(distance<=35 && distance>=8){
  tone(7, 1000);
}
else noTone(7);*/

     mval = digitalRead(msensor);   // read sensor value
  if (mval == HIGH) {           // check if the sensor is HIGH
   // digitalWrite(led, HIGH);   // turn LED ON
    //delay(100);                // delay 100 milliseconds 
    // tone(7, 1000);
    if (mstate == LOW) {
      Serial.println("Motion detected!"); 
     
      mstate = HIGH;       // update variable state to HIGH
    }
   
  } 
  else {
     // digitalWrite(led, LOW); // turn LED OFF
      //delay(200);             // delay 200 milliseconds 
      
      if (mstate == HIGH){
        Serial.println("Motion stopped!");
        mstate = LOW;       // update variable state to LOW
        
    }
   // noTone(7);
  }
}
  else {
    noTone(7);
        lidFlag=1;
       // Serial.print("LID Detected ");
       // Serial.println(gss);
   
  }
  
 sensors.requestTemperatures(); // Send the command to get temperature readings 
  
 //Serial.println("Temperature is: "); 
 int tempPrint=(int)sensors.getTempCByIndex(0); 

 
    if(coGasFlag==1)digitalWrite(LED, HIGH);
    else digitalWrite(LED, LOW);
  
  tmp=tempPrint;
  long prs=(long)bmp.readPressure();
  co=coGasFlag;
  lid=lidFlag;
  lpgg=lpgGasFlag;
  ov=overFlowFlag;

  esp8266.begin(115200);
  esp8266.print("AT+CIFSR\r\n");
  delay(100);
  esp8266.print("AT+CIPMUX=1\r\n");
  delay(100);
  esp8266.print("AT+CIPSTART=1,\"TCP\",\"192.168.43.23\",80\r\n");
  delay(100);
  esp8266.print("AT+CIPSEND=1,135\r\n");
  delay(100);

   /*Serial.print("GET /basementlog.php?level=");
  Serial.print(tmp);
  Serial.print("&fahrenheit=");
  Serial.print(prs);
  Serial.print("&humidity=");
  Serial.print(co);
  Serial.print("&lid=");
  Serial.print(lid);
  Serial.print("&lpg=");
  Serial.print(lpgg);
  Serial.print("&over=");
  Serial.print(ov);
  Serial.print(" HTTP/1.1\r\n");
*/



  esp8266.print("GET /basementlog.php?level=");
  esp8266.print(tmp);
  esp8266.print("&fahrenheit=");
  esp8266.print(prs);
  esp8266.print("&humidity=");
  esp8266.print(co);
  esp8266.print("&lid=");
  esp8266.print(lid);
  esp8266.print("&lpg=");
  esp8266.print(lpgg);
  esp8266.print("&over=");
  esp8266.print(ov);
  esp8266.print(" HTTP/1.1\r\n");
  esp8266.print("Host: 192.168.43.23\r\n");
  esp8266.print("connection:keep-alive\r\n");
  esp8266.print("\r\n");
  esp8266.print("\r\n");

/*int gss;
 int aValue =analogRead(A0);
  // Serial.println(aValue);
   gss = map(aValue, 102, 922 , -640, 640);
   Serial.println(gss);
//Serial.println(distance);
if(distance<=35 && distance>1) tone(7, 1000);
else noTone(7);*/

delay(100);
}

