#ifndef TEMP_HUM_SENSOR_H
#define TEMP_HUM_SENSOR_H

#include <Wire.h>
#include <DFRobot_DHT20.h>

static DFRobot_DHT20 dht20;

void initTempHumSensor(TwoWire i2c_driver);
void readTempHumSensor(double *temp, double *humidity);
#endif
