/*
 * Wayland Client Mock for L1 Testing
 * Copyright 2024 RDK Management
 */
#ifndef _WAYLAND_CLIENT_H
#define _WAYLAND_CLIENT_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Opaque struct definitions for mock objects
struct wl_display { int dummy; };
struct wl_surface { int dummy; };
struct wl_buffer { int dummy; };
struct wl_shm { int dummy; };
struct wl_shm_pool { int dummy; };
struct wl_compositor { int dummy; };
#ifndef WL_ARRAY_DEFINED
#define WL_ARRAY_DEFINED
struct wl_array { void *data; size_t size; size_t alloc; };
#endif
struct wl_registry { int dummy; };
struct wl_output { int dummy; };
struct wl_seat { int dummy; };
struct wl_keyboard { int dummy; };
struct wl_pointer { int dummy; };
struct wl_touch { int dummy; };

// VPC specific types
struct wl_vpc { int dummy; };
struct wl_vpc_surface { int dummy; };

// Simple shell types
struct wl_simple_shell { int dummy; };

// wl_interface structure (needed for interface definitions)
#ifndef WL_INTERFACE_DEFINED
#define WL_INTERFACE_DEFINED
struct wl_interface {
    const char *name;
    int version;
    int method_count;
    const void *methods;
    int event_count;
    const void *events;
};
#endif // WL_INTERFACE_DEFINED

// Fixed point type
#ifndef WL_FIXED_T_DEFINED
#define WL_FIXED_T_DEFINED
typedef int32_t wl_fixed_t;
#endif

#ifndef wl_fixed_to_double
#define wl_fixed_to_double(f) ((double) f / 256.0)
#endif
#ifndef wl_fixed_from_double
#define wl_fixed_from_double(d) ((wl_fixed_t) (d * 256.0))
#endif
#ifndef wl_fixed_to_int
#define wl_fixed_to_int(f) ((f) / 256)
#endif
#ifndef wl_fixed_from_int
#define wl_fixed_from_int(i) ((i) * 256)
#endif

// Seat capabilities
#define WL_SEAT_CAPABILITY_POINTER 1
#define WL_SEAT_CAPABILITY_KEYBOARD 2
#define WL_SEAT_CAPABILITY_TOUCH 4

// Keyboard constants
#define WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1 1
#define WL_KEYBOARD_KEY_STATE_RELEASED 0
#define WL_KEYBOARD_KEY_STATE_PRESSED 1

// Output mode flags
#ifndef WL_OUTPUT_MODE_CURRENT
#define WL_OUTPUT_MODE_CURRENT 0x1
#endif

// SHM formats (use enum instead of macros to avoid conflicts)
#ifndef WL_SHM_FORMAT_ENUM_DEFINED
#define WL_SHM_FORMAT_ENUM_DEFINED
enum wl_shm_format {
    WL_SHM_FORMAT_ARGB8888 = 0,
    WL_SHM_FORMAT_XRGB8888 = 1,
    WL_SHM_FORMAT_BGRA8888 = 2,
    WL_SHM_FORMAT_BGRX8888 = 3,
    WL_SHM_FORMAT_RGB565 = 4,
    WL_SHM_FORMAT_ARGB4444 = 5
};
#endif

// Listener structures
struct wl_output_listener {
    void (*geometry)(void *data, struct wl_output *output, int32_t x, int32_t y,
                     int32_t physical_width, int32_t physical_height, int32_t subpixel,
                     const char *make, const char *model, int32_t transform);
    void (*mode)(void *data, struct wl_output *output, uint32_t flags,
                 int32_t width, int32_t height, int32_t refresh);
    void (*done)(void *data, struct wl_output *output);
    void (*scale)(void *data, struct wl_output *output, int32_t factor);
};

