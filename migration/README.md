# L1 Test Suite for Westeros

Unified L1 testing framework for Westeros compositor and components. This project consolidates all L1 tests while referencing source code from the original westeros-main repository architecture.

## 🎯 Project Overview

This unified test suite provides:
- **L1 Tests** for all Westeros components
- **Mock Infrastructure** for isolated unit testing
- **Code Coverage** support with lcov/genhtml
- **Unified Build System** referencing external source repositories
- **No Source Duplication** - tests reference original westeros-main sources

## 📁 Project Structure

```
L1-TestSuiteWesteros/
├── CMakeLists.txt              # Root build configuration
├── build.sh / build.ps1        # Build scripts (Linux/Windows)
├── common/                     # Shared test utilities
│   ├── include/               # Common test headers
│   └── CMakeLists.txt
├── L1/                         # Component test suites
│   ├── westeros-main/         # Main compositor tests
│   │   ├── tests/
│   │   │   ├── L1/            # L1 test cases
│   │   │   └── mocks/         # Mock implementations
│   │   └── CMakeLists.txt
│   ├── simpleshell/           # SimpleShell tests (optional)
│   ├── simplebuffer/          # SimpleBuffer tests (optional)
│   ├── linux-dmabuf/          # DMA-BUF tests (optional)
│   └── linux-expsync/         # Explicit Sync tests (optional)
└── scripts/                    # Utility scripts
```

## 🔧 Prerequisites

### Required
- CMake >= 3.14
- C++11 compiler (GCC/Clang)
- GoogleTest/GoogleMock
- pthread

### Optional (for coverage)
- lcov
- genhtml

### Installation (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install -y \
    cmake \
    build-essential \
    libgtest-dev \
    libgmock-dev \
    lcov \
    git
```

## 🚀 Quick Start

### 1. Clone/Setup Repositories

Ensure you have the source repositories:
```
Westeros/
├── westeros-main/              # Original westeros-main repository
│   └── westeros-main/          # Source files here
├── simpleshell/                # Optional: simpleshell source
├── linux-dmabuf/               # Optional: linux-dmabuf source
├── linux-expsync/              # Optional: linux-expsync source
└── L1-TestSuiteWesteros/       # This test suite
```

### 2. Build with Default Settings

**Linux:**
```bash
cd L1-TestSuiteWesteros
chmod +x build.sh
./build.sh
```

**Windows:**
```powershell
cd L1-TestSuiteWesteros
.\build.ps1
```

### 3. Run Tests

```bash
cd build
ctest --output-on-failure
```

## 📊 Code Coverage

### Generate Coverage Report

**Linux:**
```bash
./build.sh --coverage --clean
cd build
cmake --build . --target coverage
```

**Windows:**
```powershell
.\build.ps1 -Coverage -Clean
cd build
cmake --build . --target coverage
```

### View Coverage Report

Open `build/coverage_html/index.html` in your browser.

**Current Coverage Targets:**
- **Overall:** >75%
- **westeros-compositor.cpp:** >85%
- **westeros-nested.cpp:** >65%
- **westeros-render.cpp:** >95%

## 🔨 Build Options

### Linux Build Script

```bash
./build.sh [OPTIONS]

Options:
  -h, --help              Show help message
  -c, --coverage          Enable code coverage
  -r, --release           Build in Release mode
  -C, --clean             Clean build directory
  -v, --verbose           Verbose output
  -j, --jobs N            Parallel jobs (default: nproc)
  --westeros-main PATH    Path to westeros-main source
  --simpleshell PATH      Path to simpleshell source
  --linux-dmabuf PATH     Path to linux-dmabuf source
  --linux-expsync PATH    Path to linux-expsync source
```

### Windows Build Script

```powershell
.\build.ps1 [OPTIONS]

