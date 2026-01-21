/*
 * Mock Linux DMABUF Protocol Header
 */

#ifndef _LINUX_DMABUF_PROTOCOL_MOCK_H
#define _LINUX_DMABUF_PROTOCOL_MOCK_H

#include <stdint.h>
#include "wayland-server.h"

#ifdef __cplusplus
extern "C" {
#endif

// Protocol version
#define ZWP_LINUX_DMABUF_V1_MODIFIER_SINCE_VERSION 3

// Error codes
enum zwp_linux_buffer_params_v1_error {
    ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_ALREADY_USED = 0,
    ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_PLANE_IDX = 1,
    ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_PLANE_SET = 2,
    ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INCOMPLETE = 3,
    ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_FORMAT = 4,
    ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_DIMENSIONS = 5,
    ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_OUT_OF_BOUNDS = 6,
    ZWP_LINUX_BUFFER_PARAMS_V1_ERROR_INVALID_WL_BUFFER = 7
};

// Interface structure definitions
struct wl_buffer_interface {
    void (*destroy)(struct wl_client *client, struct wl_resource *resource);
};

struct zwp_linux_buffer_params_v1_interface {
    void (*destroy)(struct wl_client *client, struct wl_resource *resource);
    void (*add)(struct wl_client *client, struct wl_resource *resource,
                int32_t name_fd, uint32_t plane_idx, uint32_t offset,
                uint32_t stride, uint32_t modifier_hi, uint32_t modifier_lo);
    void (*create)(struct wl_client *client, struct wl_resource *resource,
                   int32_t width, int32_t height, uint32_t format, uint32_t flags);
    void (*create_immed)(struct wl_client *client, struct wl_resource *resource,
                         uint32_t buffer_id, int32_t width, int32_t height,
                         uint32_t format, uint32_t flags);
};

struct zwp_linux_dmabuf_v1_interface {
    void (*destroy)(struct wl_client *client, struct wl_resource *resource);
    void (*create_params)(struct wl_client *client, struct wl_resource *resource,
                          uint32_t params_id);
};

// External interface declarations
extern const struct wl_interface zwp_linux_dmabuf_v1_interface;
extern const struct wl_interface zwp_linux_buffer_params_v1_interface;
extern const struct wl_interface wl_buffer_interface;

// Protocol functions
void zwp_linux_dmabuf_v1_send_format(struct wl_resource *resource, uint32_t format);

void zwp_linux_dmabuf_v1_send_modifier(struct wl_resource *resource,
                                       uint32_t format,
                                       uint32_t modifier_hi,
                                       uint32_t modifier_lo);

void zwp_linux_buffer_params_v1_send_created(struct wl_resource *resource,
                                             struct wl_resource *buffer);

void zwp_linux_buffer_params_v1_send_failed(struct wl_resource *resource);

#ifdef __cplusplus
}
#endif

#endif // _LINUX_DMABUF_PROTOCOL_MOCK_H
