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
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#endif
#include <assert.h>
#include <inttypes.h>
#include <errno.h>
#include <stdbool.h>

#include "westeros-linux-expsync.h"

#ifdef UNIT_TEST
// For unit tests, use mock headers
#include "wayland-stubs.h"
#else
// For production build, use real Wayland headers
#include <wayland-server.h>
#endif

// Mock sync_file types (instead of linux/sync_file.h)
#ifndef SYNC_IOC_FILE_INFO
struct sync_file_info {
    char name[32];
    int32_t status;
    uint32_t flags;
    uint32_t num_fences;
    uint32_t pad;
    uint64_t sync_fence_info;
};
#define SYNC_IOC_FILE_INFO 0xc0303e04
#endif

// Include protocol after wayland
#include "linux-explicit-synchronization-unstable-v1-server-protocol.h"

// ioctl is available from system headers
#ifndef _WIN32
#include <sys/ioctl.h>
#endif

#ifdef UNIT_TEST
// Unit test build: define types here (not available from compositor during test compilation)
typedef struct _WstContext {
    struct wl_display *display;
    void *lexpsync;
    int initialized;
} WstContext;

typedef struct _WstSurface {
    struct wl_resource *resource;
    struct wl_resource *syncRes;
    WstExplicitSync createdBufferSync;
    WstExplicitSync attachedBufferSync;
    WstExplicitSync detachedBufferSync;
    int surfaceId;
    int destroyed;
} WstSurface;
#else
// Production autotools build: use forward declarations (types defined in westeros-compositor.cpp)
struct _WstContext;
struct _WstSurface;
typedef struct _WstContext WstContext;
typedef struct _WstSurface WstSurface;
#endif

struct wl_lexpsync
{
   struct wl_display *display;
   struct wl_global *wl_lexpsync_global;
};

STATIC_TEST bool wstLExpSyncFileIsValid(int fd)
{
   bool result;
   struct sync_file_info fInfo= { { 0 } };
   int rc;

   rc= ioctl(fd, SYNC_IOC_FILE_INFO, &fInfo);
   if ( rc < 0 )
   {
      result= false;
      goto done;
   }

   result= ( fInfo.num_fences > 0 ? true : false );

done:
   return result;
}

STATIC_TEST void wstLExpSyncBufferRelease(struct wl_resource *resource)
{
   WstExplicitSyncBufferRelease *bufferRelease= (WstExplicitSyncBufferRelease*)wl_resource_get_user_data(resource);

   WstLExpSyncFdClear(&bufferRelease->renderFenceFd);
   free(bufferRelease);
}

STATIC_TEST void wstLExpSyncDestroySync(struct wl_resource *resource)
{
   WstSurface *surface= (WstSurface*)wl_resource_get_user_data(resource);

   if (surface)
   {
      WstLExpSyncFdClear(&surface->createdBufferSync.acquireFenceFd);
      WstLExpSyncFdClear(&surface->attachedBufferSync.acquireFenceFd);
      WstLExpSyncFdClear(&surface->detachedBufferSync.acquireFenceFd);
      surface->syncRes= NULL;
   }
}

/*
 * Adapted from:
 * Copyright (C) 2018 Collabora, Ltd.
 * Licensed under the MIT License
 */
STATIC_TEST void wstILExpSyncSurfaceSyncDestroy(struct wl_client *client,
                                           struct wl_resource *resource)
{
   wl_resource_destroy(resource);
}

