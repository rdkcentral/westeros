# SimpleBuffer Component

# Source files
set(SIMPLEBUFFER_SOURCES
    src/westeros-simplebuffer.cpp
)

# Header files
set(SIMPLEBUFFER_HEADERS
    include/simplebuffer/westeros-simplebuffer.h
    include/simplebuffer/simplebuffer-server-protocol.h
)

# Create simplebuffer library
add_library(simplebuffer SHARED ${SIMPLEBUFFER_SOURCES})

target_include_directories(simplebuffer
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/simplebuffer>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks
)

# Link dependencies
target_link_libraries(simplebuffer
    PUBLIC
        westeros-common
    PRIVATE
        ${CMAKE_DL_LIBS}
)

if(WAYLAND_FOUND)
    target_include_directories(simplebuffer PRIVATE ${WAYLAND_INCLUDE_DIRS})
    target_link_libraries(simplebuffer PRIVATE ${WAYLAND_LIBRARIES})
endif()

# Set library properties
set_target_properties(simplebuffer PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${SIMPLEBUFFER_HEADERS}"
)

# Tests
if(BUILD_TESTS AND GTest_FOUND)
    add_executable(test_simplebuffer_standalone_l1
        tests/L1/westeros_simplebuffer_test.cpp
        src/westeros-simplebuffer.cpp
        ${CMAKE_SOURCE_DIR}/components/simpleshell/tests/mocks/wayland_mock.cpp
    )
    
    target_include_directories(test_simplebuffer_standalone_l1 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/simplebuffer
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/components/simpleshell/include
        ${CMAKE_SOURCE_DIR}/components/simpleshell/tests/mocks
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    
    # Don't link against any Wayland libraries - use mocks only
    target_link_libraries(test_simplebuffer_standalone_l1
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        ${CMAKE_DL_LIBS}
    )
    
    # Ensure no Wayland linking
    target_compile_definitions(test_simplebuffer_standalone_l1 PRIVATE
        WESTEROS_TEST_MODE=1
        UNIT_TEST
    )
    
    # Enable coverage for this test
    if(ENABLE_COVERAGE)
        target_compile_options(test_simplebuffer_standalone_l1 PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(test_simplebuffer_standalone_l1 PRIVATE --coverage)
    endif()
    
    add_test(NAME SimpleBufferStandaloneL1Tests COMMAND test_simplebuffer_standalone_l1)
    
    message(STATUS "    ✓ SimpleBuffer L1 tests configured")
endif()

# Installation
install(TARGETS simplebuffer
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/simplebuffer
)

message(STATUS "    ✓ SimpleBuffer component configured")
