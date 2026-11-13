# Common test utilities library

# Create interface library for common test includes and utilities
add_library(westeros-test-common INTERFACE)

target_include_directories(westeros-test-common INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Install headers
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

message(STATUS "    ✓ Common test utilities configured")
