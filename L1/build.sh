#!/bin/bash
# Build script for L1 Westeros Components
# Supports flexible building of individual components or all components

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Default values
BUILD_DIR="build"
BUILD_TYPE="Debug"
BUILD_ALL=true
BUILD_TESTS=true
ENABLE_COVERAGE=false
CLEAN_BUILD=false
COMPONENTS=()

# Help function
show_help() {
    echo -e "${CYAN}L1 Westeros Components Build Script${NC}"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help              Show this help message"
    echo "  -c, --clean             Clean build directory before building"
    echo "  -r, --release           Build in Release mode (default: Debug)"
    echo "  -t, --no-tests          Disable building tests"
    echo "  -v, --coverage          Enable code coverage"
    echo "  -a, --all               Build all components (default)"
    echo "  -o, --only COMPONENT    Build only specific component(s)"
    echo "                          Can be used multiple times"
    echo "                          Valid: simpleshell, simplebuffer,"
    echo "                                 linux-dmabuf, linux-explicit-sync"
    echo "  -j, --jobs N            Number of parallel jobs (default: nproc)"
    echo ""
    echo "Examples:"
    echo "  $0                                    # Build all components in Debug mode"
    echo "  $0 -r                                 # Build all in Release mode"
    echo "  $0 -o simpleshell                     # Build only simpleshell component"
    echo "  $0 -o simpleshell -o simplebuffer     # Build simpleshell and simplebuffer"
    echo "  $0 -c -r -v                           # Clean build, Release mode, with coverage"
    echo "  $0 -o linux-dmabuf -t                 # Build linux-dmabuf without tests"
    exit 0
}

# Parse arguments
JOBS=$(nproc 2>/dev/null || echo "4")

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            ;;
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        -r|--release)
            BUILD_TYPE="Release"
            shift
            ;;
        -t|--no-tests)
            BUILD_TESTS=false
            shift
            ;;
        -v|--coverage)
            ENABLE_COVERAGE=true
            shift
            ;;
        -a|--all)
            BUILD_ALL=true
            COMPONENTS=()
            shift
            ;;
        -o|--only)
            BUILD_ALL=false
            COMPONENTS+=("$2")
            shift 2
            ;;
        -j|--jobs)
            JOBS="$2"
            shift 2
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            show_help
            ;;
    esac
done

# Print configuration
echo -e "${CYAN}╔════════════════════════════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║        L1 Westeros Components - Build Configuration            ║${NC}"
echo -e "${CYAN}╚════════════════════════════════════════════════════════════════╝${NC}"
echo -e "${YELLOW}Build Directory:${NC} $BUILD_DIR"
echo -e "${YELLOW}Build Type:${NC}      $BUILD_TYPE"
echo -e "${YELLOW}Build Tests:${NC}     $BUILD_TESTS"
echo -e "${YELLOW}Coverage:${NC}        $ENABLE_COVERAGE"
echo -e "${YELLOW}Parallel Jobs:${NC}   $JOBS"

if [ "$BUILD_ALL" = true ]; then
    echo -e "${YELLOW}Components:${NC}      All"
else
    echo -e "${YELLOW}Components:${NC}      ${COMPONENTS[*]}"
fi
echo ""

# Clean build directory if requested
if [ "$CLEAN_BUILD" = true ]; then
    echo -e "${YELLOW}[1/3] Cleaning build directory...${NC}"
    rm -rf "$BUILD_DIR"
    echo -e "${GREEN}✓ Build directory cleaned${NC}"
    echo ""
fi

# Create build directory
mkdir -p "$BUILD_DIR"

# Configure CMake
echo -e "${YELLOW}[2/3] Configuring with CMake...${NC}"
cd "$BUILD_DIR"

CMAKE_ARGS=(
    "-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    "-DBUILD_TESTS=$BUILD_TESTS"
    "-DENABLE_COVERAGE=$ENABLE_COVERAGE"
)

if [ "$BUILD_ALL" = true ]; then
    CMAKE_ARGS+=("-DBUILD_ALL_COMPONENTS=ON")
else
    CMAKE_ARGS+=("-DBUILD_ALL_COMPONENTS=OFF")
    
    # Enable specific components
    for comp in "${COMPONENTS[@]}"; do
        COMP_UPPER=$(echo "$comp" | tr '[:lower:]' '[:upper:]' | tr '-' '_')
        CMAKE_ARGS+=("-DBUILD_${COMP_UPPER}=ON")
    done
fi

echo -e "${CYAN}Running: cmake .. ${CMAKE_ARGS[*]}${NC}"
cmake .. "${CMAKE_ARGS[@]}"

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ CMake configuration successful${NC}"
else
    echo -e "${RED}✗ CMake configuration failed${NC}"
    exit 1
fi
echo ""

# Build
echo -e "${YELLOW}[3/3] Building...${NC}"
cmake --build . -- -j"$JOBS"

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Build successful${NC}"
else
    echo -e "${RED}✗ Build failed${NC}"
    exit 1
fi
echo ""

# Run tests if enabled
if [ "$BUILD_TESTS" = true ]; then
    echo -e "${CYAN}════════════════════════════════════════════════════════════${NC}"
    echo -e "${YELLOW}Running L1 Tests...${NC}"
    echo -e "${CYAN}════════════════════════════════════════════════════════════${NC}"
    echo ""
    
    # Run tests with verbose output to show test results
    ctest --verbose --output-on-failure
    TEST_RESULT=$?
    
    echo ""
    echo -e "${CYAN}════════════════════════════════════════════════════════════${NC}"
    echo -e "${YELLOW}Test Results Summary:${NC}"
    echo -e "${CYAN}════════════════════════════════════════════════════════════${NC}"
    
    # Show test summary
    ctest --quiet
    
    echo -e "${CYAN}════════════════════════════════════════════════════════════${NC}"
    if [ $TEST_RESULT -eq 0 ]; then
        echo -e "${GREEN}✓ All L1 tests passed successfully!${NC}"
    else
        echo -e "${RED}✗ Some L1 tests failed - see details above${NC}"
        echo -e "${CYAN}════════════════════════════════════════════════════════════${NC}"
        exit 1
    fi
    echo -e "${CYAN}════════════════════════════════════════════════════════════${NC}"
    echo ""
fi

# Generate coverage if enabled
if [ "$ENABLE_COVERAGE" = true ]; then
    echo -e "${YELLOW}Generating coverage report...${NC}"
    cmake --build . --target coverage
    
    if [ -d "coverage_html" ]; then
        echo -e "${GREEN}✓ Coverage report generated: $BUILD_DIR/coverage_html/index.html${NC}"
    fi
    echo ""
fi

echo -e "${GREEN}╔════════════════════════════════════════════════════════════════╗${NC}"
echo -e "${GREEN}║                   BUILD COMPLETED SUCCESSFULLY!                 ║${NC}"
echo -e "${GREEN}╚════════════════════════════════════════════════════════════════╝${NC}"
echo ""
echo -e "${CYAN}Build artifacts are in: $BUILD_DIR${NC}"
echo -e "${CYAN}To install: cd $BUILD_DIR && sudo make install${NC}"
