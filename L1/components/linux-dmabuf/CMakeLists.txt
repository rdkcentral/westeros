# Linux DMA-BUF Component

# Source files
set(LINUX_DMABUF_SOURCES
    src/westeros-linux-dmabuf.cpp
)

# Header files
set(LINUX_DMABUF_HEADERS
    include/linux-dmabuf/westeros-linux-dmabuf.h
)

# Mock source files (compiled into the library for standalone builds)
set(LINUX_DMABUF_MOCK_SOURCES
    tests/mocks/wayland-server-mock.cpp
    tests/mocks/westeros-render-mock.cpp
)

# Create linux-dmabuf library
add_library(linux-dmabuf SHARED ${LINUX_DMABUF_SOURCES})

target_include_directories(linux-dmabuf
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/linux-dmabuf>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks
)

# Link dependencies
target_link_libraries(linux-dmabuf
    PUBLIC
        westeros-common
    PRIVATE
        ${CMAKE_DL_LIBS}
)

if(WAYLAND_FOUND)
    target_include_directories(linux-dmabuf PRIVATE ${WAYLAND_INCLUDE_DIRS})
    target_link_libraries(linux-dmabuf PRIVATE ${WAYLAND_LIBRARIES})
endif()

# Set library properties
set_target_properties(linux-dmabuf PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${LINUX_DMABUF_HEADERS}"
)

# Enable coverage for the library
if(ENABLE_COVERAGE)
    target_compile_options(linux-dmabuf PRIVATE --coverage -fprofile-arcs -ftest-coverage)
    target_link_options(linux-dmabuf PRIVATE --coverage)
endif()

# Tests
if(BUILD_TESTS AND GTest_FOUND)
    add_executable(test_linux_dmabuf_l1
        tests/L1/test_linux_dmabuf_l1.cpp
        src/westeros-linux-dmabuf.cpp
        ${LINUX_DMABUF_MOCK_SOURCES}
    )
    
    target_include_directories(test_linux_dmabuf_l1 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/linux-dmabuf
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    
    # Define UNIT_TEST to make static functions testable
    target_compile_definitions(test_linux_dmabuf_l1 PRIVATE UNIT_TEST)
    
    # Don't link against any Wayland libraries - use mocks only
    target_link_libraries(test_linux_dmabuf_l1
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        ${CMAKE_DL_LIBS}
    )
    
    # Ensure no Wayland linking
    target_compile_definitions(test_linux_dmabuf_l1 PRIVATE
        WESTEROS_TEST_MODE=1
    )
    
    # Enable coverage for this test
    if(ENABLE_COVERAGE)
        target_compile_options(test_linux_dmabuf_l1 PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(test_linux_dmabuf_l1 PRIVATE --coverage)
    endif()
    
    add_test(NAME LinuxDMABufL1Tests COMMAND test_linux_dmabuf_l1)
    
    message(STATUS "    ✓ Linux DMA-BUF L1 tests configured")
endif()

# Installation
install(TARGETS linux-dmabuf
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/linux-dmabuf
)

message(STATUS "    ✓ Linux DMA-BUF component configured")
