# SimpleBuffer Component - L1 Test Suite
# All files (source, test, and mock) are from the current repository

cmake_minimum_required(VERSION 3.14)

message(STATUS "Configuring SimpleBuffer component...")

# Validate paths
if(NOT DEFINED WESTEROS_MAIN_SRC_ROOT)
    message(FATAL_ERROR "WESTEROS_MAIN_SRC_ROOT must be defined")
endif()

# Test files are in the current repository
set(SIMPLEBUFFER_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/L1")
set(SIMPLEBUFFER_MOCK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks")

# Source files - from local repository
set(SIMPLEBUFFER_SOURCES
    ${WESTEROS_MAIN_SRC_ROOT}/simplebuffer/westeros-simplebuffer.cpp
)

# Header files to ensure they're visible
set(SIMPLEBUFFER_HEADERS
    ${WESTEROS_MAIN_SRC_ROOT}/simplebuffer/westeros-simplebuffer.h
)

# Verify source file exists
if(NOT EXISTS "${WESTEROS_MAIN_SRC_ROOT}/simplebuffer/westeros-simplebuffer.cpp")
    message(FATAL_ERROR "SimpleBuffer source file not found: ${WESTEROS_MAIN_SRC_ROOT}/simplebuffer/westeros-simplebuffer.cpp")
endif()

# Protocol files - use pre-existing headers from source
set(SIMPLEBUFFER_PROTOCOL_DIR "${WESTEROS_MAIN_SRC_ROOT}/simplebuffer/protocol")

# No need to generate - headers already exist in source
set(SIMPLEBUFFER_PROTOCOL_SOURCES "")

# Test files - from local repository
set(SIMPLEBUFFER_TEST_SOURCES
    ${SIMPLEBUFFER_TEST_DIR}/westeros_simplebuffer_test.cpp
)

# Mock files - from local repository
set(SIMPLEBUFFER_MOCK_SOURCES
    ${SIMPLEBUFFER_MOCK_DIR}/wayland_mock.cpp
)

# Tests
if(GTest_Found OR TARGET GTest::gtest)
    # SimpleBuffer L1 Tests - compile source directly into executable
    add_executable(test_simplebuffer_l1
        ${SIMPLEBUFFER_TEST_SOURCES}
        ${SIMPLEBUFFER_SOURCES}
        ${SIMPLEBUFFER_HEADERS}
        ${SIMPLEBUFFER_MOCK_SOURCES}
        ${SIMPLEBUFFER_PROTOCOL_SOURCES}
    )
    
    target_include_directories(test_simplebuffer_l1 PRIVATE
        ${WESTEROS_MAIN_SRC_ROOT}/simplebuffer       # Source directory FIRST (for westeros-simplebuffer.h)
        ${WESTEROS_MAIN_SRC_ROOT}                    # Root directory SECOND  
        ${SIMPLEBUFFER_PROTOCOL_DIR}                 # Pre-existing protocol headers from source
        ${WESTEROS_MAIN_SRC_ROOT}/simplebuffer/protocol  # Same as above (explicit)
        ${SIMPLEBUFFER_MOCK_DIR}                     # Mock headers from local repo
    )
    
    target_link_libraries(test_simplebuffer_l1
        GTest::gtest
        GTest::gmock
        GTest::gtest_main
        ${CMAKE_DL_LIBS}
    )
    
    # Add UNIT_TEST definition to make static functions testable
    target_compile_definitions(test_simplebuffer_l1 PRIVATE
        UNIT_TEST
    )
    
    # Enable coverage if requested
    if(ENABLE_COVERAGE)
        target_compile_options(test_simplebuffer_l1 PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(test_simplebuffer_l1 PRIVATE --coverage)
        target_link_libraries(test_simplebuffer_l1 gcov)
        message(STATUS "    [COVERAGE] Enabled for simplebuffer with flags: --coverage -fprofile-arcs -ftest-coverage + gcov")
    endif()
    
    # Register test
    add_test(NAME SimpleBufferL1Tests COMMAND test_simplebuffer_l1)
    set_tests_properties(SimpleBufferL1Tests PROPERTIES
        TIMEOUT 300
        LABELS "L1;SimpleBuffer;Component"
    )
    
    message(STATUS "  ✓ SimpleBuffer L1 tests configured")
    message(STATUS "    Source: ${SIMPLEBUFFER_SOURCES}")
    message(STATUS "    Tests:  ${SIMPLEBUFFER_TEST_SOURCES}")
    message(STATUS "    Mocks:  ${SIMPLEBUFFER_MOCK_SOURCES}")
    message(STATUS "    Tests:  ${SIMPLEBUFFER_TEST_DIR}/westeros_simplebuffer_test.cpp")
endif()

message(STATUS "  ✓ SimpleBuffer component configured")
