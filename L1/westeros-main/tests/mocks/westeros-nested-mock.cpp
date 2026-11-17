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
 * Westeros Nested Connection Mock Implementation
 * Copyright 2024 RDK Management
 */
#include "westeros-nested.h"
#include "wayland-client.h"

// Mock nested connection structure
struct _WstNestedConnection {
    struct _WstCompositor *compositor;
    void *userData;
};

// Create nested connection
WstNestedConnection* WstNestedConnectionCreate(WstCompositor *compositor, 
                                                const char *displayName, 
                                                int width, int height,
                                                WstNestedConnectionListener *listener, 
                                                void *userData) {
    (void)displayName;
    (void)width;
    (void)height;
    (void)listener;
    
    WstNestedConnection *conn = new WstNestedConnection();
    conn->compositor = compositor;
    conn->userData = userData;
    return conn;
}

// Destroy nested connection
void WstNestedConnectionDestroy(WstNestedConnection *conn) {
    if (conn) {
        delete conn;
    }
}

// Disconnect nested connection
void WstNestedConnectionDisconnect(WstNestedConnection *conn) {
    (void)conn;
    // Mock does nothing
}

// Release remote buffers
void WstNestedConnectionReleaseRemoteBuffers(WstNestedConnection *conn) {
    (void)conn;
    // Mock does nothing
}

// Surface visibility
void WstNestedConnectionSurfaceSetVisible(WstNestedConnection *conn, 
                                          wl_surface *surface, 
                                          bool visible) {
    (void)conn;
    (void)surface;
    (void)visible;
    // Mock does nothing
}

// Surface geometry
void WstNestedConnectionSurfaceSetGeometry(WstNestedConnection *conn, 
                                           wl_surface *surface,
                                           int x, int y, int width, int height) {
    (void)conn;
    (void)surface;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
    // Mock does nothing
}

// Surface opacity
void WstNestedConnectionSurfaceSetOpacity(WstNestedConnection *conn, 
                                          wl_surface *surface, 
                                          float opacity) {
    (void)conn;
    (void)surface;
    (void)opacity;
    // Mock does nothing
}

// Surface z-order
void WstNestedConnectionSurfaceSetZOrder(WstNestedConnection *conn, 
                                         wl_surface *surface, 
                                         float zorder) {
    (void)conn;
    (void)surface;
    (void)zorder;
    // Mock does nothing
}

// SHM buffer pool operations
wl_buffer* WstNestedConnectionShmPoolCreateBuffer(WstNestedConnection *conn,
                                                   wl_shm_pool *pool,
                                                   int offset, int width, int height,
                                                   int stride, uint32_t format) {
    (void)conn;
    (void)pool;
    (void)offset;
    (void)width;
    (void)height;
    (void)stride;
    (void)format;
    return nullptr; // Mock returns null
}

void WstNestedConnectionShmBufferPoolDestroy(WstNestedConnection *conn,
                                             wl_shm_pool *pool,
                                             wl_buffer *buffer) {
    (void)conn;
    (void)pool;
    (void)buffer;
    // Mock does nothing
}
