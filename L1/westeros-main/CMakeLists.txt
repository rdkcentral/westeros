# Westeros Main L1 Test Suite
# All files (source, test, and mock) are from the current repository

cmake_minimum_required(VERSION 3.14)

message(STATUS "Configuring Westeros Main component...")

# Validate paths
if(NOT DEFINED WESTEROS_MAIN_SRC_ROOT)
    message(FATAL_ERROR "WESTEROS_MAIN_SRC_ROOT must be defined")
endif()

# Test and mock files are in the current repository
set(WESTEROS_MAIN_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/L1")
set(WESTEROS_MAIN_MOCK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks")

# Note: WESTEROS_MAIN_SRC_ROOT is already resolved as absolute path in parent CMakeLists.txt
# Validation will occur during add_executable when source files are used

# Source files from local repository
set(WESTEROS_MAIN_SOURCES
    ${WESTEROS_MAIN_SRC_ROOT}/westeros-compositor.cpp
    ${WESTEROS_MAIN_SRC_ROOT}/westeros-render.cpp
    ${WESTEROS_MAIN_SRC_ROOT}/westeros-nested.cpp
)

# L1 Test executables
if(GTest_FOUND OR GTEST_FOUND)
    # Westeros Compositor + Render L1 tests
    add_executable(test_westeros_main_l1
        ${WESTEROS_MAIN_TEST_DIR}/westeros_main_test.cpp
        ${WESTEROS_MAIN_SRC_ROOT}/westeros-compositor.cpp
        ${WESTEROS_MAIN_SRC_ROOT}/westeros-render.cpp
        ${WESTEROS_MAIN_SRC_ROOT}/westeros-nested.cpp
        ${WESTEROS_MAIN_MOCK_DIR}/wayland-server-mock.cpp
        ${WESTEROS_MAIN_MOCK_DIR}/wayland-client-mock.cpp
        ${WESTEROS_MAIN_MOCK_DIR}/simpleshell-mock.cpp
        ${WESTEROS_MAIN_MOCK_DIR}/egl-mock.cpp
        ${WESTEROS_MAIN_MOCK_DIR}/xkbcommon-mock.cpp
        ${WESTEROS_MAIN_MOCK_DIR}/gstreamer-mock.cpp
    )
    
    target_include_directories(test_westeros_main_l1 PRIVATE
        ${WESTEROS_MAIN_SRC_ROOT}  # For source files
        ${WESTEROS_MAIN_SRC_ROOT}/simpleshell  # For simpleshell headers
        ${WESTEROS_MAIN_SRC_ROOT}/simplebuffer  # For simplebuffer headers
        ${WESTEROS_MAIN_SRC_ROOT}/linux-dmabuf  # For linux-dmabuf headers
        ${WESTEROS_MAIN_SRC_ROOT}/linux-expsync  # For linux-expsync headers
        ${WESTEROS_MAIN_SRC_ROOT}/protocol  # For protocol headers
        ${WESTEROS_MAIN_MOCK_DIR}  # For mock headers
        ${CMAKE_SOURCE_DIR}/common/include  # For common test utilities
    )
    
    target_link_libraries(test_westeros_main_l1
        westeros-test-common
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        ${CMAKE_DL_LIBS}
        pthread
    )
    
    # Add UNIT_TEST definition for test build
    target_compile_definitions(test_westeros_main_l1 PRIVATE 
        UNIT_TEST
        USE_XDG_STABLE  # Use XDG stable protocol version
        WESTEROS_HAVE_WAYLAND_EGL  # Enable Wayland EGL support
    )
    
    # Enable coverage for the source files being tested
    if(ENABLE_COVERAGE)
        target_compile_options(test_westeros_main_l1 PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(test_westeros_main_l1 PRIVATE --coverage)
    endif()
    
    add_test(NAME WesterosMainL1Tests COMMAND test_westeros_main_l1)
    set_tests_properties(WesterosMainL1Tests PROPERTIES
        TIMEOUT 300
        LABELS "L1;WesterosMain;Component"
    )
    
    message(STATUS "    ✓ Westeros Main L1 tests configured")
    
    # Westeros Nested L1 tests
    add_executable(test_westeros_nested_l1
        ${WESTEROS_MAIN_TEST_DIR}/westeros_nested_test.cpp
        ${WESTEROS_MAIN_SRC_ROOT}/westeros-nested.cpp
        ${WESTEROS_MAIN_SRC_ROOT}/westeros-compositor.cpp
        ${WESTEROS_MAIN_SRC_ROOT}/westeros-render.cpp
        ${WESTEROS_MAIN_MOCK_DIR}/wayland-server-mock.cpp
        ${WESTEROS_MAIN_MOCK_DIR}/wayland-client-mock.cpp
        ${WESTEROS_MAIN_MOCK_DIR}/simpleshell-mock.cpp
        ${WESTEROS_MAIN_MOCK_DIR}/egl-mock.cpp
        ${WESTEROS_MAIN_MOCK_DIR}/xkbcommon-mock.cpp
        ${WESTEROS_MAIN_MOCK_DIR}/gstreamer-mock.cpp
    )
    
    target_include_directories(test_westeros_nested_l1 PRIVATE
        ${WESTEROS_MAIN_SRC_ROOT}  # For source files
        ${WESTEROS_MAIN_SRC_ROOT}/simpleshell  # For simpleshell headers
        ${WESTEROS_MAIN_SRC_ROOT}/simplebuffer  # For simplebuffer headers
        ${WESTEROS_MAIN_SRC_ROOT}/linux-dmabuf  # For linux-dmabuf headers
        ${WESTEROS_MAIN_SRC_ROOT}/linux-expsync  # For linux-expsync headers
        ${WESTEROS_MAIN_SRC_ROOT}/protocol  # For protocol headers
        ${WESTEROS_MAIN_MOCK_DIR}  # For mock headers
        ${CMAKE_SOURCE_DIR}/common/include  # For common test utilities
    )
    
    target_link_libraries(test_westeros_nested_l1
        westeros-test-common
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        ${CMAKE_DL_LIBS}
        pthread
    )
    
    # Add UNIT_TEST definition for test build
    target_compile_definitions(test_westeros_nested_l1 PRIVATE 
        UNIT_TEST
        USE_XDG_STABLE  # Use XDG stable protocol version
        WESTEROS_HAVE_WAYLAND_EGL  # Enable Wayland EGL support
    )
    
    # Enable coverage for the source files being tested
    if(ENABLE_COVERAGE)
        target_compile_options(test_westeros_nested_l1 PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(test_westeros_nested_l1 PRIVATE --coverage)
    endif()
    
    add_test(NAME WesterosNestedL1Tests COMMAND test_westeros_nested_l1)
    set_tests_properties(WesterosNestedL1Tests PROPERTIES
        TIMEOUT 300
        LABELS "L1;WesterosMain;Nested;Component"
    )
    
    message(STATUS "    ✓ Westeros Nested L1 tests configured")
endif()

message(STATUS "    ✓ Westeros Main test suite configured")
