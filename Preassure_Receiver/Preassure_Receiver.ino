#include <RH_ASK.h>
#include <SPI.h>

RH_ASK rf_driver;  // RadioHead ASK driver

const float minPressureKgPerCm2 = 0.0;  // Minimum pressure (kg/cm^2)
const float maxPressureKgPerCm2 = 20.0;  // Maximum pressure (kg/cm^2)

void setup(void) {
  Serial.begin(9600);  // Initialize Serial monitor for debugging
  rf_driver.init();  // Initialize RadioHead ASK driver
}

void loop(void) {
  byte pressureData[sizeof(float)];  // Buffer for received pressure data
  uint8_t buflen = sizeof(pressureData);  // Length of the buffer

  if (rf_driver.recv(pressureData, &buflen)) {
    if (buflen == sizeof(float)) {
      float receivedPressure;  // Variable to store received pressure
      memcpy(&receivedPressure, pressureData, sizeof(float));  // Copy received data to the variable

      if (receivedPressure >= minPressureKgPerCm2 && receivedPressure <= maxPressureKgPerCm2) {
        Serial.print("Pressure Received (kg/cm^2): ");
        Serial.println(receivedPressure);
      } else {
        Serial.println("Pressure out of range");
      }
    } else {
      Serial.println("Received data size doesn't match the expected size");
    }
  }
}
