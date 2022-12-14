#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include "I2Cdev.h"     
#include "MPU6050.h"    
#include <Servo.h>


#define REPORTING_PERIOD_MS     1000

PulseOximeter pox;

uint32_t tsLastReport = 0;

#include <DHT.h>
#define DHTPIN D7   
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

Servo myservo1, myservo2;
MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;

int axaxis;
int ayaxis;

char auth[] = "FKku0n26hGbMihD8EDbQ3xYL98YkcpGT";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Ullas";
char pass[] = "ullas@2000";

BlynkTimer timer;

int swi = D6;
int swival;

int buz=D0;

void setup()
{
   Serial.begin(9600);
   dht.begin();
   Wire.begin();
   Blynk.begin(auth, ssid, pass);
   mpu.initialize();
   myservo1.attach(D3);   //D3
   myservo2.attach(D5); //D5 
   pinMode(swi,INPUT_PULLUP);
   pinMode(buz,OUTPUT);
   timer.setInterval(500,acclerometer); 
   timer.setInterval(100,pul);
   timer.setInterval(3000,temp);
   timer.setInterval(1000,s);
   Serial.print("Initializing pulse oximeter..");
    if (!pox.begin())
    {
        Serial.println("FAILED");
        for(;;);
    } else 
    {
        Serial.println("SUCCESS");
    }

    pox.setOnBeatDetectedCallback(onBeatDetected);

}

void onBeatDetected()
{
    Serial.println("Beat!");
}

void loop()
{
  Blynk.run();
  timer.run();
}

void pul()
{
  pox.update();

    // Asynchronously dump heart rate and oxidation levels to the serial
    // For both, a value of 0 means "invalid"
    if (millis() - tsLastReport > REPORTING_PERIOD_MS)
    {
        Serial.print("Heart rate:");
        Serial.print(pox.getHeartRate());
        Serial.print("bpm / SpO2:");
        Serial.print(pox.getSpO2());
        Serial.println("%");
        Blynk.virtualWrite(V20,pox.getHeartRate());

        Blynk.virtualWrite(V21,pox.getSpO2());
        

        tsLastReport = millis();
    }
    
}

void acclerometer()
{
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz); 
  axaxis = map(ax, -17000, 17000, 0, 180);
  ayaxis = map(ay, -17000, 17000, 0, 180);
  Serial.println(" ax axis position=" + String(axaxis));
  Serial.println(" ay axis position=" + String(ayaxis));
  myservo1.write(axaxis);
  myservo2.write(ayaxis);  
}
void s()
{
  swival=digitalRead(swi);
  if(swival==0)
  {
   Blynk.notify("helppppp....!!!");
   digitalWrite(buz,1);
  }
  else
  {
    digitalWrite(buz,0);
  }
}

void temp()
{
  float t = dht.readTemperature();
   Blynk.virtualWrite(V22, t);
   Serial.println("temp " + String(t));
}
