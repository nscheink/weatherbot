# Creating a Developer Environment

## Requirements

    - Raspberry Pi Model 3B (will likely work with newer models, but hasn't been
      tested).
    - 8GB (or larger) Micro SD card
    - WiFi with Internet

## Initial Setup

### Install Raspberry Pi OS

Download the Raspberry Pi Imager from
[here](https://www.raspberrypi.com/software/). Plugin in your SD card, and then
start the imager software.

Select "Choose OS" -> "Raspberry Pi OS (other)" -> "Raspberry Pi OS Lite (64
bit)".

Select "Choose Storage" -> Your SD card. 

Select the Gear button on the bottom right corner to bring up the settings. 

Enable the checkbox for "Set hostname" and change the hostname to
"weatherbot.local".

Enable the checkbox for "Enable SSH", make sure "Use password authentication" is
selected.

Enable the checkbox for "Set username and password". Keep the username as
"user", and enter a password. 

Enable the checkbox for "Configure wireless LAN" and enter the SSID and password
for your WiFi network. Optionally, set your Locale settings.

Select "Save", then select "Write". 

It will take a couple minutes to write the OS to the SD card. Once it's
complete, a window will pop up saying "Write Successful" and that the SD card
can be removed. Select continue, and eject the SD card.

### Configure the OS

Insert the SD card and power on the Raspberry Pi. Wait for the Raspberry Pi to
boot, then find its IP address through your router's interface. 

Open a terminal, enter 
```bash
ssh {USERNAME}@{PI_IP_ADDRESS}
```
and insert the password you configured earlier when prompted. It should pull up
a message from Debian, and launch a bash shell. 

#### Update the System

In the bash session, enter
```bash
sudo apt update && sudo apt upgrade -y
```

#### Install helper programs

Now is the best time to install software.  Later on, the raspberry pi will lose
access to the internet, which will make it much harder to install software. This
command will install what is needed for the
[pico-sdk](https://github.com/raspberrypi/pico-sdk) and development.

```bash
sudo apt install -y git screen build-essential cmake gcc-arm-none-eabi \
libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib
```

_(Optional)_ Install whatever software you anticipate needing to use.
Personally, I install
```bash
sudo apt install -y tmux vim htop
```

_(Optional)_ [Use public key
authentication](https://www.raspberrypi.com/documentation/computers/remote-access.html#passwordless-ssh-access).

### Install arduino-cli

First, we'll make an install directory
```bash
mkdir -p ~/.local/bin
```

Add the install directory to your path
```bash
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

Then, we'll install the arduino-cli
```bash
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=~/.local/bin sh
```

Ensure it's working by checking the version
```bash
arduino-cli version
```

### Copy the code over

*_TODO:_* Create instructions on downloading the code

If downloaded on your host machine, copy it over using the following command
```bash
scp -r {DOWNLOAD_FOLDER} {USERNAME}@{PI_IP_ADDRESS}:~/
```

### Download the required libraries

In the code directory on the Raspberry Pi, run
```bash
sh auto_download_required.sh
```
This will download all the required libraries used in development to your
Raspberry Pi.

We can test that things function properly by compiling the code
```bash
./compile.sh {CODE_DIRECTORY}
```
This should return no errors. This can be confirmed by 
```bash
echo $?
```
returning 0, not 1 or other number.

## Networking Setup

This step will setup the Raspberry Pi as its own access point router, allowing
communication between the pico and a client device through the Raspberry Pi.
This will remove internet access from the Raspberry Pi, so do everything that
you need the internet for before continuing. 

Once you are sure you want to continue, enter the following:
```bash
sudo -Es
./{CODE_DIRECTORY}/access-point-setup.sh
``` 
The Raspberry Pi will begin setting up the access point, lose connection, finish
setting things up, then shut down. Once fully off, turn the raspberry pi back
on. 

Once on, it should create a wifi access point with the SSID "Weatherbot{RANDOM 6
DIGIT CODE}". Connect to the wifi network.

You should now be able to reconnect to the pi using
```bash
ssh user@192.168.1.1
```

If you connect, the development environment was successful.
