#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <Wire.h>
#include <Adafruit_VEML7700.h>

/**
 * The internal object communicating with the light sensor hardware
 */
static Adafruit_VEML7700 veml7700 = Adafruit_VEML7700();

/**
 * Initialize the light sensor
 * @param TwoWire i2c_driver - the i2c driver that handles the sensor connection
 */
void initLightSensor(TwoWire i2c_driver);

/**
 * Read the light sensor data
 * @param float* light_lx - where to store the read light_lx data
 * @param float* light_intensity - where to store the read light_intensity data
 */
void readLightSensor(float *light_lx, double *light_intensity);

#endif
