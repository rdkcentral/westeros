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

# Verify source file exists
if(NOT EXISTS "${WESTEROS_MAIN_SRC_ROOT}/simpleshell/westeros-simpleshell.cpp")
    message(FATAL_ERROR "SimpleShell source file not found: ${WESTEROS_MAIN_SRC_ROOT}/simpleshell/westeros-simpleshell.cpp")
endif()

# Generate protocol files from XML
find_program(WAYLAND_SCANNER wayland-scanner)
if(WAYLAND_SCANNER)
    set(SIMPLESHELL_PROTOCOL_DIR "${WESTEROS_MAIN_SRC_ROOT}/simpleshell/protocol")
    set(SIMPLESHELL_PROTOCOL_XML "${SIMPLESHELL_PROTOCOL_DIR}/simpleshell.xml")
    set(SIMPLESHELL_PROTOCOL_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/protocol")
    set(SIMPLESHELL_PROTOCOL_GEN_SUBDIR "${SIMPLESHELL_PROTOCOL_GEN_DIR}/simpleshell/protocol")
    
    file(MAKE_DIRECTORY ${SIMPLESHELL_PROTOCOL_GEN_DIR})
    file(MAKE_DIRECTORY ${SIMPLESHELL_PROTOCOL_GEN_SUBDIR})
    
    # Generate server header in both locations
    add_custom_command(
        OUTPUT ${SIMPLESHELL_PROTOCOL_GEN_DIR}/simpleshell-server-protocol.h
        COMMAND ${WAYLAND_SCANNER} server-header ${SIMPLESHELL_PROTOCOL_XML} ${SIMPLESHELL_PROTOCOL_GEN_DIR}/simpleshell-server-protocol.h
        DEPENDS ${SIMPLESHELL_PROTOCOL_XML}
        COMMENT "Generating simpleshell server protocol header (root)"
    )
    
    add_custom_command(
        OUTPUT ${SIMPLESHELL_PROTOCOL_GEN_SUBDIR}/simpleshell-server-protocol.h
        COMMAND ${WAYLAND_SCANNER} server-header ${SIMPLESHELL_PROTOCOL_XML} ${SIMPLESHELL_PROTOCOL_GEN_SUBDIR}/simpleshell-server-protocol.h
        DEPENDS ${SIMPLESHELL_PROTOCOL_XML}
        COMMENT "Generating simpleshell server protocol header (subdir)"
    )
    
    # Generate client header
    add_custom_command(
        OUTPUT ${SIMPLESHELL_PROTOCOL_GEN_DIR}/simpleshell-client-protocol.h
        COMMAND ${WAYLAND_SCANNER} client-header ${SIMPLESHELL_PROTOCOL_XML} ${SIMPLESHELL_PROTOCOL_GEN_DIR}/simpleshell-client-protocol.h
        DEPENDS ${SIMPLESHELL_PROTOCOL_XML}
        COMMENT "Generating simpleshell client protocol header"
    )
    
    # Generate protocol code
    add_custom_command(
        OUTPUT ${SIMPLESHELL_PROTOCOL_GEN_DIR}/simpleshell-protocol.c
        COMMAND ${WAYLAND_SCANNER} private-code ${SIMPLESHELL_PROTOCOL_XML} ${SIMPLESHELL_PROTOCOL_GEN_DIR}/simpleshell-protocol.c
        DEPENDS ${SIMPLESHELL_PROTOCOL_XML}
        COMMENT "Generating simpleshell protocol code"
    )
    
    set(SIMPLESHELL_PROTOCOL_SOURCES
        ${SIMPLESHELL_PROTOCOL_GEN_DIR}/simpleshell-protocol.c
        ${SIMPLESHELL_PROTOCOL_GEN_DIR}/simpleshell-server-protocol.h
        ${SIMPLESHELL_PROTOCOL_GEN_SUBDIR}/simpleshell-server-protocol.h
        ${SIMPLESHELL_PROTOCOL_GEN_DIR}/simpleshell-client-protocol.h
    )
    
    message(STATUS "  ✓ Wayland scanner found: ${WAYLAND_SCANNER}")
    message(STATUS "  ✓ Protocol generation configured")
else()
    message(WARNING "  ✗ wayland-scanner not found - protocol files may be missing")
    set(SIMPLESHELL_PROTOCOL_SOURCES "")
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
    # SimpleShell L1 Tests - compile source directly into executable
    add_executable(test_simpleshell_l1
        ${SIMPLESHELL_TEST_SOURCES}
        ${SIMPLESHELL_SOURCES}
        ${SIMPLESHELL_MOCK_SOURCES}
        ${SIMPLESHELL_PROTOCOL_SOURCES}
    )
    
    target_include_directories(test_simpleshell_l1 PRIVATE
        ${WESTEROS_MAIN_SRC_ROOT}                    # Root for westeros-simpleshell.h
        ${WESTEROS_MAIN_SRC_ROOT}/simpleshell        # Source directory from local repo
        ${WESTEROS_MAIN_SRC_ROOT}/simpleshell/protocol  # Protocol XML directory
        ${SIMPLESHELL_PROTOCOL_GEN_DIR}              # Generated protocol headers (root level)
        ${CMAKE_CURRENT_BINARY_DIR}                  # For nested protocol includes
        ${WESTEROS_MAIN_SRC_ROOT}/simplebuffer       # SimpleBuffer headers from local repo
        ${WESTEROS_MAIN_SRC_ROOT}/simplebuffer/protocol  # SimpleBuffer protocol
        ${SIMPLESHELL_MOCK_DIR}                      # Mock headers from local repo
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
