#!/bin/bash

#
# If not stated otherwise in this file or this component's LICENSE file the
# following copyright and licenses apply:
#
# Copyright 2016 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

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
