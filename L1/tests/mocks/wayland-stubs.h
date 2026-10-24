#ifndef WAYLAND_STUBS_H
#define WAYLAND_STUBS_H

#if defined(UNIT_TEST) && !defined(WAYLAND_SERVER_H) && !defined(_WAYLAND_SERVER_H) && !defined(__WAYLAND_SERVER_H) && !defined(__WAYLAND_SERVER_H__) && !defined(WL_SERVER_H)

#define WAYLAND_SERVER_H 1
#define _WAYLAND_SERVER_H 1
#define __WAYLAND_SERVER_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/types.h>

#ifndef wl_container_of
#define wl_container_of(ptr, sample, member) \
    ((__typeof__(sample))((char *)(ptr) - offsetof(__typeof__(*(sample)), member)))
#endif

#ifndef wl_list_for_each
#define wl_list_for_each(pos, head, member) \
    for (pos = wl_container_of((head)->next, pos, member); \
         &pos->member != (head); \
         pos = wl_container_of(pos->member.next, pos, member))
#endif

#ifndef wl_list_for_each_safe
#define wl_list_for_each_safe(pos, tmp, head, member) \
    for (pos = wl_container_of((head)->next, pos, member), \
         tmp = wl_container_of((pos)->member.next, tmp, member); \
         &pos->member != (head); \
         pos = tmp, tmp = wl_container_of(pos->member.next, tmp, member))
#endif

typedef int32_t wl_fixed_t;

#define WL_FIXED_BITS 8
#define wl_fixed_from_int(i) ((wl_fixed_t)((i) << WL_FIXED_BITS))
#define wl_fixed_to_int(f) ((int32_t)((f) >> WL_FIXED_BITS))
#define wl_fixed_from_double(d) ((wl_fixed_t)((int64_t)((d) * 256.0 + ((d) >= 0 ? 0.5 : -0.5))))
#define wl_fixed_to_double(f) ((double)(f) / 256.0)

struct wl_display;
struct wl_client;
struct wl_event_loop;
struct wl_event_source;
struct wl_global;
struct wl_resource;
struct wl_listener;
struct wl_signal;

struct wl_list {
    struct wl_list *prev;
    struct wl_list *next;
};

struct wl_message {
    const char *name;
    const char *signature;
    const struct wl_interface **types;
};

struct wl_interface {
    const char *name;
    int version;
    int method_count;
    const struct wl_message *methods;
    int event_count;
    const struct wl_message *events;
};

struct wl_display {
    int dummy;
};

struct wl_client {
    int dummy;
};

