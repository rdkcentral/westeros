#ifndef LINUX_EXPLICIT_SYNCHRONIZATION_UNSTABLE_V1_SERVER_PROTOCOL_H
#define LINUX_EXPLICIT_SYNCHRONIZATION_UNSTABLE_V1_SERVER_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef UNIT_TEST
#include "wayland-stubs.h"
#else
#include <wayland-server.h>
#endif

/* Protocol version */
#ifndef ZWP_LINUX_EXPLICIT_SYNCHRONIZATION_V1_VERSION
#define ZWP_LINUX_EXPLICIT_SYNCHRONIZATION_V1_VERSION 2
#endif

/* zwp_linux_explicit_synchronization_v1 interface */
struct zwp_linux_explicit_synchronization_v1_interface {
    void (*destroy)(struct wl_client *client, struct wl_resource *resource);
    void (*get_synchronization)(struct wl_client *client,
                               struct wl_resource *resource,
                               uint32_t id,
                               struct wl_resource *surface);
};

/* zwp_linux_surface_synchronization_v1 interface */
struct zwp_linux_surface_synchronization_v1_interface {
    void (*destroy)(struct wl_client *client, struct wl_resource *resource);
    void (*set_acquire_fence)(struct wl_client *client,
                             struct wl_resource *resource,
                             int32_t fd);
    void (*get_release)(struct wl_client *client,
                       struct wl_resource *resource,
                       uint32_t release);
};

/* zwp_linux_buffer_release_v1 interface */
struct zwp_linux_buffer_release_v1_interface {
    /* No requests */
};

/* Error enums */
enum zwp_linux_explicit_synchronization_v1_error {
    ZWP_LINUX_EXPLICIT_SYNCHRONIZATION_V1_ERROR_INVALID_SURFACE = 0,
    ZWP_LINUX_EXPLICIT_SYNCHRONIZATION_V1_ERROR_DUPLICATE_SYNCHRONIZATION = 1,
};

enum zwp_linux_surface_synchronization_v1_error {
    ZWP_LINUX_SURFACE_SYNCHRONIZATION_V1_ERROR_INVALID_FENCE = 0,
    ZWP_LINUX_SURFACE_SYNCHRONIZATION_V1_ERROR_DUPLICATE_FENCE = 1,
    ZWP_LINUX_SURFACE_SYNCHRONIZATION_V1_ERROR_DUPLICATE_RELEASE = 2,
    ZWP_LINUX_SURFACE_SYNCHRONIZATION_V1_ERROR_NO_SURFACE = 3,
    ZWP_LINUX_SURFACE_SYNCHRONIZATION_V1_ERROR_UNSUPPORTED_BUFFER = 4,
    ZWP_LINUX_SURFACE_SYNCHRONIZATION_V1_ERROR_NO_BUFFER = 5,
};

/* Global interface definitions */
extern const struct wl_interface zwp_linux_explicit_synchronization_v1_interface;
extern const struct wl_interface zwp_linux_surface_synchronization_v1_interface;
extern const struct wl_interface zwp_linux_buffer_release_v1_interface;

/* Send event functions */
static inline void
zwp_linux_buffer_release_v1_send_fenced_release(struct wl_resource *resource,
                                                int32_t fence)
{
    // Mock: Event would be sent to client
    // In real implementation, this marshals the event
}

static inline void
zwp_linux_buffer_release_v1_send_immediate_release(struct wl_resource *resource)
{
    // Mock: Event would be sent to client
    // In real implementation, this marshals the event
}

#ifdef __cplusplus
}
#endif

#endif /* LINUX_EXPLICIT_SYNCHRONIZATION_UNSTABLE_V1_SERVER_PROTOCOL_H */
