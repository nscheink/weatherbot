#!/bin/sh

arduino-cli core update-index
arduino-cli core install rp2040-ethernet:rp2040
arduino-cli lib install Ethernet
arduino-cli lib install DFRobot_DHT20
arduino-cli lib install "SparkFun BMP384 Arduino Library"