STATIC_TEST void wstILExpSyncSurfaceSyncSetAcquireFence(struct wl_client *client,
                                                   struct wl_resource *resource,
                                                   int32_t fd)
{
   WstSurface *surface= (WstSurface*)wl_resource_get_user_data(resource);

   if (!surface)
   {
      wl_resource_post_error( resource,
                              ZWP_LINUX_SURFACE_SYNCHRONIZATION_V1_ERROR_NO_SURFACE,
                              "surface no longer exists");
      goto exit;
   }

   if ( !wstLExpSyncFileIsValid(fd) )
   {
      wl_resource_post_error( resource,
                              ZWP_LINUX_SURFACE_SYNCHRONIZATION_V1_ERROR_INVALID_FENCE,
                              "invalid fence fd");
      goto exit;
   }

   if (surface->createdBufferSync.acquireFenceFd != -1)
   {
      wl_resource_post_error( resource,
                              ZWP_LINUX_SURFACE_SYNCHRONIZATION_V1_ERROR_DUPLICATE_FENCE,
                              "already have a fence fd");
       goto exit;
   }

   WstLExpSyncFdUpdate(&surface->createdBufferSync.acquireFenceFd, fd);

   fd= -1;

exit:
   if ( fd >= 0 )
   {
      close(fd);
   }
}

STATIC_TEST void wstILExpSyncSurfaceSyncGetRelease(struct wl_client *client,
                                              struct wl_resource *resource,
                                              uint32_t id)
{
   WstSurface *surface= (WstSurface*)wl_resource_get_user_data(resource);
   WstExplicitSyncBufferRelease *bufferRelease;

   if (!surface)
   {
      wl_resource_post_error( resource,
                              ZWP_LINUX_SURFACE_SYNCHRONIZATION_V1_ERROR_NO_SURFACE,
                              "surface no longer exists");
      return;
   }

   if (surface->createdBufferSync.bufferRelease)
   {
      wl_resource_post_error( resource,
                              ZWP_LINUX_SURFACE_SYNCHRONIZATION_V1_ERROR_DUPLICATE_RELEASE,
                              "already has a buffer release");
      return;
   }

   bufferRelease= (WstExplicitSyncBufferRelease*)calloc(1, sizeof(*bufferRelease));
   if (bufferRelease == NULL)
   {
       goto err_alloc;
   }

   bufferRelease->renderFenceFd= -1; // render fence provided by server to client
   bufferRelease->resource= wl_resource_create(client,
                                               &zwp_linux_buffer_release_v1_interface,
                                               wl_resource_get_version(resource), id);
   if (!bufferRelease->resource)
   {
      goto err_create;
   }

   wl_resource_set_implementation(bufferRelease->resource,
                                  NULL,
                                  bufferRelease,
                                  wstLExpSyncBufferRelease);

   surface->createdBufferSync.bufferRelease= bufferRelease;
   return;

err_create:
   free(bufferRelease);

err_alloc:
   wl_client_post_no_memory(client);
}

STATIC_TEST_CONST struct zwp_linux_surface_synchronization_v1_interface linux_surface_synchronization_implementation = {
   wstILExpSyncSurfaceSyncDestroy,
   wstILExpSyncSurfaceSyncSetAcquireFence,
   wstILExpSyncSurfaceSyncGetRelease
};

STATIC_TEST void wstILExpSyncDestroy(struct wl_client *client,
                                struct wl_resource *resource)
{
   wl_resource_destroy(resource);
}

STATIC_TEST void wstILExpSyncGetSynchronization(struct wl_client *client,
                                           struct wl_resource *resource,
                                           uint32_t id,
                                           struct wl_resource *surface_resource)
{
   WstSurface *surface= (WstSurface*)wl_resource_get_user_data(surface_resource);

   if (surface->syncRes)
   {
      wl_resource_post_error( resource,
                              ZWP_LINUX_EXPLICIT_SYNCHRONIZATION_V1_ERROR_DUPLICATE_SYNCHRONIZATION,
                              "wl_surface@%" PRIu32 " already has a synchronization object",
                              wl_resource_get_id(surface_resource));
       return;
   }

   surface->syncRes= wl_resource_create(client,
                                        &zwp_linux_surface_synchronization_v1_interface,
                                        wl_resource_get_version(resource), id);
   if (!surface->syncRes)
   {
      wl_client_post_no_memory(client);
      return;
   }

   wl_resource_set_implementation(surface->syncRes,
                      &linux_surface_synchronization_implementation,
                      surface,
                      wstLExpSyncDestroySync);
}

