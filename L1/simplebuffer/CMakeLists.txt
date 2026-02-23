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

# Protocol generation using wayland-scanner
# Source XML location
set(SIMPLEBUFFER_PROTOCOL_XML "${WESTEROS_MAIN_SRC_ROOT}/simplebuffer/protocol/simple-buffer.xml")

# Generated files go to build directory (not source tree)
# Generate to simplebuffer/protocol/ to match the include path in source files
set(SIMPLEBUFFER_PROTOCOL_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/simplebuffer/protocol")
set(SIMPLEBUFFER_SERVER_HEADER "${SIMPLEBUFFER_PROTOCOL_BUILD_DIR}/simplebuffer-server-protocol.h")

if(WAYLAND_SCANNER)
    # Create protocol output directory
    file(MAKE_DIRECTORY "${SIMPLEBUFFER_PROTOCOL_BUILD_DIR}")
    
    # Generate only the server header (the only one actually used by tests)
    add_custom_command(
        OUTPUT "${SIMPLEBUFFER_SERVER_HEADER}"
        COMMAND ${WAYLAND_SCANNER} server-header "${SIMPLEBUFFER_PROTOCOL_XML}" "${SIMPLEBUFFER_SERVER_HEADER}"
        DEPENDS "${SIMPLEBUFFER_PROTOCOL_XML}"
        COMMENT "Generating simplebuffer server protocol header"
    )
    
    message(STATUS "  ✓ Protocol generation configured for SimpleBuffer (output: ${SIMPLEBUFFER_PROTOCOL_BUILD_DIR})")
else()
    message(STATUS "  ⚠ wayland-scanner not found - using pre-generated protocol headers")
    # Fallback to source tree headers if scanner not available
    set(SIMPLEBUFFER_SERVER_HEADER "${WESTEROS_MAIN_SRC_ROOT}/simplebuffer/protocol/simplebuffer-server-protocol.h")
endif()

# Source files - from local repository
set(SIMPLEBUFFER_SOURCES
    ${WESTEROS_MAIN_SRC_ROOT}/simplebuffer/westeros-simplebuffer.cpp
)

# Verify source file exists
if(NOT EXISTS "${WESTEROS_MAIN_SRC_ROOT}/simplebuffer/westeros-simplebuffer.cpp")
    message(FATAL_ERROR "SimpleBuffer source file not found: ${WESTEROS_MAIN_SRC_ROOT}/simplebuffer/westeros-simplebuffer.cpp")
endif()

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
    # List of sources for the test executable
    set(TEST_EXEC_SOURCES
        ${SIMPLEBUFFER_TEST_SOURCES}
        ${SIMPLEBUFFER_SOURCES}
        ${SIMPLEBUFFER_MOCK_SOURCES}
    )
    
    # Add generated headers as sources if wayland-scanner is available
    if(WAYLAND_SCANNER)
        list(APPEND TEST_EXEC_SOURCES "${SIMPLEBUFFER_SERVER_HEADER}")
    endif()
    
    # SimpleBuffer L1 Tests - compile source directly into executable
    add_executable(test_simplebuffer_l1 ${TEST_EXEC_SOURCES})
    
    # Mark generated files as generated so CMake knows to create them first
    if(WAYLAND_SCANNER)
        set_source_files_properties(
            "${SIMPLEBUFFER_SERVER_HEADER}"
            PROPERTIES GENERATED TRUE
        )
    endif()
    
    target_include_directories(test_simplebuffer_l1 PRIVATE
        ${WESTEROS_MAIN_SRC_ROOT}/simplebuffer       # Source directory from local repo
        ${SIMPLEBUFFER_MOCK_DIR}                     # Mock headers from local repo
        ${WESTEROS_MAIN_SRC_ROOT}                    # For any westeros-compositor.h dependencies
    )
    
    # Add protocol header directory (build dir if generated, source dir if fallback)
    if(WAYLAND_SCANNER)
        # Add both the build root (for source/test files) and the specific protocol dir
        target_include_directories(test_simplebuffer_l1 PRIVATE 
            ${CMAKE_CURRENT_BINARY_DIR}
            ${SIMPLEBUFFER_PROTOCOL_BUILD_DIR}
        )
    else()
        target_include_directories(test_simplebuffer_l1 PRIVATE ${WESTEROS_MAIN_SRC_ROOT}/simplebuffer/protocol)
    endif()
    
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
