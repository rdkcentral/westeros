#!/bin/bash
# Run tests with coverage for all L1 Westeros Components

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}L1 Westeros Components - Test & Coverage${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

# Build with coverage enabled
echo -e "${YELLOW}Step 1: Building with coverage enabled...${NC}"
./build.sh -c -v

# Run tests
echo ""
echo -e "${YELLOW}Step 2: Running all tests...${NC}"
cd build

# Component directories
COMPONENTS=("simpleshell" "simplebuffer" "linux-dmabuf" "linux-explicit-sync")

for component in "${COMPONENTS[@]}"; do
    if [ -d "$component" ]; then
        echo -e "${CYAN}Running tests for $component...${NC}"
        cd "$component"
        
        # Find and run test executable
        TEST_EXEC=$(find . -name "test_*" -o -name "*_test" | head -n 1)
        if [ -f "$TEST_EXEC" ]; then
            ./$TEST_EXEC
        else
            echo -e "${RED}No test executable found for $component${NC}"
        fi
        
        cd ..
    fi
done

cd ..

# Generate coverage report
echo ""
echo -e "${YELLOW}Step 3: Generating coverage report...${NC}"

# Capture coverage data
lcov --capture --directory build --output-file coverage.info

# Filter out system headers and test files
lcov --remove coverage.info '/usr/*' '*/test/*' '*/tests/*' '*/googletest/*' --output-file coverage_filtered.info

# Generate HTML report
genhtml coverage_filtered.info --output-directory coverage_html

echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Tests completed successfully!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo -e "${CYAN}Coverage report generated in: ${YELLOW}coverage_html/index.html${NC}"
echo ""
echo "Component Coverage:"
lcov --list coverage_filtered.info | grep -E "(simpleshell|simplebuffer|linux-dmabuf|linux-expsync)" | head -10

echo ""
echo -e "${CYAN}To view the full coverage report:${NC}"
echo "  xdg-open coverage_html/index.html"
echo "  or"
echo "  firefox coverage_html/index.html"
