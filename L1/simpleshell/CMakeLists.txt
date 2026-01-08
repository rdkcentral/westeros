# SimpleShell Component - L1 Test Suite
# All files (source, test, and mock) are from the current repository

cmake_minimum_required(VERSION 3.14)

message(STATUS "Configuring SimpleShell component...")

# Validate source root path
if(NOT DEFINED WESTEROS_MAIN_SRC_ROOT)
    message(FATAL_ERROR "WESTEROS_MAIN_SRC_ROOT must be defined (path to local westeros source)")
endif()

# Test and mock files are in the current repository
set(SIMPLESHELL_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/L1")
set(SIMPLESHELL_MOCK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks")

# Source files - from local L1-TestSuiteWesteros repository
set(SIMPLESHELL_SOURCES
    ${WESTEROS_MAIN_SRC_ROOT}/simpleshell/westeros-simpleshell.cpp
)

# Header files to ensure they're visible
set(SIMPLESHELL_HEADERS
    ${WESTEROS_MAIN_SRC_ROOT}/simpleshell/westeros-simpleshell.h
)

# Verify source file exists
if(NOT EXISTS "${WESTEROS_MAIN_SRC_ROOT}/simpleshell/westeros-simpleshell.cpp")
    message(FATAL_ERROR "SimpleShell source file not found: ${WESTEROS_MAIN_SRC_ROOT}/simpleshell/westeros-simpleshell.cpp")
endif()

# Protocol files - use pre-existing headers from source
set(SIMPLESHELL_PROTOCOL_DIR "${WESTEROS_MAIN_SRC_ROOT}/simpleshell/protocol")

# No need to generate - headers already exist in source
set(SIMPLESHELL_PROTOCOL_SOURCES "")

# Test files - from local repository
set(SIMPLESHELL_TEST_SOURCES
    ${SIMPLESHELL_TEST_DIR}/westeros_simpleshell_test.cpp
)

# Mock files - from local repository
set(SIMPLESHELL_MOCK_SOURCES
    ${SIMPLESHELL_MOCK_DIR}/wayland_mock.cpp
)

# Tests
if(GTest_Found OR TARGET GTest::gtest)
    # SimpleShell L1 Tests - compile source directly into executable
    add_executable(test_simpleshell_l1
        ${SIMPLESHELL_TEST_SOURCES}
        ${SIMPLESHELL_SOURCES}
        ${SIMPLESHELL_HEADERS}
        ${SIMPLESHELL_MOCK_SOURCES}
        ${SIMPLESHELL_PROTOCOL_SOURCES}
    )
    
    target_include_directories(test_simpleshell_l1 PRIVATE
        ${WESTEROS_MAIN_SRC_ROOT}/simpleshell        # For westeros-simpleshell.h
        ${SIMPLESHELL_PROTOCOL_DIR}                  # For "simpleshell-server-protocol.h" (flat include)
        ${WESTEROS_MAIN_SRC_ROOT}                    # For "simpleshell/protocol/..." and "simplebuffer/protocol/..." (nested includes)
        ${SIMPLESHELL_MOCK_DIR}                      # Mock headers
    )
    
    target_link_libraries(test_simpleshell_l1
        GTest::gtest
        GTest::gmock
        GTest::gtest_main
        ${CMAKE_DL_LIBS}
    )
    
    # Add UNIT_TEST definition to make static functions testable
    target_compile_definitions(test_simpleshell_l1 PRIVATE
        UNIT_TEST
    )
    
    # Enable coverage if requested
    if(ENABLE_COVERAGE)
        target_compile_options(test_simpleshell_l1 PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(test_simpleshell_l1 PRIVATE --coverage)
        target_link_libraries(test_simpleshell_l1 gcov)
        message(STATUS "    [COVERAGE] Enabled for simpleshell with flags: --coverage -fprofile-arcs -ftest-coverage + gcov")
    endif()
    
    # Register test
    add_test(NAME SimpleShellL1Tests COMMAND test_simpleshell_l1)
    set_tests_properties(SimpleShellL1Tests PROPERTIES
        TIMEOUT 300
        LABELS "L1;SimpleShell;Component"
    )
    
    message(STATUS "  ✓ SimpleShell L1 tests configured")
    message(STATUS "    Source: ${SIMPLESHELL_SOURCES}")
    message(STATUS "    Tests:  ${SIMPLESHELL_TEST_SOURCES}")
    message(STATUS "    Mocks:  ${SIMPLESHELL_MOCK_SOURCES}")
    message(STATUS "    Tests:  ${SIMPLESHELL_TEST_DIR}/westeros_simpleshell_test.cpp")
    message(STATUS "    Mocks:  ${SIMPLESHELL_MOCK_DIR}/")
endif()

message(STATUS "  ✓ SimpleShell component configured")
