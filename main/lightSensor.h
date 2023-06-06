#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <Wire.h>
#include <Adafruit_VEML7700.h>

static Adafruit_VEML7700 veml7700 = Adafruit_VEML7700();

void initLightSensor(TwoWire i2c_driver);
void readLightSensor(float *light_lx, double *light_intensity);

#endif
