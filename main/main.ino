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

void setup() {
//    Serial.begin(9600);
//    while (!Serial) {
//        delay(100);
//        ; // wait for serial port to connect. Needed for native USB port only
//    }

    initEthernet();
    initTempHumSensor(i2c_driver);
    initLightSensor(i2c_driver);
    initPressureSensor(i2c_driver);
    initWeatherKit();
}

double temp = 0; // Celsius
double humidity = 0; // % RH
double light_intensity = 0; // % Intensity
float light_lx = 0; // lx
double pressure = 0; // Pascal
double bmp_temp = 0; // Celsius
float wind_dir = 0; // Degrees
float wind_speed = 0; // km/h
float rainfall = 0; // mm

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

double convertCelsiusToFahrenheit(double celsius) {
    return (celsius * (9./5.)) + 32.;
}

int counter=0;

void loop() {
    counter += 1;
    if (counter >= 4000){
        readTempHumSensor(&temp, &humidity);
        readLightSensor(&light_lx, &light_intensity);
        readPressureSensor(&pressure, &bmp_temp);
        readWeatherMeterKit(&wind_speed, &wind_dir, &rainfall);
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

        counter = 0;
    }

    //serialMonitor();
    maintainEthernet();
    httpServer();
    delay(1);
}
