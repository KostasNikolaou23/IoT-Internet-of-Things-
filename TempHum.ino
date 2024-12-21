
#define THINGER_SERIAL_DEBUG
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "DHT.h"
#include <WiFi.h>
#define DHTPIN 2     
#define DHTTYPE DHT11   // DHT 11
LiquidCrystal_I2C lcd(0x27,20,4);



DHT dht(DHTPIN, DHTTYPE);

#include <ThingerESP32.h>
#include "arduino_secrets.h"

ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

const long interval = 300000;
unsigned long previousMillis = 0;
float temperatureSum = 0;
int measureCount = 0;

void setup() {

   dht.begin();
   lcd.init();
   lcd.backlight();
  // open serial for debugging
  Serial.begin(115200);

  pinMode(4, OUTPUT);
  thing["GPIO_4"] << digitalPin(4);//light gpkn
  thing.add_wifi(SSID);

  
  thing["TempHum"] >> [](pson &out){ 
    out["temperature"] = dht.readTemperature();
    out["humidity"] = dht.readHumidity();
  };

  thing["average_temperature"] >> [](pson& out){

    if(measureCount > 0){
      float averageTemp = temperatureSum / measureCount;
      out = averageTemp;
      Serial.print("Μέση Θερμοκρασία: ");
      Serial.println(averageTemp);

      temperatureSum = 0;
      measureCount = 0;
    } else{

      out = 0;
    }
  };
}

void loop() {

  thing.handle();

  unsigned long currentMillis = millis();
   float Temperature = dht.readTemperature();

  if(!isnan(Temperature)){

    temperatureSum += Temperature;
    measureCount++; 
  }



  float Humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temperature = dht.readTemperature();

   lcd.init();
   lcd.backlight();

   lcd.setCursor(0,0);
   lcd.print("Temp:");
   lcd.print(Temperature);
   delay(2000);
   


   lcd.setCursor(0,1);
   lcd.print("Hum:");
   lcd.print(Humidity);
   delay(2000);
   

  Serial.print("Humidity: ");
  Serial.print(Humidity);
  Serial.println("%");
  delay(2000);

  Serial.print("Temperature: ");
  Serial.print(Temperature);
  Serial.println("°C");
  delay(2000);

  lcd.clear();
  lcd.print(WiFi.localIP());
  delay(2000);
  thing.handle();
  // write to bucket BucketId the TempHum resource
  thing.write_bucket("BucketId", "TempHum");
  delay(2000);

  if (currentMillis - previousMillis >= interval) {

    previousMillis = currentMillis;
    thing.stream("average_temperature");
  }

  
}
