#ifndef MOCK_WAYLAND_SERVER_H
#define MOCK_WAYLAND_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

// Define complete structs for Wayland types to allow usage
struct wl_display {
    void* data;  // Opaque data for mock
};

struct wl_client {
    void* data;  // Opaque data for mock
};

struct wl_resource {
    const struct wl_interface *interface;  // Interface pointer
    const void *implementation;  // Implementation pointer
    void *data;  // User data pointer
    void (*destroy)(struct wl_resource *resource);  // Destroy callback
    uint32_t version;  // Version number
    uint32_t id;  // Resource ID
};

struct wl_global {
    void* data;  // Opaque data for mock
};

struct wl_event_loop {
    void* data;  // Opaque data for mock
};

struct wl_event_source {
    void* data;  // Opaque data pointer for mock implementation
};

// Define wl_interface structure to match Wayland protocol
struct wl_interface {
    const char *name;
    int version;
    int method_count;
    const void *methods;
    int event_count;
    const void *events;
};

// wl_buffer interface structure
struct wl_buffer_interface {
    void (*destroy)(struct wl_client *client, struct wl_resource *resource);
};

// Standard Wayland interfaces (extern declarations)
extern const struct wl_interface wl_buffer_interface;

// Wayland fixed point type
typedef int32_t wl_fixed_t;

// Function pointer types
typedef void (*wl_global_bind_func_t)(struct wl_client *client, void *data, uint32_t version, uint32_t id);
typedef int (*wl_event_loop_timer_func_t)(void *data);

// Mock functions for Wayland server API
struct wl_display* wl_display_create(void);
void wl_display_destroy(struct wl_display *display);
struct wl_event_loop* wl_display_get_event_loop(struct wl_display *display);

struct wl_global* wl_global_create(struct wl_display *display, const struct wl_interface *interface, int version, void *data, wl_global_bind_func_t bind);
void wl_global_destroy(struct wl_global *global);

struct wl_resource* wl_resource_create(struct wl_client *client, const struct wl_interface *interface, int version, uint32_t id);
void* wl_resource_get_user_data(struct wl_resource *resource);
void wl_resource_set_user_data(struct wl_resource *resource, void *data);
uint32_t wl_resource_get_version(struct wl_resource *resource);
void wl_resource_set_implementation(struct wl_resource *resource, const void *implementation, void *data, void (*destroy)(struct wl_resource *resource));
void wl_resource_destroy(struct wl_resource *resource);
int wl_resource_instance_of(struct wl_resource *resource, const struct wl_interface *interface, const void *implementation);
void wl_resource_post_event(struct wl_resource *resource, uint32_t opcode, ...);
void wl_resource_post_error(struct wl_resource *resource, uint32_t code, const char *msg, ...);
void wl_resource_post_no_memory(struct wl_resource *resource);

void wl_client_post_no_memory(struct wl_client *client);

struct wl_event_source* wl_event_loop_add_timer(struct wl_event_loop *loop, wl_event_loop_timer_func_t func, void *data);
int wl_event_source_timer_update(struct wl_event_source *source, int ms_delay);
int wl_event_source_remove(struct wl_event_source *source);

// Fixed point conversion functions
static inline double wl_fixed_to_double(wl_fixed_t f) {
    union {
        double d;
        int64_t i;
    } u;
    u.i = ((1023LL + 44LL) << 52) + (1LL << 51) + f;
    return u.d - (3LL << 43);
}

static inline wl_fixed_t wl_fixed_from_double(double d) {
    union {
        double d;
        int64_t i;
    } u;
    u.d = d + (3LL << 43);
    return u.i;
}

static inline int wl_fixed_to_int(wl_fixed_t f) {
    return f / 256;
}

static inline wl_fixed_t wl_fixed_from_int(int i) {
    return i * 256;
}

#ifdef __cplusplus
}
#endif

#endif /* MOCK_WAYLAND_SERVER_H */
