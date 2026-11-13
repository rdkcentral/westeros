/**
* If not stated otherwise in this file or this component's LICENSE
* file the following copyright and licenses apply:
*
* Copyright 2024 RDK Management
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
**/
/*
 * Simpleshell Client Protocol Mock Header
 * Auto-generated stub for L1 testing
 */
#ifndef SIMPLESHELL_CLIENT_PROTOCOL_H
#define SIMPLESHELL_CLIENT_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "wayland-server.h"

struct wl_shell;
struct wl_shell_surface;

// Shell interface
extern const struct wl_interface wl_shell_interface;
extern const struct wl_interface wl_shell_surface_interface;

// Listener structures
struct wl_shell_listener {
    void (*dummy)(void);
};

struct wl_shell_surface_listener {
    void (*ping)(void *data, struct wl_shell_surface *shell_surface, uint32_t serial);
    void (*configure)(void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height);
    void (*popup_done)(void *data, struct wl_shell_surface *shell_surface);
};

// Stub functions
static inline struct wl_shell *
wl_shell_create(struct wl_display *display) {
    return nullptr;
}

static inline void
wl_shell_destroy(struct wl_shell *shell) {
}

static inline struct wl_shell_surface *
wl_shell_surface_create(struct wl_shell *shell, struct wl_surface *surface) {
    return nullptr;
}

static inline void
wl_shell_surface_destroy(struct wl_shell_surface *shell_surface) {
}

static inline int
wl_shell_surface_add_listener(struct wl_shell_surface *shell_surface,
                               const struct wl_shell_surface_listener *listener,
                               void *data) {
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* SIMPLESHELL_CLIENT_PROTOCOL_H */
