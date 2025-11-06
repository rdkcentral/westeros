# L1 Westeros Components - Root CMakeLists.txt
cmake_minimum_required(VERSION 3.14)
project(L1-Westeros-Components VERSION 1.0.0 LANGUAGES C CXX)

# Project metadata
set(PROJECT_DESCRIPTION "L1 Testing Framework for Westeros Components")
set(PROJECT_HOMEPAGE_URL "https://github.com/YOUR_ORG/L1-Westeros-Components")

# C++ Standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(BUILD_ALL_COMPONENTS "Build all components" ON)
option(BUILD_SIMPLESHELL "Build SimpleShell component" ON)
option(BUILD_SIMPLEBUFFER "Build SimpleBuffer component" ON)
option(BUILD_LINUX_DMABUF "Build Linux DMA-BUF component" ON)
option(BUILD_LINUX_EXPLICIT_SYNC "Build Linux Explicit Sync component" ON)
option(BUILD_TESTS "Build test suites" ON)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(ENABLE_STATIC_ANALYSIS "Enable static analysis" OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "")
message(STATUS "========================================")
message(STATUS "L1 Westeros Components Configuration")
message(STATUS "========================================")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Coverage: ${ENABLE_COVERAGE}")
message(STATUS "========================================")
message(STATUS "")

# Find required packages
find_package(PkgConfig REQUIRED)

# Wayland support
pkg_check_modules(WAYLAND wayland-client wayland-egl)
if(WAYLAND_FOUND)
    message(STATUS "✓ Wayland found: ${WAYLAND_VERSION}")
else()
    message(STATUS "✗ Wayland not found - using stubs")
endif()

# EGL/GLES support
pkg_check_modules(EGL egl)
pkg_check_modules(GLES glesv2)

