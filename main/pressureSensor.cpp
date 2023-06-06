#include "pressureSensor.h"
#include <Arduino.h>

void initPressureSensor(TwoWire i2c_driver) {
    while(pressureSensor.beginI2C(pressureSensorAddr, i2c_driver) != BMP3_OK) {
        // Inform and wait for connection
        Serial.println("Initializing BMP384 (Pressure) sensor failed!");
        delay(1000);
    }

    int8_t err = BMP3_OK;

    bmp3_odr_filter_settings osrMultipliers = 
    {
        .press_os = BMP3_OVERSAMPLING_32X,
        .temp_os = BMP3_OVERSAMPLING_2X,
        0,0
    };
    err = pressureSensor.setOSRMultipliers(osrMultipliers);
    if(err)
    {
        Serial.print("Error! while setting Pressure OSR, error code: ");
        Serial.println(err);
    }

    uint8_t odr=0;
    err = pressureSensor.getODRFrequency(&odr);
    if(err)
    {
        Serial.print("Error! while getting Pressure ODR! Error code: ");
        Serial.println(err);
    }

    Serial.print("ODR Frequency: ");
    Serial.print(200 / pow(2, odr));
    Serial.println("Hz");

}

void readPressureSensor(double *pressure, double *bmp_temp) {
    bmp3_data pressure_data;
    int8_t err = pressureSensor.getSensorData(&pressure_data);
    if (err == BMP3_OK) {
        *pressure = pressure_data.pressure;         
        *bmp_temp = pressure_data.temperature;
    } 
    else {
        Serial.print("Error! while retrieving pressure data, error code: ");
        Serial.println(err);
    }
}
