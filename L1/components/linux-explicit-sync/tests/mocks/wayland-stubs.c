#include "wayland-stubs.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Global mock state */
static int mock_resource_id_counter = 1000;

/* wl_global functions */
struct wl_global* wl_global_create(struct wl_display *display,
                                   const struct wl_interface *interface,
                                   int version,
                                   void *data,
                                   void (*bind)(struct wl_client *client,
                                               void *data,
                                               uint32_t version,
                                               uint32_t id))
{
    struct wl_global *global = (struct wl_global*)calloc(1, sizeof(struct wl_global));
    return global;
}

void wl_global_destroy(struct wl_global *global)
{
    if (global) {
        free(global);
    }
}

/* wl_resource functions */
struct wl_resource* wl_resource_create(struct wl_client *client,
                                       const struct wl_interface *interface,
                                       int version,
                                       uint32_t id)
{
    struct wl_resource *resource = (struct wl_resource*)calloc(1, sizeof(struct wl_resource));
    if (resource) {
        resource->client = client;
        resource->interface = interface;
        resource->version = version;
        resource->id = id ? id : mock_resource_id_counter++;
        wl_list_init(&resource->link);
        wl_list_init(&resource->destroy_signal);
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

void wl_resource_post_error(struct wl_resource *resource,
                           uint32_t code,
                           const char *msg, ...)
{
    // Mock: just print to stderr
    fprintf(stderr, "Mock: wl_resource_post_error code=%u msg=%s\n", code, msg);
}

void wl_resource_post_no_memory(struct wl_resource *resource)
{
    fprintf(stderr, "Mock: wl_resource_post_no_memory\n");
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

void wl_resource_set_implementation(struct wl_resource *resource,
                                   const void *implementation,
                                   void *data,
                                   void (*destroy)(struct wl_resource *resource))
{
    if (resource) {
        resource->object = (void*)implementation;
        resource->data = data;
        resource->destroy = destroy;
    }
}

struct wl_client* wl_resource_get_client(struct wl_resource *resource)
{
    return resource ? resource->client : NULL;
}

void wl_resource_add_destroy_listener(struct wl_resource *resource,
                                     struct wl_listener *listener)
{
    if (resource && listener) {
        wl_list_insert(&resource->destroy_signal, &listener->link);
    }
}

void wl_resource_get_destroy_listener(struct wl_resource *resource,
                                     void (*notify)(struct wl_listener *listener,
                                                   void *data))
{
    // Mock: not implemented
}

int wl_resource_get_version(struct wl_resource *resource)
{
    return resource ? resource->version : 0;
}

uint32_t wl_resource_get_id(struct wl_resource *resource)
{
    return resource ? resource->id : 0;
}

struct wl_list* wl_resource_get_link(struct wl_resource *resource)
{
    return resource ? &resource->link : NULL;
}

struct wl_resource* wl_resource_from_link(struct wl_list *link)
{
    // Mock: simple offset calculation
    return link ? (struct wl_resource*)((char*)link - offsetof(struct wl_resource, link)) : NULL;
}

/* wl_list functions */
void wl_list_init(struct wl_list *list)
{
    if (list) {
        list->prev = list;
        list->next = list;
    }
}

void wl_list_insert(struct wl_list *list, struct wl_list *elm)
{
    if (list && elm) {
        elm->prev = list;
        elm->next = list->next;
        list->next->prev = elm;
        list->next = elm;
    }
}

void wl_list_remove(struct wl_list *elm)
{
    if (elm) {
        elm->prev->next = elm->next;
        elm->next->prev = elm->prev;
        elm->prev = NULL;
        elm->next = NULL;
    }
}

int wl_list_length(const struct wl_list *list)
{
    if (!list) return 0;
    
    int count = 0;
    struct wl_list *e;
    for (e = list->next; e != list; e = e->next) {
        count++;
    }
    return count;
}

int wl_list_empty(const struct wl_list *list)
{
    return list && list->next == list;
}

/* wl_signal functions */
void wl_signal_init(struct wl_signal *signal)
{
    if (signal) {
        wl_list_init(&signal->listener_list);
    }
}

void wl_signal_add(struct wl_signal *signal, struct wl_listener *listener)
{
    if (signal && listener) {
        wl_list_insert(signal->listener_list.prev, &listener->link);
    }
}

void wl_signal_emit(struct wl_signal *signal, void *data)
{
    if (!signal) return;
    
    struct wl_listener *l, *next;
    struct wl_list *pos;
    
    for (pos = signal->listener_list.next;
         pos != &signal->listener_list;
         pos = pos->next) {
        l = (struct wl_listener*)((char*)pos - offsetof(struct wl_listener, link));
        if (l->notify) {
            l->notify(l, data);
        }
    }
}

/* wl_array functions */
void wl_array_init(struct wl_array *array)
{
    if (array) {
        memset(array, 0, sizeof(*array));
    }
}

void wl_array_release(struct wl_array *array)
{
    if (array && array->data) {
        free(array->data);
        array->data = NULL;
        array->size = 0;
        array->alloc = 0;
    }
}

void* wl_array_add(struct wl_array *array, size_t size)
{
    if (!array) return NULL;
    
    size_t new_size = array->size + size;
    if (new_size > array->alloc) {
        size_t new_alloc = array->alloc ? array->alloc : 16;
        while (new_alloc < new_size) {
            new_alloc *= 2;
        }
        void *new_data = realloc(array->data, new_alloc);
        if (!new_data) return NULL;
        array->data = new_data;
        array->alloc = new_alloc;
    }
    
    void *ptr = (char*)array->data + array->size;
    array->size = new_size;
    return ptr;
}

int wl_array_copy(struct wl_array *array, struct wl_array *source)
{
    if (!array || !source) return -1;
    
    wl_array_release(array);
    if (source->size == 0) return 0;
    
    array->data = malloc(source->size);
    if (!array->data) return -1;
    
    memcpy(array->data, source->data, source->size);
    array->size = source->size;
    array->alloc = source->size;
    return 0;
}
