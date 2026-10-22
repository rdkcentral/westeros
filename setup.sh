#!/bin/bash
# Setup script for L1 Test Suite for Westeros
# Copies test files and mocks from existing L1-Westeros-Main project

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_msg() {
    local color=$1
    shift
    echo -e "${color}$@${NC}"
}

print_msg $BLUE "=========================================="
print_msg $BLUE "L1 Test Suite Setup"
print_msg $BLUE "=========================================="
echo ""

# Source paths
L1_MAIN_ROOT="../L1-Westeros-Main"
L1_COMPONENTS_ROOT="../L1-Westeros-Components"

# Check if source exists
if [ ! -d "$L1_MAIN_ROOT" ]; then
    print_msg $RED "ERROR: L1-Westeros-Main not found at: $L1_MAIN_ROOT"
    print_msg $YELLOW "Please ensure L1-Westeros-Main is in the parent directory"
    exit 1
fi

print_msg $GREEN "Copying Westeros Main test files..."

# Copy L1 test files
print_msg $YELLOW "  Copying L1 test cases..."
cp -v "$L1_MAIN_ROOT/components/westeros-main/tests/L1/"*.cpp \
    components/westeros-main/tests/L1/

# Copy mock files
print_msg $YELLOW "  Copying mock implementations..."
cp -v "$L1_MAIN_ROOT/components/westeros-main/tests/mocks/"*.cpp \
    components/westeros-main/tests/mocks/
cp -v "$L1_MAIN_ROOT/components/westeros-main/tests/mocks/"*.h \
    components/westeros-main/tests/mocks/

# Copy common headers if they exist
if [ -d "$L1_MAIN_ROOT/common/include" ]; then
    print_msg $YELLOW "  Copying common headers..."
    mkdir -p common/include
    cp -r "$L1_MAIN_ROOT/common/include/"* common/include/ 2>/dev/null || true
fi

print_msg $GREEN ""
print_msg $GREEN "=========================================="
print_msg $GREEN "Setup Complete!"
print_msg $GREEN "=========================================="
echo ""
print_msg $YELLOW "Next steps:"
print_msg $NC "  1. Verify westeros-main source path"
print_msg $NC "  2. Run: ./build.sh --coverage"
print_msg $NC "  3. Run tests: cd build && ctest"
echo ""