# GoogleTest setup - Multi-method detection
if(BUILD_TESTS)
    message(STATUS "Searching for GoogleTest...")
    
    # Method 1: Try CMake's find_package
    find_package(GTest QUIET)
    
    # Method 2: Try pkg-config
    if(NOT GTest_FOUND)
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(GTEST QUIET gtest)
            pkg_check_modules(GMOCK QUIET gmock)
            if(GTEST_FOUND)
                message(STATUS "✓ GoogleTest found via pkg-config")
            endif()
        endif()
    endif()
    
    # Method 3: Try legacy FindGTest with common paths
    if(NOT GTest_FOUND AND NOT GTEST_FOUND)
        foreach(gtest_path "/usr" "/usr/local" "$ENV{HOME}/.local")
            if(EXISTS "${gtest_path}/lib/libgtest.a" OR EXISTS "${gtest_path}/lib/x86_64-linux-gnu/libgtest.a")
                set(GTEST_ROOT "${gtest_path}" CACHE PATH "GTest root directory" FORCE)
                find_package(GTest QUIET)
                if(GTest_FOUND)
                    message(STATUS "✓ GoogleTest found at ${gtest_path}")
                    break()
                endif()
            endif()
        endforeach()
    endif()
    
    # Method 4: Manual library detection
    if(NOT GTest_FOUND AND NOT GTEST_FOUND)
        find_library(GTEST_LIBRARY NAMES gtest PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu)
        find_library(GTEST_MAIN_LIBRARY NAMES gtest_main PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu)
        find_library(GMOCK_LIBRARY NAMES gmock PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu)
        find_path(GTEST_INCLUDE_DIR gtest/gtest.h PATHS /usr/include /usr/local/include)
        
        if(GTEST_LIBRARY AND GTEST_MAIN_LIBRARY AND GTEST_INCLUDE_DIR)
            message(STATUS "✓ GoogleTest found manually:")
            message(STATUS "    Libraries: ${GTEST_LIBRARY}")
            message(STATUS "    Headers: ${GTEST_INCLUDE_DIR}")
            
            # Create imported targets
            add_library(GTest::gtest UNKNOWN IMPORTED)
            set_target_properties(GTest::gtest PROPERTIES
                IMPORTED_LOCATION "${GTEST_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
                INTERFACE_LINK_LIBRARIES "pthread"
            )
            
            add_library(GTest::gtest_main UNKNOWN IMPORTED)
            set_target_properties(GTest::gtest_main PROPERTIES
                IMPORTED_LOCATION "${GTEST_MAIN_LIBRARY}"
                INTERFACE_LINK_LIBRARIES "GTest::gtest"
            )
            
            if(GMOCK_LIBRARY)
                add_library(GTest::gmock UNKNOWN IMPORTED)
                set_target_properties(GTest::gmock PROPERTIES
                    IMPORTED_LOCATION "${GMOCK_LIBRARY}"
                    INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
                    INTERFACE_LINK_LIBRARIES "GTest::gtest"
                )
            endif()
            
            set(GTest_FOUND TRUE)
        endif()
    endif()
    
    # Method 5: Use FetchContent as fallback (if USE_SYSTEM_GTEST is not forced)
    if(NOT GTest_FOUND AND NOT GTEST_FOUND AND NOT USE_SYSTEM_GTEST)
        message(STATUS "GoogleTest not found in system, using FetchContent...")
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
            GIT_SHALLOW TRUE
        )
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
        set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
        
        # Create GTest::* aliases for consistency
        if(NOT TARGET GTest::gtest)
            add_library(GTest::gtest ALIAS gtest)
            add_library(GTest::gtest_main ALIAS gtest_main)
            add_library(GTest::gmock ALIAS gmock)
            add_library(GTest::gmock_main ALIAS gmock_main)
        endif()
        
        set(GTest_FOUND TRUE)
        message(STATUS "✓ GoogleTest fetched via FetchContent")
    endif()
    
    # Final verification
    if(NOT GTest_FOUND AND NOT GTEST_FOUND AND NOT TARGET gtest AND NOT TARGET GTest::gtest)
        message(FATAL_ERROR 
            "\n========================================\n"
            "GoogleTest is required but not found!\n"
            "Please install it:\n"
            "  Ubuntu/Debian: sudo apt-get install libgtest-dev libgmock-dev\n"
            "  Then build from source:\n"
            "    cd /usr/src/googletest\n"
            "    sudo cmake .\n"
            "    sudo cmake --build . --target install\n"
            "Or set USE_SYSTEM_GTEST=OFF to fetch from GitHub\n"
            "========================================\n")
    endif()
    
    # Legacy support: create modern targets from old variables
    if(GTEST_FOUND AND NOT TARGET GTest::gtest)
        add_library(GTest::gtest INTERFACE IMPORTED)
        target_include_directories(GTest::gtest INTERFACE ${GTEST_INCLUDE_DIRS})
        target_link_libraries(GTest::gtest INTERFACE ${GTEST_LIBRARIES} pthread)
        
        add_library(GTest::gtest_main INTERFACE IMPORTED)
        target_link_libraries(GTest::gtest_main INTERFACE GTest::gtest ${GTEST_MAIN_LIBRARIES})
        
        if(GMOCK_FOUND)
            add_library(GTest::gmock INTERFACE IMPORTED)
            target_include_directories(GTest::gmock INTERFACE ${GMOCK_INCLUDE_DIRS})
            target_link_libraries(GTest::gmock INTERFACE ${GMOCK_LIBRARIES} GTest::gtest)
        endif()
    endif()
else()
    message(STATUS "Tests disabled (BUILD_TESTS=OFF)")
endif()

# Coverage configuration
if(ENABLE_COVERAGE)
    message(STATUS "✓ Code coverage enabled")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage -lgcov")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage -lgcov")
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/components
)

# Add common utilities
add_subdirectory(common)

# Enable testing BEFORE adding components (so add_test() works in subdirectories)
if(BUILD_TESTS)
    enable_testing()
    message(STATUS "")
    message(STATUS "✓ Testing framework enabled")
endif()

# Add components based on build options
message(STATUS "")
message(STATUS "Building Components:")

