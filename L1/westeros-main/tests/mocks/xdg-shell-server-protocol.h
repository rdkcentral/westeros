/*
 * XDG Shell Server Protocol Mock Header
 * Auto-generated stub for L1 testing
 */
#ifndef XDG_SHELL_SERVER_PROTOCOL_H
#define XDG_SHELL_SERVER_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "wayland-server.h"

// Forward declarations
struct xdg_shell;
struct xdg_surface;
struct xdg_popup;
struct xdg_toplevel;
struct xdg_wm_base;
struct xdg_positioner;

// Interface declarations
extern const struct wl_interface xdg_shell_interface;
extern const struct wl_interface xdg_surface_interface;
extern const struct wl_interface xdg_popup_interface;
extern const struct wl_interface xdg_toplevel_interface;
extern const struct wl_interface xdg_wm_base_interface;
extern const struct wl_interface xdg_positioner_interface;

// XDG Shell interface (for v4/v5)
struct xdg_shell_interface {
    void (*use_unstable_version)(struct wl_client *client,
                                  struct wl_resource *resource,
                                  int32_t version);
    void (*get_xdg_surface)(struct wl_client *client,
                            struct wl_resource *resource,
                            uint32_t id,
                            struct wl_resource *surface);
    void (*get_xdg_popup)(struct wl_client *client,
                          struct wl_resource *resource,
                          uint32_t id,
                          struct wl_resource *surface,
                          struct wl_resource *parent,
                          struct wl_resource *seat,
                          uint32_t serial,
                          int32_t x, int32_t y);
    void (*pong)(struct wl_client *client,
                 struct wl_resource *resource,
                 uint32_t serial);
};

// XDG WM Base interface (for stable)
struct xdg_wm_base_interface {
    void (*destroy)(struct wl_client *client,
                    struct wl_resource *resource);
    void (*create_positioner)(struct wl_client *client,
                              struct wl_resource *resource,
                              uint32_t id);
    void (*get_xdg_surface)(struct wl_client *client,
                            struct wl_resource *resource,
                            uint32_t id,
                            struct wl_resource *surface);
    void (*pong)(struct wl_client *client,
                 struct wl_resource *resource,
                 uint32_t serial);
};

// XDG Surface interface
struct xdg_surface_interface {
    void (*destroy)(struct wl_client *client,
                    struct wl_resource *resource);
    void (*get_toplevel)(struct wl_client *client,
                         struct wl_resource *resource,
                         uint32_t id);
    void (*get_popup)(struct wl_client *client,
                      struct wl_resource *resource,
                      uint32_t id,
                      struct wl_resource *parent,
                      struct wl_resource *positioner);
    void (*set_window_geometry)(struct wl_client *client,
                                struct wl_resource *resource,
                                int32_t x, int32_t y,
                                int32_t width, int32_t height);
    void (*ack_configure)(struct wl_client *client,
                          struct wl_resource *resource,
                          uint32_t serial);
};

// XDG Toplevel interface
struct xdg_toplevel_interface {
    void (*destroy)(struct wl_client *client,
                    struct wl_resource *resource);
    void (*set_parent)(struct wl_client *client,
                       struct wl_resource *resource,
                       struct wl_resource *parent);
    void (*set_title)(struct wl_client *client,
                      struct wl_resource *resource,
                      const char *title);
    void (*set_app_id)(struct wl_client *client,
                       struct wl_resource *resource,
                       const char *app_id);
    void (*show_window_menu)(struct wl_client *client,
                             struct wl_resource *resource,
                             struct wl_resource *seat,
                             uint32_t serial,
                             int32_t x, int32_t y);
    void (*move)(struct wl_client *client,
                 struct wl_resource *resource,
                 struct wl_resource *seat,
                 uint32_t serial);
    void (*resize)(struct wl_client *client,
                   struct wl_resource *resource,
                   struct wl_resource *seat,
                   uint32_t serial,
                   uint32_t edges);
    void (*set_max_size)(struct wl_client *client,
                         struct wl_resource *resource,
                         int32_t width, int32_t height);
    void (*set_min_size)(struct wl_client *client,
                         struct wl_resource *resource,
                         int32_t width, int32_t height);
    void (*set_maximized)(struct wl_client *client,
                          struct wl_resource *resource);
    void (*unset_maximized)(struct wl_client *client,
                            struct wl_resource *resource);
    void (*set_fullscreen)(struct wl_client *client,
                           struct wl_resource *resource,
                           struct wl_resource *output);
    void (*unset_fullscreen)(struct wl_client *client,
                             struct wl_resource *resource);
    void (*set_minimized)(struct wl_client *client,
                          struct wl_resource *resource);
};

// XDG Popup interface
struct xdg_popup_interface {
    void (*destroy)(struct wl_client *client,
                    struct wl_resource *resource);
    void (*grab)(struct wl_client *client,
                 struct wl_resource *resource,
                 struct wl_resource *seat,
                 uint32_t serial);
};

// XDG Positioner interface
struct xdg_positioner_interface {
    void (*destroy)(struct wl_client *client,
                    struct wl_resource *resource);
    void (*set_size)(struct wl_client *client,
                     struct wl_resource *resource,
                     int32_t width, int32_t height);
    void (*set_anchor_rect)(struct wl_client *client,
                            struct wl_resource *resource,
                            int32_t x, int32_t y,
                            int32_t width, int32_t height);
    void (*set_anchor)(struct wl_client *client,
                       struct wl_resource *resource,
                       uint32_t anchor);
    void (*set_gravity)(struct wl_client *client,
                        struct wl_resource *resource,
                        uint32_t gravity);
    void (*set_constraint_adjustment)(struct wl_client *client,
                                       struct wl_resource *resource,
                                       uint32_t constraint_adjustment);
    void (*set_offset)(struct wl_client *client,
                       struct wl_resource *resource,
                       int32_t x, int32_t y);
};

// Helper functions
static inline void
xdg_surface_send_configure(struct wl_resource *resource, uint32_t serial) {
    (void)resource;
    (void)serial;
}

static inline void
xdg_toplevel_send_configure(struct wl_resource *resource,
                             int32_t width, int32_t height,
                             struct wl_array *states) {
    (void)resource;
    (void)width;
    (void)height;
    (void)states;
}

static inline void
xdg_wm_base_send_ping(struct wl_resource *resource, uint32_t serial) {
    (void)resource;
    (void)serial;
}

#ifdef __cplusplus
}
#endif

#endif /* XDG_SHELL_SERVER_PROTOCOL_H */
