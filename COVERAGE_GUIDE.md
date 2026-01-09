# Coverage Report Guide - L1 Test Suite for Westeros

## Quick Start

### Generate Coverage for All Components
```bash
# Build with coverage enabled
./build.sh --coverage

# Generate comprehensive coverage report
cd build
cmake --build . --target coverage

# View report
# Report location: build/coverage_html/index.html
```

## Coverage Targets

### 1. Full Coverage Report (All Components)
Generates a comprehensive coverage report for all components combined.

```bash
cmake --build build --target coverage
```

**Output:**
- HTML Report: `build/coverage_html/index.html`
- Coverage Data: `build/coverage.info`
- Terminal summary showing coverage percentages

**Includes:**
- Westeros Main (westeros-compositor.cpp, westeros-render.cpp, westeros-nested.cpp)
- SimpleShell (westeros-simpleshell.cpp)
- SimpleBuffer (westeros-simplebuffer.cpp)
- Linux DMA-BUF (westeros-linux-dmabuf.cpp)
- Linux Explicit Sync (westeros-linux-expsync.cpp)

**Excludes:**
- System headers (/usr/*)
- Test files (*/tests/*, */test/*)
- Mock files (*/mocks/*, */mock/*)
- GTest/GMock files

### 2. Individual Component Coverage Reports

Generate coverage for specific components:

#### Westeros Main
```bash
cmake --build build --target coverage-westeros-main
# Report: build/coverage_html_westeros_main/index.html
```

#### SimpleShell
```bash
cmake --build build --target coverage-simpleshell
# Report: build/coverage_html_simpleshell/index.html
```

#### SimpleBuffer
```bash
cmake --build build --target coverage-simplebuffer
# Report: build/coverage_html_simplebuffer/index.html
```

#### Linux DMA-BUF
```bash
cmake --build build --target coverage-linux-dmabuf
# Report: build/coverage_html_dmabuf/index.html
```

#### Linux Explicit Sync
```bash
cmake --build build --target coverage-linux-expsync
# Report: build/coverage_html_expsync/index.html
```

### 3. View Report in Browser
```bash
cmake --build build --target coverage-report
# Automatically opens build/coverage_html/index.html in default browser
```

## Manual Coverage Workflow

If you need fine-grained control:

```bash
# 1. Build with coverage
./build.sh --coverage

# 2. Run specific tests
cd build
ctest -R WesterosMainL1Tests --output-on-failure

# 3. Capture coverage
lcov --directory . --capture --output-file my_coverage.info --rc lcov_branch_coverage=1

# 4. Filter unwanted files
lcov --remove my_coverage.info '/usr/*' '*/tests/*' '*/mocks/*' \
     --output-file my_coverage_filtered.info --rc lcov_branch_coverage=1

# 5. Generate HTML report
genhtml -o my_coverage_html my_coverage_filtered.info \
        --title "My Custom Coverage" --legend --branch-coverage

# 6. View report
xdg-open my_coverage_html/index.html  # Linux
open my_coverage_html/index.html      # macOS
start my_coverage_html/index.html     # Windows
```

## Running Specific Component Tests

Use CTest labels to run tests for specific components:

```bash
cd build

# Run all L1 tests
ctest -L L1 --output-on-failure

# Run Westeros Main tests only
ctest -L WesterosMain --output-on-failure

# Run SimpleShell tests only
ctest -L SimpleShell --output-on-failure

# Run SimpleBuffer tests only
ctest -L SimpleBuffer --output-on-failure

# Run Linux DMA-BUF tests only
ctest -L LinuxDMABuf --output-on-failure

# Run Linux Explicit Sync tests only
ctest -L LinuxExplicitSync --output-on-failure

# List all available tests
ctest -N
```

## Coverage Threshold Requirements

**Minimum Coverage: 75%**

Each component must maintain at least 75% line coverage:
- Line Coverage: ≥75%
- Function Coverage: ≥75%
- Branch Coverage: ≥60% (recommended)

## Interpreting Coverage Reports

### HTML Report Structure
```
coverage_html/index.html
├── Directory view (L1/)
├── File view (individual .cpp files)
└── Line-by-line coverage (click on files)
```

### Coverage Metrics
- **Line Coverage**: Percentage of code lines executed during tests
- **Function Coverage**: Percentage of functions called during tests
- **Branch Coverage**: Percentage of conditional branches executed

### Color Coding
- 🟢 **Green** (80-100%): Excellent coverage
- 🟡 **Yellow** (60-79%): Acceptable coverage
- 🔴 **Red** (<60%): Needs improvement

## Troubleshooting

### Issue: Coverage report only shows Westeros Main

**Solution:** Make sure all component tests are being built and run:

```bash
# Check that all components are enabled
cd build
cmake .. -DBUILD_WESTEROS_MAIN_TESTS=ON \
         -DBUILD_SIMPLESHELL_TESTS=ON \
         -DBUILD_SIMPLEBUFFER_TESTS=ON \
         -DBUILD_LINUX_DMABUF_TESTS=ON \
         -DBUILD_LINUX_EXPSYNC_TESTS=ON \
         -DENABLE_COVERAGE=ON

# Rebuild
cmake --build .

# Run all tests to generate coverage data
ctest --output-on-failure

# Generate coverage
cmake --build . --target coverage
```

### Issue: No .gcda files generated

**Cause:** Tests not executed before capturing coverage.

**Solution:**
```bash
# Run tests first
cd build
ctest --output-on-failure

# Then generate coverage
cmake --build . --target coverage
```

### Issue: lcov or genhtml not found

**Solution:** Install coverage tools:

```bash
# Ubuntu/Debian
sudo apt-get install lcov

# Fedora/RHEL
sudo dnf install lcov

# macOS
brew install lcov

# Verify installation
lcov --version
genhtml --version
```

### Issue: Coverage percentage is 0% for a component

**Possible causes:**
1. Component tests not enabled in build
2. Tests not executed
3. Coverage flags not applied to target

**Solution:**
```bash
# Check build configuration
cd build
cmake .. -L | grep BUILD_

# Verify coverage flags are set
grep -r "fprofile-arcs" .

# Run tests with verbose output
ctest -V -R <ComponentName>
```

## CI/CD Integration

### Example: Automated Coverage Check
```bash
#!/bin/bash
# coverage_check.sh

# Build with coverage
./build.sh --clean --coverage

# Generate coverage report
cd build
cmake --build . --target coverage

# Extract coverage percentage
COVERAGE=$(lcov --list coverage.info | grep "Total:" | awk '{print $2}' | sed 's/%//')

echo "Coverage: ${COVERAGE}%"

# Check threshold
if (( $(echo "$COVERAGE < 75" | bc -l) )); then
    echo "ERROR: Coverage ${COVERAGE}% is below 75% threshold"
    exit 1
else
    echo "SUCCESS: Coverage ${COVERAGE}% meets 75% threshold"
    exit 0
fi
```

## Best Practices

1. **Always build with --coverage flag**
   ```bash
   ./build.sh --coverage
   ```

2. **Run all tests before generating coverage**
   ```bash
   cd build
   ctest --output-on-failure
   cmake --build . --target coverage
   ```

3. **Review coverage reports regularly**
   - Check for untested code paths
   - Identify dead code
   - Ensure critical functions are tested

4. **Use component-specific coverage for focused development**
   ```bash
   cmake --build build --target coverage-westeros-main
   ```

5. **Exclude only necessary files**
   - System headers (always exclude)
   - Test files (always exclude)
   - Mock implementations (always exclude)
   - Never exclude production code

6. **Track coverage trends**
   - Save coverage reports with version tags
   - Compare coverage between releases
   - Set increasing coverage goals

## Coverage Report Locations

After running coverage targets:

| Target | Report Location |
|--------|----------------|
| `coverage` | `build/coverage_html/index.html` |
| `coverage-westeros-main` | `build/coverage_html_westeros_main/index.html` |
| `coverage-simpleshell` | `build/coverage_html_simpleshell/index.html` |
| `coverage-simplebuffer` | `build/coverage_html_simplebuffer/index.html` |
| `coverage-linux-dmabuf` | `build/coverage_html_dmabuf/index.html` |
| `coverage-linux-expsync` | `build/coverage_html_expsync/index.html` |

## Additional Commands

### Clean coverage data
```bash
cd build
find . -name "*.gcda" -delete
find . -name "*.gcno" -delete
rm -f coverage*.info
rm -rf coverage_html*
```

### Merge multiple coverage files
```bash
lcov -a coverage1.info -a coverage2.info -o merged_coverage.info
```

### Generate coverage for specific source files only
```bash
lcov --extract coverage.info '*/westeros-compositor.cpp' -o compositor_only.info
genhtml -o compositor_coverage compositor_only.info
```

## Support

For issues or questions about coverage:
1. Check this guide first
2. Verify build configuration (`cmake .. -L`)
3. Check test execution (`ctest -N`)
4. Review CMakeLists.txt coverage configuration
5. Consult project maintainers

---

**Last Updated:** November 20, 2025
**Minimum Coverage Requirement:** 75%
**Tools Required:** lcov, genhtml, gcov
