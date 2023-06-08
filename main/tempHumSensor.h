#ifndef TEMP_HUM_SENSOR_H
#define TEMP_HUM_SENSOR_H

#include <Wire.h>
#include <DFRobot_DHT20.h>

/**
 * The internal object communicating with the temperature/humidity sensor 
 * hardware
 */
static DFRobot_DHT20 dht20;

/**
 * Initialize the temperature/humidity sensor
 * @param TwoWire i2c_driver - the i2c driver that handles the sensor connection
 */
void initTempHumSensor(TwoWire i2c_driver);

/**
 * Read the temperature/humidity sensor data
 * @param double* temp - where to store the read temperature data
 * @param double* humidity - where to store the read humidity data
 */
void readTempHumSensor(double *temp, double *humidity);
#endif
