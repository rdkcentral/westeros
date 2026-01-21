/*
 * SimpleShell Protocol - Server-side Header
 * Auto-generated placeholder for L1 testing
 */

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

/**
 * External interface declaration for tests
 */
extern const struct wl_interface wl_simple_shell_interface;

/**
 * wl_simple_shell_interface - simple shell interface
 * This must match the implementation in westeros-simpleshell.cpp
 */
struct wl_simple_shell_interface {
    void (*set_name)(struct wl_client *client,
                     struct wl_resource *resource,
                     uint32_t surfaceId,
                     const char *name);
    void (*set_visible)(struct wl_client *client,
                        struct wl_resource *resource,
                        uint32_t surfaceId,
                        uint32_t visible);
    void (*set_geometry)(struct wl_client *client,
                         struct wl_resource *resource,
                         uint32_t surfaceId,
                         int32_t x,
                         int32_t y,
                         int32_t width,
                         int32_t height);
    void (*set_opacity)(struct wl_client *client,
                        struct wl_resource *resource,
                        uint32_t surfaceId,
                        wl_fixed_t opacity);
    void (*set_zorder)(struct wl_client *client,
                       struct wl_resource *resource,
                       uint32_t surfaceId,
                       wl_fixed_t zorder);
    void (*get_status)(struct wl_client *client,
                       struct wl_resource *resource,
                       uint32_t surfaceId);
    void (*get_surfaces)(struct wl_client *client,
                         struct wl_resource *resource);
    void (*set_focus)(struct wl_client *client,
                      struct wl_resource *resource,
                      uint32_t surfaceId);
    void (*set_scale)(struct wl_client *client,
                      struct wl_resource *resource,
                      uint32_t surfaceId,
                      wl_fixed_t scaleX,
                      wl_fixed_t scaleY);
};

/**
 * wl_simple_shell event opcodes
 */
#define WL_SIMPLE_SHELL_SURFACE_INFO 0
#define WL_SIMPLE_SHELL_SURFACE_STATUS 1
#define WL_SIMPLE_SHELL_SURFACE_NAME 2
#define WL_SIMPLE_SHELL_SURFACES_DONE 3
#define WL_SIMPLE_SHELL_SURFACE_ID 4
#define WL_SIMPLE_SHELL_SURFACE_DESTROYED 5

/**
 * Send surface_status event
 * Used in: wstISimpleShellGetStatus, wstSimpleShellBroadcastSurfaceUpdate
 */
static inline void
wl_simple_shell_send_surface_status(struct wl_resource *resource_,
                                    uint32_t surfaceId,
                                    const char *name,
                                    uint32_t visible,
                                    int32_t x,
                                    int32_t y,
                                    int32_t width,
                                    int32_t height,
                                    wl_fixed_t opacity,
                                    wl_fixed_t zorder)
{
    // Mock implementation - does nothing
}

/**
 * Send get_surfaces_done event
 * Used in: wstISimpleShellGetSurfaces
 */
static inline void
wl_simple_shell_send_get_surfaces_done(struct wl_resource *resource_)
{
    // Mock implementation - does nothing
}

/**
 * Send surfaces_done event
 */
static inline void
wl_simple_shell_send_surfaces_done(struct wl_resource *resource_)
{
    // Mock implementation - does nothing
}

/**
 * Send surface_id event
 * Used in: WstSimpleShellNotifySurfaceCreated
 */
static inline void
wl_simple_shell_send_surface_id(struct wl_resource *resource_,
                                struct wl_resource *surface_resource,
                                uint32_t surfaceId)
{
    // Mock implementation - does nothing
}

/**
 * Send surface_created event
 * Used in: wstSimpleShellBroadcastCreation
 */
static inline void
wl_simple_shell_send_surface_created(struct wl_resource *resource_,
                                     uint32_t surfaceId,
                                     const char *name)
{
    // Mock implementation - does nothing
}

/**
 * Send surface_destroyed event
 * Used in: WstSimpleShellNotifySurfaceDestroyed
 */
static inline void
wl_simple_shell_send_surface_destroyed(struct wl_resource *resource_,
                                       uint32_t surfaceId,
                                       const char *name)
{
    // Mock implementation - does nothing
}

/**
 * Send surface_info event
 */
static inline void
wl_simple_shell_send_surface_info(struct wl_resource *resource_,
                                 struct wl_resource *surface,
                                 uint32_t visible,
                                 int32_t x,
                                 int32_t y,
                                 int32_t width,
                                 int32_t height,
                                 wl_fixed_t opacity,
                                 wl_fixed_t zorder)
{
    // Mock implementation - does nothing
}

/**
 * Send surface_name event
 */
static inline void
wl_simple_shell_send_surface_name(struct wl_resource *resource_,
                                  struct wl_resource *surface,
                                  const char *name)
{
    // Mock implementation - does nothing
}

#ifdef __cplusplus
}
#endif

#endif /* SIMPLESHELL_SERVER_PROTOCOL_H */
