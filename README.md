# Weatherbot Software

This software was developed as part of a senior project for Cal Poly, sponsored
by [FarmBot](https://farm.bot). It is meant to be run on a Raspberry Pi Pico
that manages a weather station.

It interfaces with all of the integrated sensors, collects data from them and
organizes them into a JSON object. The Pico will also connect to the network the
Ethernet cable, request an IP address, and then serve an HTTP server that will
respond to GET requests with the JSON object containing the data. 

## Hardware

This software was built to integrate with multiple off the shelf sensors
connected together. The sensors used include:

 - Light Sensor - [VEML7700](https://www.adafruit.com/product/4162)
 - Pressure Sensor - [BMP384](https://www.sparkfun.com/products/19662)
 - Temperature/Humidity Sensor -
   [DHT20](https://www.sparkfun.com/products/18364)
 - Rainfall/Wind speed/Wind direction - [Sparkfun
   Kit](https://www.sparkfun.com/products/15901)

This integrates with a Raspberry Pi Pico with an integrated Wiznet Ethernet
driver and port which can be found [here](https://mou.sr/3iMsWy1) as of June
2023.

## Setting up a developer environment on your machine

Code development for this project is done using the [Arduino
CLI](https://github.com/arduino/arduino-cli) which can be installed to your
local computer by following the instructions found
[here](https://arduino.github.io/arduino-cli/latest/installation/).

### Installing necessary libraries and developer tools

The software uses a number of open source libraries in order to function. These
all can be installed on Mac or Linux by running the included shell script
`auto_download_required.sh` in your terminal:

```bash
cd weatherbot/
./auto_download_required.sh
```

### Testing your setup

#### Compiling Code

At this point, code should be able to be compiled. The easiest way to compile
code is to use the included `compile.sh` helper script:

```bash
cd weatherbot/
./compile.sh main/
```

It should have a lot of logs, and have no error message at the end of
compilation. It should also create a new directory called `build/` that contains
a lot of build artifacts but also the built binary, `main.ino.uf2`.

#### Uploading Code

To upload code, you have to start with the Raspberry Pi Pico unplugged from your
development machine. You will soon connect the Micro USB for data on the Pico to
a USB port on your development machine. Hold down the BOOTSEL button, and with
the button held, plug the Raspberry Pi Pico into your USB port. You can now
release the BOOTSEL button. This boots the Pico into a state that allows it to
be programmed.

Now, use the included helper script `upload.sh` to upload the compiled uf2 file
to your Pico.

```bash
cd weatherbot
./upload.sh build/main.ino.uf2
```

This should take a small amount of time, and produce no errors. Once you regain
control of your terminal, the process should be complete, and you should unplug
and replug the Pico. This boots the Pico into a state where it should now be
running the compiled code.

## Setting up a developer environment on a Raspberry Pi 3

The full WeatherBot for the senior project included capabilities for Power over
Ethernet, and had a dedicated Raspberry Pi 3 for networked communication. This
Raspberry Pi 3 was meant to simulate a possible setup with a Raspberry Pi on the
FarmBot. However, to allow for networked communication between the Pico and the
Pi 3 and allow a developer environment, a complicated network setup was
necessary for the Pi 3 that should NOT be necessary to work on a FarmBot. 

Currently, a Raspberry Pi 3 developer environment for the Weatherbot involves
turning the Pi 3 into a Wifi access point with ethernet communication. It cannot
access the internet in this configuration, so it's necessary to download and
install all necessary packages before setting it up. It's a complicated process,
but there are bash scripts that automate most of it, and there is a separate
README for this that can be found at `pi3b-setup-guide.md`. The relevant scripts
that automate the installation process on the Pi 3 can be found at `setup.sh`
and `access-point-setup.sh`.
