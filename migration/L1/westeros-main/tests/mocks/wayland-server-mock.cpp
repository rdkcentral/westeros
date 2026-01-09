/*
 * Wayland Server Mock Implementation
 * Copyright 2024 RDK Management
 */
#include "wayland-server.h"
#include <cstdlib>
#include <cstring>
#include <map>
#include <vector>

// Mock internal structures
struct wl_display {
    bool running;
    uint32_t serial;
    wl_event_loop *event_loop;
    std::vector<wl_client*> clients;
};

struct wl_client {
    wl_display *display;
    void *user_data;
};

// wl_resource structure is defined in wayland-server.h

struct wl_event_loop {
    std::vector<wl_event_source*> sources;
};

struct wl_event_source {
    int type; // 0=fd, 1=timer
    union {
        struct {
            int fd;
            int (*func)(int fd, uint32_t mask, void *data);
        } fd_data;
        struct {
            int (*func)(void *data);
            int ms_delay;
        } timer_data;
    };
    void *data;
};

// Display functions
wl_display* wl_display_create(void) {
    wl_display *display = new wl_display();
    display->running = false;
    display->serial = 0;
    display->event_loop = new wl_event_loop();
    return display;
}

void wl_display_destroy(wl_display *display) {
    if (!display) return;
    
    for (auto client : display->clients) {
        delete client;
    }
    
    if (display->event_loop) {
        for (auto source : display->event_loop->sources) {
            delete source;
        }
        delete display->event_loop;
    }
    
    delete display;
}

int wl_display_add_socket(wl_display *display, const char *name) {
    if (!display) return -1;
    (void)name; // Unused in mock
    return 0;
}

void wl_display_run(wl_display *display) {
    if (!display) return;
    display->running = true;
}

void wl_display_terminate(wl_display *display) {
    if (!display) return;
    display->running = false;
}

wl_event_loop* wl_display_get_event_loop(wl_display *display) {
    return display ? display->event_loop : nullptr;
}

void wl_display_flush_clients(wl_display *display) {
    (void)display; // Mock does nothing
}

uint32_t wl_display_get_serial(wl_display *display) {
    return display ? display->serial : 0;
}

uint32_t wl_display_next_serial(wl_display *display) {
    return display ? ++display->serial : 0;
}

// Resource functions
wl_resource* wl_resource_create(wl_client *client, const wl_interface *interface, int version, uint32_t id) {
    wl_resource *resource = new wl_resource();
    resource->client = client;
    resource->interface = interface;
    resource->version = version;
    resource->id = id;
    resource->data = nullptr;
    resource->destroy = nullptr;
    return resource;
}

void wl_resource_destroy(wl_resource *resource) {
    if (!resource) return;
    if (resource->destroy) {
        resource->destroy(resource);
    }
    delete resource;
}

void wl_resource_set_implementation(wl_resource *resource, const void *implementation, void *data, void (*destroy)(wl_resource *resource)) {
    if (!resource) return;
    (void)implementation; // Not used in mock
    resource->data = data;
    resource->destroy = destroy;
}

void* wl_resource_get_user_data(wl_resource *resource) {
    return resource ? resource->data : nullptr;
}

void wl_resource_set_user_data(wl_resource *resource, void *data) {
    if (resource) {
        resource->data = data;
    }
}

wl_client* wl_resource_get_client(wl_resource *resource) {
    return resource ? resource->client : nullptr;
}

void wl_resource_post_event(wl_resource *resource, uint32_t opcode, ...) {
    (void)resource;
    (void)opcode;
    // Mock does nothing
}

void wl_resource_post_error(wl_resource *resource, uint32_t code, const char *msg, ...) {
    (void)resource;
    (void)code;
    (void)msg;
    // Mock does nothing
}

// Client functions
void wl_client_flush(wl_client *client) {
    (void)client;
}

void wl_client_destroy(wl_client *client) {
    delete client;
}

wl_display* wl_client_get_display(wl_client *client) {
    return client ? client->display : nullptr;
}

// Buffer server functions
void wl_buffer_send_release(wl_resource *resource) {
    if (!resource) return;
    // Mock implementation - in real code would send release event to client
}

// Signal functions
void wl_signal_init(wl_signal *signal) {
    if (!signal) return;
    wl_list_init(&signal->listener_list);
}

void wl_signal_add(wl_signal *signal, wl_listener *listener) {
    if (!signal || !listener) return;
    wl_list_insert(signal->listener_list.prev, &listener->link);
}

