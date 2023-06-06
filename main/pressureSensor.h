#ifndef PRESSURE_SENSOR_H
#define PRESSURE_SENSOR_H

#include <SparkFunBMP384.h>
#include <Wire.h>

static BMP384 pressureSensor;
static uint8_t pressureSensorAddr = BMP384_I2C_ADDRESS_DEFAULT; // 0x77

void initPressureSensor(TwoWire i2c_driver);
void readPressureSensor(double *pressure, double *bmp_temp); 

#endif
