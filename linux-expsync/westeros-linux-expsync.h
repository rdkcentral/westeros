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

#ifndef _WESTEROS_LINUX_EXPSYNC_H
#define _WESTEROS_LINUX_EXPSYNC_H

#include <unistd.h>

struct wl_lexpsync;

typedef struct _WstExplicitSyncBufferRelease
{
   struct wl_resource *resource;
   int renderFenceFd;
} WstExplicitSyncBufferRelease;

typedef struct _WstExplicitSync
{
   int acquireFenceFd;
   WstExplicitSyncBufferRelease* bufferRelease;
} WstExplicitSync;

inline void WstLExpSyncClear(WstExplicitSync *sync)
{
  sync->bufferRelease= 0;
  sync->acquireFenceFd= -1;
}

inline void WstLExpSyncMove(WstExplicitSync *target, WstExplicitSync *source)
{
  *target= *source;
  WstLExpSyncClear(source);
}

inline void WstLExpSyncCopy(WstExplicitSync *target, WstExplicitSync *source)
{
  *target= *source;
}

inline void WstLExpSyncFdUpdate(int *fd, int newFd)
{
   if ( *fd == newFd )
   {
      return;
   }

   if ( *fd >= 0 )
   {
      close(*fd);
   }

   *fd= newFd;
}

inline void WstLExpSyncFdMove(int *dest, int *src)
{
   if (dest == src)
   {
      return;
   }
   WstLExpSyncFdUpdate(dest, *src);
   *src= -1;
}

inline void WstLExpSyncFdClear(int *fd)
{
   WstLExpSyncFdUpdate(fd, -1);
}


wl_lexpsync* WstLExpSyncInit( struct wl_display *display, void *userData );
void WstLExpSyncUninit( struct wl_lexpsync *lexpsync );
void WstLExpSyncFireRelease( WstExplicitSync *bufferSync );

#endif

