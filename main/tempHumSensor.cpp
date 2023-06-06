#include "tempHumSensor.h"

void initTempHumSensor(TwoWire i2c_driver) {
    dht20 = DFRobot_DHT20(&i2c_driver, 0x38);
    while(dht20.begin()){
        Serial.println("Initializing DHT20 (Temp & Humidity) sensor failed!");
        delay(1000);
    }
}

void readTempHumSensor(double *temp, double *humidity) {
    *temp = dht20.getTemperature(); // Celsius
    *humidity = dht20.getHumidity()*100; // %RH
}

