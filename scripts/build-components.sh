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

# Script to build all Westeros components
# This script builds simpleshell, simplebuffer, protocol files, linux-dmabuf, and the main project

set -e

echo "Starting Westeros component build..."

# Build simpleshell
echo "Building simpleshell..."
cd simpleshell/protocol
make SCANNER_TOOL=wayland-scanner
cd ..
autoreconf -fi
./configure --prefix=/usr/local
make -j$(nproc)
sudo make install
sudo ldconfig
# Ensure headers are accessible from main project
sudo mkdir -p /usr/local/include/simpleshell/protocol
sudo cp protocol/*.h /usr/local/include/simpleshell/protocol/ 2>/dev/null || true
cd ..

# Build simplebuffer
echo "Building simplebuffer..."
cd simplebuffer/protocol
make SCANNER_TOOL=wayland-scanner
cd ..
autoreconf -fi
./configure --prefix=/usr/local
make -j$(nproc)
sudo make install
sudo ldconfig
cd ..

# Generate protocol files
echo "Generating protocol files..."
cd linux-dmabuf/protocol && make SCANNER_TOOL=wayland-scanner && cd ../..
cd linux-expsync/protocol && make SCANNER_TOOL=wayland-scanner && cd ../..
cd protocol && make SCANNER_TOOL=wayland-scanner && cd ..

# Build and install linux-dmabuf library first
echo "Building linux-dmabuf library..."
autoreconf -fi
./configure \
  --prefix=/usr/local \
  --enable-ldbprotocol \
  --disable-embedded \
  --enable-xdgstable \
  CXXFLAGS="-I/usr/include/libdrm"
# Build only the linux-dmabuf library target
make -j$(nproc) libwesteros_linux_dmabuf_server.la
# Install the library and its headers
sudo mkdir -p /usr/local/lib /usr/local/include
sudo cp .libs/libwesteros_linux_dmabuf_server.so* /usr/local/lib/ 2>/dev/null || true
sudo cp libwesteros_linux_dmabuf_server.la /usr/local/lib/
sudo cp linux-dmabuf/westeros-linux-dmabuf.h /usr/local/include/
sudo cp linux-dmabuf/protocol/linux-dmabuf-unstable-v1-server-protocol.h /usr/local/include/
sudo ldconfig

# Configure and build main project
echo "Building main project..."
./configure \
  --prefix=/usr/local \
  --enable-app \
  --enable-test \
  --enable-player \
  --enable-rendergl \
  --enable-ldbprotocol \
  --enable-lexpsyncprotocol \
  --disable-embedded \
  --enable-xdgstable \
  CXXFLAGS="-I/usr/include/libdrm"
make -j$(nproc)
sudo make install
sudo ldconfig

echo "All components built successfully"
