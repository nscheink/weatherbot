#ifndef W_METER_H
#define W_METER_H

#include <RPi_Pico_TimerInterrupt.h>
#define WM_ADC_RESOLUTION 4096
#include <WeatherMeters.h>

/**
 * The windvane output pin is connected to analog pin A1
**/
const int windvane_pin = A1;

/**
 * The anemometer output pin is connected to GPIO pin 15
**/
const int anemometer_pin = 15;

/**
 * The raingauge output pin is connected to GPIO pin 14
**/
const int raingauge_pin = 14;

/**
 * The bit resolution to use for analog reads
 *
 * 12 is the maximum the Pi Pico supports
**/
const int ANALOG_READ_RESOLUTION = 12; // bits

/**
 * The sampling time to use for the weather station in seconds
 *
 * Rainfall and windspeed is integrated for these sampling times, and reset each
 * sample.
**/
const int SAMPLING_TIME = 4; // seconds

/**
 * The internal object communicating with the weather station sensor hardware
 *
 * The <0> disables the built-in moving average computations
**/
WeatherMeters <0> meters(windvane_pin, SAMPLING_TIME);
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
