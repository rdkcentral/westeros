/*
 * Wayland Server Mock for L1 Testing
 * Copyright 2024 RDK Management
 */
#ifndef _WAYLAND_SERVER_H
#define _WAYLAND_SERVER_H

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

// Forward declarations
struct wl_display;
struct wl_client;
struct wl_resource;
struct wl_listener;
struct wl_signal;
struct wl_event_loop;
struct wl_event_source;
struct wl_interface;

// wl_list structure (must be defined before wl_resource)
struct wl_list {
    struct wl_list *prev;
    struct wl_list *next;
};

// Ensure wl_resource is fully defined for offsetof usage
struct wl_resource {
    struct wl_list link;
    struct wl_client *client;
    const struct wl_interface *interface;
    void *data;
    int version;
    uint32_t id;
    void (*destroy)(struct wl_resource *resource);
};

#ifdef __cplusplus
extern "C" {
#endif

// wl_array structure
#ifndef WL_ARRAY_DEFINED
#define WL_ARRAY_DEFINED
struct wl_array {
    size_t size;
    size_t alloc;
    void *data;
};
#endif

// wl_listener structure
struct wl_listener {
    struct wl_list link;
    void (*notify)(struct wl_listener *listener, void *data);
};

// wl_signal structure
struct wl_signal {
    struct wl_list listener_list;
};

// wl_interface structure
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

// Forward declarations for global
struct wl_global;
typedef void (*wl_global_bind_func_t)(struct wl_client *client, void *data, uint32_t version, uint32_t id);

// Display functions
struct wl_display* wl_display_create(void);
void wl_display_destroy(struct wl_display *display);
int wl_display_add_socket(struct wl_display *display, const char *name);
const char* wl_display_add_socket_auto(struct wl_display *display);
int wl_display_get_fd(struct wl_display *display);
int wl_display_init_shm(struct wl_display *display);
void wl_display_run(struct wl_display *display);
void wl_display_terminate(struct wl_display *display);
struct wl_event_loop* wl_display_get_event_loop(struct wl_display *display);
void wl_display_flush_clients(struct wl_display *display);
uint32_t wl_display_get_serial(struct wl_display *display);
uint32_t wl_display_next_serial(struct wl_display *display);

// Resource functions
struct wl_resource* wl_resource_create(struct wl_client *client, const struct wl_interface *interface, int version, uint32_t id);
void wl_resource_destroy(struct wl_resource *resource);
void wl_resource_set_implementation(struct wl_resource *resource, const void *implementation, void *data, void (*destroy)(struct wl_resource *resource));
void* wl_resource_get_user_data(struct wl_resource *resource);
void wl_resource_set_user_data(struct wl_resource *resource, void *data);
struct wl_client* wl_resource_get_client(struct wl_resource *resource);
uint32_t wl_resource_get_id(struct wl_resource *resource);
int wl_resource_get_version(struct wl_resource *resource);
bool wl_resource_instance_of(struct wl_resource *resource, const struct wl_interface *interface, const void *implementation);
struct wl_list* wl_resource_get_link(struct wl_resource *resource);
void wl_resource_post_event(struct wl_resource *resource, uint32_t opcode, ...);
void wl_resource_post_error(struct wl_resource *resource, uint32_t code, const char *msg, ...);
void wl_resource_post_no_memory(struct wl_resource *resource);

// Resource iteration macro
#ifndef wl_resource_for_each
#define wl_resource_for_each(resource, list) \
    for (resource = wl_resource_from_link((list)->next); \
         wl_resource_get_link(resource) != (list); \
         resource = wl_resource_from_link(wl_resource_get_link(resource)->next))
#endif

#ifndef wl_resource_for_each_safe
#define wl_resource_for_each_safe(resource, tmp, list) \
    for (resource = wl_resource_from_link((list)->next), \
         tmp = wl_resource_from_link(wl_resource_get_link(resource)->next); \
         wl_resource_get_link(resource) != (list); \
         resource = tmp, \
         tmp = wl_resource_from_link(wl_resource_get_link(resource)->next))
#endif

static inline struct wl_resource* wl_resource_from_link(struct wl_list *resource) {
    return (struct wl_resource*)resource;
}

// Buffer server functions
void wl_buffer_send_release(struct wl_resource *resource);

// Callback functions
void wl_callback_send_done(struct wl_resource *resource, uint32_t callback_data);

// Client functions
void wl_client_flush(struct wl_client *client);
void wl_client_destroy(struct wl_client *client);
struct wl_display* wl_client_get_display(struct wl_client *client);
void wl_client_get_credentials(struct wl_client *client, pid_t *pid, uid_t *uid, gid_t *gid);
void wl_client_post_no_memory(struct wl_client *client);
void wl_client_add_destroy_listener(struct wl_client *client, struct wl_listener *listener);
void wl_resource_add_destroy_listener(struct wl_resource *resource, struct wl_listener *listener);

// Global functions
struct wl_global* wl_global_create(struct wl_display *display, const struct wl_interface *interface, int version, void *data, void (*bind)(struct wl_client *client, void *data, uint32_t version, uint32_t id));
void wl_global_destroy(struct wl_global *global);

// Signal functions
void wl_signal_init(struct wl_signal *signal);
void wl_signal_add(struct wl_signal *signal, struct wl_listener *listener);
void wl_signal_emit(struct wl_signal *signal, void *data);

// List functions
void wl_list_init(struct wl_list *list);
void wl_list_insert(struct wl_list *list, struct wl_list *elm);
void wl_list_insert_list(struct wl_list *list, struct wl_list *other);
void wl_list_remove(struct wl_list *elm);
int wl_list_length(const struct wl_list *list);
int wl_list_empty(const struct wl_list *list);

// Array functions
void wl_array_init(struct wl_array *array);
void wl_array_release(struct wl_array *array);
void* wl_array_add(struct wl_array *array, size_t size);
int wl_array_copy(struct wl_array *dest, struct wl_array *source);

// Event loop functions
struct wl_event_source* wl_event_loop_add_fd(struct wl_event_loop *loop, int fd, uint32_t mask, int (*func)(int fd, uint32_t mask, void *data), void *data);
struct wl_event_source* wl_event_loop_add_timer(struct wl_event_loop *loop, int (*func)(void *data), void *data);
int wl_event_source_remove(struct wl_event_source *source);
int wl_event_source_timer_update(struct wl_event_source *source, int ms_delay);
void wl_event_loop_dispatch(struct wl_event_loop *loop, int timeout);

// Fixed point conversion
#ifndef wl_fixed_from_double
#define wl_fixed_from_double(d) ((int32_t)((d) * 256.0))
#endif
#ifndef wl_fixed_to_double
#define wl_fixed_to_double(f) ((double)(f) / 256.0)
#endif
#ifndef wl_fixed_from_int
#define wl_fixed_from_int(i) ((i) * 256)
#endif
#ifndef wl_fixed_to_int
#define wl_fixed_to_int(f) ((f) / 256)
#endif

#ifndef WL_FIXED_T_DEFINED
#define WL_FIXED_T_DEFINED
typedef int32_t wl_fixed_t;
#endif

// Container of macro - using variable-based pattern to avoid GNU statement expressions
#ifndef wl_container_of
#define wl_container_of(ptr, sample, member) \
    (__typeof__(sample))((char *)(ptr) - offsetof(__typeof__(*(sample)), member))
#endif

// List iteration macros
#ifndef wl_list_for_each
#define wl_list_for_each(pos, head, member)                         \
    for (pos = wl_container_of((head)->next, pos, member);         \
         &pos->member != (head);                                    \
         pos = wl_container_of(pos->member.next, pos, member))
#endif

#ifndef wl_list_for_each_safe
#define wl_list_for_each_safe(pos, tmp, head, member)              \
    for (pos = wl_container_of((head)->next, pos, member),         \
         tmp = wl_container_of((pos)->member.next, tmp, member);   \
         &pos->member != (head);                                    \
         pos = tmp, tmp = wl_container_of(pos->member.next, tmp, member))
#endif

// Wayland protocol interface structures (forward declarations for protocol implementations)
struct wl_shm_interface {
    void (*create_pool)(struct wl_client *client, struct wl_resource *resource, uint32_t id, int fd, int32_t size);
};

struct wl_shm_pool_interface {
    void (*create_buffer)(struct wl_client *client, struct wl_resource *resource, uint32_t id, int32_t offset, int32_t width, int32_t height, int32_t stride, uint32_t format);
    void (*destroy)(struct wl_client *client, struct wl_resource *resource);
    void (*resize)(struct wl_client *client, struct wl_resource *resource, int32_t size);
};

struct wl_buffer_interface {
    void (*destroy)(struct wl_client *client, struct wl_resource *resource);
};

struct wl_compositor_interface {
    void (*create_surface)(struct wl_client *client, struct wl_resource *resource, uint32_t id);
    void (*create_region)(struct wl_client *client, struct wl_resource *resource, uint32_t id);
};

struct wl_surface_interface {
    void (*destroy)(struct wl_client *client, struct wl_resource *resource);
    void (*attach)(struct wl_client *client, struct wl_resource *resource, struct wl_resource *buffer, int32_t x, int32_t y);
    void (*damage)(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height);
    void (*frame)(struct wl_client *client, struct wl_resource *resource, uint32_t callback);
    void (*set_opaque_region)(struct wl_client *client, struct wl_resource *resource, struct wl_resource *region);
    void (*set_input_region)(struct wl_client *client, struct wl_resource *resource, struct wl_resource *region);
    void (*commit)(struct wl_client *client, struct wl_resource *resource);
    void (*set_buffer_transform)(struct wl_client *client, struct wl_resource *resource, int32_t transform);
    void (*set_buffer_scale)(struct wl_client *client, struct wl_resource *resource, int32_t scale);
};

struct wl_region_interface {
    void (*destroy)(struct wl_client *client, struct wl_resource *resource);
    void (*add)(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height);
    void (*subtract)(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height);
};

struct wl_shell_interface {
    void (*get_shell_surface)(struct wl_client *client, struct wl_resource *resource, uint32_t id, struct wl_resource *surface);
};

struct wl_shell_surface_interface {
    void (*pong)(struct wl_client *client, struct wl_resource *resource, uint32_t serial);
    void (*move)(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial);
    void (*resize)(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial, uint32_t edges);
    void (*set_toplevel)(struct wl_client *client, struct wl_resource *resource);
    void (*set_transient)(struct wl_client *client, struct wl_resource *resource, struct wl_resource *parent, int32_t x, int32_t y, uint32_t flags);
    void (*set_fullscreen)(struct wl_client *client, struct wl_resource *resource, uint32_t method, uint32_t framerate, struct wl_resource *output);
    void (*set_popup)(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial, struct wl_resource *parent, int32_t x, int32_t y, uint32_t flags);
    void (*set_maximized)(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output);
    void (*set_title)(struct wl_client *client, struct wl_resource *resource, const char *title);
    void (*set_class)(struct wl_client *client, struct wl_resource *resource, const char *class_);
};

struct wl_seat_interface {
    void (*get_pointer)(struct wl_client *client, struct wl_resource *resource, uint32_t id);
    void (*get_keyboard)(struct wl_client *client, struct wl_resource *resource, uint32_t id);
    void (*get_touch)(struct wl_client *client, struct wl_resource *resource, uint32_t id);
    void (*release)(struct wl_client *client, struct wl_resource *resource);
};

struct wl_keyboard_interface {
    void (*release)(struct wl_client *client, struct wl_resource *resource);
};

struct wl_pointer_interface {
    void (*set_cursor)(struct wl_client *client, struct wl_resource *resource, uint32_t serial, struct wl_resource *surface, int32_t hotspot_x, int32_t hotspot_y);
    void (*release)(struct wl_client *client, struct wl_resource *resource);
};

struct wl_touch_interface {
    void (*release)(struct wl_client *client, struct wl_resource *resource);
};

struct wl_vpc_interface {
    void (*get_vpc_surface)(struct wl_client *client, struct wl_resource *resource, uint32_t id, struct wl_resource *surface);
};

struct wl_vpc_surface_interface {
    void (*set_geometry)(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height);
    void (*set_geometry_with_crop)(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height, int32_t crop_x, int32_t crop_y, int32_t crop_width, int32_t crop_height);
};

// Protocol interface globals (these must be defined somewhere in the implementation)
extern const struct wl_interface wl_compositor_interface;
extern const struct wl_interface wl_surface_interface;
extern const struct wl_interface wl_region_interface;
extern const struct wl_interface wl_buffer_interface;
extern const struct wl_interface wl_shm_interface;
extern const struct wl_interface wl_shm_pool_interface;
extern const struct wl_interface wl_shell_interface;
extern const struct wl_interface wl_shell_surface_interface;
extern const struct wl_interface wl_seat_interface;
extern const struct wl_interface wl_keyboard_interface;
extern const struct wl_interface wl_pointer_interface;
extern const struct wl_interface wl_touch_interface;
extern const struct wl_interface wl_output_interface;
extern const struct wl_interface xdg_wm_base_interface;
extern const struct wl_interface xdg_surface_interface;
extern const struct wl_interface xdg_toplevel_interface;
extern const struct wl_interface xdg_popup_interface;
extern const struct wl_interface xdg_positioner_interface;
extern const struct wl_interface wl_vpc_interface;
extern const struct wl_interface wl_vpc_surface_interface;

// Protocol event sending functions
void wl_shm_send_format(struct wl_resource *resource, uint32_t format);
void wl_keyboard_send_keymap(struct wl_resource *resource, uint32_t format, int fd, uint32_t size);
void wl_keyboard_send_enter(struct wl_resource *resource, uint32_t serial, struct wl_resource *surface, struct wl_array *keys);
void wl_keyboard_send_leave(struct wl_resource *resource, uint32_t serial, struct wl_resource *surface);
void wl_keyboard_send_key(struct wl_resource *resource, uint32_t serial, uint32_t time, uint32_t key, uint32_t state);
void wl_keyboard_send_modifiers(struct wl_resource *resource, uint32_t serial, uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group);
void wl_keyboard_send_repeat_info(struct wl_resource *resource, int32_t rate, int32_t delay);
void wl_pointer_send_enter(struct wl_resource *resource, uint32_t serial, struct wl_resource *surface, wl_fixed_t surface_x, wl_fixed_t surface_y);
void wl_pointer_send_leave(struct wl_resource *resource, uint32_t serial, struct wl_resource *surface);
void wl_pointer_send_motion(struct wl_resource *resource, uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y);
void wl_pointer_send_button(struct wl_resource *resource, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
void wl_touch_send_down(struct wl_resource *resource, uint32_t serial, uint32_t time, struct wl_resource *surface, int32_t id, wl_fixed_t x, wl_fixed_t y);
void wl_touch_send_up(struct wl_resource *resource, uint32_t serial, uint32_t time, int32_t id);
void wl_touch_send_motion(struct wl_resource *resource, uint32_t time, int32_t id, wl_fixed_t x, wl_fixed_t y);
void wl_touch_send_frame(struct wl_resource *resource);
void wl_seat_send_capabilities(struct wl_resource *resource, uint32_t capabilities);
void wl_seat_send_name(struct wl_resource *resource, const char *name);
void wl_output_send_geometry(struct wl_resource *resource, int32_t x, int32_t y, int32_t physical_width, int32_t physical_height, int32_t subpixel, const char *make, const char *model, int32_t transform);
void wl_output_send_mode(struct wl_resource *resource, uint32_t flags, int32_t width, int32_t height, int32_t refresh);
void wl_output_send_done(struct wl_resource *resource);
void wl_output_send_scale(struct wl_resource *resource, int32_t factor);

// Wayland protocol constants
#define WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION 4
#define WL_SEAT_NAME_SINCE_VERSION 2
#define WL_OUTPUT_SCALE_SINCE_VERSION 2
#define WL_OUTPUT_DONE_SINCE_VERSION 2
#define WL_OUTPUT_SUBPIXEL_HORIZONTAL_RGB 1
#define WL_OUTPUT_TRANSFORM_NORMAL 0
#define WL_DISPLAY_ERROR_INVALID_OBJECT 0
#define WL_POINTER_BUTTON_STATE_RELEASED 0
#define WL_POINTER_BUTTON_STATE_PRESSED 1

// Callback interface
extern const struct wl_interface wl_callback_interface;

// SHM buffer support
struct wl_shm_buffer;

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

struct wl_shm_buffer* wl_shm_buffer_get(struct wl_resource *resource);
void wl_shm_buffer_begin_access(struct wl_shm_buffer *buffer);
void wl_shm_buffer_end_access(struct wl_shm_buffer *buffer);
void* wl_shm_buffer_get_data(struct wl_shm_buffer *buffer);
int32_t wl_shm_buffer_get_stride(struct wl_shm_buffer *buffer);
uint32_t wl_shm_buffer_get_format(struct wl_shm_buffer *buffer);
int32_t wl_shm_buffer_get_width(struct wl_shm_buffer *buffer);
int32_t wl_shm_buffer_get_height(struct wl_shm_buffer *buffer);

#ifdef __cplusplus
}
#endif

#endif // _WAYLAND_SERVER_H
