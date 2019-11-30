#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEBeacon.h"
#include "esp_sleep.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <iostream>   
#include <string> 
#include <math.h>

#define SEALEVELPRESSURE_HPA (1013.25)
#define GPIO_DEEP_SLEEP_DURATION     2
Adafruit_BME680 bme;

BLEAdvertising *pAdvertising; 
std::string values ="ffffffffffffffffffffffffffffffff";

void setBeacon() {

  BLEBeacon Beacon = BLEBeacon();
  Beacon.setManufacturerId(0x4C00); //0x02E5 espres
  Beacon.setProximityUUID(BLEUUID(values));
  BLEAdvertisementData advert = BLEAdvertisementData();
  advert.setFlags(0x04); 
  
  std::string strServiceData = "";
  strServiceData += (char)26;
  strServiceData += (char)0xFF;
  strServiceData += Beacon.getData();
  advert.addData(strServiceData);

  pAdvertising->setAdvertisementData(advert);
}

void setup() {

  Serial.begin(115200);
 
  if (!bme.begin(0x76)) 
  {
    Serial.println("BME680 sensor not found!");
    while (1);
  }
 
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  if (! bme.performReading()) 
  {
    Serial.println("Failed to perform reading :(");
    return;
  }
  Serial.print("Temperature = ");
  Serial.print(bme.temperature);
  Serial.println(" *C");
 
  Serial.print("Pressure = ");
  Serial.print(bme.pressure / 100.0);
  Serial.println(" hPa");
 
  Serial.print("Humidity = ");
  Serial.print(bme.humidity);
  Serial.println(" %");
 
  Serial.print("Gas = ");
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(" KOhms");
 
  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  String temperature = String((int)((ceilf(bme.temperature * 100) / 100) * 100));
  String pressure = String((int)((ceilf(bme.pressure * 100) / 10000) * 100));
  String humidity = String((int)((ceilf(bme.humidity * 100) / 100) * 100));
  String gas_resistance = String((int)((ceilf(bme.gas_resistance * 100) / 100000) * 100));
  Serial.println(temperature);
  Serial.println(pressure);
  Serial.println(humidity);
  Serial.println(gas_resistance);

  String StringValues = humidity + 'a';
  StringValues += pressure+ 'a';
  //StringValues += '-';
  StringValues += temperature + 'a';
  //StringValues += bme.gas_resistance + 'a';

  StringValues.replace('-','.');
 
  Serial.println(StringValues);
 int m = 0;
  for (int i = values.length() -1 ; i >= 0 ; i--)
  {
     if (values.length() - 1 - i >= StringValues.length()) break;
    if(m % 2 == 0) values.at(i) = StringValues.charAt(m+1);
    else values.at(i) = StringValues.charAt(m-1);
    m++;    
  }

  values.insert(8, 1, '-');
  values.insert(13, 1, '-');
  values.insert(18, 1, '-');
  values.insert(23, 1, '-');
 Serial.println("omg"); 
  Serial.println(values.c_str()); 
 Serial.println("omg"); 

 Serial.println("wtf"); 
 
 
  BLEDevice::init("Outdoor");
  pAdvertising = BLEDevice::getAdvertising();
  BLEDevice::startAdvertising();
  setBeacon();
  pAdvertising->start();
  Serial.println("Advertizing started...");

  delay(100);
  pAdvertising->stop();

  Serial.printf("enter deep sleep\n");
  esp_deep_sleep(1000000LL * GPIO_DEEP_SLEEP_DURATION);
}

void loop() {}
