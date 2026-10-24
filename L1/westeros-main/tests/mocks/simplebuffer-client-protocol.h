/*
 * Simplebuffer Client Protocol Mock Header
 * Auto-generated stub for L1 testing
 */
#ifndef SIMPLEBUFFER_CLIENT_PROTOCOL_H
#define SIMPLEBUFFER_CLIENT_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "wayland-server.h"

struct wl_sb;
struct wl_sb_buffer;

// Simplebuffer interface
extern const struct wl_interface wl_sb_interface;
extern const struct wl_interface wl_sb_buffer_interface;

// Listener structures
struct wl_sb_listener {
    void (*format)(void *data, struct wl_sb *sb, uint32_t format);
};

struct wl_sb_buffer_listener {
    void (*release)(void *data, struct wl_sb_buffer *sb_buffer);
};

// Stub functions
static inline struct wl_sb *
wl_sb_create(struct wl_display *display) {
    return nullptr;
}

static inline struct wl_buffer *
wl_sb_create_buffer(struct wl_sb *sb, uintptr_t device_buffer,
                    int32_t width, int32_t height,
                    int32_t stride, uint32_t format) {
    return nullptr;
}

static inline void
wl_sb_destroy(struct wl_sb *sb) {
}

static inline struct wl_sb_buffer *
wl_sb_buffer_create(struct wl_sb *sb, int32_t width, int32_t height,
                    int32_t stride, uint32_t format) {
    return nullptr;
}

static inline void
wl_sb_buffer_destroy(struct wl_sb_buffer *sb_buffer) {
}

static inline int
wl_sb_add_listener(struct wl_sb *sb,
                   const struct wl_sb_listener *listener,
                   void *data) {
    return 0;
}

static inline int
wl_sb_buffer_add_listener(struct wl_sb_buffer *sb_buffer,
                          const struct wl_sb_buffer_listener *listener,
                          void *data) {
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEBUFFER_CLIENT_PROTOCOL_H */