Options:
  -Help                   Show help message
  -Coverage               Enable code coverage
  -Release                Build in Release mode
  -Clean                  Clean build directory
  -Verbose                Verbose output
  -Jobs N                 Parallel jobs (default: CPU count)
  -WesterosMainSrc PATH   Path to westeros-main source
  -SimpleShellSrc PATH    Path to simpleshell source
  -LinuxDmabufSrc PATH    Path to linux-dmabuf source
  -LinuxExpsyncSrc PATH   Path to linux-expsync source
```

### CMake Options

```cmake
-DCMAKE_BUILD_TYPE=<Debug|Release>
-DENABLE_COVERAGE=<ON|OFF>
-DBUILD_WESTEROS_MAIN_TESTS=<ON|OFF>
-DBUILD_SIMPLESHELL_TESTS=<ON|OFF>
-DBUILD_SIMPLEBUFFER_TESTS=<ON|OFF>
-DBUILD_LINUX_DMABUF_TESTS=<ON|OFF>
-DBUILD_LINUX_EXPSYNC_TESTS=<ON|OFF>
-DWESTEROS_MAIN_SRC_ROOT=<path>
-DSIMPLESHELL_SRC_ROOT=<path>
-DLINUX_DMABUF_SRC_ROOT=<path>
-DLINUX_EXPSYNC_SRC_ROOT=<path>
```

## 🧪 Test Components

### Westeros Main (136 tests)
- **Compositor Tests (67):** Core compositor functionality
- **Nested Connection Tests (69):** Nested compositor operations
- **Coverage:** 60.6% overall, targeting >75%

### Component Tests (Optional)
- **SimpleShell:** Wayland simple shell protocol
- **SimpleBuffer:** Buffer management
- **Linux DMA-BUF:** DMA-BUF integration
- **Linux Explicit Sync:** Synchronization primitives

## 📝 Development Workflow

### Adding New Tests

1. Create test file in `L1/<component>/tests/L1/`
2. Add mock implementations in `L1/<component>/tests/mocks/`
3. Update `L1/<component>/CMakeLists.txt`
4. Build and run tests

### Modifying Mock Behavior

Mock files are in `L1/<component>/tests/mocks/`:
- `wayland-server-mock.cpp` - Wayland server functions
- `wayland-client-mock.cpp` - Wayland client functions
- `gstreamer-mock.cpp` - GStreamer functions
- `vpc-client-protocol.h` - VPC protocol

### Running Specific Tests

```bash
# Run specific test suite
./build/test_westeros_main_l1
./build/test_westeros_nested_l1

# Run with GTest filters
./build/test_westeros_main_l1 --gtest_filter="*CreateContext*"
```

## 🐛 Troubleshooting

### Source Path Not Found

```bash
ERROR: Westeros Main source not found at: ../westeros-main/westeros-main
```

**Solution:** Specify correct path:
```bash
./build.sh --westeros-main /path/to/westeros-main/westeros-main
```

### GoogleTest Not Found

```bash
ERROR: GoogleTest not found. Tests cannot be built.
```

**Solution:**
```bash
sudo apt-get install libgtest-dev libgmock-dev
```

### Coverage Tools Missing

```bash
WARNING: Coverage tools not found. Install lcov/genhtml.
```

**Solution:**
```bash
sudo apt-get install lcov
```

## 📈 Coverage History

| Date       | Overall | Compositor | Nested | Render |
|------------|---------|------------|--------|--------|
| 2025-01-17 | 60.6%   | 87.9%      | 41.7%  | 97.8%  |
| Target     | >75%    | >85%       | >65%   | >95%   |

## 🤝 Contributing

1. Maintain test isolation (no dependencies between tests)
2. Keep coverage above target thresholds
3. Document complex mock behaviors
4. Follow existing naming conventions

## 📄 License

Copyright 2024-2025 RDK Management

## 🔗 Related Projects

- **westeros-main:** https://github.com/rdkcentral/westeros
- **L1-Westeros-Main:** Original test suite (archived)
- **L1-Westeros-Components:** Component tests (archived)

## 📞 Support

For issues or questions:
1. Check existing documentation
2. Review test output and coverage reports
3. Consult westeros-main repository documentation
