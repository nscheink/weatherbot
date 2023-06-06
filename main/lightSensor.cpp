#include "lightSensor.h"

void initLightSensor(TwoWire i2c_driver) {
    while(!veml7700.begin(&i2c_driver)) {
        Serial.println("Initializing VEML7700 (Light) sensor failed!");
        delay(1000);
    }
}

void readLightSensor(float *light_lx, double *light_intensity) {
    *light_lx = veml7700.readLux(VEML_LUX_AUTO);
    *light_intensity = (*light_lx / 140000.) * 100.;
}