void wl_signal_emit(wl_signal *signal, void *data) {
    if (!signal) return;
    wl_listener *l, *next;
    wl_list_for_each_safe(l, next, &signal->listener_list, link) {
        if (l->notify) {
            l->notify(l, data);
        }
    }
}

// List functions
void wl_list_init(wl_list *list) {
    if (!list) return;
    list->prev = list;
    list->next = list;
}

void wl_list_insert(wl_list *list, wl_list *elm) {
    if (!list || !elm) return;
    elm->prev = list;
    elm->next = list->next;
    list->next = elm;
    elm->next->prev = elm;
}

void wl_list_remove(wl_list *elm) {
    if (!elm) return;
    elm->prev->next = elm->next;
    elm->next->prev = elm->prev;
    elm->next = nullptr;
    elm->prev = nullptr;
}

int wl_list_length(const wl_list *list) {
    if (!list) return 0;
    int count = 0;
    wl_list *e;
    for (e = list->next; e != list; e = e->next) {
        count++;
    }
    return count;
}

int wl_list_empty(const wl_list *list) {
    return !list || (list->next == list);
}

// Array functions
void wl_array_init(wl_array *array) {
    if (!array) return;
    array->size = 0;
    array->alloc = 0;
    array->data = nullptr;
}

void wl_array_release(wl_array *array) {
    if (!array) return;
    free(array->data);
    array->data = nullptr;
    array->size = 0;
    array->alloc = 0;
}

void* wl_array_add(wl_array *array, size_t size) {
    if (!array || size == 0) return nullptr;
    
    size_t new_size = array->size + size;
    if (new_size > array->alloc) {
        size_t new_alloc = (new_size + 15) & ~15; // Align to 16 bytes
        void *new_data = realloc(array->data, new_alloc);
        if (!new_data) return nullptr;
        array->data = new_data;
        array->alloc = new_alloc;
    }
    
    void *ptr = (char*)array->data + array->size;
    array->size = new_size;
    return ptr;
}

int wl_array_copy(wl_array *dest, wl_array *source) {
    if (!dest || !source) return -1;
    
    wl_array_release(dest);
    if (source->size == 0) return 0;
    
    dest->data = malloc(source->size);
    if (!dest->data) return -1;
    
    memcpy(dest->data, source->data, source->size);
    dest->size = source->size;
    dest->alloc = source->size;
    return 0;
}

// Event loop functions
wl_event_source* wl_event_loop_add_fd(wl_event_loop *loop, int fd, uint32_t mask, int (*func)(int fd, uint32_t mask, void *data), void *data) {
    if (!loop) return nullptr;
    
    wl_event_source *source = new wl_event_source();
    source->type = 0;
    source->fd_data.fd = fd;
    source->fd_data.func = func;
    source->data = data;
    loop->sources.push_back(source);
    return source;
}

wl_event_source* wl_event_loop_add_timer(wl_event_loop *loop, int (*func)(void *data), void *data) {
    if (!loop) return nullptr;
    
    wl_event_source *source = new wl_event_source();
    source->type = 1;
    source->timer_data.func = func;
    source->timer_data.ms_delay = 0;
    source->data = data;
    loop->sources.push_back(source);
    return source;
}

int wl_event_source_remove(wl_event_source *source) {
    if (!source) return -1;
    delete source;
    return 0;
}

int wl_event_source_timer_update(wl_event_source *source, int ms_delay) {
    if (!source || source->type != 1) return -1;
    source->timer_data.ms_delay = ms_delay;
    return 0;
}

void wl_event_loop_dispatch(wl_event_loop *loop, int timeout) {
    (void)loop;
    (void)timeout;
    // Mock does nothing
}

// SHM buffer mock structure
struct wl_shm_buffer {
    void *data;
    int32_t width;
    int32_t height;
    int32_t stride;
    uint32_t format;
};

wl_shm_buffer* wl_shm_buffer_get(wl_resource *resource) {
    (void)resource;
    return nullptr;
}

void wl_shm_buffer_begin_access(wl_shm_buffer *buffer) {
    (void)buffer;
}

void wl_shm_buffer_end_access(wl_shm_buffer *buffer) {
    (void)buffer;
}

void* wl_shm_buffer_get_data(wl_shm_buffer *buffer) {
    return buffer ? buffer->data : nullptr;
}

int32_t wl_shm_buffer_get_stride(wl_shm_buffer *buffer) {
    return buffer ? buffer->stride : 0;
}

