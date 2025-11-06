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
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <assert.h>

#include "westeros-simpleshell.h"
#include "wayland-server.h"
#include "simpleshell-server-protocol.h"

#define MIN(x,y) (((x) < (y)) ? (x) : (y))

// SimpleShell structure definition
struct wl_simple_shell {
    struct wl_display *display;
    struct wl_global *wl_simple_shell_global;
    void *userData;
    struct wayland_simple_shell_callbacks *callbacks;
};

// Protocol request handlers
STATIC_TEST void wstSimpleShellSetName(struct wl_client *client, struct wl_resource *resource, uint32_t surfaceId, const char *name) {
    struct wl_simple_shell *shell = (struct wl_simple_shell*)wl_resource_get_user_data(resource);
    if (shell && shell->callbacks && shell->callbacks->set_name) {
        shell->callbacks->set_name(shell->userData, surfaceId, name);
    }
}

STATIC_TEST void wstSimpleShellSetVisible(struct wl_client *client, struct wl_resource *resource, uint32_t surfaceId, uint32_t visible) {
    struct wl_simple_shell *shell = (struct wl_simple_shell*)wl_resource_get_user_data(resource);
    if (shell && shell->callbacks && shell->callbacks->set_visible) {
        shell->callbacks->set_visible(shell->userData, surfaceId, visible != 0);
    }
}

STATIC_TEST void wstSimpleShellSetGeometry(struct wl_client *client, struct wl_resource *resource, uint32_t surfaceId, int32_t x, int32_t y, int32_t width, int32_t height) {
    struct wl_simple_shell *shell = (struct wl_simple_shell*)wl_resource_get_user_data(resource);
    if (shell && shell->callbacks && shell->callbacks->set_geometry) {
        shell->callbacks->set_geometry(shell->userData, surfaceId, x, y, width, height);
    }
}

STATIC_TEST void wstSimpleShellSetOpacity(struct wl_client *client, struct wl_resource *resource, uint32_t surfaceId, wl_fixed_t opacity) {
    struct wl_simple_shell *shell = (struct wl_simple_shell*)wl_resource_get_user_data(resource);
    if (shell && shell->callbacks && shell->callbacks->set_opacity) {
        float opacityFloat = wl_fixed_to_double(opacity);
        shell->callbacks->set_opacity(shell->userData, surfaceId, opacityFloat);
    }
}

STATIC_TEST void wstSimpleShellSetZorder(struct wl_client *client, struct wl_resource *resource, uint32_t surfaceId, wl_fixed_t zorder) {
    struct wl_simple_shell *shell = (struct wl_simple_shell*)wl_resource_get_user_data(resource);
    if (shell && shell->callbacks && shell->callbacks->set_zorder) {
        float zorderFloat = wl_fixed_to_double(zorder);
        shell->callbacks->set_zorder(shell->userData, surfaceId, zorderFloat);
    }
}

STATIC_TEST void wstSimpleShellGetStatus(struct wl_client *client, struct wl_resource *resource, uint32_t surfaceId) {
    struct wl_simple_shell *shell = (struct wl_simple_shell*)wl_resource_get_user_data(resource);
    if (shell && shell->callbacks && shell->callbacks->get_status) {
        bool visible = false;
        int32_t x = 0, y = 0, width = 0, height = 0;
        float opacity = 0.0f, zorder = 0.0f;
        const char *name = "";
        
        shell->callbacks->get_status(shell->userData, surfaceId, &visible, &x, &y, &width, &height, &opacity, &zorder);
        
        wl_simple_shell_send_surface_status(resource, surfaceId, name, visible ? 1 : 0, x, y, width, height,
                                           wl_fixed_from_double(opacity), wl_fixed_from_double(zorder));
    }
}

STATIC_TEST void wstSimpleShellGetSurfaces(struct wl_client *client, struct wl_resource *resource) {
    // Mock implementation - send done immediately
    wl_simple_shell_send_get_surfaces_done(resource);
}

STATIC_TEST void wstSimpleShellSetFocus(struct wl_client *client, struct wl_resource *resource, uint32_t surfaceId) {
    struct wl_simple_shell *shell = (struct wl_simple_shell*)wl_resource_get_user_data(resource);
    if (shell && shell->callbacks && shell->callbacks->set_focus) {
        shell->callbacks->set_focus(shell->userData, surfaceId);
    }
}

