/*
 * Mock header for Wayland Server
 * Used for testing westeros-linux-dmabuf without full Wayland implementation
 */

#ifndef _WAYLAND_SERVER_MOCK_H
#define _WAYLAND_SERVER_MOCK_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
struct wl_buffer_interface;

// External interface declarations (defined in mock implementation)
extern const struct wl_interface wl_buffer_interface;

// Forward declaration of callback type
typedef void (*wl_global_bind_func_t)(struct wl_client *client, void *data,
                                      uint32_t version, uint32_t id);

// Mock structure definitions (needed for sizeof operations in tests)
struct wl_display {
    int dummy;
};

struct wl_client {
    int dummy;
};

struct wl_interface {
    const char *name;
    int version;
};

struct wl_resource {
    const struct wl_interface *interface;
    const void *implementation;
    void *data;
    void (*destroy)(struct wl_resource *resource);
    uint32_t version;
    uint32_t id;
};

struct wl_global {
    struct wl_display *display;
    const struct wl_interface *interface;
    void *data;
    wl_global_bind_func_t bind;
};

// Mock implementations
struct wl_global* wl_global_create(struct wl_display *display,
                                  const struct wl_interface *interface,
                                  int version,
                                  void *data,
                                  wl_global_bind_func_t bind);

void wl_global_destroy(struct wl_global *global);

struct wl_resource* wl_resource_create(struct wl_client *client,
                                      const struct wl_interface *interface,
                                      int version,
                                      uint32_t id);

void wl_resource_destroy(struct wl_resource *resource);

void wl_resource_set_implementation(struct wl_resource *resource,
                                   const void *implementation,
                                   void *data,
                                   void (*destroy)(struct wl_resource *resource));

void* wl_resource_get_user_data(struct wl_resource *resource);

void wl_resource_set_user_data(struct wl_resource *resource, void *data);

uint32_t wl_resource_get_version(struct wl_resource *resource);

int wl_resource_instance_of(struct wl_resource *resource,
                           const struct wl_interface *interface,
                           const void *implementation);

void wl_resource_post_error(struct wl_resource *resource,
                           uint32_t code,
                           const char *msg, ...);

void wl_resource_post_no_memory(struct wl_resource *resource);

void wl_client_post_no_memory(struct wl_client *client);

// Protocol simulation helpers (for testing protocol handlers)
void wl_global_simulate_bind(struct wl_global *global, 
                             struct wl_client *client,
                             uint32_t version,
                             uint32_t id);

wl_global_bind_func_t wl_global_get_bind_func(struct wl_global *global);

void* wl_global_get_user_data(struct wl_global *global);

#ifdef __cplusplus
}
#endif

#endif // _WAYLAND_SERVER_MOCK_H