uint32_t wl_shm_buffer_get_format(wl_shm_buffer *buffer) {
    return buffer ? buffer->format : 0;
}

int32_t wl_shm_buffer_get_width(wl_shm_buffer *buffer) {
    return buffer ? buffer->width : 0;
}

int32_t wl_shm_buffer_get_height(wl_shm_buffer *buffer) {
    return buffer ? buffer->height : 0;
}

// Global interface mocking
struct wl_global {
    const wl_interface *interface;
    void *data;
};

wl_global* wl_global_create(wl_display *display, const wl_interface *interface, int version, void *data, wl_global_bind_func_t bind) {
    (void)display;
    (void)version;
    (void)bind;
    wl_global *global = new wl_global();
    global->interface = interface;
    global->data = data;
    return global;
}

void wl_global_destroy(wl_global *global) {
    if (global) delete global;
}

// Display functions
const char* wl_display_add_socket_auto(wl_display *display) {
    (void)display;
    return "wayland-0"; // Mock socket name
}

int wl_display_init_shm(wl_display *display) {
    (void)display;
    return 0; // Success
}

// Client functions
void wl_client_get_credentials(wl_client *client, pid_t *pid, uid_t *uid, gid_t *gid) {
    (void)client;
    if (pid) *pid = 1000;
    if (uid) *uid = 1000;
    if (gid) *gid = 1000;
}

void wl_client_add_destroy_listener(wl_client *client, wl_listener *listener) {
    (void)client;
    (void)listener;
}

// Resource functions
void wl_resource_add_destroy_listener(wl_resource *resource, wl_listener *listener) {
    (void)resource;
    (void)listener;
}

wl_list* wl_resource_get_link(wl_resource *resource) {
    return resource ? &resource->link : nullptr;
}

uint32_t wl_resource_get_id(wl_resource *resource) {
    return resource ? resource->id : 0;
}

int wl_resource_get_version(wl_resource *resource) {
    return resource ? resource->version : 0;
}

bool wl_resource_instance_of(wl_resource *resource, const wl_interface *interface, const void *implementation) {
    (void)implementation;
    if (!resource || !interface) return false;
    return (resource->interface == interface);
}

void wl_resource_post_no_memory(wl_resource *resource) {
    (void)resource;
}

void wl_client_post_no_memory(wl_client *client) {
    (void)client;
}

// List manipulation functions
void wl_list_insert_list(wl_list *list, wl_list *other) {
    (void)list;
    (void)other;
    // Mock implementation - does nothing
}

// Protocol interface declarations (these need to be defined somewhere)
extern "C" {
    // Wayland core interfaces
    const wl_interface wl_compositor_interface = { "wl_compositor", 4, 0, nullptr, 0, nullptr };
    const wl_interface wl_surface_interface = { "wl_surface", 4, 0, nullptr, 0, nullptr };
    const wl_interface wl_region_interface = { "wl_region", 1, 0, nullptr, 0, nullptr };
    const wl_interface wl_shm_interface = { "wl_shm", 1, 0, nullptr, 0, nullptr };
    const wl_interface wl_shm_pool_interface = { "wl_shm_pool", 1, 0, nullptr, 0, nullptr };
    const wl_interface wl_buffer_interface = { "wl_buffer", 1, 0, nullptr, 0, nullptr };
    const wl_interface wl_callback_interface = { "wl_callback", 1, 0, nullptr, 0, nullptr };
    const wl_interface wl_output_interface = { "wl_output", 3, 0, nullptr, 0, nullptr };
    const wl_interface wl_seat_interface = { "wl_seat", 7, 0, nullptr, 0, nullptr };
    const wl_interface wl_pointer_interface = { "wl_pointer", 7, 0, nullptr, 0, nullptr };
    const wl_interface wl_keyboard_interface = { "wl_keyboard", 7, 0, nullptr, 0, nullptr };
    const wl_interface wl_touch_interface = { "wl_touch", 7, 0, nullptr, 0, nullptr };
    
    // Shell interfaces
    const wl_interface wl_shell_interface = { "wl_shell", 1, 0, nullptr, 0, nullptr };
    const wl_interface wl_shell_surface_interface = { "wl_shell_surface", 1, 0, nullptr, 0, nullptr };
    
    // XDG shell interfaces
    const wl_interface xdg_wm_base_interface = { "xdg_wm_base", 3, 0, nullptr, 0, nullptr };
    const wl_interface xdg_surface_interface = { "xdg_surface", 3, 0, nullptr, 0, nullptr };
    const wl_interface xdg_toplevel_interface = { "xdg_toplevel", 3, 0, nullptr, 0, nullptr };
    const wl_interface xdg_popup_interface = { "xdg_popup", 3, 0, nullptr, 0, nullptr };
    const wl_interface xdg_positioner_interface = { "xdg_positioner", 3, 0, nullptr, 0, nullptr };
    
    // VPC interface
    const wl_interface wl_vpc_surface_interface = { "wl_vpc_surface", 1, 0, nullptr, 0, nullptr };
    const wl_interface wl_vpc_interface = { "wl_vpc", 1, 0, nullptr, 0, nullptr };
}