if(BUILD_ALL_COMPONENTS OR BUILD_SIMPLESHELL)
    if(EXISTS ${CMAKE_SOURCE_DIR}/components/simpleshell)
        add_subdirectory(components/simpleshell)
        message(STATUS "  ✓ SimpleShell")
    endif()
endif()

if(BUILD_ALL_COMPONENTS OR BUILD_SIMPLEBUFFER)
    if(EXISTS ${CMAKE_SOURCE_DIR}/components/simplebuffer)
        add_subdirectory(components/simplebuffer)
        message(STATUS "  ✓ SimpleBuffer")
    endif()
endif()

if(BUILD_ALL_COMPONENTS OR BUILD_LINUX_DMABUF)
    if(EXISTS ${CMAKE_SOURCE_DIR}/components/linux-dmabuf)
        add_subdirectory(components/linux-dmabuf)
        message(STATUS "  ✓ Linux DMA-BUF")
    endif()
endif()

if(BUILD_ALL_COMPONENTS OR BUILD_LINUX_EXPLICIT_SYNC)
    if(EXISTS ${CMAKE_SOURCE_DIR}/components/linux-explicit-sync)
        add_subdirectory(components/linux-explicit-sync)
        message(STATUS "  ✓ Linux Explicit Sync")
    endif()
endif()

message(STATUS "")

# Integration tests
if(BUILD_TESTS)
    if(EXISTS ${CMAKE_SOURCE_DIR}/tests AND EXISTS ${CMAKE_SOURCE_DIR}/tests/CMakeLists.txt)
        add_subdirectory(tests)
        message(STATUS "  ✓ Integration tests configured")
    else()
        message(STATUS "  ℹ Integration tests directory not configured (component tests only)")
    endif()
endif()

