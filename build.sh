#!/bin/bash
# Build script for L1 Test Suite for Westeros
# Copyright 2024 RDK Management

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Default values
BUILD_TYPE="Debug"
ENABLE_COVERAGE="OFF"
BUILD_DIR="build"
CLEAN_BUILD=false
VERBOSE=false
JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Default source paths - use local repository
WESTEROS_MAIN_SRC="."
SIMPLESHELL_SRC="./simpleshell"
SIMPLEBUFFER_SRC="./simplebuffer"
LINUX_DMABUF_SRC="./linux-dmabuf"
LINUX_EXPSYNC_SRC="./linux-expsync"

# Print colored message
print_msg() {
    local color=$1
    shift
    echo -e "${color}$@${NC}"
}

# Print usage
usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Build script for L1 Test Suite for Westeros - Unified test framework.

Options:
    -h, --help                  Show this help message
    -c, --coverage              Enable code coverage (implies Debug build)
    -r, --release               Build in Release mode (default: Debug)
    -C, --clean                 Clean build directory before building
    -v, --verbose               Verbose build output
    -j, --jobs N                Number of parallel build jobs (default: $(nproc))
    --westeros-main PATH        Path to westeros-main source (default: . [current directory])
    --simpleshell PATH          Path to simpleshell source (default: ./simpleshell)
    --simplebuffer PATH         Path to simplebuffer source (default: ./simplebuffer)
    --linux-dmabuf PATH         Path to linux-dmabuf source (default: ./linux-dmabuf)
    --linux-expsync PATH        Path to linux-expsync source (default: ./linux-expsync)

Examples:
    $0                          # Build with default settings
    $0 --coverage               # Build with coverage
    $0 --release                # Build release version
    $0 --clean --coverage       # Clean and rebuild with coverage
    $0 --westeros-main /path/to/westeros-main/westeros-main  # Custom source path
    # Note: Sources are in this repository by default

EOF
    exit 0
}

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            usage
            ;;
        -c|--coverage)
            ENABLE_COVERAGE="ON"
            BUILD_TYPE="Debug"
            shift
            ;;
        -r|--release)
            BUILD_TYPE="Release"
            shift
            ;;
        -C|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -j|--jobs)
            JOBS="$2"
            shift 2
            ;;
        --westeros-main)
            WESTEROS_MAIN_SRC="$2"
            shift 2
            ;;
        --simpleshell)
            SIMPLESHELL_SRC="$2"
            shift 2
            ;;
        --simplebuffer)
            SIMPLEBUFFER_SRC="$2"
            shift 2
            ;;
        --linux-dmabuf)
            LINUX_DMABUF_SRC="$2"
            shift 2
            ;;
        --linux-expsync)
            LINUX_EXPSYNC_SRC="$2"
            shift 2
            ;;
        *)
            print_msg $RED "Unknown option: $1"
            usage
            ;;
    esac
done

# Print banner
print_msg $BLUE "=========================================="
print_msg $BLUE "L1 Test Suite for Westeros - Unified Build"
print_msg $BLUE "=========================================="
echo ""

# Verify westeros-main source path (check for key source files)
if [ ! -f "$WESTEROS_MAIN_SRC/westeros-compositor.cpp" ]; then
    print_msg $RED "ERROR: Westeros Main source not found at: $WESTEROS_MAIN_SRC"
    print_msg $RED "Required file westeros-compositor.cpp not found"
    print_msg $RED "Please specify correct path with --westeros-main parameter"
    exit 1
fi

# Print source paths
print_msg $CYAN "Source Paths:"
print_msg $YELLOW "  Westeros Main: $WESTEROS_MAIN_SRC"
[ -d "$SIMPLESHELL_SRC" ] && print_msg $YELLOW "  SimpleShell: $SIMPLESHELL_SRC"
[ -d "$SIMPLEBUFFER_SRC" ] && print_msg $YELLOW "  SimpleBuffer: $SIMPLEBUFFER_SRC"
[ -d "$LINUX_DMABUF_SRC" ] && print_msg $YELLOW "  Linux DMA-BUF: $LINUX_DMABUF_SRC"
[ -d "$LINUX_EXPSYNC_SRC" ] && print_msg $YELLOW "  Linux ExplicitSync: $LINUX_EXPSYNC_SRC"
echo ""

# Clean build if requested
if $CLEAN_BUILD && [ -d "$BUILD_DIR" ]; then
    print_msg $YELLOW "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

# Create build directory
if [ ! -d "$BUILD_DIR" ]; then
    print_msg $GREEN "Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

