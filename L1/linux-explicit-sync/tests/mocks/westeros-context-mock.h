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
#ifndef WESTEROS_CONTEXT_MOCK_H
#define WESTEROS_CONTEXT_MOCK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "wayland-stubs.h"
#include "westeros-linux-expsync.h"

/* WstExplicitSync types are defined in westeros-linux-expsync.h */

/* Mock WstContext structure */
typedef struct _WstContext {
    struct wl_display *display;
    void *lexpsync;  /* pointer to wl_lexpsync */
    int initialized;
} WstContext;

/* Mock WstSurface structure */
typedef struct _WstSurface {
    struct wl_resource *resource;
    struct wl_resource *syncRes;
    WstExplicitSync createdBufferSync;
    WstExplicitSync attachedBufferSync;
    WstExplicitSync detachedBufferSync;
    int surfaceId;
    int destroyed;
} WstSurface;

/* Mock WstRenderer structure */
typedef struct _WstRenderer {
    WstContext *ctx;
    int initialized;
} WstRenderer;

/* Mock functions for test support */
WstContext* mock_create_context(void);
void mock_destroy_context(WstContext *ctx);

WstSurface* mock_create_surface(WstContext *ctx);
void mock_destroy_surface(WstSurface *surface);

WstRenderer* mock_create_renderer(WstContext *ctx);
void mock_destroy_renderer(WstRenderer *renderer);

/* Helper for creating mock resources */
struct wl_resource* mock_create_resource(struct wl_client *client, uint32_t id);

#ifdef __cplusplus
}
#endif

#endif /* WESTEROS_CONTEXT_MOCK_H */
