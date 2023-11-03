double sensorValue1 = 0;
double sensorValue2 = 0;
int crosscount = 0;
int climb_flag = 0;
int val[100];
int max_v = 0;
double VmaxD = 0;
double VeffD = 0;
double Veff = 0;
int sensor = 2; 
const int sensorIn = A0;      // pin where the OUT pin from ACS712 sensor is connected on Arduino
int mVperAmp = 66;           // this the 5A version of the ACS712 - use 100 for 20A Module and 66 for 30A Module
double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;
double Power = 0;
double Energy = 0;

unsigned long previousMillis = 0;
const long interval = 1000; // measure power every second

#include <SoftwareSerial.h>
#include <ArduinoJson.h>
SoftwareSerial nodemcu(5, 6);
void setup() {
  pinMode(sensor, INPUT); 
  Serial.begin(9600);
  nodemcu.begin(9600);
  delay(1000);
  
  Serial.println("Program started");
}

void loop() {

  // Read voltage using ZMPT101B sensor
  for ( int i = 0; i < 100; i++ ) {
    sensorValue1 = analogRead(A1);
    if (analogRead(A1) > 511) {
      val[i] = sensorValue1;
    }
    else {
      val[i] = 0;
    }
    delay(1);
  }

  max_v = 0;

  for ( int i = 0; i < 100; i++ )
  {
    if ( val[i] > max_v )
    {
      max_v = val[i];
    }
    val[i] = 0;
  }
  if (max_v != 0) {
    VmaxD = max_v;
    VeffD = VmaxD / sqrt(2);
    Veff = ((((VeffD - 420.76) / -90.24) * -210.2) + 210.2)-83.30;
  }
  else {
    Veff = 0;
  }
//  Serial.print("Voltage: ");
//  Serial.println(Veff);

  // Read current using ACS712 sensor
  Voltage = getVPP();
  VRMS = (Voltage/2.0) *0.707;   //root 2 is 0.707
  AmpsRMS = (VRMS * 1000)/mVperAmp;
//  Serial.print("Current: ");
//  Serial.print(AmpsRMS);
//  Serial.println(" A");

  // Calculate power
  Power = AmpsRMS * Veff;
//  Serial.print("Power: ");
//  Serial.print(Power);
//  Serial.println(" W");

  // Calculate energy consumption
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    double EnergyIncrement = Power * ((currentMillis - previousMillis) / 1000.0 / 3600.0);
    Energy += EnergyIncrement;
    previousMillis = currentMillis;
  }
//  Serial.print("Energy: ");
//  Serial.print(Energy);
//  Serial.println(" Wh");
//  Serial.println(" ");
  int val = digitalRead(sensor);
  float cost = Energy*4.35;
  int motion = val;
  send_to_esp(Veff,AmpsRMS,Power,Energy,motion, cost);
  
  delay(100);
}

void send_to_esp(float voltage, float current, float power, float energy, int motion, float cost)
{
  StaticJsonDocument<200> doc;
  doc["volt"] = voltage;
  doc["current"] = current;
  doc["Energy"] = energy;
  doc["motion"] = motion;
  doc["cost"] = cost;

  String json;
  serializeJson(doc, json);
  
  Serial.println(json);

  nodemcu.println(json);
  delay(1000);
  }

float getVPP()
{
  float result;
  int readValue;               
  int maxValue = 0;            
  int minValue = 1024;         
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) 
   {
       readValue = analogRead(sensorIn);

       if (readValue > maxValue) 
       {
           
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
          
           minValue = readValue;
       }
   }
   
  
   result = ((maxValue - minValue) * 5.0)/1024.0;
      
   return result;
 }
