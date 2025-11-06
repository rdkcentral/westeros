#ifndef WESTEROS_COMMON_TYPES_H
#define WESTEROS_COMMON_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @file types.h
 * @brief Common type definitions for Westeros components
 */

namespace westeros {
namespace common {

/**
 * @brief Result codes for Westeros operations
 */
enum class Result {
    SUCCESS = 0,
    ERROR_INVALID_PARAMETER = -1,
    ERROR_NOT_INITIALIZED = -2,
    ERROR_ALREADY_INITIALIZED = -3,
    ERROR_OUT_OF_MEMORY = -4,
    ERROR_NOT_SUPPORTED = -5,
    ERROR_OPERATION_FAILED = -6
};

/**
 * @brief Convert Result to string
 */
inline const char* resultToString(Result result) {
    switch (result) {
        case Result::SUCCESS:
            return "SUCCESS";
        case Result::ERROR_INVALID_PARAMETER:
            return "ERROR_INVALID_PARAMETER";
        case Result::ERROR_NOT_INITIALIZED:
            return "ERROR_NOT_INITIALIZED";
        case Result::ERROR_ALREADY_INITIALIZED:
            return "ERROR_ALREADY_INITIALIZED";
        case Result::ERROR_OUT_OF_MEMORY:
            return "ERROR_OUT_OF_MEMORY";
        case Result::ERROR_NOT_SUPPORTED:
            return "ERROR_NOT_SUPPORTED";
        case Result::ERROR_OPERATION_FAILED:
            return "ERROR_OPERATION_FAILED";
        default:
            return "UNKNOWN";
    }
}

/**
 * @brief Rectangle structure
 */
struct Rectangle {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
};

/**
 * @brief Size structure
 */
struct Size {
    uint32_t width;
    uint32_t height;
};

/**
 * @brief Point structure
 */
struct Point {
    int32_t x;
    int32_t y;
};

}  // namespace common
}  // namespace westeros

#endif  // WESTEROS_COMMON_TYPES_H
