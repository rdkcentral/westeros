# L1 Test Suite for Westeros - Unified CMakeLists.txt
cmake_minimum_required(VERSION 3.14)
project(L1-TestSuiteWesteros VERSION 1.0.0 LANGUAGES C CXX)

# Project metadata
set(PROJECT_DESCRIPTION "Unified L1 Testing Framework for Westeros Components")
set(PROJECT_HOMEPAGE_URL "https://github.com/YOUR_ORG/L1-TestSuiteWesteros")

# C++ Standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ========================================
# Source Code Path Configuration
# ========================================
# Use local source files from this repository
# ALWAYS use the source directory - ignore any cached or passed values pointing to build directory
if(NOT DEFINED WESTEROS_MAIN_SRC_ROOT OR 
   WESTEROS_MAIN_SRC_ROOT STREQUAL "" OR 
   WESTEROS_MAIN_SRC_ROOT MATCHES ".*/build$" OR
   WESTEROS_MAIN_SRC_ROOT MATCHES ".*\\\\build$")
    set(WESTEROS_MAIN_SRC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
    message(STATUS "Setting WESTEROS_MAIN_SRC_ROOT to source directory: ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

# Convert to absolute path if it's relative
if(NOT IS_ABSOLUTE "${WESTEROS_MAIN_SRC_ROOT}")
    get_filename_component(WESTEROS_MAIN_SRC_ROOT "${WESTEROS_MAIN_SRC_ROOT}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

# Verify the path exists and has the required files
if(NOT EXISTS "${WESTEROS_MAIN_SRC_ROOT}/westeros-compositor.cpp")
    message(FATAL_ERROR "Invalid WESTEROS_MAIN_SRC_ROOT: ${WESTEROS_MAIN_SRC_ROOT}\nCannot find westeros-compositor.cpp\nPlease ensure WESTEROS_MAIN_SRC_ROOT points to the source directory, not the build directory.")
endif()

# Set as cache variable
set(WESTEROS_MAIN_SRC_ROOT "${WESTEROS_MAIN_SRC_ROOT}" CACHE PATH "Path to westeros source" FORCE)

message(STATUS "")
message(STATUS "========================================")
message(STATUS "Source Code Paths (Local Repository):")
message(STATUS "========================================")
message(STATUS "Westeros Main:          ${WESTEROS_MAIN_SRC_ROOT}")
message(STATUS "SimpleShell:            ${WESTEROS_MAIN_SRC_ROOT}/simpleshell")
message(STATUS "SimpleBuffer:           ${WESTEROS_MAIN_SRC_ROOT}/simplebuffer")
message(STATUS "Linux DMA-BUF:          ${WESTEROS_MAIN_SRC_ROOT}/linux-dmabuf")
message(STATUS "Linux Explicit Sync:    ${WESTEROS_MAIN_SRC_ROOT}/linux-expsync")
message(STATUS "========================================")
message(STATUS "")

# Build options
option(BUILD_WESTEROS_MAIN_TESTS "Build Westeros Main L1 tests" ON)
option(BUILD_SIMPLESHELL_TESTS "Build SimpleShell L1 tests" ON)
option(BUILD_SIMPLEBUFFER_TESTS "Build SimpleBuffer L1 tests" ON)
option(BUILD_LINUX_DMABUF_TESTS "Build Linux DMA-BUF L1 tests" ON)
option(BUILD_LINUX_EXPSYNC_TESTS "Build Linux Explicit Sync L1 tests" ON)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(ENABLE_STATIC_ANALYSIS "Enable static analysis" OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Coverage flags
if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "")
message(STATUS "========================================")
message(STATUS "L1 Test Suite Configuration")
message(STATUS "========================================")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Coverage: ${ENABLE_COVERAGE}")
message(STATUS "========================================")
message(STATUS "")

# Find required packages
find_package(PkgConfig REQUIRED)

# Wayland support
pkg_check_modules(WAYLAND wayland-client wayland-server wayland-egl)
if(WAYLAND_FOUND)
    message(STATUS "✓ Wayland found: ${WAYLAND_VERSION}")
else()
    message(STATUS "✗ Wayland not found - using mock implementations")
endif()

# EGL/GLES support
pkg_check_modules(EGL egl)
pkg_check_modules(GLES glesv2)
if(EGL_FOUND AND GLES_FOUND)
    message(STATUS "✓ EGL/GLES found")
else()
    message(STATUS "✗ EGL/GLES not found - using mock implementations")
endif()

# GoogleTest setup - Multi-method detection
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
            add_library(GTest::gtest INTERFACE IMPORTED)
            add_library(GTest::gtest_main INTERFACE IMPORTED)
            add_library(GTest::gmock INTERFACE IMPORTED)
            set_target_properties(GTest::gtest PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIRS}"
                INTERFACE_LINK_LIBRARIES "${GTEST_LIBRARIES}")
            set_target_properties(GTest::gtest_main PROPERTIES
                INTERFACE_LINK_LIBRARIES "${GTEST_MAIN_LIBRARIES}")
            set_target_properties(GTest::gmock PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${GMOCK_INCLUDE_DIRS}"
                INTERFACE_LINK_LIBRARIES "${GMOCK_LIBRARIES}")
        endif()
    endif()
else()
    message(STATUS "✓ GoogleTest found via CMake")
endif()

# Method 3: Manual library detection
if(NOT GTest_FOUND AND NOT GTEST_FOUND)
    find_library(GTEST_LIBRARY NAMES gtest PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu)
    find_library(GTEST_MAIN_LIBRARY NAMES gtest_main PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu)
    find_library(GMOCK_LIBRARY NAMES gmock PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu)
    find_path(GTEST_INCLUDE_DIR gtest/gtest.h PATHS /usr/include /usr/local/include)
    
    if(GTEST_LIBRARY AND GTEST_MAIN_LIBRARY AND GTEST_INCLUDE_DIR)
        message(STATUS "✓ GoogleTest found manually:")
        message(STATUS "    Libraries: ${GTEST_LIBRARY}")
        message(STATUS "    Include: ${GTEST_INCLUDE_DIR}")
        
        add_library(GTest::gtest INTERFACE IMPORTED)
        add_library(GTest::gtest_main INTERFACE IMPORTED)
        add_library(GTest::gmock INTERFACE IMPORTED)
        set_target_properties(GTest::gtest PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
            INTERFACE_LINK_LIBRARIES "${GTEST_LIBRARY};pthread")
        set_target_properties(GTest::gtest_main PROPERTIES
            INTERFACE_LINK_LIBRARIES "${GTEST_MAIN_LIBRARY}")
        if(GMOCK_LIBRARY)
            set_target_properties(GTest::gmock PROPERTIES
                INTERFACE_LINK_LIBRARIES "${GMOCK_LIBRARY}")
        endif()
        set(GTest_FOUND TRUE)
    else()
        message(FATAL_ERROR "✗ GoogleTest not found. Tests cannot be built.\n"
                          "   Install with: sudo apt-get install libgtest-dev libgmock-dev")
    endif()
endif()

if(GTest_FOUND OR GTEST_FOUND)
    message(STATUS "✓ GoogleTest integration ready")
    enable_testing()
endif()

# Coverage setup
if(ENABLE_COVERAGE)
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(WARNING "Coverage enabled but build type is ${CMAKE_BUILD_TYPE}. Recommend Debug build.")
    endif()
    
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)
    
    if(LCOV_PATH AND GENHTML_PATH)
        message(STATUS "✓ Coverage tools found:")
        message(STATUS "    lcov: ${LCOV_PATH}")
        message(STATUS "    genhtml: ${GENHTML_PATH}")
        
        # Full coverage target for all components
        add_custom_target(coverage
            # Zero all counters
            COMMAND ${LCOV_PATH} --directory . --zerocounters
            
            # Run all tests
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            
            # Capture coverage from all component directories
            # Use --base-directory to ensure source files are found correctly
            COMMAND ${LCOV_PATH} --directory . --base-directory ${CMAKE_SOURCE_DIR} --capture --output-file coverage_all.info --rc lcov_branch_coverage=1 --no-external --ignore-errors mismatch
            
            # Remove unwanted files (system headers, test files, mock files)
            COMMAND ${LCOV_PATH} --remove coverage_all.info 
                '/usr/*' 
                '*/usr/include/*'
                '*/test/*' 
                '*/tests/*' 
                '*/mocks/*' 
                '*/mock/*'
                '*/gtest/*'
                '*/gmock/*'
                '*gtest*'
                '*gmock*'
                --output-file coverage.info --rc lcov_branch_coverage=1
            
            # Generate HTML report for all components
            COMMAND ${GENHTML_PATH} -o coverage_html coverage.info 
                --title "L1 Test Suite Coverage - All Components"
                --legend 
                --show-details
                --branch-coverage
            
            # Display summary
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMAND ${CMAKE_COMMAND} -E echo "=========================================="
            COMMAND ${CMAKE_COMMAND} -E echo "Coverage Report Summary:"
            COMMAND ${CMAKE_COMMAND} -E echo "=========================================="
            COMMAND ${LCOV_PATH} --list coverage.info
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMAND ${CMAKE_COMMAND} -E echo "Full report: ${CMAKE_BINARY_DIR}/coverage_html/index.html"
            COMMAND ${CMAKE_COMMAND} -E echo "=========================================="
            
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating comprehensive coverage report for all components"
        )
        
        # Individual component coverage targets
        add_custom_target(coverage-westeros-main
            COMMAND ${LCOV_PATH} --directory L1/westeros-main --zerocounters
            COMMAND ${CMAKE_CTEST_COMMAND} -L "WesterosMain" --output-on-failure || true
            COMMAND ${LCOV_PATH} --directory L1/westeros-main --capture --output-file coverage_westeros_main.info --rc lcov_branch_coverage=1 --ignore-errors mismatch
            COMMAND ${LCOV_PATH} --extract coverage_westeros_main.info '*/westeros-compositor.cpp' '*/westeros-render.cpp' '*/westeros-nested.cpp' --output-file coverage_westeros_main_filtered.info
            COMMAND ${GENHTML_PATH} -o coverage_html_westeros_main coverage_westeros_main_filtered.info --title "Westeros Main Coverage" --legend --branch-coverage
            COMMAND ${LCOV_PATH} --list coverage_westeros_main_filtered.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating coverage report for Westeros Main component"
        )
        
        add_custom_target(coverage-simpleshell
            COMMAND ${LCOV_PATH} --directory L1/simpleshell --zerocounters
            COMMAND ${CMAKE_CTEST_COMMAND} -L "SimpleShell" --output-on-failure || true
            COMMAND ${LCOV_PATH} --directory L1/simpleshell --capture --output-file coverage_simpleshell.info --rc lcov_branch_coverage=1 --ignore-errors mismatch
            COMMAND ${LCOV_PATH} --extract coverage_simpleshell.info '*/westeros-simpleshell.cpp' --output-file coverage_simpleshell_filtered.info
            COMMAND ${GENHTML_PATH} -o coverage_html_simpleshell coverage_simpleshell_filtered.info --title "SimpleShell Coverage" --legend --branch-coverage
            COMMAND ${LCOV_PATH} --list coverage_simpleshell_filtered.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating coverage report for SimpleShell component"
        )
        
        add_custom_target(coverage-simplebuffer
            COMMAND ${LCOV_PATH} --directory L1/simplebuffer --zerocounters
            COMMAND ${CMAKE_CTEST_COMMAND} -L "SimpleBuffer" --output-on-failure || true
            COMMAND ${LCOV_PATH} --directory L1/simplebuffer --capture --output-file coverage_simplebuffer.info --rc lcov_branch_coverage=1 --ignore-errors mismatch
            COMMAND ${LCOV_PATH} --extract coverage_simplebuffer.info '*/westeros-simplebuffer.cpp' --output-file coverage_simplebuffer_filtered.info
            COMMAND ${GENHTML_PATH} -o coverage_html_simplebuffer coverage_simplebuffer_filtered.info --title "SimpleBuffer Coverage" --legend --branch-coverage
            COMMAND ${LCOV_PATH} --list coverage_simplebuffer_filtered.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating coverage report for SimpleBuffer component"
        )
        
        add_custom_target(coverage-linux-dmabuf
            COMMAND ${LCOV_PATH} --directory L1/linux-dmabuf --zerocounters
            COMMAND ${CMAKE_CTEST_COMMAND} -L "LinuxDMABuf" --output-on-failure || true
            COMMAND ${LCOV_PATH} --directory L1/linux-dmabuf --capture --output-file coverage_dmabuf.info --rc lcov_branch_coverage=1 --ignore-errors mismatch
            COMMAND ${LCOV_PATH} --extract coverage_dmabuf.info '*/westeros-linux-dmabuf.cpp' --output-file coverage_dmabuf_filtered.info
            COMMAND ${GENHTML_PATH} -o coverage_html_dmabuf coverage_dmabuf_filtered.info --title "Linux DMA-BUF Coverage" --legend --branch-coverage
            COMMAND ${LCOV_PATH} --list coverage_dmabuf_filtered.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating coverage report for Linux DMA-BUF component"
        )
        
        add_custom_target(coverage-linux-expsync
            COMMAND ${LCOV_PATH} --directory L1/linux-explicit-sync --zerocounters
            COMMAND ${CMAKE_CTEST_COMMAND} -L "LinuxExplicitSync" --output-on-failure || true
            COMMAND ${LCOV_PATH} --directory L1/linux-explicit-sync --capture --output-file coverage_expsync.info --rc lcov_branch_coverage=1 --ignore-errors mismatch
            COMMAND ${LCOV_PATH} --extract coverage_expsync.info '*/westeros-linux-expsync.cpp' --output-file coverage_expsync_filtered.info
            COMMAND ${GENHTML_PATH} -o coverage_html_expsync coverage_expsync_filtered.info --title "Linux Explicit Sync Coverage" --legend --branch-coverage
            COMMAND ${LCOV_PATH} --list coverage_expsync_filtered.info
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating coverage report for Linux Explicit Sync component"
        )
        
        add_custom_target(coverage-report
            COMMAND ${CMAKE_COMMAND} -E echo "Opening coverage report..."
            COMMAND xdg-open coverage_html/index.html || open coverage_html/index.html || start coverage_html/index.html
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS coverage
            COMMENT "Opening coverage report in browser"
        )
    else()
        message(WARNING "✗ Coverage tools not found. Install lcov/genhtml.")
    endif()
endif()

# Common library (shared utilities and test infrastructure)
add_subdirectory(common)

# Components
if(BUILD_WESTEROS_MAIN_TESTS)
    message(STATUS "")
    message(STATUS "Configuring Westeros Main L1 tests...")
    add_subdirectory(L1/westeros-main)
endif()

if(BUILD_SIMPLESHELL_TESTS)
    message(STATUS "")
    message(STATUS "Configuring SimpleShell L1 tests...")
    add_subdirectory(L1/simpleshell)
endif()

if(BUILD_SIMPLEBUFFER_TESTS)
    message(STATUS "")
    message(STATUS "Configuring SimpleBuffer L1 tests...")
    add_subdirectory(L1/simplebuffer)
endif()

if(BUILD_LINUX_DMABUF_TESTS)
    message(STATUS "")
    message(STATUS "Configuring Linux DMA-BUF L1 tests...")
    add_subdirectory(L1/linux-dmabuf)
endif()

if(BUILD_LINUX_EXPSYNC_TESTS)
    message(STATUS "")
    message(STATUS "Configuring Linux Explicit Sync L1 tests...")
    add_subdirectory(L1/linux-explicit-sync)
endif()

# Summary
message(STATUS "")
message(STATUS "========================================")
message(STATUS "Configuration Complete")
message(STATUS "========================================")
message(STATUS "Components to test:")
if(BUILD_WESTEROS_MAIN_TESTS)
    message(STATUS "  ✓ Westeros Main")
endif()
if(BUILD_SIMPLESHELL_TESTS)
    message(STATUS "  ✓ SimpleShell")
endif()
if(BUILD_SIMPLEBUFFER_TESTS)
    message(STATUS "  ✓ SimpleBuffer")
endif()
if(BUILD_LINUX_DMABUF_TESTS)
    message(STATUS "  ✓ Linux DMA-BUF")
endif()
if(BUILD_LINUX_EXPSYNC_TESTS)
    message(STATUS "  ✓ Linux Explicit Sync")
endif()
message(STATUS "========================================")
message(STATUS "")
