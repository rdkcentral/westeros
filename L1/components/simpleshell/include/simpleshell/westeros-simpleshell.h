/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
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
 */
#ifndef _WESTEROS_SIMPLESHELL_H
#define _WESTEROS_SIMPLESHELL_H

/*
 * STATIC_TEST macro for unit testing
 * 
 * Purpose: Makes static functions and const objects externally linkable during unit tests
 * 
 * For functions:
 * - Production: STATIC_TEST expands to 'static' (internal linkage)
 * - Test: STATIC_TEST expands to empty (external linkage by default)
 * 
 * For const objects:
 * - Production: STATIC_TEST_CONST expands to 'static const' (internal linkage)
 * - Test: STATIC_TEST_CONST expands to 'extern const' (explicit external linkage needed)
 * 
 * Note: Const objects need 'extern' because they have internal linkage by default in C++
 */
#ifdef UNIT_TEST
#define STATIC_TEST
#define STATIC_TEST_CONST extern const
#else
#define STATIC_TEST static
#define STATIC_TEST_CONST static const
#endif

#include "wayland-server.h"

// Forward declaration
struct wl_simple_shell;

// Callback structure for SimpleShell
struct wayland_simple_shell_callbacks {
    void (*set_name)(void *userData, uint32_t surfaceId, const char *name);
    void (*set_visible)(void *userData, uint32_t surfaceId, bool visible);
    void (*set_geometry)(void *userData, uint32_t surfaceId, int x, int y, int width, int height);
    void (*set_opacity)(void *userData, uint32_t surfaceId, float opacity);
    void (*set_zorder)(void *userData, uint32_t surfaceId, float zorder);
    void (*get_name)(void *userData, uint32_t surfaceId, const char **name);
    void (*get_status)(void *userData, uint32_t surfaceId, bool *visible, int32_t *x, int32_t *y, int32_t *width, int32_t *height, float *opacity, float *zorder);
    void (*set_focus)(void *userData, uint32_t surfaceId);
    void (*set_scale)(void *userData, uint32_t surfaceId, float scaleX, float scaleY);
};

// SimpleShell API functions
#ifdef __cplusplus
extern "C" {
#endif

wl_simple_shell* WstSimpleShellInit(struct wl_display *display, struct wayland_simple_shell_callbacks *callbacks, void *userData);
void WstSimpleShellUninit(struct wl_simple_shell *shell);
void WstSimpleShellNotifySurfaceCreated(struct wl_simple_shell *shell, struct wl_client *client, struct wl_resource *resource, uint32_t surfaceId);
void WstSimpleShellNotifySurfaceDestroyed(struct wl_simple_shell *shell, struct wl_client *client, uint32_t surfaceId);
void WstSimpleShellNotifySurfaceStatus(struct wl_simple_shell *shell, uint32_t surfaceId, const char *name, bool visible, int x, int y, int width, int height, float opacity, float zorder);

#ifdef __cplusplus
}
#endif

#endif
