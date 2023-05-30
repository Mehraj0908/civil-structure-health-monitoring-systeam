#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS D3 // Data wire is plugged into pin D1 on the NodeMCU8266 temprature
#define VIBRATION_SENSOR_PIN D2 // The vibration sensor is plugged into pin D2 on the NodeMCU8266
#define SOIL_MOISTURE_SENSOR_PIN A0 // The soil moisture sensor is plugged into the analog pin A0 on the NodeMCU8266

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature tempSensor(&oneWire);

// WiFi settings
const char* ssid = "realme 7";
const char* password = "12345678";

// ThingSpeak settings
unsigned long channelID = 2129382;
const char* writeAPIKey = "19W06OCX3GP0CYDU";

// Initialize WiFi client
WiFiClient client;

void setup() {
  Serial.begin(9600); // Initialize serial communication
  tempSensor.begin(); // Start the DS18B20 temperature sensor
  
  pinMode(VIBRATION_SENSOR_PIN, INPUT); // Configure the vibration sensor pin as an input
  pinMode(SOIL_MOISTURE_SENSOR_PIN, INPUT); // Configure the soil moisture sensor pin as an input
  
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  // Initialize ThingSpeak client
  ThingSpeak.begin(client);
}

void loop() {
  float temp = getTemperature(); // Get the temperature reading from the DS18B20 sensor
  int vibration = getVibrationReading(); // Get the vibration reading from the vibration sensor
  int soilMoisture = getSoilMoistureReading(); // Get the soil moisture reading from the soil moisture sensor
  //Serial.println(vibration);
 
  delay(100);
  // Check if the temperature is below a certain threshold (e.g., 5°C)
  if (temp < 50) {
    // If the temperature is below the threshold, check if the vibration and soil moisture readings are high
    if (vibration > 100 && soilMoisture > 500) {
      // If the vibration and soil moisture readings are high, send an alert
      Serial.println("Potential landslide detected!");
      // You can add additional code here to send an alert to your phone or email, for example
    }
  }
  
  // Send data to ThingSpeak
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, vibration);
  ThingSpeak.setField(3, soilMoisture);
  int response = ThingSpeak.writeFields(channelID, writeAPIKey);
  Serial.println(response);
}

// Function to get the temperature reading from the DS18B20 sensor
float getTemperature() {
  tempSensor.requestTemperatures(); // Send the command to get temperatures
  return tempSensor.getTempCByIndex(0); // Return the temperature in Celsius
}

// Function to get the vibration reading from the vibration sensor
int getVibrationReading() {
  return analogRead(VIBRATION_SENSOR_PIN); // Return the analog reading from the vibration sensor
}

// Function to get the soil moisture reading from the soil moisture sensor
int getSoilMoistureReading() {
  return analogRead(SOIL_MOISTURE_SENSOR_PIN);
}