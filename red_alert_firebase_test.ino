#include <OneWire.h>            // Library for the temperature sensor
#include <DallasTemperature.h>  // Library for the temperature sensor
#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include "time.h"

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "PLDTHOMEFIBRr93Pb"
#define WIFI_PASSWORD "PLDTWIFIRzy9a"

// Insert Firebase project API Key
#define API_KEY "AIzaSyAcfqwMQSPENZOECuIoZi1U-HlQCGGaHK4"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "mariejodene@gmail.com"
#define USER_PASSWORD "jodenecute"

// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "https://test-b113d-default-rtdb.asia-southeast1.firebasedatabase.app/"

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

// Database main path (to be updated in setup with the user UID)
String databasePath;

// Database child nodes
String tempPath = "/temperature";
String phPath = "/ph";
String timePath = "/timestamp";

// Parent Node (to be updated in every loop)
String parentPath;

int timestamp;
FirebaseJson json;

const char* ntpServer = "pool.ntp.org";

// Timer variables (send new readings every three minutes)
unsigned long sendDataPrevMillis = 0;
// unsigned long timerDelay = 180000;
unsigned long timerDelay = 60000;

// Temperature sensor setup
#define ONE_WIRE_BUS 2          // Pin for the temperature sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// pH sensor setup
#define pHpin 34                // Pin for the pH sensor

// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

void setup() {
  // Initialize temperature sensor
  sensors.begin();

  // Initialize serial communication
  Serial.begin(9600);

   // Initialize BME280 sensor
  initWiFi();
  configTime(0, 0, ntpServer);

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }

  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/UsersData/" + uid + "/readings";
}

void loop() {
  // Read temperature value
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);

  // Read pH value
  int sensorValue = analogRead(pHpin);
  float pH = map(sensorValue, 0, 4095, 0, 14); // Map the sensor reading to pH scale (ESP32 has a 12-bit ADC, range 0-4095)

  // Print values to the serial monitor
  // Serial.print("Temperature: ");
  // Serial.print(temperature);
  // Serial.print(" °C, pH: ");
  // Serial.println(pH);

  // Send new readings to database
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    //Get current timestamp
    timestamp = getTime();
    Serial.print ("time: ");
    Serial.println (timestamp);

    parentPath= databasePath + "/" + String(timestamp);

    json.set(tempPath.c_str(), String(temperature));
    json.set(phPath.c_str(), String(pH));
    json.set(timePath, String(timestamp));
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }

  delay(1000);  // Delay for 1 second
}

void programmer() {
  // Dummy programmer function, no action needed
}