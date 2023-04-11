#!/bin/bash
CODE_DIR=$(dirname -- "$( readlink -f -- "$0"; )")

# Update the system
sudo apt update && sudo apt upgrade -y

# Install essential packages
sudo apt install -y git screen build-essential cmake gcc-arm-none-eabi \
    libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib

# Install recommended packages
sudo apt install -y tmux vim htop

# Install arduino-cli
mkdir -p ~/.local/bin
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
export PATH="$HOME/.local/bin:$PATH"
source ~/.bashrc
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=~/.local/bin sh

# Install required libraries
bash ${CODE_DIR}/auto_download_required.sh

# Setup the access point
sudo ${CODE_DIR}/access-point-setup.sh
