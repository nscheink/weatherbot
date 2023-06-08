#ifndef PRESSURE_SENSOR_H
#define PRESSURE_SENSOR_H

#include <SparkFunBMP384.h>
#include <Wire.h>

/**
 * The internal object communicating with the pressure sensor hardware
 */
static BMP384 pressureSensor;

/**
 * The i2c address of the pressure sensor
 */
static uint8_t pressureSensorAddr = BMP384_I2C_ADDRESS_DEFAULT; // 0x77

/**
 * Initialize the pressure sensor
 * @param TwoWire i2c_driver - the i2c driver that handles the sensor connection
 */
void initPressureSensor(TwoWire i2c_driver);

/**
 * Read the pressure sensor data
 * @param double* pressure - where to store the read pressure data
 * @param double* bmp_temp - where to store the read temperature data
 */
void readPressureSensor(double *pressure, double *bmp_temp); 

#endif
