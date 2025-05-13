#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS     5000
#define RETRY_PERIOD_MS         5000

// Create a PulseOximeter object
PulseOximeter pox;

// Time tracking variables
uint32_t tsLastReport = 0;
uint32_t tsLastInitAttempt = 0;

// Variables to store pulse and oxygen readings
float pulse = 0;  
float oxgn = 0;  

// Sensor initialization status
bool sensorInitialized = false;

// Callback routine is executed when a pulse is detected
void onBeatDetected() {
    Serial.println("Beat!");
}

void setup() {
    Serial.begin(9600);
    Serial.println("MAX30100 Pulse Oximeter");
    Serial.println("Attempting first initialization...");
    
    // Initial attempt to initialize the sensor
    initializeSensor();
}

bool initializeSensor() {
    Serial.print("Initializing pulse oximeter... ");
    
    // Initialize sensor
    if (!pox.begin()) {
        Serial.println("FAILED");
        return false;
    } else {
        Serial.println("SUCCESS");
        pox.setIRLedCurrent(MAX30100_LED_CURR_46_8MA);
        
        // Register a callback routine
        pox.setOnBeatDetectedCallback(onBeatDetected);
        return true;
    }
}

void loop() {
    // Check if sensor is initialized
    if (sensorInitialized) {
        // Read from the sensor
        pox.update();

        // Grab the updated heart rate and SpO2 levels
        if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
            pulse = pox.getHeartRate();
            oxgn = pox.getSpO2();  

            Serial.print("Heart rate: ");  
            Serial.print(pulse);
            Serial.print(" bpm / SpO2: ");  
            Serial.print(oxgn);
            Serial.println("%");

            tsLastReport = millis();
        }
    } 
    // If not initialized, try again every RETRY_PERIOD_MS
    else if (millis() - tsLastInitAttempt > RETRY_PERIOD_MS) {
        Serial.println("Retrying sensor initialization...");
        sensorInitialized = initializeSensor();
        tsLastInitAttempt = millis();
        
        if (sensorInitialized) {
            Serial.println("Sensor initialized successfully after retry!");
        }
    }
}