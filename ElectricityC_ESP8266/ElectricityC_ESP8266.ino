#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "ESP8266WiFi.h"
#include "ThingSpeak.h"

//const char* ssid = "sadguru Hostel _EXT";  // Replace with your network SSID
//const char* password = "sadguru@1"; 
//const char* ssid = "VIT_CAMPUSs";  // Replace with your network SSID
//const char* password = "vedh12345"; 
const char* ssid = "Redmi Note 10S";  // Replace with your network SSID
const char* password = "kisikonahimilega";  // Replace with your network password
const char* api_key = "K75F2IJ7KTVT8YD1";  // Replace with your ThingSpeak API key
const unsigned long channel_id = 2111814;  // Replace with your ThingSpeak channel ID

WiFiClient client;
unsigned long lastTime = 0;
unsigned long timerDelay = 300;


//D6 = Rx & D5 = Tx
SoftwareSerial espSerial(D6, D5);


void setup() {
  // Initialize Serial port
  Serial.begin(9600);
  espSerial.begin(9600);
  WiFi.mode(WIFI_STA);
  while (!Serial) continue;

  ThingSpeak.begin(client);
  Serial.println("Program started");
  delay(1000);
}

void loop() {
  
  if (espSerial.available()) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, espSerial);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.f_str());
      return;
    }
    float voltage = doc["volt"];
    float current = doc["current"];
    float energy = doc["Energy"];
    float motion = doc["motion"];
    float cost = doc["cost"];
    Serial.print("Voltage: ");
    Serial.println(voltage);
    Serial.print("Current: ");
    Serial.println(current);
    Serial.print("Energy: ");
    Serial.println(energy);
    Serial.print("Motion: ");
    Serial.println(motion);
    Serial.print("Cost: ");
    Serial.println(cost);
    sendDataToThingSpeak(voltage, current, energy, motion, cost);
    Serial.println("-----------------------------------------");
  }
  
  
}
void sendDataToThingSpeak(float voltage, float current, float energy, int motion, float cost) {

  if ((millis() - lastTime) > timerDelay) {
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password); 
        delay(5000);     
      } 
      Serial.println("\nConnected.");
    }
  // Set the fields and values to send to ThingSpeak
  ThingSpeak.setField(1, voltage);
  ThingSpeak.setField(2, current);
  ThingSpeak.setField(3, energy);
  ThingSpeak.setField(4, motion);
  ThingSpeak.setField(5, cost);

  // Send the data to ThingSpeak
  int response = ThingSpeak.writeFields(channel_id, api_key);

  // Check if the data was successfully sent
  if (response == 200) {
    Serial.println("Data sent to ThingSpeak");
  } else {
    Serial.print("Error sending data to ThingSpeak. Response code: ");
    Serial.println(response);
  }
  }
}
