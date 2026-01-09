/*
 * Wayland Client Mock Implementation for L1 Testing
 * Copyright 2024 RDK Management
 */
#include "wayland-client.h"
#include <cstdlib>
#include <cstring>

// Interface declarations (extern to avoid multiple definition with wayland-server-mock.cpp)
extern "C" {
extern const struct wl_interface wl_compositor_interface;
extern const struct wl_interface wl_output_interface;
extern const struct wl_interface wl_seat_interface;
extern const struct wl_interface wl_shm_interface;
extern const struct wl_interface xdg_wm_base_interface;
extern const struct wl_interface xdg_surface_interface;
}

// Client-side specific interfaces
const struct wl_interface wl_simple_shell_interface = { "wl_simple_shell", 1, 0, nullptr, 0, nullptr };

// Client display functions
wl_display* wl_display_connect(const char *name) {
    (void)name;
    // Return a mock display object instead of nullptr to support nested connection testing
    static wl_display mockDisplay;
    return &mockDisplay;
}

void wl_display_disconnect(wl_display *display) {
    (void)display;
}

int wl_display_dispatch(wl_display *display) {
    (void)display;
    // Return -1 to prevent nested thread from running indefinitely in tests
    return -1;
}

int wl_display_dispatch_pending(wl_display *display) {
    (void)display;
    return 0;
}

int wl_display_flush(wl_display *display) {
    (void)display;
    return 0;
}

int wl_display_roundtrip(wl_display *display) {
    (void)display;
    return 0;
}

int wl_display_get_fd(wl_display *display) {
    (void)display;
    return -1;
}

wl_registry* wl_display_get_registry(wl_display *display) {
    (void)display;
    // Return a mock registry object instead of nullptr
    static wl_registry mockRegistry;
    return &mockRegistry;
}

// Registry functions
void* wl_registry_bind(wl_registry *registry, uint32_t name, const wl_interface *interface, uint32_t version) {
    (void)registry;
    (void)name;
    (void)version;
    
    // Return appropriate mock objects based on interface
    if (interface == &wl_compositor_interface) {
        static wl_compositor mockCompositor;
        return &mockCompositor;
    } else if (interface == &wl_output_interface) {
        static wl_output mockOutput;
        return &mockOutput;
    } else if (interface == &wl_seat_interface) {
        static wl_seat mockSeat;
        return &mockSeat;
    } else if (interface == &wl_shm_interface) {
        static wl_shm mockShm;
        return &mockShm;
    }
    
    // Return a generic mock object for unknown interfaces
    static char mockObject[64];
    return mockObject;
}

int wl_registry_add_listener(wl_registry *registry, const wl_registry_listener *listener, void *data) {
    // Call the global handler to register all interfaces for nested connection testing
    if (listener && listener->global) {
        // Simulate all Wayland interfaces being advertised
        listener->global(data, registry, 1, "wl_compositor", 4);
        listener->global(data, registry, 2, "wl_output", 2);
        listener->global(data, registry, 3, "wl_seat", 4);
        listener->global(data, registry, 4, "wl_shm", 1);
        listener->global(data, registry, 5, "wl_vpc", 1);
        listener->global(data, registry, 6, "wl_simple_shell", 1);
    }
    return 0;
}

void wl_registry_destroy(wl_registry *registry) {
    (void)registry;
}

// Compositor functions
wl_surface* wl_compositor_create_surface(wl_compositor *compositor) {
    (void)compositor;
    // Return a mock surface object instead of nullptr
    static wl_surface mockSurface;
    return &mockSurface;
}

void wl_compositor_destroy(wl_compositor *compositor) {
    (void)compositor;
}

// Surface functions
void wl_surface_attach(wl_surface *surface, wl_buffer *buffer, int32_t x, int32_t y) {
    (void)surface;
    (void)buffer;
    (void)x;
    (void)y;
}

void wl_surface_damage(wl_surface *surface, int32_t x, int32_t y, int32_t width, int32_t height) {
    (void)surface;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
}

void wl_surface_commit(wl_surface *surface) {
    (void)surface;
}

void wl_surface_destroy(wl_surface *surface) {
    (void)surface;
}

// Output functions
int wl_output_add_listener(wl_output *output, const wl_output_listener *listener, void *data) {
    // Trigger all output callbacks with mock data
    if (listener) {
        if (listener->geometry) {
            listener->geometry(data, output, 0, 0, 600, 400, 0, "Mock Vendor", "Mock Model", 0);
        }
        if (listener->mode) {
            listener->mode(data, output, 1, 1920, 1080, 60000);
        }
        if (listener->scale) {
            listener->scale(data, output, 1);
        }
        if (listener->done) {
            listener->done(data, output);
        }
    }
    return 0;
}

void wl_output_destroy(wl_output *output) {
    (void)output;
}