# Coverage target (testing already enabled above)
if(ENABLE_COVERAGE AND BUILD_TESTS)
    # Find coverage tools
    find_program(LCOV_EXECUTABLE lcov)
    find_program(GENHTML_EXECUTABLE genhtml)
    find_program(GCOV_EXECUTABLE gcov)
    
    if(NOT LCOV_EXECUTABLE)
        message(WARNING "lcov not found - coverage target will not be available")
        message(STATUS "  Install with: sudo apt-get install lcov (Linux) or brew install lcov (Mac)")
    endif()
    
    if(NOT GENHTML_EXECUTABLE)
        message(WARNING "genhtml not found - HTML coverage report will not be generated")
    endif()
    
    if(LCOV_EXECUTABLE AND GENHTML_EXECUTABLE)
        message(STATUS "✓ Coverage tools found:")
        message(STATUS "  - lcov: ${LCOV_EXECUTABLE}")
        message(STATUS "  - genhtml: ${GENHTML_EXECUTABLE}")
        
        # Main coverage target with full workflow
        add_custom_target(coverage
            # Clean old coverage data
            COMMAND ${CMAKE_COMMAND} -E echo "============================================"
            COMMAND ${CMAKE_COMMAND} -E echo "Step 1: Cleaning old coverage data..."
            COMMAND ${CMAKE_COMMAND} -E echo "============================================"
            COMMAND ${CMAKE_COMMAND} -E remove -f coverage.info coverage.cleaned.info
            COMMAND ${CMAKE_COMMAND} -E remove_directory coverage_html
            COMMAND ${CMAKE_COMMAND} -E remove_directory html_coverage_report
            
            # Reset coverage counters
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMAND ${CMAKE_COMMAND} -E echo "============================================"
            COMMAND ${CMAKE_COMMAND} -E echo "Step 2: Resetting coverage counters..."
            COMMAND ${CMAKE_COMMAND} -E echo "============================================"
            COMMAND ${LCOV_EXECUTABLE} --directory . --zerocounters 2>/dev/null || ${CMAKE_COMMAND} -E echo "Note: No existing counters to reset"
            
            # Run tests to generate coverage data
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMAND ${CMAKE_COMMAND} -E echo "============================================"
            COMMAND ${CMAKE_COMMAND} -E echo "Step 3: Running tests to generate coverage..."
            COMMAND ${CMAKE_COMMAND} -E echo "============================================"
            COMMAND ${CMAKE_COMMAND} -E echo "Listing available tests:"
            COMMAND ${CMAKE_CTEST_COMMAND} --show-only || ${CMAKE_COMMAND} -E echo "ERROR: No tests found!"
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMAND ${CMAKE_COMMAND} -E echo "Executing tests:"
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            
            # Capture coverage data
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMAND ${CMAKE_COMMAND} -E echo "============================================"
            COMMAND ${CMAKE_COMMAND} -E echo "Step 4: Capturing coverage data..."
            COMMAND ${CMAKE_COMMAND} -E echo "============================================"
            COMMAND ${LCOV_EXECUTABLE} --directory . --capture --output-file coverage.info --rc lcov_branch_coverage=0
            
            # Remove unwanted files from coverage
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMAND ${CMAKE_COMMAND} -E echo "============================================"
            COMMAND ${CMAKE_COMMAND} -E echo "Step 5: Filtering coverage data..."
            COMMAND ${CMAKE_COMMAND} -E echo "============================================"
            COMMAND ${LCOV_EXECUTABLE} --remove coverage.info '/usr/*' '*/build/_deps/*' '*/tests/*' '*/test/*' '*/mock/*' '*/mocks/*' '*/googletest/*' '*/googlemock/*' --output-file coverage.cleaned.info --rc lcov_branch_coverage=0
            
            # Generate HTML report
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMAND ${CMAKE_COMMAND} -E echo "============================================"
            COMMAND ${CMAKE_COMMAND} -E echo "Step 6: Generating HTML report..."
            COMMAND ${CMAKE_COMMAND} -E echo "============================================"
            COMMAND ${GENHTML_EXECUTABLE} coverage.cleaned.info --output-directory coverage_html
            
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMAND ${CMAKE_COMMAND} -E echo "================================================"
            COMMAND ${CMAKE_COMMAND} -E echo "SUCCESS: Coverage report generated!"
            COMMAND ${CMAKE_COMMAND} -E echo "================================================"
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMAND ${CMAKE_COMMAND} -E echo "Test Results:"
            COMMAND ${CMAKE_COMMAND} -E echo "  Run 'ctest' to see test summary"
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMAND ${CMAKE_COMMAND} -E echo "Coverage Report:"
            COMMAND ${CMAKE_COMMAND} -E echo "  Open: ${CMAKE_BINARY_DIR}/coverage_html/index.html"
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMAND ${CMAKE_COMMAND} -E echo "================================================"
            
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Running tests and generating code coverage report..."
            VERBATIM
        )
        
        # Alternative coverage_report target (simplified - assumes tests already run)
        add_custom_target(coverage_report
            COMMAND ${CMAKE_COMMAND} -E echo "Generating coverage report from existing data..."
            COMMAND ${LCOV_EXECUTABLE} --capture --directory . --output-file coverage.info
            COMMAND ${GENHTML_EXECUTABLE} coverage.info --output-directory html_coverage_report
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMAND ${CMAKE_COMMAND} -E echo "Coverage report generated: ${CMAKE_BINARY_DIR}/html_coverage_report/index.html"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating HTML coverage report..."
            VERBATIM
        )
        
        message(STATUS "")
        message(STATUS "  ℹ  Coverage targets available:")
        message(STATUS "    - make coverage         : Run tests + generate full coverage report")
        message(STATUS "    - make coverage_report  : Generate report from existing .gcda files")
        message(STATUS "")
    else()
        message(STATUS "  ⚠  Coverage tools not found - coverage targets disabled")
        message(STATUS "     Install lcov and genhtml to enable coverage reporting")
    endif()
endif()

# Install rules
install(DIRECTORY ${CMAKE_SOURCE_DIR}/common/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/L1WesterosComponentsConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Summary
message(STATUS "")
message(STATUS "========================================")
message(STATUS "Configuration Summary")
message(STATUS "========================================")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Build tests: ${BUILD_TESTS}")
message(STATUS "Enable coverage: ${ENABLE_COVERAGE}")
message(STATUS "========================================")
message(STATUS "")
