#ifndef SIMPLEBUFFER_SERVER_PROTOCOL_H
#define SIMPLEBUFFER_SERVER_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include "wayland-server.h"

struct wl_client;
struct wl_resource;

// Error codes
enum wl_sb_error {
    WL_SB_ERROR_AUTHENTICATE_FAIL = 0,
    WL_SB_ERROR_INVALID_FORMAT = 1,
    WL_SB_ERROR_INVALID_NAME = 2,
    WL_SB_ERROR_INVALID_DIMENSIONS = 3,
};

// Events
enum wl_sb_event {
    WL_SB_FORMAT = 0,
};

struct wl_sb_interface {
    void (*create_buffer)(struct wl_client *client, struct wl_resource *resource, uint32_t id, uint32_t native_handle, int32_t width, int32_t height, uint32_t stride, uint32_t format);
    void (*create_planar_buffer)(struct wl_client *client, struct wl_resource *resource, uint32_t id, uint32_t native_handle, int32_t width, int32_t height, uint32_t format, int32_t offset0, int32_t offset1, int32_t offset2, int32_t stride0, int32_t stride1, int32_t stride2);
    void (*create_planar_buffer_fd)(struct wl_client *client, struct wl_resource *resource, uint32_t id, int32_t fd, int32_t width, int32_t height, uint32_t format, int32_t offset0, int32_t offset1, int32_t offset2, int32_t stride0, int32_t stride1, int32_t stride2);
    void (*create_planar_buffer_fd2)(struct wl_client *client, struct wl_resource *resource, uint32_t id, int32_t fd0, int32_t fd1, int32_t fd2, int32_t width, int32_t height, uint32_t format, int32_t offset0, int32_t offset1, int32_t offset2, int32_t stride0, int32_t stride1, int32_t stride2);
};

extern const struct wl_interface wl_sb_interface;

#ifdef __cplusplus
}
#endif

#endif