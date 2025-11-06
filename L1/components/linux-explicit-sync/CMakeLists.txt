# Linux Explicit Sync Component

# Source files
set(LINUX_EXPLICIT_SYNC_SOURCES
    src/westeros-linux-expsync.cpp
)

# Header files
set(LINUX_EXPLICIT_SYNC_HEADERS
    include/linux-explicit-sync/westeros-linux-expsync.h
)

# Mock source files (compiled with test)
set(LINUX_EXPLICIT_SYNC_MOCK_SOURCES
    tests/mocks/linux-explicit-synchronization-protocol.c
    tests/mocks/sync-file-stubs.c
    tests/mocks/wayland-stubs.c
    tests/mocks/westeros-context-mock.c
)

# Create linux-explicit-sync library
add_library(linux-explicit-sync SHARED ${LINUX_EXPLICIT_SYNC_SOURCES})

target_include_directories(linux-explicit-sync
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/linux-explicit-sync>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks
)

# Link dependencies
target_link_libraries(linux-explicit-sync
    PUBLIC
        westeros-common
    PRIVATE
        ${CMAKE_DL_LIBS}
)

if(WAYLAND_FOUND)
    target_include_directories(linux-explicit-sync PRIVATE ${WAYLAND_INCLUDE_DIRS})
    target_link_libraries(linux-explicit-sync PRIVATE ${WAYLAND_LIBRARIES})
endif()

# Set library properties
set_target_properties(linux-explicit-sync PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${LINUX_EXPLICIT_SYNC_HEADERS}"
)

# Enable coverage for the library
if(ENABLE_COVERAGE)
    target_compile_options(linux-explicit-sync PRIVATE --coverage -fprofile-arcs -ftest-coverage)
    target_link_options(linux-explicit-sync PRIVATE --coverage)
endif()

# Tests
if(BUILD_TESTS AND GTest_FOUND)
    add_executable(test_linux_expsync_l1
        tests/L1/test_linux_expsync_l1.cpp
        src/westeros-linux-expsync.cpp
        ${LINUX_EXPLICIT_SYNC_MOCK_SOURCES}
    )
    
    target_include_directories(test_linux_expsync_l1 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/linux-explicit-sync
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    
    # Don't link against any Wayland libraries - use mocks only
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
    
    # Enable coverage for this test
    if(ENABLE_COVERAGE)
        target_compile_options(test_linux_expsync_l1 PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(test_linux_expsync_l1 PRIVATE --coverage)
    endif()
    
    add_test(NAME LinuxExplicitSyncL1Tests COMMAND test_linux_expsync_l1)
    
    message(STATUS "    ✓ Linux Explicit Sync L1 tests configured")
endif()

# Installation
install(TARGETS linux-explicit-sync
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/linux-explicit-sync
)

message(STATUS "    ✓ Linux Explicit Sync component configured")