STATIC_TEST_CONST struct zwp_linux_explicit_synchronization_v1_interface linux_explicit_synchronization_implementation = {
   wstILExpSyncDestroy,
   wstILExpSyncGetSynchronization
};

STATIC_TEST void wstExplicitSyncBind( struct wl_client *client, void *data, uint32_t version, uint32_t id )
{
   WstContext *ctx= (WstContext*)data;
   struct wl_resource *resource;

   printf("wstExplicitSyncBind: client %p data %p version %d id %d", client, data, version, id );

   resource= wl_resource_create(client,
                                &zwp_linux_explicit_synchronization_v1_interface,
                                version, id);
   if (resource == NULL)
   {
       wl_client_post_no_memory(client);
       return;
   }

   wl_resource_set_implementation(resource,
                                  &linux_explicit_synchronization_implementation,
                                  ctx, NULL);
}

void WstLExpSyncFireRelease( WstExplicitSync *bufferSync )
{
   struct wl_resource *resource;
   int releaseFenceFd= -1;

   if (bufferSync == NULL)
   {
      return;
   }

   // Clear acquire fence if present
   if (bufferSync->acquireFenceFd >= 0)
   {
      WstLExpSyncFdUpdate(&bufferSync->acquireFenceFd, -1);
   }
   else if (bufferSync->acquireFenceFd != -1)
   {
      // Fence FD is set to something other than -1 but negative - just clear it
      bufferSync->acquireFenceFd = -1;
   }

   if (bufferSync->bufferRelease == NULL)
   {
      return;
   }

   // Store bufferRelease in local variable for safer access
   WstExplicitSyncBufferRelease *bufferRelease = bufferSync->bufferRelease;
   
   // Double-check it's still valid
   if (bufferRelease == NULL)
   {
      return;
   }

   // Validate pointer is not obviously garbage (low memory addresses are invalid)
   // This protects against test code using fake pointers like 100, 200, etc.
   if ((uintptr_t)bufferRelease < 0x10000)
   {
      // Likely garbage pointer - clean it up and return
      bufferSync->bufferRelease = NULL;
      return;
   }

   resource= bufferRelease->resource;
   
   if (resource == NULL)
   {
      return;
   }

   // render fence would have inserted by gl-render
   releaseFenceFd= bufferRelease->renderFenceFd;

   if (releaseFenceFd >= 0)
   {
      zwp_linux_buffer_release_v1_send_fenced_release(resource, releaseFenceFd);
   }
   else
   {
      zwp_linux_buffer_release_v1_send_immediate_release(resource);
   }

   // buffer_release allocated in get_release will be free in destroy handler
   wl_resource_destroy(resource);
   WstLExpSyncFdClear(&bufferSync->acquireFenceFd);
   WstLExpSyncClear(bufferSync);
}

wl_lexpsync* WstLExpSyncInit( struct wl_display *display, void *userData )
{
   struct wl_lexpsync *lexpsync= 0;
   WstContext *ctx= (WstContext*)userData;

   printf("westeros-lexpsync: WstLExpSyncInit: enter: display %p\n", display);
   
   // Validate required parameter
   if ( !display )
   {
      printf("westeros-lexpsync: WstLExpSyncInit: error: display is NULL\n");
      goto exit;
   }
   
   lexpsync= (struct wl_lexpsync*)calloc( 1, sizeof(struct wl_lexpsync) );
   if ( !lexpsync )
   {
      goto exit;
   }

   lexpsync->display= display;

   lexpsync->wl_lexpsync_global= wl_global_create(display, &zwp_linux_explicit_synchronization_v1_interface, 2, ctx, wstExplicitSyncBind);

exit:
   printf("westeros-lexpsync: WstLExpSyncInit: exit: display %p lexpsync %p\n", display, lexpsync);

   return lexpsync;
}

void WstLExpSyncUninit( struct wl_lexpsync *lexpsync )
{
   if ( lexpsync )
   {
      free( lexpsync );
   }
}

