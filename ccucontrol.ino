#include "DHT.h"
#define DHTPIN 2 
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);
void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(4, OUTPUT);
  pinMode(3, OUTPUT);// sets the digital pin 13 as output
}

void loop() {
  float humid = dht.readHumidity();
  digitalWrite(3, HIGH);
  Serial.println(F("Humidity: "));
  Serial.print(humid);
  delay(10000);
         
}
