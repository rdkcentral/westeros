#ifndef SIMPLESHELL_SERVER_PROTOCOL_H
#define SIMPLESHELL_SERVER_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include "wayland-server.h"

struct wl_client;
struct wl_resource;

struct wl_simple_shell_interface {
    void (*set_name)(struct wl_client *client, struct wl_resource *resource, uint32_t surfaceId, const char *name);
    void (*set_visible)(struct wl_client *client, struct wl_resource *resource, uint32_t surfaceId, uint32_t visible);
    void (*set_geometry)(struct wl_client *client, struct wl_resource *resource, uint32_t surfaceId, int32_t x, int32_t y, int32_t width, int32_t height);
    void (*set_opacity)(struct wl_client *client, struct wl_resource *resource, uint32_t surfaceId, wl_fixed_t opacity);
    void (*set_zorder)(struct wl_client *client, struct wl_resource *resource, uint32_t surfaceId, wl_fixed_t zorder);
    void (*get_status)(struct wl_client *client, struct wl_resource *resource, uint32_t surfaceId);
    void (*get_surfaces)(struct wl_client *client, struct wl_resource *resource);
    void (*set_focus)(struct wl_client *client, struct wl_resource *resource, uint32_t surfaceId);
    void (*set_scale)(struct wl_client *client, struct wl_resource *resource, uint32_t surfaceId, wl_fixed_t scaleX, wl_fixed_t scaleY);
};

extern const struct wl_interface wl_simple_shell_interface;

// Mock functions for testing
void wl_simple_shell_send_surface_created(struct wl_resource *resource, uint32_t surfaceId, const char *name);
void wl_simple_shell_send_surface_destroyed(struct wl_resource *resource, uint32_t surfaceId, const char *name);
void wl_simple_shell_send_surface_status(struct wl_resource *resource, uint32_t surfaceId, const char *name, uint32_t visible, int32_t x, int32_t y, int32_t width, int32_t height, wl_fixed_t opacity, wl_fixed_t zorder);
void wl_simple_shell_send_surface_id(struct wl_resource *resource, struct wl_resource *surface_resource, uint32_t surfaceId);
void wl_simple_shell_send_get_surfaces_done(struct wl_resource *resource);

#ifdef __cplusplus
}
#endif

#endif