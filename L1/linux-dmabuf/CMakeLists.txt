# Linux DMA-BUF Component - L1 Test Suite
# All files (source, test, and mock) are from the current repository

cmake_minimum_required(VERSION 3.14)

message(STATUS "Configuring Linux DMA-BUF component...")

# Validate paths
if(NOT DEFINED WESTEROS_MAIN_SRC_ROOT)
    message(FATAL_ERROR "WESTEROS_MAIN_SRC_ROOT must be defined")
endif()

# Test files are in the current repository
set(DMABUF_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/L1")
set(DMABUF_MOCK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks")

# Source files - from local repository
set(LINUX_DMABUF_SOURCES
    ${WESTEROS_MAIN_SRC_ROOT}/linux-dmabuf/westeros-linux-dmabuf.cpp
)

# Verify source file exists
if(NOT EXISTS "${WESTEROS_MAIN_SRC_ROOT}/linux-dmabuf/westeros-linux-dmabuf.cpp")
    message(FATAL_ERROR "Linux DMA-BUF source file not found: ${WESTEROS_MAIN_SRC_ROOT}/linux-dmabuf/westeros-linux-dmabuf.cpp")
endif()

# Test files - from local repository
set(LINUX_DMABUF_TEST_SOURCES
    ${DMABUF_TEST_DIR}/test_linux_dmabuf_l1.cpp
)

# Mock files - from local repository
set(LINUX_DMABUF_MOCK_SOURCES
    ${DMABUF_MOCK_DIR}/wayland-server-mock.cpp
    ${DMABUF_MOCK_DIR}/westeros-render-mock.cpp
)

# Tests
if(GTest_Found OR TARGET GTest::gtest)
    # Linux DMABuf L1 Tests - compile source directly into executable
    add_executable(test_linux_dmabuf_l1
        ${LINUX_DMABUF_TEST_SOURCES}
        ${LINUX_DMABUF_SOURCES}
        ${LINUX_DMABUF_MOCK_SOURCES}
    )
    
    target_include_directories(test_linux_dmabuf_l1 PRIVATE
        ${WESTEROS_MAIN_SRC_ROOT}/linux-dmabuf       # Source directory from local repo
        ${DMABUF_MOCK_DIR}                           # Mock headers from local repo
        ${WESTEROS_MAIN_SRC_ROOT}                    # For westeros-compositor.h and westeros-render.h
    )
    
    target_link_libraries(test_linux_dmabuf_l1
        GTest::gtest
        GTest::gmock
        GTest::gtest_main
        ${CMAKE_DL_LIBS}
    )
    
    # Add UNIT_TEST definition to make static functions testable
    target_compile_definitions(test_linux_dmabuf_l1 PRIVATE
        UNIT_TEST
        WESTEROS_TEST_MODE=1
    )
    
    # Enable coverage if requested
    if(ENABLE_COVERAGE)
        target_compile_options(test_linux_dmabuf_l1 PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(test_linux_dmabuf_l1 PRIVATE --coverage)
        target_link_libraries(test_linux_dmabuf_l1 gcov)
        message(STATUS "    [COVERAGE] Enabled for linux-dmabuf with flags: --coverage -fprofile-arcs -ftest-coverage + gcov")
    endif()
    
    # Register test
    add_test(NAME LinuxDMABufL1Tests COMMAND test_linux_dmabuf_l1)
    set_tests_properties(LinuxDMABufL1Tests PROPERTIES
        TIMEOUT 300
        LABELS "L1;LinuxDMABuf;Component"
    )
    
    message(STATUS "  ✓ Linux DMA-BUF L1 tests configured")
    message(STATUS "    Source: ${LINUX_DMABUF_SOURCES}")
    message(STATUS "    Tests:  ${LINUX_DMABUF_TEST_SOURCES}")
    message(STATUS "    Mocks:  ${LINUX_DMABUF_MOCK_SOURCES}")
    message(STATUS "    Tests:  ${DMABUF_TEST_DIR}/test_linux_dmabuf_l1.cpp")
endif()

message(STATUS "  ✓ Linux DMA-BUF component configured")
