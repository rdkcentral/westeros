# Common utilities and helpers

# Header-only library for common types
add_library(westeros-common INTERFACE)
target_include_directories(westeros-common INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Make available to other targets
set(WESTEROS_COMMON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

message(STATUS "  ✓ Common utilities configured")