STATIC_TEST void wstSimpleShellSetScale(struct wl_client *client, struct wl_resource *resource, uint32_t surfaceId, wl_fixed_t scaleX, wl_fixed_t scaleY) {
    struct wl_simple_shell *shell = (struct wl_simple_shell*)wl_resource_get_user_data(resource);
    if (shell && shell->callbacks && shell->callbacks->set_scale) {
        float scaleXFloat = wl_fixed_to_double(scaleX);
        float scaleYFloat = wl_fixed_to_double(scaleY);
        shell->callbacks->set_scale(shell->userData, surfaceId, scaleXFloat, scaleYFloat);
    }
}

// Interface implementation
STATIC_TEST_CONST struct wl_simple_shell_interface simple_shell_interface = {
    wstSimpleShellSetName,
    wstSimpleShellSetVisible,
    wstSimpleShellSetGeometry,
    wstSimpleShellSetOpacity,
    wstSimpleShellSetZorder,
    wstSimpleShellGetStatus,
    wstSimpleShellGetSurfaces,
    wstSimpleShellSetFocus,
    wstSimpleShellSetScale
};

// Bind function
STATIC_TEST void wstSimpleShellBind(struct wl_client *client, void *data, uint32_t version, uint32_t id) {
    struct wl_simple_shell *shell = (struct wl_simple_shell*)data;
    struct wl_resource *resource;
    
    resource = wl_resource_create(client, &wl_simple_shell_interface, MIN(version, 1), id);
    if (!resource) {
        wl_client_post_no_memory(client);
        return;
    }
    
    wl_resource_set_implementation(resource, (const void*)&simple_shell_interface, shell, NULL);
}

// Public API implementation
wl_simple_shell* WstSimpleShellInit(struct wl_display *display, struct wayland_simple_shell_callbacks *callbacks, void *userData) {
    struct wl_simple_shell *shell = NULL;
    
    if (!display || !callbacks) {
        return NULL;
    }
    
    shell = (struct wl_simple_shell*)calloc(1, sizeof(struct wl_simple_shell));
    if (!shell) {
        return NULL;
    }
    
    shell->display = display;
    shell->callbacks = callbacks;
    shell->userData = userData;
    
    shell->wl_simple_shell_global = wl_global_create(display, &wl_simple_shell_interface, 1, shell, wstSimpleShellBind);
    if (!shell->wl_simple_shell_global) {
        free(shell);
        return NULL;
    }
    
    return shell;
}

void WstSimpleShellUninit(struct wl_simple_shell *shell) {
    if (shell) {
        if (shell->wl_simple_shell_global) {
            wl_global_destroy(shell->wl_simple_shell_global);
        }
        free(shell);
    }
}

void WstSimpleShellNotifySurfaceCreated(struct wl_simple_shell *shell, struct wl_client *client, struct wl_resource *resource, uint32_t surfaceId) {
    // Implementation for notifying surface creation
    // In a real implementation, this would send events to all bound clients
    // For now, this is a minimal implementation that validates parameters
    assert(shell != NULL);
    
    // The shell object receives the notification
    // In a full implementation, this would iterate through bound resources and send events
    (void)client;
    (void)resource;
    (void)surfaceId;
}

void WstSimpleShellNotifySurfaceDestroyed(struct wl_simple_shell *shell, struct wl_client *client, uint32_t surfaceId) {
    // Implementation for notifying surface destruction
    // Call the get_name callback to retrieve surface information before notifying
    assert(shell != NULL);
    
    if (shell->callbacks && shell->callbacks->get_name) {
        const char *name = NULL;
        shell->callbacks->get_name(shell->userData, surfaceId, &name);
        // In a real implementation, this would send events to all bound clients
        // using the retrieved surface name
        (void)name;
    }
    
    (void)client;
}

void WstSimpleShellNotifySurfaceStatus(struct wl_simple_shell *shell, uint32_t surfaceId, const char *name, bool visible, int x, int y, int width, int height, float opacity, float zorder) {
    // Mock implementation - would normally send event to all bound clients
    (void)shell;
    (void)surfaceId;
    (void)name;
    (void)visible;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
    (void)opacity;
    (void)zorder;
}