struct wl_keyboard_listener {
    void (*keymap)(void *data, struct wl_keyboard *keyboard, uint32_t format, int32_t fd, uint32_t size);
    void (*enter)(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface, struct wl_array *keys);
    void (*leave)(void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface);
    void (*key)(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
    void (*modifiers)(void *data, struct wl_keyboard *keyboard, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group);
    void (*repeat_info)(void *data, struct wl_keyboard *keyboard, int32_t rate, int32_t delay);
};

struct wl_pointer_listener {
    void (*enter)(void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y);
    void (*leave)(void *data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface);
    void (*motion)(void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y);
    void (*button)(void *data, struct wl_pointer *pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
    void (*axis)(void *data, struct wl_pointer *pointer, uint32_t time, uint32_t axis, wl_fixed_t value);
};

struct wl_touch_listener {
    void (*down)(void *data, struct wl_touch *touch, uint32_t serial, uint32_t time, struct wl_surface *surface, int32_t id, wl_fixed_t x, wl_fixed_t y);
    void (*up)(void *data, struct wl_touch *touch, uint32_t serial, uint32_t time, int32_t id);
    void (*motion)(void *data, struct wl_touch *touch, uint32_t time, int32_t id, wl_fixed_t x, wl_fixed_t y);
    void (*frame)(void *data, struct wl_touch *touch);
    void (*cancel)(void *data, struct wl_touch *touch);
};

struct wl_seat_listener {
    void (*capabilities)(void *data, struct wl_seat *seat, uint32_t capabilities);
    void (*name)(void *data, struct wl_seat *seat, const char *name);
};

struct wl_shm_listener {
    void (*format)(void *data, struct wl_shm *shm, uint32_t format);
};

struct wl_buffer_listener {
    void (*release)(void *data, struct wl_buffer *buffer);
};

struct wl_registry_listener {
    void (*global)(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version);
    void (*global_remove)(void *data, struct wl_registry *registry, uint32_t name);
};

struct wl_simple_shell_listener {
    void (*surface_id)(void *data, struct wl_simple_shell *shell, struct wl_surface *surface, uint32_t surfaceId);
    void (*surface_created)(void *data, struct wl_simple_shell *shell, uint32_t surfaceId, const char *name);
    void (*surface_destroyed)(void *data, struct wl_simple_shell *shell, uint32_t surfaceId, const char *name);
    void (*surface_status)(void *data, struct wl_simple_shell *shell, uint32_t surfaceId, const char *name,
                          uint32_t visible, int32_t x, int32_t y, int32_t width, int32_t height,
                          wl_fixed_t opacity, wl_fixed_t zorder);
    void (*get_surfaces_done)(void *data, struct wl_simple_shell *shell);
};

// Interface declarations
extern const struct wl_interface wl_compositor_interface;
extern const struct wl_interface wl_output_interface;
extern const struct wl_interface wl_seat_interface;
extern const struct wl_interface wl_shm_interface;
extern const struct wl_interface xdg_wm_base_interface;
extern const struct wl_interface xdg_surface_interface;
extern const struct wl_interface wl_vpc_interface;
extern const struct wl_interface wl_simple_shell_interface;

// Client display functions
struct wl_display* wl_display_connect(const char *name);
void wl_display_disconnect(struct wl_display *display);
int wl_display_dispatch(struct wl_display *display);
int wl_display_dispatch_pending(struct wl_display *display);
int wl_display_flush(struct wl_display *display);
int wl_display_roundtrip(struct wl_display *display);
int wl_display_get_fd(struct wl_display *display);
struct wl_registry* wl_display_get_registry(struct wl_display *display);

// Registry functions
void* wl_registry_bind(struct wl_registry *registry, uint32_t name, const struct wl_interface *interface, uint32_t version);
int wl_registry_add_listener(struct wl_registry *registry, const struct wl_registry_listener *listener, void *data);
void wl_registry_destroy(struct wl_registry *registry);

// Compositor functions
struct wl_surface* wl_compositor_create_surface(struct wl_compositor *compositor);
void wl_compositor_destroy(struct wl_compositor *compositor);

// Surface functions
void wl_surface_attach(struct wl_surface *surface, struct wl_buffer *buffer, int32_t x, int32_t y);
void wl_surface_damage(struct wl_surface *surface, int32_t x, int32_t y, int32_t width, int32_t height);
void wl_surface_commit(struct wl_surface *surface);
void wl_surface_destroy(struct wl_surface *surface);

// Output functions
int wl_output_add_listener(struct wl_output *output, const struct wl_output_listener *listener, void *data);
void wl_output_destroy(struct wl_output *output);

// Seat functions
int wl_seat_add_listener(struct wl_seat *seat, const struct wl_seat_listener *listener, void *data);
struct wl_keyboard* wl_seat_get_keyboard(struct wl_seat *seat);
struct wl_pointer* wl_seat_get_pointer(struct wl_seat *seat);
struct wl_touch* wl_seat_get_touch(struct wl_seat *seat);

// Keyboard functions
int wl_keyboard_add_listener(struct wl_keyboard *keyboard, const struct wl_keyboard_listener *listener, void *data);
void wl_keyboard_destroy(struct wl_keyboard *keyboard);

// Pointer functions
int wl_pointer_add_listener(struct wl_pointer *pointer, const struct wl_pointer_listener *listener, void *data);
void wl_pointer_set_cursor(struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, int32_t hotspot_x, int32_t hotspot_y);
void wl_pointer_destroy(struct wl_pointer *pointer);

// Touch functions
int wl_touch_add_listener(struct wl_touch *touch, const struct wl_touch_listener *listener, void *data);
void wl_touch_destroy(struct wl_touch *touch);

// SHM functions
int wl_shm_add_listener(struct wl_shm *shm, const struct wl_shm_listener *listener, void *data);
struct wl_shm_pool* wl_shm_create_pool(struct wl_shm *shm, int32_t fd, int32_t size);
void wl_shm_destroy(struct wl_shm *shm);

// SHM pool functions
struct wl_buffer* wl_shm_pool_create_buffer(struct wl_shm_pool *pool, int32_t offset, int32_t width, int32_t height, int32_t stride, uint32_t format);
void wl_shm_pool_resize(struct wl_shm_pool *pool, int32_t size);
void wl_shm_pool_destroy(struct wl_shm_pool *pool);

// Buffer functions
int wl_buffer_add_listener(struct wl_buffer *buffer, const struct wl_buffer_listener *listener, void *data);
void wl_buffer_destroy(struct wl_buffer *buffer);

// Simple shell functions
int wl_simple_shell_add_listener(struct wl_simple_shell *shell, const struct wl_simple_shell_listener *listener, void *data);
void wl_simple_shell_set_visible(struct wl_simple_shell *shell, uint32_t surfaceId, uint32_t visible);
void wl_simple_shell_set_geometry(struct wl_simple_shell *shell, uint32_t surfaceId, int32_t x, int32_t y, int32_t width, int32_t height);
void wl_simple_shell_set_zorder(struct wl_simple_shell *shell, uint32_t surfaceId, wl_fixed_t zorder);
void wl_simple_shell_set_opacity(struct wl_simple_shell *shell, uint32_t surfaceId, wl_fixed_t opacity);
void wl_simple_shell_set_name(struct wl_simple_shell *shell, uint32_t surfaceId, const char *name);
void wl_simple_shell_get_status(struct wl_simple_shell *shell, uint32_t surfaceId);
void wl_simple_shell_get_surfaces(struct wl_simple_shell *shell);
void wl_simple_shell_destroy(struct wl_simple_shell *shell);

// VPC geometry function (not in vpc-client-protocol.h)
void wl_vpc_surface_set_geometry(struct wl_vpc_surface *vpc_surface, int32_t x, int32_t y, int32_t width, int32_t height);

// Callback functions
struct wl_callback { int dummy; };
struct wl_callback_listener {
    void (*done)(void *data, struct wl_callback *callback, uint32_t callback_data);
};

int wl_callback_add_listener(struct wl_callback *callback, const struct wl_callback_listener *listener, void *data);
void wl_callback_destroy(struct wl_callback *callback);

// Surface functions
struct wl_callback* wl_surface_frame(struct wl_surface *surface);

// Seat functions
void wl_seat_destroy(struct wl_seat *seat);

// Output functions
void wl_output_destroy(struct wl_output *output);

// Touch/Pointer/Keyboard functions
void wl_touch_destroy(struct wl_touch *touch);
void wl_pointer_destroy(struct wl_pointer *pointer);
void wl_keyboard_destroy(struct wl_keyboard *keyboard);

// Proxy and event queue
struct wl_proxy { int dummy; };
struct wl_event_queue { int dummy; };

struct wl_proxy* wl_proxy_create(struct wl_proxy *factory, const struct wl_interface *interface);
void wl_proxy_destroy(struct wl_proxy *proxy);
int wl_proxy_add_listener(struct wl_proxy *proxy, void (**implementation)(void), void *data);
void wl_proxy_set_user_data(struct wl_proxy *proxy, void *user_data);
void* wl_proxy_get_user_data(struct wl_proxy *proxy);

#ifdef __cplusplus
}
#endif

#endif /* _WAYLAND_CLIENT_H */
