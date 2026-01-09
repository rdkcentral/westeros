/*
 * VPC (Video Path Control) Client Protocol Mock Header
 * Auto-generated stub for L1 testing
 */
#ifndef VPC_CLIENT_PROTOCOL_H
#define VPC_CLIENT_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "wayland-server.h"

struct wl_vpc;
struct wl_vpc_surface;

// VPC interface
extern const struct wl_interface wl_vpc_interface;
extern const struct wl_interface wl_vpc_surface_interface;

// Listener structures
struct wl_vpc_listener {
    void (*dummy)(void);
};

struct wl_vpc_surface_listener {
    void (*video_path_change)(void *data, struct wl_vpc_surface *vpc_surface,
                              uint32_t new_pathway);
    void (*video_xform_change)(void *data, struct wl_vpc_surface *vpc_surface,
                               int32_t x_translation, int32_t y_translation,
                               uint32_t x_scale_num, uint32_t x_scale_denom,
                               uint32_t y_scale_num, uint32_t y_scale_denom,
                               uint32_t output_width, uint32_t output_height);
};

// Stub functions
static inline struct wl_vpc *
wl_vpc_create(struct wl_display *display) {
    (void)display;
    return nullptr;
}

static inline void
wl_vpc_destroy(struct wl_vpc *vpc) {
    (void)vpc;
}

static inline struct wl_vpc_surface *
wl_vpc_surface_create(struct wl_vpc *vpc, struct wl_surface *surface) {
    (void)vpc;
    (void)surface;
    return nullptr;
}

static inline void
wl_vpc_surface_destroy(struct wl_vpc_surface *vpc_surface) {
    (void)vpc_surface;
}

static inline struct wl_vpc_surface *
wl_vpc_get_vpc_surface(struct wl_vpc *vpc, struct wl_surface *surface) {
    // Return nullptr for invalid inputs
    if (!vpc || !surface) {
        return nullptr;
    }
    // Return a mock VPC surface object for valid inputs
    static struct wl_vpc_surface mockVpcSurface;
    return &mockVpcSurface;
}

static inline int
wl_vpc_surface_add_listener(struct wl_vpc_surface *vpc_surface,
                            const struct wl_vpc_surface_listener *listener,
                            void *data) {
    // Trigger VPC callbacks with mock data
    if (listener) {
        // Mock video path change
        if (listener->video_path_change) {
            listener->video_path_change(data, vpc_surface, 1); // video path 1
        }
        // Mock video transform change (parameters match the actual signature)
        if (listener->video_xform_change) {
            listener->video_xform_change(data, vpc_surface, 0, 0, 1, 1, 1, 1, 1920, 1080);
        }
    }
    return 0;
}

static inline void
wl_vpc_surface_set_geometry_with_crop(struct wl_vpc_surface *vpc_surface, int32_t x, int32_t y, int32_t width, int32_t height, int32_t cx, int32_t cy, int32_t cw, int32_t ch) {
    (void)vpc_surface;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
    (void)cx;
    (void)cy;
    (void)cw;
    (void)ch;
}

#ifdef __cplusplus
}
#endif

#endif /* VPC_CLIENT_PROTOCOL_H */
