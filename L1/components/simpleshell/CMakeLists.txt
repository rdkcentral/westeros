# SimpleShell Component

# Source files
set(SIMPLESHELL_SOURCES
    src/westeros-simpleshell.cpp
    src/westeros-simplebuffer.cpp
)

# Header files
set(SIMPLESHELL_HEADERS
    include/simpleshell/westeros-simpleshell.h
    include/simpleshell/westeros-simplebuffer.h
    include/simpleshell/simpleshell-server-protocol.h
    include/simpleshell/simplebuffer-server-protocol.h
)

# Create simpleshell library
add_library(simpleshell SHARED ${SIMPLESHELL_SOURCES})

target_include_directories(simpleshell
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/simpleshell>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks
)

# Link dependencies
target_link_libraries(simpleshell
    PUBLIC
        westeros-common
    PRIVATE
        ${CMAKE_DL_LIBS}
)

if(WAYLAND_FOUND)
    target_include_directories(simpleshell PRIVATE ${WAYLAND_INCLUDE_DIRS})
    target_link_libraries(simpleshell PRIVATE ${WAYLAND_LIBRARIES})
endif()

# Set library properties
set_target_properties(simpleshell PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${SIMPLESHELL_HEADERS}"
)

# Enable coverage for the library
if(ENABLE_COVERAGE)
    target_compile_options(simpleshell PRIVATE --coverage -fprofile-arcs -ftest-coverage)
    target_link_options(simpleshell PRIVATE --coverage)
endif()

# Tests
if(BUILD_TESTS AND GTest_FOUND)
    # SimpleShell tests
    add_executable(test_simpleshell_l1
        tests/L1/westeros_simpleshell_test.cpp
        src/westeros-simpleshell.cpp
        tests/mocks/wayland_mock.cpp
    )
    
    target_include_directories(test_simpleshell_l1 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/simpleshell
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    
    target_link_libraries(test_simpleshell_l1
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
    )
    
    # Add UNIT_TEST definition for test build
    target_compile_definitions(test_simpleshell_l1 PRIVATE UNIT_TEST)
    
    # Enable coverage for this test
    if(ENABLE_COVERAGE)
        target_compile_options(test_simpleshell_l1 PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(test_simpleshell_l1 PRIVATE --coverage)
    endif()
    
    add_test(NAME SimpleShellL1Tests COMMAND test_simpleshell_l1)
    
    # SimpleBuffer tests
    add_executable(test_simplebuffer_l1
        tests/L1/westeros_simplebuffer_test.cpp
        src/westeros-simplebuffer.cpp
        tests/mocks/wayland_mock.cpp
    )
    
    target_include_directories(test_simplebuffer_l1 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/simpleshell
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    
    target_link_libraries(test_simplebuffer_l1
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
    )
    
    # Add UNIT_TEST definition for test build
    target_compile_definitions(test_simplebuffer_l1 PRIVATE UNIT_TEST)
    
    # Enable coverage for this test
    if(ENABLE_COVERAGE)
        target_compile_options(test_simplebuffer_l1 PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(test_simplebuffer_l1 PRIVATE --coverage)
    endif()
    
    add_test(NAME SimpleBufferL1Tests COMMAND test_simplebuffer_l1)
    
    message(STATUS "    ✓ SimpleShell L1 tests configured")
endif()

# Installation
install(TARGETS simpleshell
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/simpleshell
)

message(STATUS "    ✓ SimpleShell component configured")