struct wl_event_source {
    int type;
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

struct wl_event_loop {
    void *dummy;
};

struct wl_resource {
    void *data;
    void (*destroy)(struct wl_resource *resource);
    struct wl_list link;
    struct wl_list destroy_signal;
    struct wl_client *client;
    void *object;
    uint32_t id;
    const struct wl_interface *interface;
    uint32_t version;
    const void *implementation;
};

struct wl_global {
    const struct wl_interface *interface;
    void *data;
    void (*bind)(struct wl_client *client, void *data, uint32_t version, uint32_t id);
};

struct wl_listener {
    struct wl_list link;
    void (*notify)(struct wl_listener *listener, void *data);
};

struct wl_signal {
    struct wl_list listener_list;
};

struct wl_array {
    size_t size;
    size_t alloc;
    void *data;
};

/* Lightweight client-side proxy objects instantiated by the L1 client mock. */
struct wl_registry { int dummy; };
struct wl_compositor { int dummy; };
struct wl_surface { int dummy; };
struct wl_region { int dummy; };
struct wl_buffer { int dummy; };
struct wl_callback { int dummy; };
struct wl_shm { int dummy; };
struct wl_shm_pool { int dummy; };
struct wl_shell { int dummy; };
struct wl_shell_surface { int dummy; };
struct wl_seat { int dummy; };
struct wl_keyboard { int dummy; };
struct wl_pointer { int dummy; };
struct wl_touch { int dummy; };
struct wl_output { int dummy; };
struct wl_simple_shell;
struct wl_vpc { int dummy; };
struct wl_vpc_surface {
    int dummy;
};
struct wl_proxy;
struct wl_shm_buffer;

struct wl_registry_listener {
    void (*global)(void *data, struct wl_registry *registry, uint32_t id,
                   const char *interface, uint32_t version);
    void (*global_remove)(void *data, struct wl_registry *registry, uint32_t name);
};

struct wl_output_listener {
    void (*geometry)(void *data, struct wl_output *output, int32_t x, int32_t y,
                      int32_t physical_width, int32_t physical_height, int32_t subpixel,
                      const char *make, const char *model, int32_t transform);
    void (*mode)(void *data, struct wl_output *output, uint32_t flags, int32_t width,
                 int32_t height, int32_t refresh);
    void (*done)(void *data, struct wl_output *output);
    void (*scale)(void *data, struct wl_output *output, int32_t factor);
};

struct wl_seat_listener {
    void (*capabilities)(void *data, struct wl_seat *seat, uint32_t capabilities);
    void (*name)(void *data, struct wl_seat *seat, const char *name);
};

struct wl_keyboard_listener {
    void (*keymap)(void *data, struct wl_keyboard *keyboard, uint32_t format,
                   int32_t fd, uint32_t size);
    void (*enter)(void *data, struct wl_keyboard *keyboard, uint32_t serial,
                  struct wl_surface *surface, struct wl_array *keys);
    void (*leave)(void *data, struct wl_keyboard *keyboard, uint32_t serial,
                  struct wl_surface *surface);
    void (*key)(void *data, struct wl_keyboard *keyboard, uint32_t serial,
                uint32_t time, uint32_t key, uint32_t state);
    void (*modifiers)(void *data, struct wl_keyboard *keyboard, uint32_t serial,
                      uint32_t mods_depressed, uint32_t mods_latched,
                      uint32_t mods_locked, uint32_t group);
    void (*repeat_info)(void *data, struct wl_keyboard *keyboard, int32_t rate,
                        int32_t delay);
};

struct wl_pointer_listener {
    void (*enter)(void *data, struct wl_pointer *pointer, uint32_t serial,
                  struct wl_surface *surface, wl_fixed_t sx, wl_fixed_t sy);
    void (*leave)(void *data, struct wl_pointer *pointer, uint32_t serial,
                  struct wl_surface *surface);
    void (*motion)(void *data, struct wl_pointer *pointer, uint32_t time,
                   wl_fixed_t sx, wl_fixed_t sy);
    void (*button)(void *data, struct wl_pointer *pointer, uint32_t serial,
                   uint32_t time, uint32_t button, uint32_t state);
    void (*axis)(void *data, struct wl_pointer *pointer, uint32_t time,
                 uint32_t axis, wl_fixed_t value);
};

struct wl_touch_listener {
    void (*down)(void *data, struct wl_touch *touch, uint32_t serial, uint32_t time,
                 struct wl_surface *surface, int32_t id, wl_fixed_t sx, wl_fixed_t sy);
    void (*up)(void *data, struct wl_touch *touch, uint32_t serial, uint32_t time,
               int32_t id);
    void (*motion)(void *data, struct wl_touch *touch, uint32_t time, int32_t id,
                   wl_fixed_t sx, wl_fixed_t sy);
    void (*frame)(void *data, struct wl_touch *touch);
    void (*cancel)(void *data, struct wl_touch *touch);
};

struct wl_shm_listener {
    void (*format)(void *data, struct wl_shm *shm, uint32_t format);
};

struct wl_buffer_listener {
    void (*release)(void *data, struct wl_buffer *buffer);
};

struct wl_callback_listener {
    void (*done)(void *data, struct wl_callback *callback, uint32_t serial);
};

struct wl_simple_shell_listener {
    void (*surface_id)(void *data, struct wl_simple_shell *shell,
                       struct wl_surface *surface, uint32_t surface_id);
    void (*surface_created)(void *data, struct wl_simple_shell *shell,
                            uint32_t surface_id, const char *name);
    void (*surface_destroyed)(void *data, struct wl_simple_shell *shell,
                              uint32_t surface_id, const char *name);
    void (*surface_status)(void *data, struct wl_simple_shell *shell,
                           uint32_t surface_id, const char *name,
                           uint32_t visible, int32_t x, int32_t y,
                           int32_t width, int32_t height,
                           wl_fixed_t opacity, wl_fixed_t zorder);
    void (*get_surfaces_done)(void *data, struct wl_simple_shell *shell);
    void (*popup_details)(void *data, struct wl_simple_shell *shell,
                          uint32_t surface_id, uint32_t parent_surface_id,
                          int32_t popup);
};

struct wl_vpc_surface_listener {
    void (*video_path_change)(void *data, struct wl_vpc_surface *vpc_surface,
                              uint32_t new_pathway);
    void (*video_xform_change)(void *data, struct wl_vpc_surface *vpc_surface,
                               int32_t x_translation, int32_t y_translation,
                               uint32_t x_scale_num, uint32_t x_scale_denom,
                               uint32_t y_scale_num, uint32_t y_scale_denom,
                               uint32_t output_width, uint32_t output_height);
};

#ifndef WL_SHM_FORMAT_DEFINED
#define WL_SHM_FORMAT_DEFINED 1
enum wl_shm_format {
    WL_SHM_FORMAT_ARGB8888 = 0x34325241,
    WL_SHM_FORMAT_XRGB8888 = 0x34325258,
    WL_SHM_FORMAT_RGB565 = 0x00000006
};
#endif

#define WL_OUTPUT_MODE_CURRENT 0x1u
#define WL_OUTPUT_MODE_PREFERRED 0x2u
#define WL_OUTPUT_SUBPIXEL_HORIZONTAL_RGB 1
#define WL_OUTPUT_TRANSFORM_NORMAL 0
#define WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1 1u
#define WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION 4
#define WL_SEAT_NAME_SINCE_VERSION 2
#define WL_OUTPUT_SCALE_SINCE_VERSION 2
#define WL_OUTPUT_DONE_SINCE_VERSION 2
#define WL_DISPLAY_ERROR_INVALID_OBJECT 0
#define WL_KEYBOARD_KEY_STATE_RELEASED 0u
#define WL_KEYBOARD_KEY_STATE_PRESSED 1u
#define WL_POINTER_BUTTON_STATE_RELEASED 0u
#define WL_POINTER_BUTTON_STATE_PRESSED 1u
#define WL_SEAT_CAPABILITY_POINTER 1u
#define WL_SEAT_CAPABILITY_KEYBOARD 2u
#define WL_SEAT_CAPABILITY_TOUCH 4u

#ifndef WL_COMPOSITOR_INTERFACE_DEFINED
#define WL_COMPOSITOR_INTERFACE_DEFINED 1
struct wl_compositor_interface {
    void (*create_surface)(struct wl_client *client, struct wl_resource *resource, uint32_t id);
    void (*create_region)(struct wl_client *client, struct wl_resource *resource, uint32_t id);
};
#endif

#ifndef WL_SURFACE_INTERFACE_DEFINED
#define WL_SURFACE_INTERFACE_DEFINED 1
struct wl_surface_interface {
    void (*destroy)(struct wl_client *client, struct wl_resource *resource);
    void (*attach)(struct wl_client *client, struct wl_resource *resource,
                   struct wl_resource *buffer, int32_t x, int32_t y);
    void (*damage)(struct wl_client *client, struct wl_resource *resource,
                   int32_t x, int32_t y, int32_t width, int32_t height);
    void (*frame)(struct wl_client *client, struct wl_resource *resource, uint32_t callback_id);
    void (*set_opaque_region)(struct wl_client *client, struct wl_resource *resource,
                              struct wl_resource *region);
    void (*set_input_region)(struct wl_client *client, struct wl_resource *resource,
                             struct wl_resource *region);
    void (*commit)(struct wl_client *client, struct wl_resource *resource);
    void (*set_buffer_transform)(struct wl_client *client, struct wl_resource *resource,
                                int32_t transform);
    void (*set_buffer_scale)(struct wl_client *client, struct wl_resource *resource,
                             int32_t scale);
};
#endif

#ifndef WL_REGION_INTERFACE_DEFINED
#define WL_REGION_INTERFACE_DEFINED 1
struct wl_region_interface {
    void (*destroy)(struct wl_client *client, struct wl_resource *resource);
    void (*add)(struct wl_client *client, struct wl_resource *resource,
                int32_t x, int32_t y, int32_t width, int32_t height);
    void (*subtract)(struct wl_client *client, struct wl_resource *resource,
                      int32_t x, int32_t y, int32_t width, int32_t height);
};
#endif

#ifndef WL_BUFFER_INTERFACE_DEFINED
#define WL_BUFFER_INTERFACE_DEFINED 1
struct wl_buffer_interface {
    void (*destroy)(struct wl_client *client, struct wl_resource *resource);
};
#endif

#ifndef WL_SHM_INTERFACE_DEFINED
#define WL_SHM_INTERFACE_DEFINED 1
struct wl_shm_interface {
    void (*create_pool)(struct wl_client *client, struct wl_resource *resource,
                        uint32_t id, int32_t fd, int32_t size);
};
#endif

#ifndef WL_SHM_POOL_INTERFACE_DEFINED
#define WL_SHM_POOL_INTERFACE_DEFINED 1
struct wl_shm_pool_interface {
    void (*create_buffer)(struct wl_client *client, struct wl_resource *resource,
                          uint32_t id, int32_t offset, int32_t width,
                          int32_t height, int32_t stride, uint32_t format);
    void (*destroy)(struct wl_client *client, struct wl_resource *resource);
    void (*resize)(struct wl_client *client, struct wl_resource *resource, int32_t size);
};
#endif

#ifndef WL_SHELL_INTERFACE_DEFINED
#define WL_SHELL_INTERFACE_DEFINED 1
struct wl_shell_interface {
    void (*get_shell_surface)(struct wl_client *client, struct wl_resource *resource,
                              uint32_t id, struct wl_resource *surface);
};
#endif

#ifndef WL_SHELL_SURFACE_INTERFACE_DEFINED
#define WL_SHELL_SURFACE_INTERFACE_DEFINED 1
struct wl_shell_surface_interface {
    void (*pong)(struct wl_client *client, struct wl_resource *resource, uint32_t serial);
    void (*move)(struct wl_client *client, struct wl_resource *resource,
                 struct wl_resource *seat, uint32_t serial);
    void (*resize)(struct wl_client *client, struct wl_resource *resource,
                   struct wl_resource *seat, uint32_t serial, uint32_t edges);
    void (*set_toplevel)(struct wl_client *client, struct wl_resource *resource);
    void (*set_transient)(struct wl_client *client, struct wl_resource *resource,
                          struct wl_resource *parent, int32_t x, int32_t y, uint32_t flags);
    void (*set_fullscreen)(struct wl_client *client, struct wl_resource *resource,
                           uint32_t method, uint32_t framerate,
                           struct wl_resource *output);
    void (*set_popup)(struct wl_client *client, struct wl_resource *resource,
                      struct wl_resource *seat, uint32_t serial,
                      struct wl_resource *parent, int32_t x, int32_t y,
                      uint32_t flags);
    void (*set_maximized)(struct wl_client *client, struct wl_resource *resource,
                          struct wl_resource *output);
    void (*set_title)(struct wl_client *client, struct wl_resource *resource,
                      const char *title);
    void (*set_class)(struct wl_client *client, struct wl_resource *resource,
                      const char *class_name);
};
#endif

#ifndef WL_CALLBACK_INTERFACE_DEFINED
#define WL_CALLBACK_INTERFACE_DEFINED 1
struct wl_callback_interface {
    void (*destroy)(struct wl_client *client, struct wl_resource *resource);
};
#endif

#ifndef WL_OUTPUT_INTERFACE_DEFINED
#define WL_OUTPUT_INTERFACE_DEFINED 1
struct wl_output_interface {
    void (*release)(struct wl_client *client, struct wl_resource *resource);
};
#endif

#ifndef WL_SEAT_INTERFACE_DEFINED
#define WL_SEAT_INTERFACE_DEFINED 1
struct wl_seat_interface {
    void (*get_pointer)(struct wl_client *client, struct wl_resource *resource,
                        uint32_t id);
    void (*get_keyboard)(struct wl_client *client, struct wl_resource *resource,
                         uint32_t id);
    void (*get_touch)(struct wl_client *client, struct wl_resource *resource,
                     uint32_t id);
};
#endif

#ifndef WL_KEYBOARD_INTERFACE_DEFINED
#define WL_KEYBOARD_INTERFACE_DEFINED 1
struct wl_keyboard_interface {
    void (*release)(struct wl_client *client, struct wl_resource *resource);
};
#endif

#ifndef WL_POINTER_INTERFACE_DEFINED
#define WL_POINTER_INTERFACE_DEFINED 1
struct wl_pointer_interface {
    void (*set_cursor)(struct wl_client *client, struct wl_resource *resource,
                      uint32_t serial, struct wl_resource *surface,
                      int32_t hotspot_x, int32_t hotspot_y);
    void (*release)(struct wl_client *client, struct wl_resource *resource);
};
#endif

#ifndef WL_TOUCH_INTERFACE_DEFINED
#define WL_TOUCH_INTERFACE_DEFINED 1
struct wl_touch_interface {
    void (*release)(struct wl_client *client, struct wl_resource *resource);
};
#endif

typedef void (*wl_global_bind_func_t)(struct wl_client *client, void *data, uint32_t version, uint32_t id);
typedef int (*wl_event_loop_timer_func_t)(void *data);
typedef int (*wl_event_loop_fd_func_t)(int fd, uint32_t mask, void *data);

/* Component protocol interface tables are owned by their generated protocol headers. */
extern const struct wl_interface wl_compositor_interface;
extern const struct wl_interface wl_surface_interface;
extern const struct wl_interface wl_region_interface;
extern const struct wl_interface wl_buffer_interface;
extern const struct wl_interface wl_callback_interface;
extern const struct wl_interface wl_shm_interface;
extern const struct wl_interface wl_shm_pool_interface;
extern const struct wl_interface wl_shell_interface;
extern const struct wl_interface wl_shell_surface_interface;
extern const struct wl_interface wl_seat_interface;
extern const struct wl_interface wl_keyboard_interface;
extern const struct wl_interface wl_pointer_interface;
extern const struct wl_interface wl_touch_interface;
extern const struct wl_interface wl_output_interface;
extern const struct wl_interface wl_simple_shell_interface;
extern const struct wl_interface wl_vpc_interface;
extern const struct wl_interface wl_vpc_surface_interface;

void wl_list_init(struct wl_list *list);
void wl_list_insert(struct wl_list *list, struct wl_list *elm);
void wl_list_insert_list(struct wl_list *list, struct wl_list *other);
void wl_list_remove(struct wl_list *elm);
int wl_list_length(const struct wl_list *list);
int wl_list_empty(const struct wl_list *list);

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

void wl_signal_init(struct wl_signal *signal);
void wl_signal_add(struct wl_signal *signal, struct wl_listener *listener);
void wl_signal_emit(struct wl_signal *signal, void *data);

void wl_array_init(struct wl_array *array);
void wl_array_release(struct wl_array *array);
void* wl_array_add(struct wl_array *array, size_t size);
int wl_array_copy(struct wl_array *array, struct wl_array *source);

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
void wl_resource_post_event(struct wl_resource *resource, uint32_t opcode, ...);
void wl_resource_post_error(struct wl_resource *resource, uint32_t code, const char *msg, ...);
void wl_resource_post_no_memory(struct wl_resource *resource);
void* wl_resource_get_user_data(struct wl_resource *resource);
void wl_resource_set_user_data(struct wl_resource *resource, void *data);
void wl_resource_set_implementation(struct wl_resource *resource,
                                   const void *implementation,
                                   void *data,
                                   void (*destroy)(struct wl_resource *resource));
struct wl_client* wl_resource_get_client(struct wl_resource *resource);
void wl_resource_add_destroy_listener(struct wl_resource *resource, struct wl_listener *listener);
void wl_resource_get_destroy_listener(struct wl_resource *resource, void (*notify)(struct wl_listener *listener, void *data));
uint32_t wl_resource_get_id(struct wl_resource *resource);
uint32_t wl_resource_get_version(struct wl_resource *resource);
int wl_resource_instance_of(struct wl_resource *resource, const struct wl_interface *interface, const void *implementation);
struct wl_list* wl_resource_get_link(struct wl_resource *resource);
struct wl_resource* wl_resource_from_link(struct wl_list *link);
void wl_client_post_no_memory(struct wl_client *client);

void wl_buffer_send_release(struct wl_resource *resource);
void wl_callback_send_done(struct wl_resource *resource, uint32_t callback_data);
void wl_keyboard_send_keymap(struct wl_resource *resource, uint32_t format, int32_t fd, uint32_t size);
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
void wl_output_send_geometry(struct wl_resource *resource, int32_t x, int32_t y, int32_t physical_width, int32_t physical_height, int32_t subpixel, const char *make, const char *model, int32_t transform);
void wl_output_send_mode(struct wl_resource *resource, uint32_t flags, int32_t width, int32_t height, int32_t refresh);
void wl_output_send_done(struct wl_resource *resource);
void wl_output_send_scale(struct wl_resource *resource, int32_t factor);
void wl_seat_send_capabilities(struct wl_resource *resource, uint32_t capabilities);
void wl_seat_send_name(struct wl_resource *resource, const char *name);
void wl_shm_send_format(struct wl_resource *resource, uint32_t format);

struct wl_display* wl_display_create(void);
void wl_display_destroy(struct wl_display *display);
int wl_display_add_socket(struct wl_display *display, const char *name);
int wl_display_get_fd(struct wl_display *display);
void wl_display_run(struct wl_display *display);
void wl_display_terminate(struct wl_display *display);
struct wl_event_loop* wl_display_get_event_loop(struct wl_display *display);
void wl_display_flush_clients(struct wl_display *display);
uint32_t wl_display_get_serial(struct wl_display *display);
uint32_t wl_display_next_serial(struct wl_display *display);
const char* wl_display_add_socket_auto(struct wl_display *display);
int wl_display_init_shm(struct wl_display *display);
struct wl_display* wl_display_connect(const char *name);
void wl_display_disconnect(struct wl_display *display);
int wl_display_dispatch(struct wl_display *display);
int wl_display_dispatch_pending(struct wl_display *display);
int wl_display_flush(struct wl_display *display);
int wl_display_roundtrip(struct wl_display *display);
struct wl_registry* wl_display_get_registry(struct wl_display *display);

struct wl_event_source* wl_event_loop_add_fd(struct wl_event_loop *loop, int fd, uint32_t mask, wl_event_loop_fd_func_t func, void *data);
struct wl_event_source* wl_event_loop_add_timer(struct wl_event_loop *loop, wl_event_loop_timer_func_t func, void *data);
int wl_event_source_remove(struct wl_event_source *source);
int wl_event_source_timer_update(struct wl_event_source *source, int ms_delay);
void wl_event_loop_dispatch(struct wl_event_loop *loop, int timeout);

void wl_client_flush(struct wl_client *client);
void wl_client_destroy(struct wl_client *client);
struct wl_display* wl_client_get_display(struct wl_client *client);
void wl_client_get_credentials(struct wl_client *client, pid_t *pid, uid_t *uid, gid_t *gid);
void wl_client_add_destroy_listener(struct wl_client *client, struct wl_listener *listener);

void wl_shm_buffer_begin_access(struct wl_shm_buffer *buffer);
void wl_shm_buffer_end_access(struct wl_shm_buffer *buffer);
void wl_shm_buffer_release(struct wl_resource *resource);

void wl_surface_attach(struct wl_surface *surface, struct wl_buffer *buffer, int32_t x, int32_t y);
void wl_surface_damage(struct wl_surface *surface, int32_t x, int32_t y, int32_t width, int32_t height);
void wl_surface_commit(struct wl_surface *surface);
void wl_surface_destroy(struct wl_surface *surface);
struct wl_callback* wl_surface_frame(struct wl_surface *surface);

void* wl_registry_bind(struct wl_registry *registry, uint32_t name, const struct wl_interface *interface, uint32_t version);
int wl_registry_add_listener(struct wl_registry *registry, const struct wl_registry_listener *listener, void *data);
void wl_registry_destroy(struct wl_registry *registry);

struct wl_surface* wl_compositor_create_surface(struct wl_compositor *compositor);
void wl_compositor_destroy(struct wl_compositor *compositor);

int wl_output_add_listener(struct wl_output *output, const struct wl_output_listener *listener, void *data);
void wl_output_destroy(struct wl_output *output);

int wl_seat_add_listener(struct wl_seat *seat, const struct wl_seat_listener *listener, void *data);
struct wl_keyboard* wl_seat_get_keyboard(struct wl_seat *seat);
struct wl_pointer* wl_seat_get_pointer(struct wl_seat *seat);
struct wl_touch* wl_seat_get_touch(struct wl_seat *seat);
void wl_seat_destroy(struct wl_seat *seat);

int wl_keyboard_add_listener(struct wl_keyboard *keyboard, const struct wl_keyboard_listener *listener, void *data);
void wl_keyboard_destroy(struct wl_keyboard *keyboard);

int wl_pointer_add_listener(struct wl_pointer *pointer, const struct wl_pointer_listener *listener, void *data);
void wl_pointer_set_cursor(struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface, int32_t hotspot_x, int32_t hotspot_y);
void wl_pointer_destroy(struct wl_pointer *pointer);

int wl_touch_add_listener(struct wl_touch *touch, const struct wl_touch_listener *listener, void *data);
void wl_touch_destroy(struct wl_touch *touch);

int wl_shm_add_listener(struct wl_shm *shm, const struct wl_shm_listener *listener, void *data);
struct wl_shm_pool* wl_shm_create_pool(struct wl_shm *shm, int32_t fd, int32_t size);
void wl_shm_destroy(struct wl_shm *shm);
struct wl_buffer* wl_shm_pool_create_buffer(struct wl_shm_pool *pool, int32_t offset, int32_t width, int32_t height, int32_t stride, uint32_t format);
void wl_shm_pool_resize(struct wl_shm_pool *pool, int32_t size);
void wl_shm_pool_destroy(struct wl_shm_pool *pool);
int wl_buffer_add_listener(struct wl_buffer *buffer, const struct wl_buffer_listener *listener, void *data);
void wl_buffer_destroy(struct wl_buffer *buffer);

int wl_simple_shell_add_listener(struct wl_simple_shell *shell, const struct wl_simple_shell_listener *listener, void *data);
void wl_simple_shell_set_visible(struct wl_simple_shell *shell, uint32_t surface_id, uint32_t visible);
void wl_simple_shell_set_geometry(struct wl_simple_shell *shell, uint32_t surface_id, int32_t x, int32_t y, int32_t width, int32_t height);
void wl_simple_shell_set_zorder(struct wl_simple_shell *shell, uint32_t surface_id, wl_fixed_t zorder);
void wl_simple_shell_set_opacity(struct wl_simple_shell *shell, uint32_t surface_id, wl_fixed_t opacity);
void wl_simple_shell_set_name(struct wl_simple_shell *shell, uint32_t surface_id, const char *name);
void wl_simple_shell_get_status(struct wl_simple_shell *shell, uint32_t surface_id);
void wl_simple_shell_get_surfaces(struct wl_simple_shell *shell);
void wl_simple_shell_destroy(struct wl_simple_shell *shell);

void wl_vpc_surface_set_geometry(struct wl_vpc_surface *vpc_surface, int32_t x, int32_t y, int32_t width, int32_t height);
void wl_vpc_surface_set_geometry_with_crop(struct wl_vpc_surface *vpc_surface, int32_t x, int32_t y, int32_t width, int32_t height, int32_t cx, int32_t cy, int32_t cw, int32_t ch);

void mock_set_ioctl_failure(int should_fail);
void mock_set_num_fences(int num_fences);
int mock_create_fence_fd(void);
int mock_is_valid_fence_fd(int fd);

#ifdef __cplusplus
}
#endif

#endif /* UNIT_TEST */

#endif /* WAYLAND_STUBS_H */
