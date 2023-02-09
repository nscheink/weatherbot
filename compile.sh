#!/bin/sh
mkdir -p build
arduino-cli compile \
    --verbose \
    --fqbn rp2040-ethernet:rp2040:wiznet_5500_evb_pico \
    $1 \
    --build-path build/