// Seat functions
int wl_seat_add_listener(wl_seat *seat, const wl_seat_listener *listener, void *data) {
    // Trigger seat callbacks with mock data
    if (listener) {
        if (listener->capabilities) {
            // Advertise keyboard, pointer, and touch capabilities
            listener->capabilities(data, seat, 7); // 1=pointer, 2=keyboard, 4=touch
        }
        if (listener->name) {
            listener->name(data, seat, "mock_seat");
        }
    }
    return 0;
}

wl_keyboard* wl_seat_get_keyboard(wl_seat *seat) {
    (void)seat;
    // Return a mock keyboard object
    static wl_keyboard mockKeyboard;
    return &mockKeyboard;
}

wl_pointer* wl_seat_get_pointer(wl_seat *seat) {
    (void)seat;
    // Return a mock pointer object
    static wl_pointer mockPointer;
    return &mockPointer;
}

wl_touch* wl_seat_get_touch(wl_seat *seat) {
    (void)seat;
    // Return a mock touch object
    static wl_touch mockTouch;
    return &mockTouch;
}

// Keyboard functions
int wl_keyboard_add_listener(wl_keyboard *keyboard, const wl_keyboard_listener *listener, void *data) {
    // Trigger keyboard callbacks with mock data
    if (listener) {
        // Mock keymap
        if (listener->keymap) {
            listener->keymap(data, keyboard, 1, -1, 0);
        }
        // Mock enter event
        if (listener->enter) {
            static wl_surface mockSurface;
            static wl_array mockKeys = {NULL, 0, 0};
            listener->enter(data, keyboard, 1, &mockSurface, &mockKeys);
        }
        // Mock leave event
        if (listener->leave) {
            static wl_surface mockSurface;
            listener->leave(data, keyboard, 2, &mockSurface);
        }
        // Mock key press
        if (listener->key) {
            listener->key(data, keyboard, 3, 1000, 28, 1); // Enter key pressed
        }
        // Mock modifiers
        if (listener->modifiers) {
            listener->modifiers(data, keyboard, 4, 0, 0, 0, 0);
        }
        // Mock repeat info
        if (listener->repeat_info) {
            listener->repeat_info(data, keyboard, 30, 250);
        }
    }
    return 0;
}

void wl_keyboard_destroy(wl_keyboard *keyboard) {
    (void)keyboard;
}

// Pointer functions
int wl_pointer_add_listener(wl_pointer *pointer, const wl_pointer_listener *listener, void *data) {
    // Trigger pointer callbacks with mock data
    if (listener) {
        // Mock enter event
        if (listener->enter) {
            static wl_surface mockSurface;
            listener->enter(data, pointer, 1, &mockSurface, 100 << 8, 200 << 8); // Fixed point coordinates
        }
        // Mock leave event
        if (listener->leave) {
            static wl_surface mockSurface;
            listener->leave(data, pointer, 2, &mockSurface);
        }
        // Mock motion event
        if (listener->motion) {
            listener->motion(data, pointer, 1000, 150 << 8, 250 << 8);
        }
        // Mock button event
        if (listener->button) {
            listener->button(data, pointer, 3, 2000, 272, 1); // Left button pressed
        }
        // Mock axis event
        if (listener->axis) {
            listener->axis(data, pointer, 3000, 0, 10 << 8); // Vertical scroll
        }
    }
    return 0;
}

void wl_pointer_set_cursor(wl_pointer *pointer, uint32_t serial, wl_surface *surface, int32_t hotspot_x, int32_t hotspot_y) {
    (void)pointer;
    (void)serial;
    (void)surface;
    (void)hotspot_x;
    (void)hotspot_y;
}

void wl_pointer_destroy(wl_pointer *pointer) {
    (void)pointer;
}

// Touch functions
int wl_touch_add_listener(wl_touch *touch, const wl_touch_listener *listener, void *data) {
    // Trigger touch callbacks with mock data
    if (listener) {
        // Mock touch down event
        if (listener->down) {
            static wl_surface mockSurface;
            listener->down(data, touch, 1, 1000, &mockSurface, 0, 100 << 8, 200 << 8);
        }
        // Mock touch motion event
        if (listener->motion) {
            listener->motion(data, touch, 2000, 0, 150 << 8, 250 << 8);
        }
        // Mock touch up event
        if (listener->up) {
            listener->up(data, touch, 3, 3000, 0);
        }
        // Mock touch frame event
        if (listener->frame) {
            listener->frame(data, touch);
        }
    }
    return 0;
}

void wl_touch_destroy(wl_touch *touch) {
    (void)touch;
}

// SHM functions
int wl_shm_add_listener(wl_shm *shm, const wl_shm_listener *listener, void *data) {
    // Trigger SHM format callbacks
    if (listener && listener->format) {
        // Advertise common SHM formats
        listener->format(data, shm, 0); // WL_SHM_FORMAT_ARGB8888
        listener->format(data, shm, 1); // WL_SHM_FORMAT_XRGB8888
    }
    return 0;
}

wl_shm_pool* wl_shm_create_pool(wl_shm *shm, int32_t fd, int32_t size) {
    (void)shm;
    (void)fd;
    (void)size;
    // Return a mock SHM pool object
    static wl_shm_pool mockPool;
    return &mockPool;
}

