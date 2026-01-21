#!/bin/bash
# Script to install all dependencies required for building Westeros
# This script handles dependency installation with retry logic for apt-get update

set -e

echo "Installing Westeros build dependencies..."

# Retry apt-get update up to 3 times to handle mirror sync issues
for i in 1 2 3; do
  sudo apt-get update && break || {
    echo "apt-get update failed, attempt $i/3"
    [ $i -lt 3 ] && sleep 10 || exit 1
  }
done

# Install all required packages
sudo apt-get install -y \
  build-essential \
  autoconf \
  automake \
  libtool \
  pkg-config \
  libwayland-dev \
  wayland-protocols \
  libglib2.0-dev \
  libxkbcommon-dev \
  libegl1-mesa-dev \
  libgles2-mesa-dev \
  libgl1-mesa-dev \
  freeglut3-dev \
  libglew-dev \
  libdrm-dev \
  libgstreamer1.0-dev \
  libgstreamer-plugins-base1.0-dev

echo "Dependencies installed successfully"
