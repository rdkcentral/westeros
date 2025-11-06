#include "wayland-server.h"
#include "simpleshell/simpleshell-server-protocol.h"
#include "simpleshell/simplebuffer-server-protocol.h"
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <map>
#include <vector>
#include <unistd.h>

// Global state for mock implementation
static std::map<struct wl_display*, struct wl_event_loop*> g_display_loops;
static std::map<struct wl_event_loop*, std::vector<struct wl_event_source*>> g_loop_sources;
static std::map<struct wl_event_source*, wl_event_loop_timer_func_t> g_timer_callbacks;
static std::map<struct wl_event_source*, void*> g_timer_data;

// Mock Wayland server functions
struct wl_display* wl_display_create(void) {
    struct wl_display* display = (struct wl_display*)malloc(sizeof(struct wl_display));
    struct wl_event_loop* loop = (struct wl_event_loop*)malloc(sizeof(struct wl_event_loop));
    g_display_loops[display] = loop;
    return display;
}

void wl_display_destroy(struct wl_display *display) {
    if (display && g_display_loops.count(display)) {
        struct wl_event_loop* loop = g_display_loops[display];
        g_display_loops.erase(display);
        free(loop);
        free(display);
    }
}

struct wl_event_loop* wl_display_get_event_loop(struct wl_display *display) {
    if (display && g_display_loops.count(display)) {
        return g_display_loops[display];
    }
    return NULL;
}

struct wl_global* wl_global_create(struct wl_display *display, const struct wl_interface *interface, int version, void *data, wl_global_bind_func_t bind) {
    return (struct wl_global*)malloc(sizeof(struct wl_global));
}

void wl_global_destroy(struct wl_global *global) {
    if (global) {
        free(global);
    }
}

struct wl_resource* wl_resource_create(struct wl_client *client, const struct wl_interface *interface, int version, uint32_t id) {
    struct wl_resource* resource = (struct wl_resource*)malloc(sizeof(struct wl_resource));
    if (resource) {
        resource->interface = interface;
        resource->implementation = NULL;
        resource->data = NULL;
        resource->destroy = NULL;
        resource->version = version;
        resource->id = id;
    }
    return resource;
}

void* wl_resource_get_user_data(struct wl_resource *resource) {
    if (resource) {
        return resource->data;
    }
    return NULL;
}

void wl_resource_set_user_data(struct wl_resource *resource, void *data) {
    if (resource) {
        resource->data = data;
    }
}

uint32_t wl_resource_get_version(struct wl_resource *resource) {
    return resource ? resource->version : 0;
}

void wl_resource_set_implementation(struct wl_resource *resource, const void *implementation, void *data, void (*destroy)(struct wl_resource *resource)) {
    if (resource) {
        resource->implementation = (void*)implementation;
        resource->data = data;
        resource->destroy = destroy;
    }
}

void wl_client_post_no_memory(struct wl_client *client) {
    // Mock implementation - do nothing
}

struct wl_event_source* wl_event_loop_add_timer(struct wl_event_loop *loop, wl_event_loop_timer_func_t func, void *data) {
    struct wl_event_source* source = (struct wl_event_source*)malloc(sizeof(struct wl_event_source));
    g_timer_callbacks[source] = func;
    g_timer_data[source] = data;
    if (loop) {
        g_loop_sources[loop].push_back(source);
    }
    return source;
}

int wl_event_source_timer_update(struct wl_event_source *source, int ms_delay) {
    // Mock implementation - just return success
    return 0;
}

int wl_event_source_remove(struct wl_event_source *source) {
    if (source) {
        g_timer_callbacks.erase(source);
        g_timer_data.erase(source);
        free(source);
    }
    return 0;
}

// Define the interfaces that are extern-declared in protocol headers
const struct wl_interface wl_simple_shell_interface = {
    "wl_simple_shell",
    1,  // version
    0,  // method_count
    NULL,  // methods
    0,  // event_count
    NULL   // events
};

const struct wl_interface wl_sb_interface = {
    "wl_sb",
    3,  // version
    0,  // method_count
    NULL,  // methods
    0,  // event_count
    NULL   // events
};

const struct wl_interface wl_buffer_interface = {
    "wl_buffer",
    1,  // version
    0,  // method_count
    NULL,  // methods
    0,  // event_count
    NULL   // events
};

// Mock protocol functions
void wl_simple_shell_send_surface_created(struct wl_resource *resource, uint32_t surfaceId, const char *name) {
    // Mock implementation
}

void wl_simple_shell_send_surface_destroyed(struct wl_resource *resource, uint32_t surfaceId, const char *name) {
    // Mock implementation
}

void wl_simple_shell_send_surface_status(struct wl_resource *resource, uint32_t surfaceId, const char *name, uint32_t visible, int32_t x, int32_t y, int32_t width, int32_t height, wl_fixed_t opacity, wl_fixed_t zorder) {
    // Mock implementation
}

void wl_simple_shell_send_surface_id(struct wl_resource *resource, struct wl_resource *surface_resource, uint32_t surfaceId) {
    // Mock implementation
}

void wl_simple_shell_send_get_surfaces_done(struct wl_resource *resource) {
    // Mock implementation
}

// Simplebuffer protocol mock functions
void wl_resource_post_event(struct wl_resource *resource, uint32_t opcode, ...) {
    // Mock implementation
}

void wl_resource_post_error(struct wl_resource *resource, uint32_t code, const char *msg, ...) {
    // Mock implementation
}

void wl_resource_post_no_memory(struct wl_resource *resource) {
    // Mock implementation - resource out of memory error
    (void)resource;
}

void wl_resource_destroy(struct wl_resource *resource) {
    if (resource) {
        if (resource->destroy) {
            resource->destroy(resource);
        }
        free(resource);
    }
}

int wl_resource_instance_of(struct wl_resource *resource, const struct wl_interface *interface, const void *implementation) {
    // Mock implementation - return 1 for success
    return 1;
}