# Configure
print_msg $BLUE "Configuring CMake..."
print_msg $YELLOW "  Build Type: $BUILD_TYPE"
print_msg $YELLOW "  Coverage: $ENABLE_COVERAGE"
print_msg $YELLOW "  Jobs: $JOBS"
echo ""

cd "$BUILD_DIR"

WESTEROS_MAIN_ABSOLUTE=$(cd "$WESTEROS_MAIN_SRC" && pwd)
CMAKE_ARGS=(
    "-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    "-DENABLE_COVERAGE=$ENABLE_COVERAGE"
    "-DWESTEROS_MAIN_SRC_ROOT=$WESTEROS_MAIN_ABSOLUTE"
)

# Add CMAKE flags if set (for suppressing warnings)
if [ -n "$CMAKE_CXX_FLAGS" ]; then
    CMAKE_ARGS+=("-DCMAKE_CXX_FLAGS=$CMAKE_CXX_FLAGS")
fi

if [ -n "$CMAKE_C_FLAGS" ]; then
    CMAKE_ARGS+=("-DCMAKE_C_FLAGS=$CMAKE_C_FLAGS")
fi

# Add optional component paths
if [ -d "$SIMPLESHELL_SRC" ]; then
    SIMPLESHELL_ABSOLUTE=$(cd "$SIMPLESHELL_SRC" && pwd)
    CMAKE_ARGS+=("-DSIMPLESHELL_SRC_ROOT=$SIMPLESHELL_ABSOLUTE")
    CMAKE_ARGS+=("-DBUILD_SIMPLESHELL_TESTS=ON")
fi

if [ -d "$SIMPLEBUFFER_SRC" ]; then
    SIMPLEBUFFER_ABSOLUTE=$(cd "$SIMPLEBUFFER_SRC" && pwd)
    CMAKE_ARGS+=("-DSIMPLEBUFFER_SRC_ROOT=$SIMPLEBUFFER_ABSOLUTE")
    CMAKE_ARGS+=("-DBUILD_SIMPLEBUFFER_TESTS=ON")
fi

if [ -d "$LINUX_DMABUF_SRC" ]; then
    LINUX_DMABUF_ABSOLUTE=$(cd "$LINUX_DMABUF_SRC" && pwd)
    CMAKE_ARGS+=("-DLINUX_DMABUF_SRC_ROOT=$LINUX_DMABUF_ABSOLUTE")
    CMAKE_ARGS+=("-DBUILD_LINUX_DMABUF_TESTS=ON")
fi

if [ -d "$LINUX_EXPSYNC_SRC" ]; then
    LINUX_EXPSYNC_ABSOLUTE=$(cd "$LINUX_EXPSYNC_SRC" && pwd)
    CMAKE_ARGS+=("-DLINUX_EXPSYNC_SRC_ROOT=$LINUX_EXPSYNC_ABSOLUTE")
    CMAKE_ARGS+=("-DBUILD_LINUX_EXPSYNC_TESTS=ON")
fi

cmake .. "${CMAKE_ARGS[@]}"

# Build
print_msg $BLUE "Building..."
BUILD_ARGS=("--build" "." "--config" "$BUILD_TYPE" "--parallel" "$JOBS")
if $VERBOSE; then
    BUILD_ARGS+=("--verbose")
fi

cmake "${BUILD_ARGS[@]}"

cd ..

print_msg $GREEN ""
print_msg $GREEN "=========================================="
print_msg $GREEN "Build Successful!"
print_msg $GREEN "=========================================="
echo ""
print_msg $CYAN "Test executables in: $BUILD_DIR"
echo ""

# Automatically generate coverage report if coverage is enabled
if [ "$ENABLE_COVERAGE" = "ON" ]; then
    print_msg $BLUE ""
    print_msg $BLUE "=========================================="
    print_msg $BLUE "Generating Coverage Report..."
    print_msg $BLUE "=========================================="
    echo ""
    
    cd "$BUILD_DIR"
    cmake --build . --target coverage
    cd ..
    
    print_msg $GREEN ""
    print_msg $GREEN "=========================================="
    print_msg $GREEN "Coverage Report Generated!"
    print_msg $GREEN "=========================================="
    echo ""
    print_msg $CYAN "Coverage report available at:"
    print_msg $YELLOW "  $BUILD_DIR/coverage_html/index.html"
    echo ""
else
    print_msg $YELLOW "Run tests with:"
    print_msg $NC "  cd $BUILD_DIR"
    print_msg $NC "  ctest --output-on-failure"
    echo ""
fi
