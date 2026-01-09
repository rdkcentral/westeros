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

/*
 * UNIT TESTING SUPPORT:
 * The STATIC_TEST macro is used to conditionally make internal functions and
 * interfaces testable. When UNIT_TEST is defined (during test builds), it
 * expands to nothing for functions (giving them external linkage by default).
 * In production builds, it expands to 'static', keeping them file-local for encapsulation.
 * 
 * For const objects (like interface structures), we use STATIC_TEST_CONST which
 * expands to 'extern const' when UNIT_TEST is defined, because const objects have
 * internal linkage by default in C++.
 */
#ifdef UNIT_TEST
#define STATIC_TEST
#define STATIC_TEST_CONST extern const
#else
#define STATIC_TEST static
#define STATIC_TEST_CONST static const
#endif

#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#endif

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
  if ( !sync )
  {
     return;
  }
  sync->bufferRelease= 0;
  sync->acquireFenceFd= -1;
}

inline void WstLExpSyncMove(WstExplicitSync *target, WstExplicitSync *source)
{
  if ( !target || !source )
  {
     return;
  }
  *target= *source;
  WstLExpSyncClear(source);
}

inline void WstLExpSyncCopy(WstExplicitSync *target, WstExplicitSync *source)
{
  if ( !target || !source )
  {
     return;
  }
  *target= *source;
}

inline void WstLExpSyncFdUpdate(int *fd, int newFd)
{
   if ( !fd )
   {
      return;
   }
   
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
   if (!dest || !src)
   {
      return;
   }
   if (dest == src)
   {
      return;
   }
   WstLExpSyncFdUpdate(dest, *src);
   *src= -1;
}

inline void WstLExpSyncFdClear(int *fd)
{
   if ( !fd )
   {
      return;
   }
   WstLExpSyncFdUpdate(fd, -1);
}


struct wl_lexpsync* WstLExpSyncInit( struct wl_display *display, void *userData );
void WstLExpSyncUninit( struct wl_lexpsync *lexpsync );
void WstLExpSyncFireRelease( WstExplicitSync *bufferSync );

#endif

