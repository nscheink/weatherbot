#include <Wire.h>

#include "pressureSensor.h"
#include "tempHumSensor.h"
#include "lightSensor.h"
#include "weatherMeter.h"
#include "server.h"

/**
 * i2c0 connected on GPIO pins 0 and 1
 * The windvane pin is connected to ADC1
 * The anemometer pin is connected to GP15
 * The raingauge pin is connected to GP14
**/

// Device is connected to i2c0 on GPIO pins 0 and 1
TwoWire i2c_driver = TwoWire(i2c0, 0, 1);

int debug = 0;

/**
 * Raspberry Pi Pico's entry point
**/
void setup() {

    // waitForSerial();

    // Initialize the Ethernet connection, which also attempts to grab an IP
    // address from a DHCP server
    initEthernet();

    initTempHumSensor(i2c_driver);
    initLightSensor(i2c_driver);
    initPressureSensor(i2c_driver);
    initWeatherKit();
}

// Variables storing the data retrieved from the sensors

double temp = 0;            // Celsius
double humidity = 0;        // % RH
double light_intensity = 0; // % Intensity
float light_lx = 0;         // lx
double pressure = 0;        // Pascal
double bmp_temp = 0;        // Celsius
float wind_dir = 0;         // Degrees
float wind_speed = 0;       // km/h
float rainfall = 0;         // mm

/**
 * Halts code until a USB serial device is connected. Useful for debugging.
**/
void waitForSerial() {
    Serial.begin(9600);
    while (!Serial) {
        delay(100);
        ; // wait for serial port to connect. Needed for native USB port only
    }
}

/**
 * Displays sensor data on the serial monitor. Useful for debugging.
**/
void serialMonitor() {
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %RH");
    
    Serial.print("Light Intensity: ");
    Serial.print(light_intensity);
    Serial.println(" \% of max");

    Serial.print("Air Pressure: ");
    Serial.print(pressure);
    Serial.println("Pa");

    Serial.print("BMP384 Temperature: ");
    Serial.print(bmp_temp);
    Serial.println(" C");

    Serial.println(" ---------- ");
    delay(200);
}

/**
 * Helper function to convert celsius to fahrenheit
 * @param double celsius - the input temperature in celsius
 * @return double fahrenheit - the output temperature in fahrenheit
**/
double convertCelsiusToFahrenheit(double celsius) {
    return (celsius * (9./5.)) + 32.;
}

/**
 * Variable used for timing when to collect data from the sensors. Data
 * collection needs to be delayed (for instance, every 4 seconds instead of
 * every 1 milliseconds) to prevent sensors from heating up.
**/
int counter=0;

/**
 * Pico function that continuously runs after the `setup` function is completed.
**/
void loop() {

    // Increment counter to keep track of the amount of milliseconds
    // Increment by 2 because delay of 1 in this loop, and delay of 1 in
    // ethernet. Timing could be improved
    counter += 2;

    // Only collect data every 4 seconds (4000ms)
    if (counter >= 4000){

        // Collect data
        readTempHumSensor(&temp, &humidity);
        readLightSensor(&light_lx, &light_intensity);
        readPressureSensor(&pressure, &bmp_temp);
        readWeatherMeterKit(&wind_speed, &wind_dir, &rainfall);

        // Update the JSON data
        updateJsonDoc(
            &temp,
            &humidity,
            &light_lx,
            &light_intensity,
            &pressure,
            &rainfall,
            &wind_dir,
            &wind_speed
        );
        
        // Reset the counter
        counter = 0;
    }

    // Maintain the HTTP server
    maintainEthernet();
    httpServer();

    delay(1);
}
