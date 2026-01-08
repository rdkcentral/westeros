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

# Generate protocol files from XML
find_program(WAYLAND_SCANNER wayland-scanner)
if(WAYLAND_SCANNER)
    set(SIMPLEBUFFER_PROTOCOL_DIR "${WESTEROS_MAIN_SRC_ROOT}/simplebuffer/protocol")
    set(SIMPLEBUFFER_PROTOCOL_XML "${SIMPLEBUFFER_PROTOCOL_DIR}/simple-buffer.xml")
    set(SIMPLEBUFFER_PROTOCOL_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/protocol")
    set(SIMPLEBUFFER_PROTOCOL_GEN_NESTED "${CMAKE_CURRENT_BINARY_DIR}/simplebuffer/protocol")
    
    file(MAKE_DIRECTORY ${SIMPLEBUFFER_PROTOCOL_GEN_DIR})
    file(MAKE_DIRECTORY ${SIMPLEBUFFER_PROTOCOL_GEN_NESTED})
    
    # Generate server header in both locations
    add_custom_command(
        OUTPUT ${SIMPLEBUFFER_PROTOCOL_GEN_DIR}/simplebuffer-server-protocol.h
        COMMAND ${WAYLAND_SCANNER} server-header ${SIMPLEBUFFER_PROTOCOL_XML} ${SIMPLEBUFFER_PROTOCOL_GEN_DIR}/simplebuffer-server-protocol.h
        DEPENDS ${SIMPLEBUFFER_PROTOCOL_XML}
        COMMENT "Generating simplebuffer server protocol header (root)"
    )
    
    add_custom_command(
        OUTPUT ${SIMPLEBUFFER_PROTOCOL_GEN_NESTED}/simplebuffer-server-protocol.h
        COMMAND ${WAYLAND_SCANNER} server-header ${SIMPLEBUFFER_PROTOCOL_XML} ${SIMPLEBUFFER_PROTOCOL_GEN_NESTED}/simplebuffer-server-protocol.h
        DEPENDS ${SIMPLEBUFFER_PROTOCOL_XML}
        COMMENT "Generating simplebuffer server protocol header (nested)"
    )
    
    # Generate client header
    add_custom_command(
        OUTPUT ${SIMPLEBUFFER_PROTOCOL_GEN_DIR}/simplebuffer-client-protocol.h
        COMMAND ${WAYLAND_SCANNER} client-header ${SIMPLEBUFFER_PROTOCOL_XML} ${SIMPLEBUFFER_PROTOCOL_GEN_DIR}/simplebuffer-client-protocol.h
        DEPENDS ${SIMPLEBUFFER_PROTOCOL_XML}
        COMMENT "Generating simplebuffer client protocol header"
    )
    
    # Generate protocol code
    add_custom_command(
        OUTPUT ${SIMPLEBUFFER_PROTOCOL_GEN_DIR}/simplebuffer-protocol.c
        COMMAND ${WAYLAND_SCANNER} private-code ${SIMPLEBUFFER_PROTOCOL_XML} ${SIMPLEBUFFER_PROTOCOL_GEN_DIR}/simplebuffer-protocol.c
        DEPENDS ${SIMPLEBUFFER_PROTOCOL_XML}
        COMMENT "Generating simplebuffer protocol code"
    )
    
    set(SIMPLEBUFFER_PROTOCOL_SOURCES
        ${SIMPLEBUFFER_PROTOCOL_GEN_DIR}/simplebuffer-protocol.c
        ${SIMPLEBUFFER_PROTOCOL_GEN_DIR}/simplebuffer-server-protocol.h
        ${SIMPLEBUFFER_PROTOCOL_GEN_NESTED}/simplebuffer-server-protocol.h
        ${SIMPLEBUFFER_PROTOCOL_GEN_DIR}/simplebuffer-client-protocol.h
    )
    
    message(STATUS "  ✓ Wayland scanner found: ${WAYLAND_SCANNER}")
    message(STATUS "  ✓ Protocol generation configured")
else()
    message(WARNING "  ✗ wayland-scanner not found - protocol files may be missing")
    set(SIMPLEBUFFER_PROTOCOL_SOURCES "")
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
        ${SIMPLEBUFFER_PROTOCOL_GEN_DIR}             # Generated protocol headers (root level)
        ${CMAKE_CURRENT_BINARY_DIR}                  # For nested protocol includes (simplebuffer/protocol/...)
        ${WESTEROS_MAIN_SRC_ROOT}/simplebuffer/protocol  # Protocol XML directory
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
