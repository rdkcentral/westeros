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

# Protocol generation using wayland-scanner
# Source XML location
set(SIMPLESHELL_PROTOCOL_XML "${WESTEROS_MAIN_SRC_ROOT}/simpleshell/protocol/simpleshell.xml")

# Generated files go to build directory (not source tree)
# Generate to simpleshell/protocol/ to match the include path in source files
set(SIMPLESHELL_PROTOCOL_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/simpleshell/protocol")
set(SIMPLESHELL_SERVER_HEADER "${SIMPLESHELL_PROTOCOL_BUILD_DIR}/simpleshell-server-protocol.h")

if(WAYLAND_SCANNER)
    # Create protocol output directory
    file(MAKE_DIRECTORY "${SIMPLESHELL_PROTOCOL_BUILD_DIR}")
    
    # Generate only the server header (the only one actually used by tests)
    add_custom_command(
        OUTPUT "${SIMPLESHELL_SERVER_HEADER}"
        COMMAND ${WAYLAND_SCANNER} server-header "${SIMPLESHELL_PROTOCOL_XML}" "${SIMPLESHELL_SERVER_HEADER}"
        DEPENDS "${SIMPLESHELL_PROTOCOL_XML}"
        COMMENT "Generating simpleshell server protocol header"
    )
    
    message(STATUS "  ✓ Protocol generation configured for SimpleShell (output: ${SIMPLESHELL_PROTOCOL_BUILD_DIR})")
else()
    message(STATUS "  ⚠ wayland-scanner not found - using pre-generated protocol headers")
    # Fallback to source tree headers if scanner not available
    set(SIMPLESHELL_SERVER_HEADER "${WESTEROS_MAIN_SRC_ROOT}/simpleshell/protocol/simpleshell-server-protocol.h")
endif()

# SimpleBuffer protocol - needed by mocks
set(SIMPLEBUFFER_PROTOCOL_XML "${WESTEROS_MAIN_SRC_ROOT}/simplebuffer/protocol/simple-buffer.xml")
set(SIMPLEBUFFER_PROTOCOL_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/simplebuffer/protocol")
set(SIMPLEBUFFER_SERVER_HEADER "${SIMPLEBUFFER_PROTOCOL_BUILD_DIR}/simplebuffer-server-protocol.h")

if(WAYLAND_SCANNER)
    file(MAKE_DIRECTORY "${SIMPLEBUFFER_PROTOCOL_BUILD_DIR}")
    
    add_custom_command(
        OUTPUT "${SIMPLEBUFFER_SERVER_HEADER}"
        COMMAND ${WAYLAND_SCANNER} server-header "${SIMPLEBUFFER_PROTOCOL_XML}" "${SIMPLEBUFFER_SERVER_HEADER}"
        DEPENDS "${SIMPLEBUFFER_PROTOCOL_XML}"
        COMMENT "Generating simplebuffer server protocol header (for mocks)"
    )
    
    message(STATUS "  ✓ Protocol generation configured for SimpleBuffer (output: ${SIMPLEBUFFER_PROTOCOL_BUILD_DIR})")
else()
    set(SIMPLEBUFFER_SERVER_HEADER "${WESTEROS_MAIN_SRC_ROOT}/simplebuffer/protocol/simplebuffer-server-protocol.h")
endif()

# Source files - from local L1-TestSuiteWesteros repository
set(SIMPLESHELL_SOURCES
    ${WESTEROS_MAIN_SRC_ROOT}/simpleshell/westeros-simpleshell.cpp
)

# Verify source file exists
if(NOT EXISTS "${WESTEROS_MAIN_SRC_ROOT}/simpleshell/westeros-simpleshell.cpp")
    message(FATAL_ERROR "SimpleShell source file not found: ${WESTEROS_MAIN_SRC_ROOT}/simpleshell/westeros-simpleshell.cpp")
endif()

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
    # List of sources for the test executable
    set(TEST_EXEC_SOURCES
        ${SIMPLESHELL_TEST_SOURCES}
        ${SIMPLESHELL_SOURCES}
        ${SIMPLESHELL_MOCK_SOURCES}
    )
    
    # Add generated headers as sources if wayland-scanner is available
    if(WAYLAND_SCANNER)
        list(APPEND TEST_EXEC_SOURCES "${SIMPLESHELL_SERVER_HEADER}")
        list(APPEND TEST_EXEC_SOURCES "${SIMPLEBUFFER_SERVER_HEADER}")
    endif()
    
    # SimpleShell L1 Tests - compile source directly into executable
    add_executable(test_simpleshell_l1 ${TEST_EXEC_SOURCES})
    
    # Mark generated files as generated so CMake knows to create them first
    if(WAYLAND_SCANNER)
        set_source_files_properties(
            "${SIMPLESHELL_SERVER_HEADER}"
            "${SIMPLEBUFFER_SERVER_HEADER}"
            PROPERTIES GENERATED TRUE
        )
    endif()
    
    target_include_directories(test_simpleshell_l1 PRIVATE
        ${WESTEROS_MAIN_SRC_ROOT}/simpleshell        # Source directory from local repo
        ${WESTEROS_MAIN_SRC_ROOT}/simplebuffer       # SimpleBuffer headers from local repo
        ${SIMPLESHELL_MOCK_DIR}                      # Mock headers from local repo
        ${WESTEROS_MAIN_SRC_ROOT}                    # For any westeros-compositor.h dependencies
    )
    
    # Add protocol header directory (build dir if generated, source dir if fallback)
    if(WAYLAND_SCANNER)
        # Add both the build root (for source files) and the specific protocol dir (for test files)
        target_include_directories(test_simpleshell_l1 PRIVATE 
            ${CMAKE_CURRENT_BINARY_DIR}
            ${SIMPLESHELL_PROTOCOL_BUILD_DIR}
        )
    else()
        target_include_directories(test_simpleshell_l1 PRIVATE ${WESTEROS_MAIN_SRC_ROOT}/simpleshell/protocol)
    endif()
    
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
