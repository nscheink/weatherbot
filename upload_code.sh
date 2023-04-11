#!/bin/sh

sudo mkdir -p /mnt/pico
# Hold down the BOOTSEL button and plug in the pico into the computer
sudo mount $(sudo fdisk -l | grep W95 | grep 128M | cut -f1 -d' ') /mnt/pico
sudo cp $1 /mnt/pico
sudo umount /mnt/pico
