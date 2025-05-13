#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // Include the I2C LCD library
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS     5000
#define RETRY_PERIOD_MS         5000

// Create a PulseOximeter object
PulseOximeter pox;

// Create an LCD object with the I2C address (e.g., 0x27) and dimensions (16x2)
LiquidCrystal_I2C lcd(0x27, 16, 2);

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
    lcd.setCursor(0, 1);  // Set cursor to the second line
    lcd.print("Beat Detected!   ");
}

void setup() {
    lcd.init();            // Initialize the LCD
    lcd.backlight();       // Turn on the backlight
    lcd.setCursor(0, 0);
    lcd.print("MAX30100 Pulse");
    lcd.setCursor(0, 1);
    lcd.print("Oximeter Init...");
    delay(2000);
    lcd.clear();

    // Initial attempt to initialize the sensor
    initializeSensor();
}

bool initializeSensor() {
    lcd.setCursor(0, 0);
    lcd.print("Initializing...");
    
    // Initialize sensor
    if (!pox.begin()) {
        lcd.setCursor(0, 1);
        lcd.print("FAILED          ");
        return false;
    } else {
        lcd.setCursor(0, 1);
        lcd.print("SUCCESS         ");
        delay(1000);
        lcd.clear();

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

            lcd.setCursor(0, 0);
            lcd.print("HR: ");
            lcd.print(pulse);
            lcd.print(" bpm  ");
            
            lcd.setCursor(0, 1);
            lcd.print("SpO2: ");
            lcd.print(oxgn);
            lcd.print("%     ");

            tsLastReport = millis();
        }
    } 
    // If not initialized, try again every RETRY_PERIOD_MS
    else if (millis() - tsLastInitAttempt > RETRY_PERIOD_MS) {
        lcd.setCursor(0, 0);
        lcd.print("Retrying Init.. ");
        sensorInitialized = initializeSensor();
        tsLastInitAttempt = millis();
        
        if (sensorInitialized) {
            lcd.setCursor(0, 1);
            lcd.print("Init Successful!");
            delay(2000);
            lcd.clear();
        }
    }
}
