#!/bin/bash
# Script to build all Westeros components
# This script builds the entire Westeros project with all components

set -e

echo "Starting Westeros component build..."

# Generate all protocol files first
echo "Generating protocol files..."
cd "$(dirname "$0")/.."

# Generate simpleshell protocols
cd simpleshell/protocol
make SCANNER_TOOL=wayland-scanner
cd ../..

# Generate simplebuffer protocols
cd simplebuffer/protocol
make SCANNER_TOOL=wayland-scanner
cd ../..

# Generate linux-dmabuf protocols
cd linux-dmabuf/protocol
make SCANNER_TOOL=wayland-scanner
cd ../..

# Generate linux-expsync protocols
cd linux-expsync/protocol
make SCANNER_TOOL=wayland-scanner
cd ../..

# Generate main project protocols
cd protocol
make SCANNER_TOOL=wayland-scanner
cd ..

# Now build the entire project from root
echo "Configuring and building Westeros..."
autoreconf -fi
./configure \
  --prefix=/usr/local \
  --enable-app \
  --enable-test \
  --enable-player \
  --enable-rendergl \
  --enable-sbprotocol \
  --enable-ldbprotocol \
  --enable-lexpsyncprotocol \
  --disable-embedded \
  --enable-xdgstable \
  CXXFLAGS="-I/usr/include/libdrm"

make -j$(nproc)
sudo make install
sudo ldconfig

echo "All components built successfully"

