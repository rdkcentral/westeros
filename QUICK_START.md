# Quick Start Guide - L1 Test Suite for Westeros

Get up and running with the unified L1 test suite in 5 minutes.

## Prerequisites Check

```bash
# Check if required tools are installed
cmake --version          # Should be >= 3.14
g++ --version            # Any C++11 compiler
ls /usr/lib/*gtest*      # GoogleTest libraries
lcov --version           # Coverage tool (optional)
```

## Installation (If Needed)

```bash
sudo apt-get update
sudo apt-get install -y cmake build-essential libgtest-dev libgmock-dev lcov
```

## Directory Structure

Your workspace should look like:
```
Westeros/
├── westeros-main/              # Original source (required)
│   └── westeros-main/
│       ├── westeros-compositor.cpp
│       ├── westeros-nested.cpp
│       └── westeros-render.cpp
└── L1-TestSuiteWesteros/       # This test suite
    ├── build.sh
    ├── L1/
    └── CMakeLists.txt
```

## Build & Run (3 Steps)

### Step 1: Build Tests

```bash
cd L1-TestSuiteWesteros
./build.sh --coverage --clean
```

### Step 2: Run Tests

```bash
cd build
ctest --output-on-failure
```

### Step 3: View Coverage

```bash
cmake --build . --target coverage
xdg-open coverage_html/index.html  # Linux
# or
open coverage_html/index.html      # macOS
# or
start coverage_html/index.html     # Windows
```

## Expected Output

### Successful Build

```
==========================================
L1 Test Suite for Westeros - Unified Build
==========================================

Source Paths:
  Westeros Main: ../westeros-main/westeros-main

========================================== Configuration Complete
==========================================
Components to test:
  ✓ Westeros Main
==========================================

[100%] Built target test_westeros_main_l1
[100%] Built target test_westeros_nested_l1

==========================================
Build Successful!
==========================================
```

### Test Execution

```
Test project /path/to/L1-TestSuiteWesteros/build
    Start 1: WesterosMainL1Tests
1/2 Test #1: WesterosMainL1Tests ............   Passed    2.13 sec
    Start 2: WesterosNestedL1Tests
2/2 Test #2: WesterosNestedL1Tests ..........   Passed    3.47 sec

100% tests passed, 0 tests failed out of 2

Total Test time (real) =   5.60 sec
```

### Coverage Report

```
Overall coverage rate:
  lines......: 60.6% (456 of 753 lines)
  functions..: 73.7% (87 of 118 functions)

File 'westeros-compositor.cpp':
  lines......: 87.9% (124 of 141 lines)

File 'westeros-nested.cpp':
  lines......: 41.7% (198 of 475 lines)

File 'westeros-render.cpp':
  lines......: 97.8% (134 of 137 lines)
```

## Custom Source Paths

If westeros-main is in a different location:

```bash
./build.sh --coverage --westeros-main /custom/path/to/westeros-main/westeros-main
```

## Windows Quick Start

```powershell
cd L1-TestSuiteWesteros
.\build.ps1 -Coverage -Clean
cd build
ctest --output-on-failure
cmake --build . --target coverage
start coverage_html\index.html
```

## Running Individual Tests

```bash
# Run specific test executable
./build/test_westeros_main_l1

# Run with filter
./build/test_westeros_main_l1 --gtest_filter="*CreateContext*"

# List all tests
./build/test_westeros_main_l1 --gtest_list_tests
```

## Troubleshooting

### Issue: Source not found

```
ERROR: Westeros Main source not found at: ../westeros-main/westeros-main
```

**Fix:** Specify the correct path:
```bash
./build.sh --westeros-main /correct/path/to/westeros-main/westeros-main
```

### Issue: GoogleTest not found

```
ERROR: GoogleTest not found. Tests cannot be built.
```

**Fix:**
```bash
sudo apt-get install libgtest-dev libgmock-dev
```

### Issue: Coverage at 0%

**Cause:** Not using `--coverage` flag

**Fix:**
```bash
./build.sh --coverage --clean
```

## Next Steps

1. ✅ Tests passing? Great! Coverage report shows what's tested.
2. 📊 Review coverage: Focus on files below 75% coverage
3. 🧪 Add tests: See `L1/westeros-main/tests/L1/` for examples
4. 🔄 Iterate: Modify → Build → Test → Coverage

## Quick Commands Reference

```bash
# Clean build with coverage
./build.sh -c -C

# Build without coverage
./build.sh

# Run tests
cd build && ctest

# Generate coverage
cmake --build build --target coverage

# View specific test
./build/test_westeros_nested_l1 --gtest_filter="*CreateSurface*"

# Verbose build
./build.sh -c -v

# Release build
./build.sh -r

# Custom threads
./build.sh -j 8
```

## Success Criteria

✅ **Build completes without errors**
✅ **All tests pass (136+ tests)**
✅ **Coverage report generated**
✅ **Overall coverage: 60.6%+ (target: >75%)**

---

**Time to Complete:** ~5 minutes
**Difficulty:** Beginner
**Next:** See [README.md](README.md) for detailed documentation
