
#include "SoftwareSerial.h"
#include "MHZ19.h"   // https://github.com/WifWaf/MH-Z19
#include "PMS.h"     //https://github.com/fu-hsi/pms
#include "dht.h"     // https://github.com/RobTillaart/DHTlib
#include "DS3231.h"  // http://www.rinkydinkelectronics.com/library.php?id=73
#include <SPI.h>
#include <SD.h>      

#define dht22 5 // DHT22 temperature and humidity sensor

dht DHT; // Creats a DHT object
DS3231  rtc(SDA, SCL); // Initiate the DS3231 Real Time Clock module using the I2C interface
Time  t; // Init a Time-data structure
MHZ19 myMHZ19;    // CO2 Sensor
SoftwareSerial co2Serial(2, 3);  // (RX, TX) MH-Z19 serial
SoftwareSerial pmsSerial(8, 9); // Particulate Matter sensor
PMS pms(pmsSerial);
PMS::DATA data;

const int chipSelect = 10;
unsigned long dataTimer = 0;
unsigned long dataTimer3 = 0;
unsigned long dataTimer4 = 0;
int readDHT;
int temp = 0;
int tvoc = 1;
int hum = 0;
int CO2 = 0;
int pm25 = 0;
int pm10 = 0;
int pm1 = 0;
int hours = 0;
int minutes = 0;
int seconds = 0;
int sensorValueA = analogRead(A0);// read the VOC input on analog pin 0:
int sensorValueB = analogRead(A1);// read the VOC input on analog pin 1:

void setup() {

  Serial.begin(9600);
  pmsSerial.begin(9600);
  co2Serial.begin(9600);
  
//SD card check
/*
  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");
  */

  //print headers to file
  if (SD.begin(chipSelect)) {
    loggDatatoSDCard(0);
  }

  // Initialize all sensors
  rtc.begin();
  myMHZ19.begin(co2Serial);
  myMHZ19.autoCalibration();  // Turn auto calibration ON (OFF autoCalibration(false))
}


void loop() {
  // Read temperature and humidity from DHT22 sensor
  readDHT = DHT.read22(dht22); // Reads the data from the sensor
  temp = DHT.temperature; // Gets the values of the temperature
  hum = DHT.humidity; // Gets the values of the humidity

  //get voc data (1, 2, 3 or 4)
  if (sensorValueA <500){
   if (sensorValueB<500){
     tvoc = 1;
   }
   else{
     tvoc = 2;
   }
 } 
 else{
   if (sensorValueB <500){
       tvoc =3;
   }
   else{
       tvoc = 4;
   }
 }

  // Read MHZ19 - CO2 sensor for 3 seconds - if we don't use a blocking method with the while loop we won't get values from the sensor.
  co2Serial.listen();
  dataTimer = millis();
  while (millis() - dataTimer <= 3000) {
    CO2 = myMHZ19.getCO2(); // Request CO2 (as ppm)
  }

  // Read Particulate Matter sensor for 2 seconds
  pmsSerial.listen();
  dataTimer3 = millis();
  while (millis() - dataTimer3 <= 1000) {
    pms.readUntil(data);
    pm25 = data.PM_AE_UG_2_5;
    pm10 = data.PM_AE_UG_10_0;
  }

  // Get the time from the DS3231 Real Time Clock module - For setting the time use the library example
  t = rtc.getTime();
  hours = t.hour;
  minutes = t.min;
  seconds = t.sec;
  

  // Send the data to the Nextion display
  sendToDisplay();
  if (SD.begin(chipSelect)) {
    loggDatatoSDCard(1);
  }

}
void sendToDisplay(){
  dataTimer4 = millis();
  while (millis() - dataTimer4 <= 200) {
    Serial.print("co2V.val=");
    Serial.print(CO2);
    // each command ends with these three unique write commands in order the data to be send to the Nextion display
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);

    Serial.print("pm25V.val=");
    Serial.print(pm25);
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);

    Serial.print("pm10V.val=");
    Serial.print(pm10);
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);

    Serial.print("tempV.val=");
    Serial.print(temp);
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);

    Serial.print("humV.val=");
    Serial.print(hum);
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);

    Serial.print("tvocV.val=");
    Serial.print(tvoc);
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);
  }
}

void loggDatatoSDCard(int cmd){
  String dataString = "";
  if (cmd == 1){
    dataString += hours;
    dataString += ":" ;
    dataString += minutes;
    dataString += ":" ;
    dataString += seconds ;
    dataString += "," ;
    dataString += CO2 ;
    dataString += "," ;
    dataString += tvoc ;
    dataString += "," ;
    dataString += pm25 ;
    dataString += "," ;
    dataString += pm10 ;
    dataString += "," ;
    dataString += temp; 
    dataString += ","; 
    dataString += hum;


  }
  if (cmd == 0){
  //  Serial.println("printing header");
    dataString = "time, CO2, tvoc, pm2.5, pm10, temp, hum";
  }
    
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    //Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    //Serial.println("error opening datalog.txt");
  }

}