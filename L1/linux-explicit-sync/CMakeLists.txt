# Linux Explicit Sync Component - L1 Test Suite
# All files (source, test, and mock) are from the current repository

cmake_minimum_required(VERSION 3.14)

message(STATUS "Configuring Linux Explicit Sync component...")

# Validate paths
if(NOT DEFINED WESTEROS_MAIN_SRC_ROOT)
    message(FATAL_ERROR "WESTEROS_MAIN_SRC_ROOT must be defined")
endif()

# Test files are in the current repository
set(EXPSYNC_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/L1")
set(EXPSYNC_MOCK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks")

# Source files - from local repository
set(LINUX_EXPSYNC_SOURCES
    ${WESTEROS_MAIN_SRC_ROOT}/linux-expsync/westeros-linux-expsync.cpp
)

# Test files - from local repository
set(LINUX_EXPSYNC_TEST_SOURCES
    ${EXPSYNC_TEST_DIR}/test_linux_expsync_l1.cpp
)

# Mock files - from local repository (C files)
set(LINUX_EXPSYNC_MOCK_SOURCES
    ${EXPSYNC_MOCK_DIR}/wayland-stubs.c
    ${EXPSYNC_MOCK_DIR}/sync-file-stubs.c
    ${EXPSYNC_MOCK_DIR}/westeros-context-mock.c
    ${EXPSYNC_MOCK_DIR}/linux-explicit-synchronization-protocol.c
)

# Tests
if(GTest_FOUND OR TARGET GTest::gtest)
    # Linux Explicit Sync L1 Tests
    add_executable(test_linux_expsync_l1
        ${LINUX_EXPSYNC_TEST_SOURCES}
        ${LINUX_EXPSYNC_SOURCES}
        ${LINUX_EXPSYNC_MOCK_SOURCES}
    )
    
    target_include_directories(test_linux_expsync_l1 PRIVATE
        ${WESTEROS_MAIN_SRC_ROOT}/linux-expsync      # Source directory from local repo
        ${EXPSYNC_MOCK_DIR}                          # Mock headers from local repo
        ${WESTEROS_MAIN_SRC_ROOT}                    # For westeros-compositor.h and westeros-render.h
    )
    
    target_link_libraries(test_linux_expsync_l1
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        ${CMAKE_DL_LIBS}
    )
    
    # Platform-specific settings
    if(UNIX AND NOT APPLE)
        # Ensure no Wayland linking and set Linux platform
        target_compile_definitions(test_linux_expsync_l1 PRIVATE
            WESTEROS_TEST_MODE=1
            LINUX_PLATFORM
            UNIT_TEST
        )
        
        # Link-time wrapping for system calls (Linux only)
        target_link_options(test_linux_expsync_l1 PRIVATE
            -Wl,--wrap=ioctl
            -Wl,--wrap=close
        )
        message(STATUS "    ✓ Linux platform detected - linker wrapping enabled for ioctl and close")
    elseif(WIN32)
        # Windows-specific settings
        target_compile_definitions(test_linux_expsync_l1 PRIVATE
            WESTEROS_TEST_MODE=1
            WINDOWS_PLATFORM
            UNIT_TEST
        )
        message(STATUS "    ✓ Windows platform detected - using direct mock functions")
    else()
        # Ensure no Wayland linking
        target_compile_definitions(test_linux_expsync_l1 PRIVATE
            WESTEROS_TEST_MODE=1
            UNIT_TEST
        )
    endif()
    
    # Enable coverage if requested
    if(ENABLE_COVERAGE)
        target_compile_options(test_linux_expsync_l1 PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(test_linux_expsync_l1 PRIVATE --coverage)
    endif()
    
    # Register test
    add_test(NAME LinuxExplicitSyncL1Tests COMMAND test_linux_expsync_l1)
    set_tests_properties(LinuxExplicitSyncL1Tests PROPERTIES
        TIMEOUT 300
        LABELS "L1;LinuxExplicitSync;Component"
    )
    
    message(STATUS "  ✓ Linux Explicit Sync L1 tests configured")
    message(STATUS "    Source: ${WESTEROS_MAIN_SRC_ROOT}/linux-expsync/westeros-linux-expsync.cpp")
    message(STATUS "    Tests:  ${EXPSYNC_TEST_DIR}/test_linux_expsync_l1.cpp")
endif()

message(STATUS "  ✓ Linux Explicit Sync component configured")
