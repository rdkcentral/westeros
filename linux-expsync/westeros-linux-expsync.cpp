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
#include <unistd.h>
#include <assert.h>
#include <inttypes.h>

#include "westeros-linux-expsync.h"

#ifndef UNIT_TEST
#include "wayland-server.h"
#else
#include "linux-explicit-synchronization-unstable-v1-server-protocol.h"
#endif

#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/sync_file.h>
#include <sys/ioctl.h>

/* Forward declarations for compositor types */
typedef struct _WstContext WstContext;
typedef struct _WstSurface {
   struct wl_resource *syncRes;
   WstExplicitSync createdBufferSync;
   WstExplicitSync attachedBufferSync;
   WstExplicitSync detachedBufferSync;
} WstSurface;

struct wl_lexpsync
{
   struct wl_display *display;
   struct wl_global *wl_lexpsync_global;
};

#ifdef UNIT_TEST
#define UNIT_TEST_STATIC
#else
#define UNIT_TEST_STATIC static
#endif

UNIT_TEST_STATIC bool wstLExpSyncFileIsValid(int fd)
{
   bool result;
   struct sync_file_info fInfo= { { 0 } };
   int rc;

   rc= ioctl(fd, SYNC_IOC_FILE_INFO, &fInfo);
   if ( rc < 0 )
   {
      result= false;
   }

   result= ( fInfo.num_fences > 0 ? true : false );

   return result;
}

UNIT_TEST_STATIC void wstLExpSyncBufferRelease(struct wl_resource *resource)
{
   WstExplicitSyncBufferRelease *bufferRelease= (WstExplicitSyncBufferRelease*)wl_resource_get_user_data(resource);

   WstLExpSyncFdClear(&bufferRelease->renderFenceFd);
   free(bufferRelease);
}

UNIT_TEST_STATIC void wstLExpSyncDestroySync(struct wl_resource *resource)
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
UNIT_TEST_STATIC void wstILExpSyncSurfaceSyncDestroy(struct wl_client *client,
                                           struct wl_resource *resource)
{
   wl_resource_destroy(resource);
}

UNIT_TEST_STATIC void wstILExpSyncSurfaceSyncSetAcquireFence(struct wl_client *client,
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

UNIT_TEST_STATIC void wstILExpSyncSurfaceSyncGetRelease(struct wl_client *client,
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

#ifdef UNIT_TEST
extern
#else
static
#endif
const struct zwp_linux_surface_synchronization_v1_interface linux_surface_synchronization_implementation = {
   wstILExpSyncSurfaceSyncDestroy,
   wstILExpSyncSurfaceSyncSetAcquireFence,
   wstILExpSyncSurfaceSyncGetRelease
};

UNIT_TEST_STATIC void wstILExpSyncDestroy(struct wl_client *client,
                                struct wl_resource *resource)
{
   (void)client;
   wl_resource_destroy(resource);
}

UNIT_TEST_STATIC void wstILExpSyncGetSynchronization(struct wl_client *client,
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

#ifdef UNIT_TEST
extern
#else
static
#endif
const struct zwp_linux_explicit_synchronization_v1_interface linux_explicit_synchronization_implementation = {
   wstILExpSyncDestroy,
   wstILExpSyncGetSynchronization
};

UNIT_TEST_STATIC void wstExplicitSyncBind( struct wl_client *client, void *data, uint32_t version, uint32_t id )
{
   WstContext *ctx= (WstContext*)data;
   struct wl_resource *resource;

   printf("wstExplicitSyncBind: client %p data %p version %d id %d", (void*)client, data, version, id );

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

   if (bufferSync->acquireFenceFd != -1)
   {
      WstLExpSyncFdUpdate(&bufferSync->acquireFenceFd, -1);
   }

   if (bufferSync->bufferRelease == NULL)
   {
      return;
   }

   // Additional validation to ensure bufferRelease is properly initialized
   if (bufferSync->bufferRelease->resource == NULL)
   {
      return;
   }

   resource= bufferSync->bufferRelease->resource;

   // render fence would have inserted by gl-render
   releaseFenceFd= bufferSync->bufferRelease->renderFenceFd;

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
      printf("westeros-lexpsync: WstLExpSyncInit: error: null display\n");
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