// Protocol send functions
extern "C" {

void wl_keyboard_send_keymap(wl_resource *resource, uint32_t format, int32_t fd, uint32_t size) {
    (void)resource; (void)format; (void)fd; (void)size;
}

void wl_keyboard_send_enter(wl_resource *resource, uint32_t serial, wl_resource *surface, wl_array *keys) {
    (void)resource; (void)serial; (void)surface; (void)keys;
}

void wl_keyboard_send_leave(wl_resource *resource, uint32_t serial, wl_resource *surface) {
    (void)resource; (void)serial; (void)surface;
}

void wl_keyboard_send_key(wl_resource *resource, uint32_t serial, uint32_t time, uint32_t key, uint32_t state) {
    (void)resource; (void)serial; (void)time; (void)key; (void)state;
}

void wl_keyboard_send_modifiers(wl_resource *resource, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group) {
    (void)resource; (void)serial; (void)mods_depressed; (void)mods_latched; (void)mods_locked; (void)group;
}

void wl_keyboard_send_repeat_info(wl_resource *resource, int32_t rate, int32_t delay) {
    (void)resource; (void)rate; (void)delay;
}

void wl_pointer_send_enter(wl_resource *resource, uint32_t serial, wl_resource *surface, wl_fixed_t surface_x, wl_fixed_t surface_y) {
    (void)resource; (void)serial; (void)surface; (void)surface_x; (void)surface_y;
}

void wl_pointer_send_leave(wl_resource *resource, uint32_t serial, wl_resource *surface) {
    (void)resource; (void)serial; (void)surface;
}

void wl_pointer_send_motion(wl_resource *resource, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y) {
    (void)resource; (void)time; (void)surface_x; (void)surface_y;
}

void wl_pointer_send_button(wl_resource *resource, uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
    (void)resource; (void)serial; (void)time; (void)button; (void)state;
}

void wl_touch_send_down(wl_resource *resource, uint32_t serial, uint32_t time, wl_resource *surface, int32_t id, wl_fixed_t x, wl_fixed_t y) {
    (void)resource; (void)serial; (void)time; (void)surface; (void)id; (void)x; (void)y;
}

void wl_touch_send_up(wl_resource *resource, uint32_t serial, uint32_t time, int32_t id) {
    (void)resource; (void)serial; (void)time; (void)id;
}

void wl_touch_send_motion(wl_resource *resource, uint32_t time, int32_t id, wl_fixed_t x, wl_fixed_t y) {
    (void)resource; (void)time; (void)id; (void)x; (void)y;
}

void wl_touch_send_frame(wl_resource *resource) {
    (void)resource;
}

void wl_output_send_geometry(wl_resource *resource, int32_t x, int32_t y, int32_t physical_width, int32_t physical_height, int32_t subpixel, const char *make, const char *model, int32_t transform) {
    (void)resource; (void)x; (void)y; (void)physical_width; (void)physical_height; (void)subpixel; (void)make; (void)model; (void)transform;
}

void wl_output_send_mode(wl_resource *resource, uint32_t flags, int32_t width, int32_t height, int32_t refresh) {
    (void)resource; (void)flags; (void)width; (void)height; (void)refresh;
}

void wl_output_send_done(wl_resource *resource) {
    (void)resource;
}

void wl_output_send_scale(wl_resource *resource, int32_t factor) {
    (void)resource; (void)factor;
}

void wl_seat_send_capabilities(wl_resource *resource, uint32_t capabilities) {
    (void)resource; (void)capabilities;
}

void wl_seat_send_name(wl_resource *resource, const char *name) {
    (void)resource; (void)name;
}

void wl_shm_send_format(wl_resource *resource, uint32_t format) {
    (void)resource; (void)format;
}

void wl_callback_send_done(wl_resource *resource, uint32_t callback_data) {
    (void)resource; (void)callback_data;
}

}
