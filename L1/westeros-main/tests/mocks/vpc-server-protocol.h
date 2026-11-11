/*
 * VPC (Video Path Control) Server Protocol Mock Header
 * Auto-generated stub for L1 testing
 */
#ifndef VPC_SERVER_PROTOCOL_H
#define VPC_SERVER_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "wayland-server.h"

// Forward declarations
struct wl_vpc;
struct wl_vpc_surface;

// Interface declarations
extern const struct wl_interface wl_vpc_interface;
extern const struct wl_interface wl_vpc_surface_interface;

/* Server request tables for the VPC protocol, matching protocol/vpc.xml. */
struct wl_vpc_interface {
    void (*get_vpc_surface)(struct wl_client *client,
                            struct wl_resource *resource,
                            uint32_t id,
                            struct wl_resource *surface);
};

struct wl_vpc_surface_interface {
    void (*set_geometry)(struct wl_client *client,
                         struct wl_resource *resource,
                         int32_t x, int32_t y, int32_t width, int32_t height);
    void (*set_geometry_with_crop)(struct wl_client *client,
                                   struct wl_resource *resource,
                                   int32_t x, int32_t y, int32_t width, int32_t height,
                                   int32_t crop_x, int32_t crop_y,
                                   int32_t crop_width, int32_t crop_height);
};

// Video pathway values
enum wl_vpc_surface_video_pathway {
    WL_VPC_SURFACE_VIDEO_PATHWAY_HARDWARE = 0,
    WL_VPC_SURFACE_VIDEO_PATHWAY_GRAPHICS = 1,
};

// Legacy naming for backwards compatibility
#define WL_VPC_SURFACE_PATHWAY_HARDWARE WL_VPC_SURFACE_VIDEO_PATHWAY_HARDWARE
#define WL_VPC_SURFACE_PATHWAY_GRAPHICS WL_VPC_SURFACE_VIDEO_PATHWAY_GRAPHICS
#define WL_VPC_SURFACE_CROP_DENOM 100000

// Helper functions for sending events
static inline void
wl_vpc_surface_send_video_path_change(struct wl_resource *resource,
                                       uint32_t new_pathway) {
    (void)resource;
    (void)new_pathway;
}

static inline void
wl_vpc_surface_send_video_xform_change(struct wl_resource *resource,
                                        int32_t x_translation,
                                        int32_t y_translation,
                                        uint32_t x_scale_num,
                                        uint32_t x_scale_denom,
                                        uint32_t y_scale_num,
                                        uint32_t y_scale_denom,
                                        uint32_t output_width,
                                        uint32_t output_height) {
    (void)resource;
    (void)x_translation;
    (void)y_translation;
    (void)x_scale_num;
    (void)x_scale_denom;
    (void)y_scale_num;
    (void)y_scale_denom;
    (void)output_width;
    (void)output_height;
}

#ifdef __cplusplus
}
#endif

#endif /* VPC_SERVER_PROTOCOL_H */
