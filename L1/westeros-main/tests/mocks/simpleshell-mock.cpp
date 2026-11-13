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
 * SimpleShell Mock Implementation
 * Copyright 2024 RDK Management
 */
#include "westeros-simpleshell.h"
#include "wayland-server.h"

// Mock simple shell structure
struct wl_simple_shell {
    wl_display *display;
    void *userData;
};

// SimpleShell initialization
wl_simple_shell* WstSimpleShellInit(wl_display *display, 
                                     wayland_simple_shell_callbacks *callbacks, 
                                     void *userData) {
    (void)callbacks;
    wl_simple_shell *shell = new wl_simple_shell();
    shell->display = display;
    shell->userData = userData;
    return shell;
}

// SimpleShell cleanup
void WstSimpleShellUninit(wl_simple_shell *shell) {
    if (shell) {
        delete shell;
    }
}

// Surface created notification
void WstSimpleShellNotifySurfaceCreated(wl_simple_shell *shell, 
                                        wl_client *client,
                                        wl_resource *surfaceResource,
                                        uint32_t surfaceId) {
    (void)shell;
    (void)client;
    (void)surfaceResource;
    (void)surfaceId;
    // Mock does nothing
}

// Surface destroyed notification
void WstSimpleShellNotifySurfaceDestroyed(wl_simple_shell *shell, 
                                          wl_client *client,
                                          uint32_t surfaceId) {
    (void)shell;
    (void)client;
    (void)surfaceId;
    // Mock does nothing
}
