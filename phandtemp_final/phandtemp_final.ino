#include <OneWire.h>            // Library for the temperature sensor
#include <DallasTemperature.h>  // Library for the temperature sensor

// Temperature sensor setup
#define ONE_WIRE_BUS 2          // Pin for the temperature sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// pH sensor setup
#define pHpin 34                // Pin for the pH sensor

void setup() {
  // Initialize temperature sensor
  sensors.begin();

  // Initialize serial communication
  Serial.begin(9600);
}

void loop() {
  // Read temperature value
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);

  // Read pH value
  int sensorValue = analogRead(pHpin);
  float pH = map(sensorValue, 0, 4095, 0, 14); // Map the sensor reading to pH scale (ESP32 has a 12-bit ADC, range 0-4095)

  // Print values to the serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, pH: ");
  Serial.println(pH);

  delay(1000);  // Delay for 1 second
}

void programmer() {
  // Dummy programmer function, no action needed
}