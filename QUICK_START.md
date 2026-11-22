# Quick Start: Westeros L1 Tests

This repository contains the Westeros source and its L1 tests. The build script can be launched from any directory; CTest and report commands below use paths relative to the repository root (the directory containing this file and `CMakeLists.txt`). Test executables and generated files are written to `build/`.

## Prerequisites (Ubuntu/Debian)

From the repository root, install the tools and development packages used by the L1 build:

```bash
bash scripts/setup.sh
```

The setup helper installs the packages with `apt-get` on Debian/Ubuntu. `lcov` is only needed to generate coverage reports. For other distributions, install the equivalent CMake, compiler, Wayland/EGL development, GoogleTest/GoogleMock, and LCOV packages manually.

## Build and run all configured L1 tests

The build script resolves the repository root from its own location, so it can be invoked from any directory:

```bash
bash /path/to/your/westeros-checkout/scripts/build.sh --clean
```

Then, from the repository root, run CTest:

```bash
ctest --test-dir build --output-on-failure
```

The build script uses the source files in this repository and configures all L1 components that are enabled in the top-level CMake configuration.

To run only the Westeros tests (the current CTest label is `WesterosMain`):

```bash
ctest --test-dir build -L WesterosMain --output-on-failure
```

List the tests discovered by CTest with:

```bash
ctest --test-dir build -N
```

## Generate coverage

From any directory, build with coverage enabled. The script builds the tests and runs the coverage target, which runs the configured tests and creates the report:

```bash
bash /path/to/your/westeros-checkout/scripts/build.sh --clean --coverage
```

The combined HTML report is at `build/coverage_html/index.html`. To generate just the Westeros coverage report, use the existing CMake target `coverage-westeros-main`:

```bash
cmake --build build --target coverage-westeros-main
```

That target writes the report to `build/coverage_html_westeros_main/index.html`. Open it with a browser on Linux, for example:

```bash
xdg-open build/coverage_html_westeros_main/index.html
```

Other component targets are `coverage-simpleshell`, `coverage-simplebuffer`, `coverage-linux-dmabuf`, and `coverage-linux-expsync`.

## Run a specific GoogleTest case

Run a test executable directly, or list its test cases first:

```bash
./build/L1/westeros-main/test_westeros_main_l1 --gtest_list_tests
./build/L1/westeros-main/test_westeros_main_l1 --gtest_filter='TestSuiteName.TestName'
```

Executables are generated under their component build directories; use `find build -type f -executable` to locate them if your generator places them elsewhere.

## Windows

From PowerShell at the repository root, use the supplied build script:

```powershell
.\build.ps1 -Clean
ctest --test-dir build --output-on-failure
```

The PowerShell script also accepts `-Coverage` and `-Release`. Coverage reporting requires compatible `lcov` and `genhtml` tools, which are generally easiest to use in the Linux build environment.

## CI scope

The current GitHub Actions L1 workflow configures and runs the Westeros L1 tests and uploads the coverage artifact as `artifacts-L1-westeros`. Local builds can configure the additional component suites as well.

For coverage commands and report details, see [COVERAGE_GUIDE.md](COVERAGE_GUIDE.md).
