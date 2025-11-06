# Test Execution Guide

## Available Scripts

### 1. `build.sh` - Build Script
Comprehensive build script for all L1 Westeros Components.

**Usage:**
```bash
# Build all components (Debug mode with coverage)
./build.sh -c -v

# Build in Release mode
./build.sh -r

# Build specific component
./build.sh -o simpleshell

# Get help
./build.sh -h
```

### 2. `run_tests.sh` - Test & Coverage Script
Quick script to build, run tests, and generate coverage report.

**Usage:**
```bash
chmod +x run_tests.sh
./run_tests.sh
```

This script will:
1. Clean build and compile with coverage enabled
2. Run all test executables
3. Generate coverage report in `coverage_html/index.html`

## Quick Start (Linux VM)

```bash
# Make scripts executable
chmod +x build.sh run_tests.sh

# Run tests with coverage
./run_tests.sh

# View coverage report
firefox coverage_html/index.html
```

## Expected Coverage

Target: **70%+** coverage for all src/ files

Components:
- **simpleshell/src**: 70%+
- **simplebuffer/src**: 70%+
- **linux-dmabuf/src**: 70%+
- **linux-explicit-sync/src**: 70%+

## Test Details

All tests cover:
- ✅ NULL parameter handling
- ✅ Boundary conditions (negative values, out-of-range)
- ✅ Both success and error paths
- ✅ All if/else branches
- ✅ Edge cases (FD=0, max values, etc.)

Total: **400+ test cases** across all components
