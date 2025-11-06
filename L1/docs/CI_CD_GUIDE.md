# CI/CD Pipeline Guide

## Overview

This document explains the Continuous Integration setup for L1 Westeros Components.

## GitHub Actions Workflow

The CI pipeline runs automatically on:
- Push to `main`, `master`, or `develop` branches
- Pull requests to these branches
- Manual trigger via `workflow_dispatch`

### Jobs

1. **Build and Test All Components**
   - Builds each component (essos, simpleshell, simplebuffer, linux-dmabuf)
   - Runs L1 unit tests with GoogleTest
   - Generates code coverage reports
   - Uploads artifacts (test results, coverage, build logs)

2. **Static Code Analysis**
   - Runs `cppcheck` for static analysis
   - Checks for common C/C++ issues
   - Uses suppressions file `.cppcheck-suppressions`

3. **Code Quality Checks**
   - Scans for TODO/FIXME comments
   - Checks file permissions
   - Validates line endings
   - Ensures documentation completeness

4. **Build Summary**
   - Aggregates results from all jobs
   - Displays final status

## Local Testing with ACT

You can test the GitHub Actions workflow locally using [act](https://github.com/nektos/act):

### Installation

```bash
# Ubuntu/Debian
curl https://raw.githubusercontent.com/nektos/act/master/install.sh | sudo bash

# macOS
brew install act

# Windows (via scoop)
scoop install act
```

### Running ACT

```bash
# Run all workflows
act

# Run specific job
act -j build-and-test

# Run with specific component
act -j build-and-test --matrix component:simpleshell

# Dry run to see what would happen
act -n

# Use specific Docker image
act --container-architecture linux/amd64
```

### ACT Configuration

Create `.actrc` file in project root:

```
--container-architecture linux/amd64
-P ubuntu-latest=catthehacker/ubuntu:act-latest
--artifact-server-path /tmp/artifacts
```

## Local CI Simulation

For a faster local check without Docker, use the CI simulation script:

```bash
# Run full CI simulation
./ci-local.sh

# This will:
# 1. Clean build all components
# 2. Run all L1 tests
# 3. Generate coverage reports
# 4. Run cppcheck static analysis
# 5. Perform code quality checks
```

## Build Script

The main build script supports flexible component building:

```bash
# Build all components with coverage
./build.sh -v

# Clean build in release mode
./build.sh -c -r

# Build only specific component
./build.sh -o simpleshell

# Build multiple specific components
./build.sh -o simpleshell -o simplebuffer

# Build without tests
./build.sh -t

# Parallel build with 8 jobs
./build.sh -j 8
```

## Coverage Requirements

All components must maintain:
- **Minimum 75% line coverage** in `src/` folders
- **100% function coverage** where possible
- No regressions in existing coverage

### Checking Coverage

```bash
# Generate coverage report
./build.sh -v

# Open HTML report
xdg-open build/coverage_html/index.html  # Linux
open build/coverage_html/index.html      # macOS
start build/coverage_html/index.html     # Windows
```

### Current Coverage Status

| Component | Line Coverage | Function Coverage | Status |
|-----------|---------------|-------------------|--------|
| linux-dmabuf | 78.4% | 100% | ✅ Pass |
| simpleshell | Target: 75%+ | Target: 100% | 🔄 In Progress |
| simplebuffer | Target: 75%+ | Target: 100% | 🔄 In Progress |
| linux-explicit-sync | Target: 75%+ | Target: 100% | 🔄 In Progress |

## Static Analysis

### cppcheck Suppressions

Edit `.cppcheck-suppressions` to suppress false positives:

```
# System includes
missingIncludeSystem

# Unused functions (tested via UNIT_TEST)
unusedFunction

# Specific file suppressions
unusedStructMember:*/mock/*
```

### Running cppcheck Locally

```bash
cppcheck --enable=warning,style,performance,portability \
    --inline-suppr \
    --suppressions-list=.cppcheck-suppressions \
    components/ common/
```

## Troubleshooting

### Build Failures

1. **Missing dependencies**: Ensure all system packages are installed
   ```bash
   sudo apt-get install build-essential cmake pkg-config \
       libwayland-dev libgbm-dev libdrm-dev \
       wayland-protocols libegl1-mesa-dev libgles2-mesa-dev
   ```

2. **GoogleTest not found**: Install GoogleTest
   ```bash
   sudo apt-get install libgtest-dev libgmock-dev
   ```

3. **Coverage tools missing**: Install lcov
   ```bash
   sudo apt-get install lcov genhtml
   ```

### Test Failures

1. Check test logs in `build/Testing/Temporary/LastTest.log`
2. Run specific test with verbose output:
   ```bash
   cd build/components/<component>/tests/L1
   ./<test_executable> --gtest_filter=<TestName>
   ```
3. Enable test debugging:
   ```bash
   export GTEST_BREAK_ON_FAILURE=1
   ```

### ACT Issues

1. **Docker not running**: Start Docker service
   ```bash
   sudo systemctl start docker
   ```

2. **Permission denied**: Add user to docker group
   ```bash
   sudo usermod -aG docker $USER
   newgrp docker
   ```

3. **Artifacts not working**: ACT skips artifact uploads by default (this is expected)

## Best Practices

1. **Always run local CI before pushing**:
   ```bash
   ./ci-local.sh
   ```

2. **Keep coverage above 75%**: Add tests for new code

3. **Fix static analysis warnings**: Address cppcheck findings

4. **Document your changes**: Update README files

5. **Test on clean build**: Use `-c` flag occasionally
   ```bash
   ./build.sh -c -v
   ```

6. **Review coverage gaps**: Check uncovered lines in HTML report

## CI Workflow Files

- `.github/workflows/ci.yml` - Main CI pipeline
- `.github/workflows/ci-comprehensive.yml` - Extended checks (if exists)
- `.cppcheck-suppressions` - Static analysis suppressions
- `build.sh` - Main build script
- `ci-local.sh` - Local CI simulation

## Contact

For CI/CD issues or questions, please open an issue in the repository.
