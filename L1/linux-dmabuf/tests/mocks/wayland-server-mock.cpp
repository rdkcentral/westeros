/*
 * Mock Implementations for Wayland Server Functions
 * Provides minimal functionality for testing
 */

#include "wayland-server.h"
#include "linux-dmabuf-unstable-v1-server-protocol.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

// Mock structures are now defined in wayland-server.h

// Mock interfaces
static const struct wl_interface mock_zwp_linux_dmabuf_v1_interface = {
    "zwp_linux_dmabuf_v1", 3
};

static const struct wl_interface mock_zwp_linux_buffer_params_v1_interface = {
    "zwp_linux_buffer_params_v1", 3
};

static const struct wl_interface mock_wl_buffer_interface = {
    "wl_buffer", 1
};

const struct wl_interface zwp_linux_dmabuf_v1_interface = mock_zwp_linux_dmabuf_v1_interface;
const struct wl_interface zwp_linux_buffer_params_v1_interface = mock_zwp_linux_buffer_params_v1_interface;
const struct wl_interface wl_buffer_interface = mock_wl_buffer_interface;

// Global resource counter for unique IDs
static uint32_t g_resource_id = 1000;

struct wl_global* wl_global_create(struct wl_display *display,
                                  const struct wl_interface *interface,
                                  int version,
                                  void *data,
                                  wl_global_bind_func_t bind)
{
    struct wl_global *global = (struct wl_global*)calloc(1, sizeof(struct wl_global));
    if (global) {
        global->display = display;
        global->interface = interface;
        global->data = data;
        global->bind = bind;
    }
    return global;
}

void wl_global_destroy(struct wl_global *global)
{
    if (global) {
        free(global);
    }
}

struct wl_resource* wl_resource_create(struct wl_client *client,
                                      const struct wl_interface *interface,
                                      int version,
                                      uint32_t id)
{
    struct wl_resource *resource = (struct wl_resource*)calloc(1, sizeof(struct wl_resource));
    if (resource) {
        resource->interface = interface;
        resource->version = version;
        resource->id = (id == 0) ? g_resource_id++ : id;
    }
    return resource;
}

void wl_resource_destroy(struct wl_resource *resource)
{
    if (resource) {
        if (resource->destroy) {
            resource->destroy(resource);
        }
        free(resource);
    }
}

void wl_resource_set_implementation(struct wl_resource *resource,
                                   const void *implementation,
                                   void *data,
                                   void (*destroy)(struct wl_resource *resource))
{
    if (resource) {
        resource->implementation = implementation;
        resource->data = data;
        resource->destroy = destroy;
    }
}

void* wl_resource_get_user_data(struct wl_resource *resource)
{
    return resource ? resource->data : NULL;
}

void wl_resource_set_user_data(struct wl_resource *resource, void *data)
{
    if (resource) {
        resource->data = data;
    }
}

uint32_t wl_resource_get_version(struct wl_resource *resource)
{
    return resource ? resource->version : 0;
}

int wl_resource_instance_of(struct wl_resource *resource,
                           const struct wl_interface *interface,
                           const void *implementation)
{
    if (!resource) return 0;
    return (resource->interface == interface && resource->implementation == implementation);
}

void wl_resource_post_error(struct wl_resource *resource,
                           uint32_t code,
                           const char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    printf("WAYLAND ERROR [code=%u]: ", code);
    vprintf(msg, args);
    printf("\n");
    va_end(args);
}

void wl_resource_post_no_memory(struct wl_resource *resource)
{
    printf("WAYLAND ERROR: No memory\n");
}

void wl_client_post_no_memory(struct wl_client *client)
{
    printf("WAYLAND ERROR: Client no memory\n");
}

// Protocol functions
void zwp_linux_dmabuf_v1_send_format(struct wl_resource *resource, uint32_t format)
{
    printf("Send format: 0x%08x\n", format);
}

void zwp_linux_dmabuf_v1_send_modifier(struct wl_resource *resource,
                                       uint32_t format,
                                       uint32_t modifier_hi,
                                       uint32_t modifier_lo)
{
    printf("Send modifier: format=0x%08x, mod=%08x%08x\n", format, modifier_hi, modifier_lo);
}

void zwp_linux_buffer_params_v1_send_created(struct wl_resource *resource,
                                             struct wl_resource *buffer)
{
    printf("Send buffer created\n");
}

void zwp_linux_buffer_params_v1_send_failed(struct wl_resource *resource)
{
    printf("Send buffer creation failed\n");
}

// ============================================================================
// Protocol Simulation Helpers for Testing
// ============================================================================

/**
 * @brief Simulate a client binding to a global interface
 * 
 * This function triggers the bind callback that was registered when the global
 * was created. This allows tests to exercise protocol handler code paths.
 * 
 * @param global The global interface to bind to
 * @param client The client performing the bind
 * @param version Protocol version requested by client
 * @param id Resource ID for the new binding
 */
void wl_global_simulate_bind(struct wl_global *global, 
                             struct wl_client *client,
                             uint32_t version,
                             uint32_t id)
{
    if (global && global->bind) {
        printf("MOCK: Simulating client bind to global (version=%u, id=%u)\n", version, id);
        global->bind(client, global->data, version, id);
    }
}

/**
 * @brief Get the bind callback from a global (for testing)
 * 
 * @param global The global interface
 * @return The bind callback function pointer
 */
wl_global_bind_func_t wl_global_get_bind_func(struct wl_global *global)
{
    return global ? global->bind : NULL;
}

/**
 * @brief Get the user data from a global (for testing)
 * 
 * @param global The global interface
 * @return The user data pointer
 */
void* wl_global_get_user_data(struct wl_global *global)
{
    return global ? global->data : NULL;
}

