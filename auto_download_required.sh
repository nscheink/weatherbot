#!/bin/sh

arduino-cli core --config-file arduino-cli.yaml update-index
arduino-cli core install --config-file arduino-cli.yaml rp2040-ethernet:rp2040
arduino-cli lib install Ethernet
arduino-cli lib install DFRobot_DHT20
arduino-cli lib install "SparkFun BMP384 Arduino Library"
arduino-cli lib install WeatherMeters
arduino-cli lib install "Adafruit VEML7700 Library"
arduino-cli lib install RPI_PICO_TimerInterrupt