void wl_shm_destroy(wl_shm *shm) {
    (void)shm;
}

// SHM pool functions
wl_buffer* wl_shm_pool_create_buffer(wl_shm_pool *pool, int32_t offset, int32_t width, int32_t height, int32_t stride, uint32_t format) {
    (void)pool;
    (void)offset;
    (void)width;
    (void)height;
    (void)stride;
    (void)format;
    // Return a mock buffer object
    static wl_buffer mockBuffer;
    return &mockBuffer;
}

void wl_shm_pool_resize(wl_shm_pool *pool, int32_t size) {
    (void)pool;
    (void)size;
}

void wl_shm_pool_destroy(wl_shm_pool *pool) {
    (void)pool;
}

// Buffer functions
int wl_buffer_add_listener(wl_buffer *buffer, const wl_buffer_listener *listener, void *data) {
    // Trigger buffer release callback
    if (listener && listener->release) {
        listener->release(data, buffer);
    }
    return 0;
}

void wl_buffer_destroy(wl_buffer *buffer) {
    (void)buffer;
}

// Simple shell functions
int wl_simple_shell_add_listener(wl_simple_shell *shell, const wl_simple_shell_listener *listener, void *data) {
    // Trigger simple shell callbacks with mock data
    if (listener) {
        // Mock surface created event
        if (listener->surface_created) {
            listener->surface_created(data, shell, 1, "mock_client");
        }
        // Mock surface ID assignment
        if (listener->surface_id) {
            static wl_surface mockSurface;
            listener->surface_id(data, shell, &mockSurface, 1);
        }
        // Mock surface status event
        if (listener->surface_status) {
            listener->surface_status(data, shell, 1, "mock_client", 1, 0, 0, 1920, 1080, wl_fixed_from_int(1), wl_fixed_from_int(0));
        }
        // Mock surface destroyed event
        if (listener->surface_destroyed) {
            listener->surface_destroyed(data, shell, 2, "mock_client");
        }
        // Mock get surfaces done
        if (listener->get_surfaces_done) {
            listener->get_surfaces_done(data, shell);
        }
    }
    return 0;
}

void wl_simple_shell_set_visible(wl_simple_shell *shell, uint32_t surfaceId, uint32_t visible) {
    (void)shell;
    (void)surfaceId;
    (void)visible;
}

void wl_simple_shell_set_geometry(wl_simple_shell *shell, uint32_t surfaceId, int32_t x, int32_t y, int32_t width, int32_t height) {
    (void)shell;
    (void)surfaceId;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
}

void wl_simple_shell_set_zorder(wl_simple_shell *shell, uint32_t surfaceId, wl_fixed_t zorder) {
    (void)shell;
    (void)surfaceId;
    (void)zorder;
}

void wl_simple_shell_set_opacity(wl_simple_shell *shell, uint32_t surfaceId, wl_fixed_t opacity) {
    (void)shell;
    (void)surfaceId;
    (void)opacity;
}

// Proxy functions
wl_proxy* wl_proxy_create(wl_proxy *factory, const wl_interface *interface) {
    (void)factory;
    (void)interface;
    return nullptr;
}

void wl_proxy_destroy(wl_proxy *proxy) {
    (void)proxy;
}

int wl_proxy_add_listener(wl_proxy *proxy, void (**implementation)(void), void *data) {
    (void)proxy;
    (void)implementation;
    (void)data;
    return 0;
}

void wl_proxy_set_user_data(wl_proxy *proxy, void *user_data) {
    (void)proxy;
    (void)user_data;
}

void* wl_proxy_get_user_data(wl_proxy *proxy) {
    (void)proxy;
    return nullptr;
}

void wl_simple_shell_set_name(wl_simple_shell *shell, uint32_t surfaceId, const char *name) {
    (void)shell;
    (void)surfaceId;
    (void)name;
}

void wl_simple_shell_get_status(wl_simple_shell *shell, uint32_t surfaceId) {
    (void)shell;
    (void)surfaceId;
}

void wl_simple_shell_get_surfaces(wl_simple_shell *shell) {
    (void)shell;
}

void wl_simple_shell_destroy(wl_simple_shell *shell) {
    (void)shell;
}

// VPC geometry function (not in vpc-client-protocol.h)
void wl_vpc_surface_set_geometry(wl_vpc_surface *vpc_surface, int32_t x, int32_t y, int32_t width, int32_t height) {
    (void)vpc_surface;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
}

int wl_callback_add_listener(wl_callback *callback, const wl_callback_listener *listener, void *data) {
    (void)callback;
    (void)listener;
    (void)data;
    return 0;
}

void wl_callback_destroy(wl_callback *callback) {
    (void)callback;
}

wl_callback* wl_surface_frame(wl_surface *surface) {
    (void)surface;
    return nullptr;
}

void wl_seat_destroy(wl_seat *seat) {
    (void)seat;
}
