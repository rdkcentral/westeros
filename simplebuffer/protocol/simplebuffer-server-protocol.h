/*
 * SimpleBuffer Protocol - Server-side Header
 * Auto-generated placeholder for L1 testing
 */

#ifndef SIMPLEBUFFER_SERVER_PROTOCOL_H
#define SIMPLEBUFFER_SERVER_PROTOCOL_H

#include <stdint.h>
#include <stddef.h>
#include "wayland-server.h"

/**
 * wl_sb error codes
 * Note: Enum is outside extern "C" block for C++ visibility
 */
enum wl_sb_error {
    WL_SB_ERROR_AUTHENTICATE_FAIL = 0,
    WL_SB_ERROR_INVALID_FORMAT = 1,
    WL_SB_ERROR_INVALID_NAME = 2,
    WL_SB_ERROR_BAD_DIMENSIONS = 3,
};

#ifdef __cplusplus
extern "C" {
#endif

struct wl_client;
struct wl_resource;

/**
 * External interface declarations for tests
 */
extern const struct wl_interface wl_sb_interface;
extern const struct wl_interface wl_buffer_interface;

/**
 * wl_sb_interface - simple buffer interface
 */
/**
 * wl_sb interface structure
 */
struct wl_sb_interface {
    void (*create_buffer)(struct wl_client *client,
                         struct wl_resource *resource,
                         uint32_t id,
                         uint32_t native_handle,
                         int32_t width,
                         int32_t height,
                         uint32_t stride,
                         uint32_t format);
    void (*create_planar_buffer)(struct wl_client *client,
                                struct wl_resource *resource,
                                uint32_t id,
                                uint32_t native_handle,
                                int32_t width,
                                int32_t height,
                                uint32_t format,
                                int32_t offset0,
                                int32_t offset1,
                                int32_t offset2,
                                int32_t stride0,
                                int32_t stride1,
                                int32_t stride2);
    void (*create_planar_buffer_fd)(struct wl_client *client,
                                   struct wl_resource *resource,
                                   uint32_t id,
                                   int32_t fd,
                                   int32_t width,
                                   int32_t height,
                                   uint32_t format,
                                   int32_t offset0,
                                   int32_t offset1,
                                   int32_t offset2,
                                   int32_t stride0,
                                   int32_t stride1,
                                   int32_t stride2);
    void (*create_planar_buffer_fd2)(struct wl_client *client,
                                    struct wl_resource *resource,
                                    uint32_t id,
                                    int32_t fd0,
                                    int32_t fd1,
                                    int32_t fd2,
                                    int32_t width,
                                    int32_t height,
                                    uint32_t format,
                                    int32_t offset0,
                                    int32_t offset1,
                                    int32_t offset2,
                                    int32_t stride0,
                                    int32_t stride1,
                                    int32_t stride2);
};

/**
 * wl_sb event opcodes
 */
#define WL_SB_FORMAT 0
#define WL_SB_BUFFER_CREATED 1
#define WL_SB_BUFFER_RELEASED 2

/**
 * Send format event
 */
static inline void
wl_sb_send_format(struct wl_resource *resource_,
                  uint32_t format)
{
    wl_resource_post_event(resource_, WL_SB_FORMAT, format);
}

/**
 * Send buffer_created event
 */
static inline void
wl_sb_send_buffer_created(struct wl_resource *resource_,
                         struct wl_resource *buffer)
{
    // Mock implementation
}

/**
 * Send buffer_released event
 */
static inline void
wl_sb_send_buffer_released(struct wl_resource *resource_,
                          struct wl_resource *buffer)
{
    // Mock implementation
}

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEBUFFER_SERVER_PROTOCOL_H */
