#ifndef WAYLAND_STUBS_H
#define WAYLAND_STUBS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/* Wayland server types - minimal mock implementation */

struct wl_display {
    int dummy;
};

struct wl_client {
    int dummy;
};

/* Define wl_list first to avoid incomplete type errors */
struct wl_list {
    struct wl_list *prev;
    struct wl_list *next;
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
    int version;
};

struct wl_global {
    int dummy;
};

struct wl_interface {
    const char *name;
    int version;
    int method_count;
    const struct wl_message *methods;
    int event_count;
    const struct wl_message *events;
};

struct wl_message {
    const char *name;
    const char *signature;
    const struct wl_interface **types;
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

/* Wayland server functions */
struct wl_global* wl_global_create(struct wl_display *display,
                                   const struct wl_interface *interface,
                                   int version,
                                   void *data,
                                   void (*bind)(struct wl_client *client,
                                               void *data,
                                               uint32_t version,
                                               uint32_t id));

void wl_global_destroy(struct wl_global *global);

struct wl_resource* wl_resource_create(struct wl_client *client,
                                       const struct wl_interface *interface,
                                       int version,
                                       uint32_t id);

void wl_resource_destroy(struct wl_resource *resource);

void wl_resource_post_error(struct wl_resource *resource,
                           uint32_t code,
                           const char *msg, ...);

void wl_resource_post_no_memory(struct wl_resource *resource);

void* wl_resource_get_user_data(struct wl_resource *resource);

void wl_resource_set_user_data(struct wl_resource *resource, void *data);

void wl_resource_set_implementation(struct wl_resource *resource,
                                   const void *implementation,
                                   void *data,
                                   void (*destroy)(struct wl_resource *resource));

struct wl_client* wl_resource_get_client(struct wl_resource *resource);

void wl_resource_add_destroy_listener(struct wl_resource *resource,
                                     struct wl_listener *listener);

void wl_resource_get_destroy_listener(struct wl_resource *resource,
                                     void (*notify)(struct wl_listener *listener,
                                                   void *data));

int wl_resource_get_version(struct wl_resource *resource);

uint32_t wl_resource_get_id(struct wl_resource *resource);

struct wl_list* wl_resource_get_link(struct wl_resource *resource);

struct wl_resource* wl_resource_from_link(struct wl_list *link);

void wl_list_init(struct wl_list *list);

void wl_list_insert(struct wl_list *list, struct wl_list *elm);

void wl_list_remove(struct wl_list *elm);

int wl_list_length(const struct wl_list *list);

int wl_list_empty(const struct wl_list *list);

void wl_signal_init(struct wl_signal *signal);

void wl_signal_add(struct wl_signal *signal, struct wl_listener *listener);

void wl_signal_emit(struct wl_signal *signal, void *data);

void wl_array_init(struct wl_array *array);

void wl_array_release(struct wl_array *array);

void* wl_array_add(struct wl_array *array, size_t size);

int wl_array_copy(struct wl_array *array, struct wl_array *source);

/* Mock function declarations for sync-file-stubs.c */
void mock_set_ioctl_failure(int should_fail);
void mock_set_num_fences(int num_fences);
int mock_create_fence_fd(void);
int mock_is_valid_fence_fd(int fd);

/* Platform-specific system call declarations */
#if defined(WINDOWS_PLATFORM) || (defined(_WIN32) && !defined(LINUX_PLATFORM))
/* Mock system calls for Windows builds */
int ioctl(int fd, unsigned long request, ...);
int close(int fd);
#endif

#ifdef __cplusplus
}
#endif

#endif /* WAYLAND_STUBS_H */
