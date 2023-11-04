#include <RH_ASK.h>
#include <SPI.h>

int fsrPin = 0;  // Analog pin connected to the FSR
int fsrReading;   // Raw analog reading from the FSR
int LEDbrightness;

RH_ASK rf_driver;  // RadioHead ASK driver

int fsrVoltage;  // Analog reading converted to voltage
unsigned long fsrResistance;  // Resistance calculated from voltage
unsigned long fsrConductance;  // Conductance calculated from resistance
long fsrForce;  // Force calculated from conductance
float pressureKgPerCm2;  // Pressure in kg/cm^2

const float minPressureKgPerCm2 = 0.0;  // Minimum pressure (kg/cm^2)
const float maxPressureKgPerCm2 = 20.0;  // Maximum pressure (kg/cm^2)

void setup(void) {
  Serial.begin(9600);  // Initialize Serial monitor for debugging
  rf_driver.init();  // Initialize RadioHead ASK driver
}

void loop(void) {
  fsrReading = analogRead(fsrPin);  
  Serial.print("Analog reading = ");
  Serial.println(fsrReading);

  // Convert analog reading to voltage (0V to 5V)
  fsrVoltage = map(fsrReading, 0, 1023, 0, 5000);
  Serial.print("Voltage reading in mV = ");
  Serial.println(fsrVoltage);

  if (fsrVoltage == 0) {
    Serial.println("No pressure");  
  } else {
    // Calculate resistance using the voltage divider formula
    fsrResistance = 5000 - fsrVoltage;  // Voltage is in millivolts, so 5V = 5000mV
    fsrResistance *= 10000;  // 10K resistor
    fsrResistance /= fsrVoltage;
    Serial.print("FSR resistance in ohms = ");
    Serial.println(fsrResistance);

    // Calculate conductance in microMhos
    fsrConductance = 1000000;  // We measure in microMhos
    fsrConductance /= fsrResistance;
    Serial.print("Conductance in microMhos: ");
    Serial.println(fsrConductance);

    // Approximate force in Newtons using FSR guide graphs
    if (fsrConductance <= 1000) {
      fsrForce = fsrConductance / 80;
      Serial.print("Force in Newtons: ");
      Serial.println(fsrForce);
    } else {
      fsrForce = fsrConductance - 1000;
      fsrForce /= 30;
      Serial.print("Force in Newtons: ");
      Serial.println(fsrForce);
    }

    // Calculate pressure in kg/cm^2 from the force
    pressureKgPerCm2 = 0.101972 * fsrForce;

    // Check if the pressure is within the desired range
    if (pressureKgPerCm2 >= minPressureKgPerCm2 && pressureKgPerCm2 <= maxPressureKgPerCm2) {
      Serial.print("Pressure: ");
      Serial.println(pressureKgPerCm2);

      // Transmit the pressure as binary data
      byte pressureData[sizeof(float)];
      memcpy(pressureData, &pressureKgPerCm2, sizeof(float));
      rf_driver.send(pressureData, sizeof(float));
    } else {
      Serial.println("Pressure out of range");
      
      // Still transmit the pressure as binary data even if out of range
      byte pressureData[sizeof(float)];
      memcpy(pressureData, &pressureKgPerCm2, sizeof(float));
      rf_driver.send(pressureData, sizeof(float));
    }
  }
  Serial.println("--------------------");
  delay(1000);  // Delay between readings
}
