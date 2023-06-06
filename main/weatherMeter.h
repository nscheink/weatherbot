#ifndef W_METER_H
#define W_METER_H

#include <RPi_Pico_TimerInterrupt.h>
#define WM_ADC_RESOLUTION 4096
#include <WeatherMeters.h>

const int windvane_pin = A1;
const int anemometer_pin = 15;
const int raingauge_pin = 14;
const int ANALOG_READ_RESOLUTION = 12;
WeatherMeters <0> meters(windvane_pin, 4);
volatile bool meters_read_data = false;

// Create timer for weather kit
RPI_PICO_Timer ITimer(0);
const float TIMER_FREQ_HZ = 1.0;

void interruptRainGauge() {
    meters.intRaingauge();
}

void interruptAnemometer() {
    meters.intAnemometer();
}


bool weatherMeterTimerHandler(struct repeating_timer *t) {
    (void) t;
    meters.timer();
    return true;
}

void readDone(void) {
    meters_read_data = true;
}

void initWeatherKit() {
    analogReadResolution(ANALOG_READ_RESOLUTION);
    ITimer.attachInterrupt(TIMER_FREQ_HZ, weatherMeterTimerHandler);
    pinMode(anemometer_pin, INPUT_PULLUP);
    pinMode(raingauge_pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(anemometer_pin), interruptAnemometer, FALLING);
    attachInterrupt(digitalPinToInterrupt(raingauge_pin), interruptRainGauge, FALLING);
    meters.attach(readDone);
    meters.reset();
}

void readWeatherMeterKit(float *wind_speed, float *wind_dir, float *rainfall) {
    if (meters_read_data) {
        meters_read_data = false;
        *wind_speed = meters.getSpeed();
        *wind_dir = meters.getDir();
        *rainfall = meters.getRain();
    }
}

#endif
