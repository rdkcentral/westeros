/*
 * If not stated otherwise in this file or this component's LICENSE file the
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
 *
 */
 /*
 * Copyright © 2010 Intel Corporation
 * Copyright © 2011 Benjamin Franzke
 * Copyright © 2012-2013 Collabora, Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <math.h>
#include <vector>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <xkbcommon/xkbcommon.h>

#include "wayland-client.h"
#include "wayland-egl.h"
#include "simpleshell-client-protocol.h"

#define UNUSED(x) ((void)x)

#if !defined (XKB_KEYMAP_COMPILE_NO_FLAGS)
#define XKB_KEYMAP_COMPILE_NO_FLAGS XKB_MAP_COMPILE_NO_FLAGS
#endif

static void registryHandleGlobal(void *data, 
                                 struct wl_registry *registry, uint32_t id,
                                 const char *interface, uint32_t version);
static void registryHandleGlobalRemove(void *data, 
                                       struct wl_registry *registry,
                                       uint32_t name);

static const struct wl_registry_listener registryListener = 
{
   registryHandleGlobal,
   registryHandleGlobalRemove
};

static void shellSurfaceId(void *data,
                           struct wl_simple_shell *wl_simple_shell,
                           struct wl_surface *surface,
                           uint32_t surfaceId);
static void shellSurfaceCreated(void *data,
                                struct wl_simple_shell *wl_simple_shell,
                                uint32_t surfaceId,
                                const char *name);
static void shellSurfaceDestroyed(void *data,
                                  struct wl_simple_shell *wl_simple_shell,
                                  uint32_t surfaceId,
                                  const char *name);
static void shellSurfaceStatus(void *data,
                               struct wl_simple_shell *wl_simple_shell,
                               uint32_t surfaceId,
                               const char *name,
                               uint32_t visible,
                               int32_t x,
                               int32_t y,
                               int32_t width,
                               int32_t height,
                               wl_fixed_t opacity,
                               wl_fixed_t zorder);
static void shellGetSurfacesDone(void *data,
                                 struct wl_simple_shell *wl_simple_shell);
static void shellPopupDetails(void *data,
                              struct wl_simple_shell *wl_simple_shell,
                              uint32_t surfaceId,
                              uint32_t parentSurfaceId,
                              int32_t popup);

static const struct wl_simple_shell_listener shellListener = 
{
   shellSurfaceId,
   shellSurfaceCreated,
   shellSurfaceDestroyed,
   shellSurfaceStatus,
   shellGetSurfacesDone,
   shellPopupDetails
};

typedef enum _InputState
{
   InputState_main,
   InputState_attribute,
} InputState;

typedef enum _Attribute
{
   Attribute_position,
   Attribute_size,
   Attribute_visibility,
   Attribute_opacity,
   Attribute_zorder
} Attribute;

static const int SURFACE_INDEX_MAIN= 0;
static const int SURFACE_INDEX_POPUP= 1;
static const int MAX_POPUPS= 5;

typedef struct _AppSurface
{
   struct wl_surface *wlSurface;
   struct wl_buffer *shmBuffer;
   struct wl_egl_window *eglWindow;
   EGLSurface eglSurface;

   int eglWidth;
   int eglHeight;

   uint32_t surfaceId;
   uint32_t parentSurfaceId;

   int surfaceX;
   int surfaceY;
   int surfaceWidth;
   int surfaceHeight;

   int parentX;
   int parentY;
   int parentWidth;
   int parentHeight;

   int restoreX;
   int restoreY;
   int restoreWidth;
   int restoreHeight;

   float surfaceOpacity;
   float surfaceZOrder;
   bool surfaceVisible;

   bool moveMode;
   bool minimized;
   bool maximized;

   long long startTime;
   float color[4];
} AppSurface;

typedef struct _AppCtx
{
   struct wl_display *display;
   struct wl_registry *registry;
   struct wl_shm *shm;
   struct wl_compositor *compositor;
   struct wl_simple_shell *shell;
   struct wl_seat *seat;
   struct wl_keyboard *keyboard;
   struct wl_pointer *pointer;
   struct wl_touch *touch;
   struct wl_output *output;
   struct wl_callback *frameCallback;

   struct xkb_context *xkbCtx;
   struct xkb_keymap *xkbKeymap;
   struct xkb_state *xkbState;
   xkb_mod_index_t modAlt;
   xkb_mod_index_t modCtrl;

   EGLDisplay eglDisplay;
   EGLConfig eglConfig;
   EGLContext eglContext;   

   bool getShell;
   InputState inputState;
   Attribute attribute;

   bool haveMode;
   int planeX;
   int planeY;
   int planeWidth;
   int planeHeight;

   std::vector<AppSurface*> surfaces;

   int surfaceDX;
   int surfaceDY;
   int surfaceDWidth;
   int surfaceDHeight;
      
   struct
   {
      GLuint mvp;
      GLuint pos;
      GLuint col;
   } gl;

   bool noAnimation;
   bool needRedraw;
   bool verboseLog;
   int pointerX, pointerY;

   AppSurface* keyboardFocus;
   AppSurface* pointerFocus;
} AppCtx;

static AppSurface* getTopSurface( AppCtx *ctx );
static AppSurface* getSurfaceByID( AppCtx *ctx, uint32_t surfaceId );
static AppSurface* getSurfaceByWL( AppCtx *ctx, struct wl_surface* wlSurface );
static void processInput( AppCtx *ctx, uint32_t sym );
static void drawFrame( AppCtx *ctx, bool registerCallback );
static bool setupEGL( AppCtx *ctx );
static void termEGL( AppCtx *ctx );
static AppSurface* createWindowSurface( AppCtx *ctx, int width, int height );
static AppSurface* createPopupSurface( AppCtx *ctx, AppSurface *parentSurface );
static void destroySurface( AppCtx *ctx, AppSurface *surface );
static void resizeSurface( AppSurface *surface, int dx, int dy, int width, int height );
static bool setupGL( AppCtx *ctx );
static void renderGL( AppCtx *ctx, int width, int height, long long startTime, bool clearScreen );
static void renderPopup(AppCtx *ctx, int width, int height, long long startTime, const float* backgroundColor);

int g_running= 0;
int g_log= 0;

static void signalHandler(int signum)
{
   printf("signalHandler: signum %d\n", signum);
   g_running = 0;
}

static long long currentTimeMillis()
{
   long long timeMillis;
   struct timeval tv;   

   gettimeofday(&tv, NULL);
   timeMillis = (long long)tv.tv_sec * 1000LL + tv.tv_usec / 1000;
   
   return timeMillis;
}

static void shmFormat(void *data, struct wl_shm *wl_shm, uint32_t format)
{
   printf("shm format: %X\n", format);
}

struct wl_shm_listener shmListener = {
   shmFormat
};

static void keyboardKeymap( void *data, struct wl_keyboard *keyboard, uint32_t format, int32_t fd, uint32_t size )
{
   AppCtx *ctx= (AppCtx*)data;

   if ( format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1 )
   {
      void *map= mmap( 0, size, PROT_READ, MAP_SHARED, fd, 0 );
      if ( map != MAP_FAILED )
      {
         if ( !ctx->xkbCtx )
         {
            ctx->xkbCtx= xkb_context_new( XKB_CONTEXT_NO_FLAGS );
         }
         else
         {
            printf("error: xkb_context_new failed\n");
         }
         if ( ctx->xkbCtx )
         {
            if ( ctx->xkbKeymap )
            {
               xkb_keymap_unref( ctx->xkbKeymap );
               ctx->xkbKeymap= 0;
            }
            ctx->xkbKeymap= xkb_keymap_new_from_string( ctx->xkbCtx, (char*)map, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
            if ( !ctx->xkbKeymap )
            {
               printf("error: xkb_keymap_new_from_string failed\n");
            }
            if ( ctx->xkbState )
            {
               xkb_state_unref( ctx->xkbState );
               ctx->xkbState= 0;
            }
            ctx->xkbState= xkb_state_new( ctx->xkbKeymap );
            if ( !ctx->xkbState )
            {
               printf("error: xkb_state_new failed\n");
            }
            if ( ctx->xkbKeymap )
            {
               ctx->modAlt= xkb_keymap_mod_get_index( ctx->xkbKeymap, XKB_MOD_NAME_ALT );
               ctx->modCtrl= xkb_keymap_mod_get_index( ctx->xkbKeymap, XKB_MOD_NAME_CTRL );
            }
         }
         munmap( map, size );
      }
   }

   close( fd );
}

static void keyboardEnter( void *data, struct wl_keyboard *keyboard, uint32_t serial,
                           struct wl_surface *surface, struct wl_array *keys )
{
   AppCtx *ctx= (AppCtx*)data;
   UNUSED(keyboard);
   UNUSED(serial);
   UNUSED(keys);

   ctx->keyboardFocus= getSurfaceByWL(ctx, surface);
   if ( ctx->keyboardFocus )
   {
      printf("keyboardEnter surfaceId=%u wlSurface=%p\n", ctx->keyboardFocus->surfaceId, surface );
   }
   else
   {
      printf("keyboardEnter unexpected wlSurface %p\n", surface);
   }
}

static void keyboardLeave( void *data, struct wl_keyboard *keyboard, uint32_t serial, struct wl_surface *surface )
{
   AppCtx *ctx= (AppCtx*)data;
   UNUSED(keyboard);
   UNUSED(serial);

   if ( ctx->keyboardFocus && ctx->keyboardFocus->wlSurface == surface )
   {
      printf("keyboardLeave surfaceId=%u wlSurface=%p\n", ctx->keyboardFocus->surfaceId, surface );
      ctx->keyboardFocus= 0;
   }
   else
   {
      printf("keyboardLeave error: unexpected wlSurface %p\n", surface);
   }
}

static void keyboardKey( void *data, struct wl_keyboard *keyboard, uint32_t serial,
                         uint32_t time, uint32_t key, uint32_t state )
{
   AppCtx *ctx= (AppCtx*)data;
   UNUSED(keyboard);
   UNUSED(serial);
   xkb_keycode_t keyCode;
   uint32_t sym;

   if ( ctx->xkbState )
   {
      // As per wayland protocol for XKB_V1 map, we must add 8 to the key code
      keyCode= key+8;

      sym= xkb_state_key_get_one_sym( ctx->xkbState, keyCode );

      if ( ctx->verboseLog )
      {
         int ctrl= 0;
         int alt= 0;

         if ( xkb_state_mod_index_is_active( ctx->xkbState, ctx->modCtrl, XKB_STATE_MODS_DEPRESSED) == 1 )
         {
            ctrl= 1;
         }

         if ( xkb_state_mod_index_is_active( ctx->xkbState, ctx->modAlt, XKB_STATE_MODS_DEPRESSED) == 1 )
         {
            alt= 1;
         }

         printf("keyboardKey: surfaceId=%u wlSurface=%p sym=%X state=%s ctrl=%d alt=%d time=%u\n",
            ctx->keyboardFocus ? ctx->keyboardFocus->surfaceId : -1, 
            ctx->keyboardFocus ? ctx->keyboardFocus->wlSurface : 0,
            sym, (state == WL_KEYBOARD_KEY_STATE_PRESSED ? "Down" : "Up"), 
            ctrl, alt, time);
      }

      if ( state == WL_KEYBOARD_KEY_STATE_PRESSED )
      {
         processInput( ctx, sym );
      }
   }
}

static void keyboardModifiers( void *data, struct wl_keyboard *keyboard, uint32_t serial,
                               uint32_t mods_depressed, uint32_t mods_latched,
                               uint32_t mods_locked, uint32_t group )
{
   AppCtx *ctx= (AppCtx*)data;
   if ( ctx->xkbState )
   {
      if ( ctx->verboseLog )
      {
         printf("keyboardModifiers: surfaceId=%u wlSurface=%p dep=%X latch=%X lock=%X grp=%X\n",
            ctx->keyboardFocus ? ctx->keyboardFocus->surfaceId : -1, 
            ctx->keyboardFocus ? ctx->keyboardFocus->wlSurface : 0,
            mods_depressed, mods_latched, mods_locked, group);
      }
      xkb_state_update_mask( ctx->xkbState, mods_depressed, mods_latched, mods_locked, 0, 0, group );
   }
}

static void keyboardRepeatInfo( void *data, struct wl_keyboard *keyboard, int32_t rate, int32_t delay )
{
   UNUSED(keyboard);
   AppCtx *ctx= (AppCtx*)data;
   if ( ctx->verboseLog )
   {
      printf("keyboardRepeatInfo: surfaceId=%u wlSurface=%p rate=%d delay=%d\n",
         ctx->keyboardFocus ? ctx->keyboardFocus->surfaceId : -1, 
         ctx->keyboardFocus ? ctx->keyboardFocus->wlSurface : 0,
         rate, delay);
   }
}

static const struct wl_keyboard_listener keyboardListener= {
   keyboardKeymap,
   keyboardEnter,
   keyboardLeave,
   keyboardKey,
   keyboardModifiers,
   keyboardRepeatInfo
};

static void pointerEnter( void* data, struct wl_pointer *pointer, uint32_t serial,
                          struct wl_surface *surface, wl_fixed_t sx, wl_fixed_t sy )
{
   UNUSED(pointer);
   UNUSED(serial);
   AppCtx *ctx= (AppCtx*)data;
   int x, y;

   x= wl_fixed_to_int( sx );
   y= wl_fixed_to_int( sy );

   ctx->pointerX= x;
   ctx->pointerY= y;

   ctx->pointerFocus= getSurfaceByWL(ctx, surface);
   if ( ctx->pointerFocus )
   {
      printf("pointerEnter surfaceId=%u wlSurface=%p\n", ctx->pointerFocus->surfaceId, surface );
   }
   else
   {
      printf("pointerEnter unexpected wlSurface %p\n", surface);
   }
}

static void pointerLeave( void* data, struct wl_pointer *pointer, uint32_t serial, struct wl_surface *surface )
{
   AppCtx *ctx= (AppCtx*)data;
   UNUSED(pointer);
   UNUSED(serial);

   if ( ctx->pointerFocus && ctx->pointerFocus->wlSurface == surface )
   {
      printf("pointerLeave surfaceId=%u wlSurface=%p\n", ctx->pointerFocus->surfaceId, surface );
      ctx->pointerFocus= 0;
   }
   else
   {
      printf("pointerLeave error: unexpected wlSurface %p\n", surface);
   }
}

static void pointerMotion( void *data, struct wl_pointer *pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy )
{
   UNUSED(pointer);
   AppCtx *ctx= (AppCtx*)data;
   int x, y;

   x= wl_fixed_to_int( sx );
   y= wl_fixed_to_int( sy );

   ctx->pointerX= x;
   ctx->pointerY= y;

   if ( ctx->verboseLog )
   {
      //printf("pointer motion surface (%d,%d) time %u\n", x, y, time );
   }
}

static void pointerButton( void *data, struct wl_pointer *pointer, uint32_t serial,
                           uint32_t time, uint32_t button, uint32_t state )
{
   UNUSED(pointer);
   UNUSED(serial);
   UNUSED(time);
   AppCtx *ctx= (AppCtx*)data;

   AppSurface *popupSurface= ctx->surfaces.size() > 1 ? getTopSurface( ctx ) : 0;

   if ( ctx->surfaces.size() == 1 )
   {
      printf("pointerButton surfaceId=%u wlSurface=%p button=%u state=%u\n",
         ctx->pointerFocus ? ctx->pointerFocus->surfaceId : -1, 
         ctx->pointerFocus ? ctx->pointerFocus->wlSurface : 0,
         button, state );
   }
   else
   {
      AppSurface *popup= getTopSurface( ctx );
      printf("pointerButton surfaceId=%u wlSurface=%p popupId=%u wlPopup=%p button=%u state=%u\n",
         ctx->pointerFocus ? ctx->pointerFocus->surfaceId : -1, 
         ctx->pointerFocus ? ctx->pointerFocus->wlSurface : 0,
         popup ? popup->surfaceId : 0, 
         popup ? popup->wlSurface : 0,
         button, state );

      if (ctx->pointerFocus != popupSurface)
      {
         printf("pointerButton error: popup %u lost focus\n", popup ? popup->surfaceId : 0);   
      }
   }
}

static void pointerAxis( void *data, struct wl_pointer *pointer, uint32_t time,
                         uint32_t axis, wl_fixed_t value )
{
   UNUSED(data);
   UNUSED(pointer);
   UNUSED(time);
   int v;

   v= wl_fixed_to_int( value );
   printf("pointer axis %u value %d\n", axis, v);
}

static const struct wl_pointer_listener pointerListener = {
   pointerEnter,
   pointerLeave,
   pointerMotion,
   pointerButton,
   pointerAxis
};

static void touchHandleDown( void *data, struct wl_touch *touch,
                             uint32_t serial, uint32_t time, struct wl_surface *surface,
                             int32_t id, wl_fixed_t sx, wl_fixed_t sy )
{
   UNUSED(touch);
   UNUSED(serial);
   UNUSED(surface);
   AppCtx *ctx= (AppCtx*)data;

   int x, y;

   x= wl_fixed_to_int( sx );
   y= wl_fixed_to_int( sy );

   if ( ctx->verboseLog )
   {
      printf("touch down id %d (%d,%d) time %u\n", id, x, y, time);
   }
}

static void touchHandleUp( void *data, struct wl_touch *touch,
                           uint32_t serial, uint32_t time, int32_t id )
{
   UNUSED(touch);
   UNUSED(serial);
   AppCtx *ctx= (AppCtx*)data;

   if ( ctx->verboseLog )
   {
      printf("touch up id %d time %u\n", id, time);
   }
}

static void touchHandleMotion( void *data, struct wl_touch *touch,
                               uint32_t time, int32_t id, wl_fixed_t sx, wl_fixed_t sy )
{
   UNUSED(touch);
   AppCtx *ctx= (AppCtx*)data;

   int x, y;

   x= wl_fixed_to_int( sx );
   y= wl_fixed_to_int( sy );

   if ( ctx->verboseLog )
   {
      printf("touch motion id %d (%d,%d) time %u\n", id, x, y, time);
   }
}

static void touchHandleFrame( void *data, struct wl_touch *touch )
{
   UNUSED(touch);
   AppCtx *ctx= (AppCtx*)data;

   if ( ctx->verboseLog )
   {
      printf("touch frame\n");
   }
}

static const struct wl_touch_listener touchListener= {
   touchHandleDown,
   touchHandleUp,
   touchHandleMotion,
   touchHandleFrame
};

static void seatCapabilities( void *data, struct wl_seat *seat, uint32_t capabilities )
{
   AppCtx *ctx = (AppCtx*)data;

   printf("seat %p caps: %X\n", seat, capabilities );
   
   if ( capabilities & WL_SEAT_CAPABILITY_KEYBOARD )
   {
      printf("  seat has keyboard\n");
      ctx->keyboard= wl_seat_get_keyboard( ctx->seat );
      printf("  keyboard %p\n", ctx->keyboard );
      wl_keyboard_add_listener( ctx->keyboard, &keyboardListener, ctx );
   }
   if ( capabilities & WL_SEAT_CAPABILITY_POINTER )
   {
      printf("  seat has pointer\n");
      ctx->pointer= wl_seat_get_pointer( ctx->seat );
      printf("  pointer %p\n", ctx->pointer );
      wl_pointer_add_listener( ctx->pointer, &pointerListener, ctx );
   }
   if ( capabilities & WL_SEAT_CAPABILITY_TOUCH )
   {
      printf("  seat has touch\n");
      ctx->touch= wl_seat_get_touch( ctx->seat );
      printf("  touch %p\n", ctx->touch );
      wl_touch_add_listener( ctx->touch, &touchListener, ctx );
   }   
}

static void seatName( void *data, struct wl_seat *seat, const char *name )
{
   printf("seat %p name: %s\n", seat, name);
}

static const struct wl_seat_listener seatListener = {
   seatCapabilities,
   seatName 
};

static void outputGeometry( void *data, struct wl_output *output, int32_t x, int32_t y,
                            int32_t physical_width, int32_t physical_height, int32_t subpixel,
                            const char *make, const char *model, int32_t transform )
{
   UNUSED(data);
   UNUSED(output);
   UNUSED(x);
   UNUSED(y);
   UNUSED(physical_width);
   UNUSED(physical_height);
   UNUSED(subpixel);
   UNUSED(make);
   UNUSED(model);
   UNUSED(transform);
}

static void outputMode( void *data, struct wl_output *output, uint32_t flags,
                        int32_t width, int32_t height, int32_t refresh )
{
   AppCtx *ctx = (AppCtx*)data;
   AppSurface* surface = NULL;

   if ( flags & WL_OUTPUT_MODE_CURRENT )
   {
      ctx->haveMode= true;
      if ( (width !=  ctx->planeWidth) || (height != ctx->planeHeight) )
      {
         ctx->planeWidth= width;
         ctx->planeHeight= height;
         if ( ctx->verboseLog )
         {
            printf("outputMode: resize egl window to (%d,%d)\n", ctx->planeWidth, ctx->planeHeight );
         }

         if ( ctx->surfaces.empty() )
         {
            printf("outputMode: no surfaces\n");
            return;
         }

         //main surface is 0
         surface= ctx->surfaces[0];

         float widthRatio = (float)width / (float)surface->surfaceWidth;
         float heightRatio = (float)height / (float)surface->surfaceHeight;

         //resize main surface to fit the plane
         resizeSurface( surface, 0, 0, ctx->planeWidth, ctx->planeHeight);

         //scale child surfaces up or down to preserve their relative orientation
         for (uint32_t i = 1; i < ctx->surfaces.size(); ++i)
         {
            AppSurface* child = ctx->surfaces[i];
            resizeSurface( ctx->surfaces[i], 0, 0, 
               child->surfaceWidth * widthRatio, 
               child->surfaceHeight * heightRatio);
         }
      }
   }
}

static void outputDone( void *data, struct wl_output *output )
{
   UNUSED(data);
   UNUSED(output);
}

static void outputScale( void *data, struct wl_output *output, int32_t factor )
{
   UNUSED(data);
   UNUSED(output);
   UNUSED(factor);
}

static const struct wl_output_listener outputListener = {
   outputGeometry,
   outputMode,
   outputDone,
   outputScale
};

static void registryHandleGlobal(void *data, 
                                 struct wl_registry *registry, uint32_t id,
                                 const char *interface, uint32_t version)
{
   AppCtx *ctx = (AppCtx*)data;
   int len;

   printf("westeros-test: registry: id %d interface (%s) version %d\n", id, interface, version );

   len= strlen(interface);
   if ( (len==6) && !strncmp(interface, "wl_shm", len)) {
      ctx->shm= (struct wl_shm*)wl_registry_bind(registry, id, &wl_shm_interface, 1);
      printf("shm %p\n", ctx->shm);
      wl_shm_add_listener(ctx->shm, &shmListener, ctx);
   }
   else if ( (len==13) && !strncmp(interface, "wl_compositor", len) ) {
      ctx->compositor= (struct wl_compositor*)wl_registry_bind(registry, id, &wl_compositor_interface, 1);
      printf("compositor %p\n", ctx->compositor);
   } 
   else if ( (len==7) && !strncmp(interface, "wl_seat", len) ) {
      ctx->seat= (struct wl_seat*)wl_registry_bind(registry, id, &wl_seat_interface, 4);
      printf("seat %p\n", ctx->seat);
      wl_seat_add_listener(ctx->seat, &seatListener, ctx);
   }
   else if ( (len==9) && !strncmp(interface, "wl_output", len) ) {
      ctx->output= (struct wl_output*)wl_registry_bind(registry, id, &wl_output_interface, 2);
      printf("output %p\n", ctx->output);
      wl_output_add_listener(ctx->output, &outputListener, ctx);
      wl_display_roundtrip(ctx->display);
   }
   else if ( (len==15) && !strncmp(interface, "wl_simple_shell", len) ) {
      if ( ctx->getShell ) {
         ctx->shell= (struct wl_simple_shell*)wl_registry_bind(registry, id, &wl_simple_shell_interface, 1);
         printf("shell %p\n", ctx->shell );
         wl_simple_shell_add_listener(ctx->shell, &shellListener, ctx);
      }
   }
}

static void registryHandleGlobalRemove(void *data, 
                                       struct wl_registry *registry,
                                       uint32_t name)
{
}

static void shellSurfaceId(void *data,
                           struct wl_simple_shell *wl_simple_shell,
                           struct wl_surface *surface,
                           uint32_t surfaceId)
{
   AppCtx *ctx = (AppCtx*)data;
   char name[50];
   AppSurface* surf = NULL;

   printf("shellSurfaceId: id=%u\n", surfaceId);

   surf = getSurfaceByWL( ctx, surface );
   if (!surf)
   {
      printf("shellSurfaceId: surface not found\n");
      return;
   }
   surf->surfaceId = surfaceId;

   sprintf( name, "westeros-test-surface-%u", surfaceId );
   wl_simple_shell_set_name( ctx->shell, surfaceId, name );
}
                           
static void shellSurfaceCreated(void *data,
                                struct wl_simple_shell *wl_simple_shell,
                                uint32_t surfaceId,
                                const char *name)
{
   AppCtx *ctx = (AppCtx*)data;
   printf("shellSurfaceCreated: id=%u name=%s\n", surfaceId, name);
   wl_simple_shell_get_status( ctx->shell, surfaceId );
}

static void shellSurfaceDestroyed(void *data,
                                  struct wl_simple_shell *wl_simple_shell,
                                  uint32_t surfaceId,
                                  const char *name)
{
   AppCtx *ctx = (AppCtx*)data;
   printf("shellSurfaceDestroyed: id=%u name=%s\n", surfaceId, name);
}
                                  
static void shellSurfaceStatus(void *data,
                               struct wl_simple_shell *wl_simple_shell,
                               uint32_t surfaceId,
                               const char *name,
                               uint32_t visible,
                               int32_t x,
                               int32_t y,
                               int32_t width,
                               int32_t height,
                               wl_fixed_t opacity,
                               wl_fixed_t zorder)
{
   AppCtx *ctx = (AppCtx*)data;
   AppSurface* surf;

   printf("shellSurfaceStatus: id=%u name=%s x=%d y=%d w=%d h=%d visible=%u opacity=%f zorder=%f\n", 
      surfaceId, name, x, y, width, height, visible, 
      wl_fixed_to_double(opacity), wl_fixed_to_double(zorder));

   surf= getSurfaceByID( ctx, surfaceId );
   if ( !surf )
   {
      printf("shellSurfaceStatus: surface not found\n");
      return;
   }

   surf->surfaceVisible= visible;
   surf->surfaceX= x;
   surf->surfaceY= y;
   surf->surfaceWidth= width;
   surf->surfaceHeight= height;
   surf->surfaceOpacity= wl_fixed_to_double(opacity);
   surf->surfaceZOrder= wl_fixed_to_double(zorder);   
}                               

static void shellGetSurfacesDone(void *data,
                                 struct wl_simple_shell *wl_simple_shell)
{
   printf("shell: get all surfaces done\n");
}                                        

static void shellPopupDetails(void *data,
                              struct wl_simple_shell *wl_simple_shell,
                              uint32_t surfaceId,
                              uint32_t parentSurfaceId,
                              int32_t popup)
{
   AppCtx *ctx = (AppCtx*)data;
   AppSurface *surf;
   UNUSED(wl_simple_shell);

   surf= getSurfaceByID( ctx, surfaceId );
   if ( !surf )
   {
      printf("shellPopupDetails: surface not found\n");
      return;
   }

   printf("shellPopupDetails: surface %u popup=%d parent=%u\n",
      surfaceId, popup, parentSurfaceId);
}

#define NON_BLOCKING_ENABLED (0)
#define NON_BLOCKING_DISABLED (1)

static void setBlockingMode(int blockingState )  
{  
   struct termios ttystate;
   int mask, bits;  
 
   mask= (blockingState == NON_BLOCKING_ENABLED) ? ~(ICANON|ECHO) : -1;
   bits= (blockingState == NON_BLOCKING_ENABLED) ? 0 : (ICANON|ECHO);

   // Obtain the current terminal state and alter the attributes to achieve 
   // the requested blocking behaviour
   tcgetattr(STDIN_FILENO, &ttystate);  

   ttystate.c_lflag= ((ttystate.c_lflag & mask) | bits);  
 
   if (blockingState == NON_BLOCKING_ENABLED)  
   {  
       ttystate.c_cc[VMIN]= 1;  
   }  

   tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);   
}

static bool isKeyHit()
{
   bool keyHit= false;
   fd_set fdset;
   struct timeval tval;

   // do a non-blocking check to see if any keys
   // are ready to read from stdin
   tval.tv_sec= 0;
   tval.tv_usec= 0;
   FD_ZERO(&fdset);
   FD_SET(STDIN_FILENO, &fdset);
   select(STDIN_FILENO+1, &fdset, NULL, NULL, &tval);

   keyHit= FD_ISSET(STDIN_FILENO, &fdset);

   return keyHit;
}

static void adjustAttribute( AppCtx* ctx, AppSurface *surface, uint32_t sym )
{
   switch( ctx->attribute )
   {
      case Attribute_position:
         switch( sym )
         {
            case XKB_KEY_Up:
               ctx->surfaceDY -= 5;
               break;
            case XKB_KEY_Down:
               ctx->surfaceDY += 5;
               break;
            case XKB_KEY_Right:
               ctx->surfaceDX += 5;
               break;
            case XKB_KEY_Left:
               ctx->surfaceDX -= 5;
               break;
         }
         break;
      case Attribute_size:
         switch( sym )
         {
            case XKB_KEY_Up:
               ctx->surfaceDHeight += 5;
               break;
            case XKB_KEY_Down:
               ctx->surfaceDHeight -= 5;
               break;
            case XKB_KEY_Right:
               ctx->surfaceDWidth += 5;
               break;
            case XKB_KEY_Left:
               ctx->surfaceDWidth -= 5;
               break;
         }
         break;
      case Attribute_visibility:
         switch( sym )
         {
            case XKB_KEY_Up:
            case XKB_KEY_Right:
            case XKB_KEY_Down:
            case XKB_KEY_Left:
               surface->surfaceVisible= !surface->surfaceVisible;
               wl_simple_shell_set_visible( ctx->shell, surface->surfaceId, (surface->surfaceVisible ? 1 : 0) );
               break;
         }
         break;
      case Attribute_opacity:
         switch( sym )
         {
            case XKB_KEY_Up:
            case XKB_KEY_Right:
               surface->surfaceOpacity += 0.1;
               if ( surface->surfaceOpacity > 1.0 )
               {
                  surface->surfaceOpacity= 1.0;
               }
               wl_simple_shell_set_opacity( ctx->shell, surface->surfaceId, wl_fixed_from_double(surface->surfaceOpacity) );
               break;
            case XKB_KEY_Down:
            case XKB_KEY_Left:
               surface->surfaceOpacity -= 0.1;
               if ( surface->surfaceOpacity < 0.0 )
               {
                  surface->surfaceOpacity= 0.0;
               }
               wl_simple_shell_set_opacity( ctx->shell, surface->surfaceId, wl_fixed_from_double(surface->surfaceOpacity) );
               break;
         }
         break;
      case Attribute_zorder:
         switch( sym )
         {
            case XKB_KEY_Up:
            case XKB_KEY_Right:
               surface->surfaceZOrder += 0.1;
               if ( surface->surfaceZOrder > 1.0 )
               {
                  surface->surfaceZOrder= 1.0;
               }
               wl_simple_shell_set_zorder( ctx->shell, surface->surfaceId, wl_fixed_from_double(surface->surfaceZOrder) );
               break;
            case XKB_KEY_Down:
            case XKB_KEY_Left:
               surface->surfaceZOrder -= 0.1;
               if ( surface->surfaceZOrder < 0.0 )
               {
                  surface->surfaceZOrder= 0.0;
               }
               wl_simple_shell_set_zorder( ctx->shell, surface->surfaceId, wl_fixed_from_double(surface->surfaceZOrder) );
               break;
         }
         break;
   }
}

static void processInputMain( AppCtx *ctx, uint32_t sym )
{
   AppSurface* surface = getTopSurface( ctx );
   if (!surface)
   {
      printf("processInputMain: no surfaces\n");
      return;
   }

   switch( sym )
   {
      case XKB_KEY_Left:
      case XKB_KEY_Up:
      case XKB_KEY_Right:
      case XKB_KEY_Down:
         adjustAttribute( ctx, surface, sym );
         break;
      case XKB_KEY_a:
         ctx->inputState= InputState_attribute;
         printf("attribute: (p) osition, (s) ize, (v) isible, (o) pacity, (z) order (x) back to main\n");
         break;
      case XKB_KEY_s:
         wl_simple_shell_get_status( ctx->shell, surface->surfaceId );
         break;
      case XKB_KEY_l:
         printf("get all surfaces:\n");
         wl_simple_shell_get_surfaces( ctx->shell );
         break;
      case XKB_KEY_r:
         if ( ctx->haveMode )
         {
            int width= (surface->surfaceWidth == ctx->planeWidth) ? ctx->planeWidth/2 : ctx->planeWidth;
            int height= (surface->surfaceHeight == ctx->planeHeight) ? ctx->planeHeight/2 : ctx->planeHeight;
            printf("resize egl window to (%d,%d)\n", width, height);
            resizeSurface( surface, 0, 0, width, height );
         }
         break;
      case XKB_KEY_p:
         createPopupSurface( ctx, surface );
         break;
      case XKB_KEY_d:
         if (surface != ctx->surfaces[0])
         {
            printf("Destroying popup surface\n");
            destroySurface( ctx, surface );
            wl_display_flush( ctx->display );
            ctx->needRedraw= true;
         }
         break;
      case XKB_KEY_i:
         if (surface != ctx->surfaces[0])
         {
            printf("Getting popup details\n");
            wl_simple_shell_is_surface_popup( ctx->shell, surface->surfaceId );
            wl_display_flush( ctx->display );
         }
         break;
      case XKB_KEY_q:
         printf("exiting\n");
         g_running = 0;
         break;
      default:
         break;
   }
}

static void processInputAttribute( AppCtx *ctx, uint32_t sym )
{
   switch( sym )
   {
      case XKB_KEY_p:
         ctx->attribute= Attribute_position;
         break;
      case XKB_KEY_s:
         ctx->attribute= Attribute_size;
         break;
      case XKB_KEY_v:
         ctx->attribute= Attribute_visibility;
         break;
      case XKB_KEY_o:
         ctx->attribute= Attribute_opacity;
         break;
      case XKB_KEY_z:
         ctx->attribute= Attribute_zorder;
         break;
      default:
      case 'x':
         break;
   }
   ctx->inputState= InputState_main;
}

static void processInput( AppCtx *ctx, uint32_t sym )
{
   switch( ctx->inputState )
   {
      case InputState_main:
         processInputMain( ctx, sym );
         break;
      case InputState_attribute:
         processInputAttribute( ctx, sym );
         break;
   }
}

static AppSurface* getTopSurface( AppCtx* ctx )
{
   AppSurface* surface = NULL;
   float topZOrder = -1.0f;
   for (uint32_t i = 0; i < ctx->surfaces.size(); i++)
   {
      if ( ctx->surfaces[i]->surfaceZOrder > topZOrder )
      {
         surface = ctx->surfaces[i];
         topZOrder = surface->surfaceZOrder;
      }
   }
   return surface;
}

static AppSurface* getSurfaceByID( AppCtx *ctx, uint32_t surfaceId )
{
   for (uint32_t i = 0; i < ctx->surfaces.size(); i++)
   {
      if ( ctx->surfaces[i]->surfaceId == surfaceId)
      {
         return ctx->surfaces[i];
      }
   }
   return NULL;
}

static AppSurface* getSurfaceByWL( AppCtx *ctx, struct wl_surface* wlSurface )
{
   for (uint32_t i = 0; i < ctx->surfaces.size(); i++)
   {
      if ( ctx->surfaces[i]->wlSurface == wlSurface)
      {
         return ctx->surfaces[i];
      }
   }
   return NULL;
}

static void showUsage()
{
   printf("usage:\n");
   printf(" westeros_test [options]\n" );
   printf("where [options] are:\n" );
   printf("  --delay <delay> : render loop delay\n" );
   printf("  --shell : use wl_simple_shell protocol\n" );
   printf("  --display <name> : wayland display to connect to\n" );
   printf("  --noframe : don't pace rendering with frame requests\n" );
   printf("  --noanimate : don't use animation\n" );
   printf("  --verbose : verbose logging\n" );
   printf("  -? : show usage\n" );
   printf("\n" );
}

static void redraw( void *data, struct wl_callback *callback, uint32_t time )
{
   AppCtx *ctx= (AppCtx*)data;

   if ( g_log ) printf("redraw: time %u\n", time);
   wl_callback_destroy( callback );

   ctx->needRedraw= true;
}

static struct wl_callback_listener frameListener=
{
   redraw
};

static void drawFrame( AppCtx *ctx, bool registerCallback )
{
   if ( ctx->surfaceDX || ctx->surfaceDY || ctx->surfaceDWidth || ctx->surfaceDHeight )
   {
      AppSurface* top = getTopSurface( ctx );
      top->surfaceX += ctx->surfaceDX;
      top->surfaceY += ctx->surfaceDY;
      top->surfaceWidth += ctx->surfaceDWidth;
      top->surfaceHeight += ctx->surfaceDHeight;
      
      wl_simple_shell_set_geometry( ctx->shell, top->surfaceId, 
                                    top->surfaceX, top->surfaceY, top->surfaceWidth, top->surfaceHeight );
   }

   for ( uint32_t i = 0; i < ctx->surfaces.size(); ++i )
   {
      AppSurface* surface = ctx->surfaces[i];

      if ( ctx->surfaces.size() > 1 )
      {
         //EGL context switching per surface; render as a window with border
         eglMakeCurrent( ctx->eglDisplay, surface->eglSurface, surface->eglSurface, ctx->eglContext );
      }

      if ( i == 0 )
      {
         renderGL( ctx, surface->eglWidth, surface->eglHeight, surface->startTime, true );
      }
      else
      {
         renderPopup( ctx, surface->eglWidth, surface->eglHeight, surface->startTime, surface->color );
      }

      if ( registerCallback && i == 0 )
      {  
         // register frame callback on main surface before surface commit is performed by eglSwapBuffers
         ctx->frameCallback= wl_surface_frame( surface->wlSurface );
         wl_callback_add_listener( ctx->frameCallback, &frameListener, ctx );
      }

      eglSwapBuffers(ctx->eglDisplay, surface->eglSurface );
   }
}

#define NUM_EVENTS (20)
int main( int argc, char** argv)
{
   int nRC= 0;
   AppCtx ctx;
   AppSurface *mainSurface = NULL;
   struct sigaction sigint;
   struct wl_display *display= 0;
   struct wl_registry *registry= 0;
   int delay= 16667;
   const char *display_name= 0;
   bool paceRendering= true;
   bool isBackgroundProcess;

   printf("westeros_test: v1.0\n" );

   isBackgroundProcess= ( getpgrp() != tcgetpgrp(STDOUT_FILENO) );

   memset( &ctx, 0, sizeof(AppCtx) );

   for( int i= 1; i < argc; ++i )
   {
      if ( !strcmp( (const char*)argv[i], "--delay") )
      {
         printf("got delay: i %d argc %d\n", i, argc );
         if ( i+1 < argc )
         {
            int v= atoi(argv[++i]);
            printf("v=%d\n", v);
            if ( v > 0 && v < 1000000 )
            {
               delay= v;
               printf("using delay=%d\n", delay );
            }
	    	else if ( v >= 1000000 )
            {
               printf("warning: delay value %d exceeds maximum (999999), keeping previous value\n", v);
            }
         }
      }
      else if (!strcmp( (const char*)argv[i], "--shell" ) )
      {
         ctx.getShell= true;
      }
      else if ( !strcmp( (const char*)argv[i], "--display") )
      {
         if ( i+1 < argc )
         {
            ++i;
            display_name= argv[i];
         }
      }
      else if (!strcmp( (const char*)argv[i], "--noframe" ) )
      {
         paceRendering= false;
      }
      else if (!strcmp( (const char*)argv[i], "--verbose" ) )
      {
         ctx.verboseLog= true;
      }
      else if (!strcmp( (const char*)argv[i], "--log" ) )
      {
         g_log= true;
      }
      else if (!strcmp( (const char*)argv[i], "--noanimate" ) )
      {
         ctx.noAnimation= true;
      }
      else if ( !strcmp( (const char*)argv[i], "-?" ) )
      {
         showUsage();
         goto exit;
      }
   }

   
   if ( display_name )
   {
      printf("calling wl_display_connect for display name %s\n", display_name);
   }
   else
   {
      printf("calling wl_display_connect for default display\n");
   }
   display= wl_display_connect(display_name);
   printf("wl_display_connect: display=%p\n", display);
   if ( !display )
   {
      printf("error: unable to connect to primary display\n");
      nRC= -1;
      goto exit;
   }

   printf("calling wl_display_get_registry\n");
   registry= wl_display_get_registry(display);
   printf("wl_display_get_registry: registry=%p\n", registry);
   if ( !registry )
   {
      printf("error: unable to get display registry\n");
      nRC= -2;
      goto exit;
   }

   ctx.display= display;
   ctx.registry= registry;
   ctx.planeX= 0;
   ctx.planeY= 0;
   ctx.planeWidth= 1280;
   ctx.planeHeight= 720;
   wl_registry_add_listener(registry, &registryListener, &ctx);
   
   wl_display_roundtrip(ctx.display);
   
   if ( !setupEGL(&ctx) )
   {
      printf("setupEGL failed\n");
      return 1;
   }

   mainSurface= createWindowSurface( &ctx, ctx.planeWidth, ctx.planeHeight);
   if ( !mainSurface )
   {
      printf("createWindowSurface failed\n");
      termEGL( &ctx );
      return 1;
   }
   
   
   eglSwapInterval( ctx.eglDisplay, 1 );
   printf("swap interface set to 1\n" );//TODO add 'swap' command line parameter
   
   setupGL(&ctx);
   
   if ( paceRendering )
   {
      drawFrame(&ctx, true);
   }
  
   sigint.sa_handler = signalHandler;
   sigemptyset(&sigint.sa_mask);
   sigint.sa_flags = SA_RESETHAND;
   sigaction(SIGINT, &sigint, NULL);

   if ( !isBackgroundProcess )
   {
      //setBlockingMode(NON_BLOCKING_ENABLED);
   }

   ctx.inputState= InputState_main;
   ctx.attribute= Attribute_position;
   
   g_running= 1;
   while( g_running )
   {
      if ( wl_display_dispatch( ctx.display ) == -1 )
      {
         int err= wl_display_get_error( ctx.display );
         printf("failed wl_display_dispatch (wl_display_get_error=%d: %s)\n", err, strerror(err) );
         break;
      }

      if ( !paceRendering )
      {
         if ( delay > 0 )
         {
            usleep(delay);
         }
         drawFrame( &ctx, false );
      }
      else if ( ctx.needRedraw )
      {
         ctx.needRedraw= false;
         drawFrame( &ctx, true );
      }

      if ( ctx.getShell && !isBackgroundProcess )
      {      
         if ( isKeyHit() )
         {
            uint32_t sym= XKB_KEY_NoSymbol;
            int c= fgetc(stdin);
            switch( c )
            {
               case 0x1B:
                  c= fgetc(stdin);
                  if ( c == 0x5B )
                  {
                     c= fgetc(stdin);
                     switch( c )
                     {
                        case 0x41: //UP
                           sym= XKB_KEY_Up;
                           break;
                        case 0x42: //DOWN
                           sym= XKB_KEY_Down;
                           break;
                        case 0x43: //RIGHT
                           sym= XKB_KEY_Right;
                           break;
                        case 0x44: // LEFT
                           sym= XKB_KEY_Left;
                           break;
                     }
                  }
                  break;
               default:
                  sym= c;
                  break;
            }
            processInput(&ctx, sym);
            
            // Prevent keys from building up while held down
            tcflush(STDIN_FILENO,TCIFLUSH);
         }
         else
         {
            if ( ctx.surfaceDX || ctx.surfaceDY || ctx.surfaceDWidth || ctx.surfaceDHeight )
            {
              // Key has been released - reset deltas
              ctx.surfaceDX= ctx.surfaceDY= ctx.surfaceDWidth= ctx.surfaceDHeight= 0;         
            }
         }
      }
   }   

exit:

   printf("westeros_test: exiting...\n");

   if ( !isBackgroundProcess )
   {
      setBlockingMode(NON_BLOCKING_DISABLED);
   }
   
   if ( ctx.compositor )
   {
      wl_compositor_destroy( ctx.compositor );
      ctx.compositor= 0;
   }
   
   if ( ctx.shell )
   {
      wl_simple_shell_destroy( ctx.shell );
      ctx.shell= 0;
   }
   
   termEGL(&ctx);

   if ( ctx.xkbState )
   {
      xkb_state_unref( ctx.xkbState );
      ctx.xkbState= 0;
   }

   if ( ctx.xkbKeymap )
   {
      xkb_keymap_unref( ctx.xkbKeymap );
      ctx.xkbKeymap= 0;
   }

   if ( ctx.xkbCtx )
   {
      xkb_context_unref( ctx.xkbCtx );
      ctx.xkbCtx= 0;
   }

   if ( ctx.pointer )
   {
      wl_pointer_destroy(ctx.pointer);
      ctx.pointer= 0;
   }

   if ( ctx.keyboard )
   {
     wl_keyboard_destroy(ctx.keyboard);
     ctx.keyboard= 0;
   }

   if ( ctx.seat )
   {
      wl_seat_destroy(ctx.seat);
      ctx.seat= 0;
   }

   if ( ctx.output )
   {
      wl_output_destroy(ctx.output);
      ctx.output= 0;
   }

   if ( registry )
   {
      wl_registry_destroy(registry);
      registry= 0;
   }
   
   if ( display )
   {
      wl_display_disconnect(display);
      display= 0;
   }
   
   printf("westeros_test: exit\n");
      
   return nRC;
}

#define RED_SIZE (8)
#define GREEN_SIZE (8)
#define BLUE_SIZE (8)
#define ALPHA_SIZE (8)
#define DEPTH_SIZE (0)

static bool setupEGL( AppCtx *ctx )
{
   bool result= false;
   EGLint major, minor;
   EGLBoolean b;
   EGLint configCount;
   EGLConfig *eglConfigs= 0;
   EGLint attr[32];
   EGLint redSize, greenSize, blueSize, alphaSize, depthSize;
   EGLint ctxAttrib[3];
   int i;

   /*
    * Get default EGL display
    */
   #ifdef EGL_PLATFORM_WAYLAND_EXT
   PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT= (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
   printf("eglGetPlatformDisplayEXT %p\n", eglGetPlatformDisplayEXT);
   if (eglGetPlatformDisplayEXT)
   {
      ctx->eglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_WAYLAND_EXT, (NativeDisplayType)ctx->display, NULL);
      if ( ctx->eglDisplay == EGL_NO_DISPLAY )
      {
         ctx->eglDisplay = eglGetDisplay((NativeDisplayType)ctx->display);
      }
   }
   else
   #endif
   ctx->eglDisplay = eglGetDisplay((NativeDisplayType)ctx->display);
   printf("eglDisplay=%p\n", ctx->eglDisplay );
   if ( ctx->eglDisplay == EGL_NO_DISPLAY )
   {
      printf("error: EGL not available\n" );
      goto exit;
   }
    
   /*
    * Initialize display
    */
   b= eglInitialize( ctx->eglDisplay, &major, &minor );
   if ( !b )
   {
      printf("error: unable to initialize EGL display\n" );
      goto exit;
   }

   printf("eglInitiialize: major: %d minor: %d\n", major, minor );
   printf("EGL vendor: %s\n", eglQueryString(ctx->eglDisplay, EGL_VENDOR));
   printf("EGL version string: %s\n", eglQueryString(ctx->eglDisplay, EGL_VERSION));
   printf("EGL client APIs: %s\n", eglQueryString(ctx->eglDisplay, EGL_CLIENT_APIS));
    
   /*
    * Get number of available configurations
    */
   b= eglGetConfigs( ctx->eglDisplay, NULL, 0, &configCount );
   if ( !b )
   {
      printf("error: unable to get count of EGL configurations: %X\n", eglGetError() );
      goto exit;
   }
   printf("Number of EGL configurations: %d\n", configCount );
    
   eglConfigs= (EGLConfig*)malloc( configCount*sizeof(EGLConfig) );
   if ( !eglConfigs )
   {
      printf("error: unable to alloc memory for EGL configurations\n");
      goto exit;
   }
    
   i= 0;
   attr[i++]= EGL_RED_SIZE;
   attr[i++]= RED_SIZE;
   attr[i++]= EGL_GREEN_SIZE;
   attr[i++]= GREEN_SIZE;
   attr[i++]= EGL_BLUE_SIZE;
   attr[i++]= BLUE_SIZE;
   attr[i++]= EGL_ALPHA_SIZE;
   attr[i++]= ALPHA_SIZE;
   attr[i++]= EGL_DEPTH_SIZE;
   attr[i++]= DEPTH_SIZE;
   attr[i++]= EGL_STENCIL_SIZE;
   attr[i++]= 0;
   attr[i++]= EGL_SURFACE_TYPE;
   attr[i++]= EGL_WINDOW_BIT;
   attr[i++]= EGL_RENDERABLE_TYPE;
   attr[i++]= EGL_OPENGL_ES2_BIT;
   attr[i++]= EGL_NONE;
    
   /*
    * Get a list of configurations that meet or exceed our requirements
    */
   b= eglChooseConfig( ctx->eglDisplay, attr, eglConfigs, configCount, &configCount );
   if ( !b )
   {
      printf("error: eglChooseConfig failed: %X\n", eglGetError() );
      goto exit;
   }
   printf("eglChooseConfig: matching configurations: %d\n", configCount );
    
   /*
    * Choose a suitable configuration
    */
   for( i= 0; i < configCount; ++i )
   {
      eglGetConfigAttrib( ctx->eglDisplay, eglConfigs[i], EGL_RED_SIZE, &redSize );
      eglGetConfigAttrib( ctx->eglDisplay, eglConfigs[i], EGL_GREEN_SIZE, &greenSize );
      eglGetConfigAttrib( ctx->eglDisplay, eglConfigs[i], EGL_BLUE_SIZE, &blueSize );
      eglGetConfigAttrib( ctx->eglDisplay, eglConfigs[i], EGL_ALPHA_SIZE, &alphaSize );
      eglGetConfigAttrib( ctx->eglDisplay, eglConfigs[i], EGL_DEPTH_SIZE, &depthSize );

      printf("config %d: red: %d green: %d blue: %d alpha: %d depth: %d\n",
              i, redSize, greenSize, blueSize, alphaSize, depthSize );
      if ( (redSize == RED_SIZE) &&
           (greenSize == GREEN_SIZE) &&
           (blueSize == BLUE_SIZE) &&
           (alphaSize == ALPHA_SIZE) &&
           (depthSize >= DEPTH_SIZE) )
      {
         printf( "choosing config %d\n", i);
         break;
      }
   }
   if ( i == configCount )
   {
      printf("error: no suitable configuration available\n");
      goto exit;
   }
   ctx->eglConfig= eglConfigs[i];

   ctxAttrib[0]= EGL_CONTEXT_CLIENT_VERSION;
   ctxAttrib[1]= 2; // ES2
   ctxAttrib[2]= EGL_NONE;
    
   /*
    * Create an EGL context
    */
   ctx->eglContext= eglCreateContext( ctx->eglDisplay, ctx->eglConfig, EGL_NO_CONTEXT, ctxAttrib );
   if ( ctx->eglContext == EGL_NO_CONTEXT )
   {
      printf( "eglCreateContext failed: %X\n", eglGetError() );
      goto exit;
   }
   printf("eglCreateContext: eglContext %p\n", ctx->eglContext );

   result= true;
    
exit:

   if ( eglConfigs )
   {
      free( eglConfigs );
      eglConfigs= 0;
   }

   return result;       
}

static void termEGL( AppCtx *ctx )
{
   if ( ctx->display )
   {
      eglMakeCurrent( ctx->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
      
      while ( !ctx->surfaces.empty() )
      {
         destroySurface( ctx, ctx->surfaces.back() );
      }
      
      eglTerminate( ctx->eglDisplay );
      eglReleaseThread();
   }
}

static AppSurface* createWindowSurface( AppCtx *ctx, int width, int height )
{
   AppSurface* surface = NULL;
   
   if (ctx->surfaces.size() == MAX_POPUPS + 1)
   {
      printf("createWindowSurface: max surface count reached\n");
      return NULL;
   }

   surface= (AppSurface*)calloc( 1, sizeof(AppSurface) );
   if ( !surface )
   {
      printf("createWindowSurface: out of memory\n");
      return NULL;
   }

   surface->wlSurface= wl_compositor_create_surface(ctx->compositor);
   if ( !surface->wlSurface )
   {
      printf("createWindowSurface: wl_compositor_create_surface failed\n");
      free(surface);
      return NULL;
   }
   ctx->surfaces.push_back(surface);
   printf("createWindowSurface: wl_compositor_create_surface returned %p\n", surface->wlSurface);
   if ( ctx->shell )
   {
      //to get the surfaceId immediately
      wl_display_roundtrip(ctx->display);   
   }
 
   surface->eglWindow= wl_egl_window_create(surface->wlSurface, width, height);
   if ( !surface->eglWindow )
   {
      printf("createWindowSurface: wl_egl_window_create failed\n");
      destroySurface( ctx, surface );
      return NULL;
   }
   printf("createWindowSurface: wl_egl_window_create returned %p\n", surface->eglWindow);

   surface->eglWidth= width;
   surface->eglHeight= height;

   surface->eglSurface= eglCreateWindowSurface( ctx->eglDisplay,
                                                ctx->eglConfig,
                                                (EGLNativeWindowType)surface->eglWindow,
                                                NULL );
   if ( surface->eglSurface == EGL_NO_SURFACE )
   {
      printf("createWindowSurface: eglCreateWindowSurface failed on attempt 1 of 2: error %X\n", eglGetError() );
      surface->eglSurface= eglCreateWindowSurface( ctx->eglDisplay,
                                                   ctx->eglConfig,
                                                   (EGLNativeWindowType)NULL,
                                                   NULL );
      if ( surface->eglSurface == EGL_NO_SURFACE )
      {
         printf("createWindowSurface: eglCreateWindowSurface failed on attempt 2 of 2: error %X\n", eglGetError() );
         destroySurface( ctx, surface );
         return NULL;
      }
   }
   printf("createWindowSurface: eglCreateWindowSurface returned %p\n", surface->eglSurface);
 
   if ( !eglMakeCurrent( ctx->eglDisplay, surface->eglSurface, surface->eglSurface, ctx->eglContext ) )
   {
      printf("eglMakeCurrent failed: %X\n", eglGetError() );
      destroySurface( ctx, surface );
      return NULL;
   }

   if (!ctx->shell)
   {
      surface->surfaceVisible= true;
      surface->surfaceX= 0;
      surface->surfaceY= 0;
      surface->surfaceWidth= width;
      surface->surfaceHeight= height;
      surface->surfaceOpacity= 1.0;
      surface->surfaceZOrder= 0.5f * (float)ctx->surfaces.size();
   }
   surface->startTime= currentTimeMillis();

   printf("createWindowSurface success surfaceId=%u wlSurface=%p\n", surface->surfaceId, surface->wlSurface);

   return surface;
}

static AppSurface* createPopupSurface( AppCtx *ctx, AppSurface *parentSurface )
{
   AppSurface* surface = NULL;

   printf("Creating popup surface over current surface\n");

   if ( !ctx->shell )
   {
      printf("createPopupSurface: wl_simple_shell interface not available\n");
      return NULL;
   }

   if ( !parentSurface || !parentSurface->surfaceId )
   {
      printf("createPopupSurface: invalid parent surface\n");
      return NULL;
   }

   //get the current parent size
   wl_simple_shell_get_status( ctx->shell, parentSurface->surfaceId );
   wl_display_roundtrip(ctx->display);

   //layout popup (in this test example we allow)
   int parentX= parentSurface->surfaceX;
   int parentY= parentSurface->surfaceY;
   int parentW= parentSurface->surfaceWidth;
   int parentH= parentSurface->surfaceHeight;
   int screenW = ctx->surfaces[0]->surfaceWidth;
   int screenH = ctx->surfaces[0]->surfaceHeight;

   int popupX= 0;
   int popupY= 0;
   int popupW= 128;
   int popupH= 128;

   uint32_t numPopup = ctx->surfaces.size();
   if ( numPopup == 1 )//first popup
   {
      // Typical popup: smaller than parent, centered on parent 
      printf("Creating popup 1 centered on screen\n");
      popupW= parentW * 0.4;
      popupH= parentH * 0.4;
      popupX= parentX + (parentW-popupW)/2;
      popupY= parentY + (parentH-popupH)/2;
   }
   else
   {
      //small popups in the corners
      popupW= screenW/4;
      popupH= screenH/4;
      if ( numPopup == 2 )//top-left
      {
         printf("Creating popup 2 in top-left corner\n");
         popupX= screenW/8;
         popupY= screenH - screenH/8 - popupH;
      }
      else if ( numPopup == 3 )//top-right
      {
         printf("Creating popup 3 in top-right corner\n");
         popupX= screenW - screenW/8 - popupW;
         popupY= screenH - screenH/8 - popupH;
      }
      else if ( numPopup == 4 )//bottom-right
      {
         printf("Creating popup 4 in bottom-right corner\n");
         popupX= screenW - screenW/8 - popupW;
         popupY= screenH/8;
      }
      else if ( numPopup == 5 )//bottom-left
      {
         printf("Creating popup 5 in bottom-left corner\n");
         popupX= screenW/8;
         popupY= screenH/8;
      }
      else
      {
         //shouldn't come here because MAX_POPUPS limit
         printf("Creating yet another popup centered on screen\n");
         popupX= parentX + (parentW-popupW)/2;
         popupY= parentY + (parentH-popupH)/2;
      }
   }
      
   surface = createWindowSurface( ctx, popupW, popupH );

   if ( !surface )
   {
      printf("createPopupSurface: createWindowSurface failed\n");
      return NULL;
   }

   wl_display_roundtrip(ctx->display);//to acquire the surfaceId right now

   if ( !surface->surfaceId )
   {
      printf("createPopupSurface: failed to obtain popup surface id\n");
      destroySurface( ctx, surface );
      return NULL;
   }

   surface->parentSurfaceId= parentSurface->surfaceId;
   surface->surfaceWidth= popupW;
   surface->surfaceHeight= popupH;
   surface->surfaceX= popupX;
   surface->surfaceY= popupY;

   //set a custom color for each popup
   surface->color[0]= 0.25f;
   surface->color[1]= 0.25f;
   surface->color[2]= 0.25f;
   surface->color[3]= 1.0f;
   if ( numPopup == 1 )
   {
      surface->color[0]= 0.75f;
      surface->color[1]= 0.75f;
      surface->color[2]= 0.75f;
   }
   else if ( numPopup == 2 )
   {//red
      surface->color[0]= 1.0f;
   }
   else if ( numPopup == 3 )
   {//green
      surface->color[1]= 1.0f;
   }
   else if ( numPopup == 4 )
   {//blue
      surface->color[2]= 1.0f;
   }
   else if ( numPopup == 5 )
   {//semi-transparent
      surface->color[0]= 0.75f;
      surface->color[1]= 0.75f;
      surface->color[2]= 0.75f;
      surface->color[3]= 0.75f;
   }

   printf("createPopupSurface: create popup surfaceId=%u parentId=%u at (%d,%d,%d,%d)\n",
            surface->surfaceId, surface->parentSurfaceId, popupX, popupY, popupW, popupH);

   wl_simple_shell_get_popup( ctx->shell,
                              surface->surfaceId,
                              parentSurface->surfaceId,
                              popupX, popupY, popupW, popupH );
   wl_display_flush(ctx->display);
   ctx->needRedraw= true;

   if (ctx->surfaces.size() == 3)
   {
      wl_simple_shell_set_zorder(ctx->shell, ctx->surfaces[1]->surfaceId, wl_fixed_from_double(10));
   }
   return surface;
}

static void destroySurface( AppCtx *ctx, AppSurface *surface )
{
   if ( !ctx || !surface )
   {
      printf("destroySurface: invalid argument");
      return;
   }   

   printf("destroySurface surfaceId=%u wlSurface=%p\n", surface->surfaceId, surface->wlSurface);

   ctx->surfaces.erase( std::remove(ctx->surfaces.begin(), ctx->surfaces.end(), surface), ctx->surfaces.end() );

   if ( surface->eglSurface )
   {
      if ( surface->eglSurface == eglGetCurrentSurface(EGL_DRAW) )
      {
         AppSurface* top = getTopSurface( ctx );
         if ( top )
            eglMakeCurrent( ctx->eglDisplay, top->eglSurface, top->eglSurface, ctx->eglContext );
         else
            eglMakeCurrent( ctx->eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
      }

      eglDestroySurface( ctx->eglDisplay, surface->eglSurface );
      surface->eglSurface= EGL_NO_SURFACE;
   }

   if ( surface->eglWindow )
   {
      wl_egl_window_destroy( surface->eglWindow );
      surface->eglWindow= 0;
   }

   if ( surface->shmBuffer )
   {
      wl_buffer_destroy( surface->shmBuffer );
      surface->shmBuffer= 0;
   } 

   if ( surface->wlSurface )
   {
      printf("destroySurface before wl_surface_destroy\n");
      wl_surface_destroy( surface->wlSurface );
      surface->wlSurface= 0;
   }

   printf("destroySurface before wl_display_roundtrip\n");
   wl_display_roundtrip( ctx->display );
   printf("destroySurface after wl_display_roundtrip\n");

   if ( ctx->keyboardFocus == surface )
   {
      printf("destroySurface setting keyboardFocus=NULL\n");
      ctx->keyboardFocus= 0;
   }

   if ( ctx->pointerFocus == surface )
   {
      printf("destroySurface setting pointerFocus=NULL\n");
      ctx->pointerFocus= 0;
   }

   free((void*)surface);
}

static void resizeSurface( AppSurface *surface, int dx, int dy, int width, int height )
{
   if ( !surface->eglWindow )
   {
      printf("resizeSurface: invalid egl window\n");
      return;
   }
   surface->surfaceWidth= width;
   surface->surfaceHeight= height;
   wl_egl_window_resize( surface->eglWindow, width, height, dx, dy );
}

static const char *vert_shader_text =
   "uniform mat4 mvp;\n"
   "attribute vec4 pos;\n"
   "attribute vec4 color;\n"
   "varying vec4 v_color;\n"
   "void main() {\n"
   "  gl_Position = mvp * pos;\n"
   "  v_color = color;\n"
   "}\n";

static const char *frag_shader_text =
   "precision mediump float;\n"
   "varying vec4 v_color;\n"
   "void main() {\n"
   "  gl_FragColor = v_color;\n"
   "}\n";

static GLuint createShader(AppCtx *ctx, GLenum shaderType, const char *shaderSource )
{
   GLuint shader= 0;
   GLint shaderStatus;
   GLsizei length;
   char logText[1000];
   
   shader= glCreateShader( shaderType );
   if ( shader )
   {
      glShaderSource( shader, 1, (const char **)&shaderSource, NULL );
      glCompileShader( shader );
      glGetShaderiv( shader, GL_COMPILE_STATUS, &shaderStatus );
      if ( !shaderStatus )
      {
         glGetShaderInfoLog( shader, sizeof(logText), &length, logText );
         printf("Error compiling %s shader: %*s\n",
                ((shaderType == GL_VERTEX_SHADER) ? "vertex" : "fragment"),
                length,
                logText );
      }
   }

   return shader;
}

static bool setupGL( AppCtx *ctx )
{
   bool result= false;
   GLuint frag, vert;
   GLuint program;
   GLint status;

   frag= createShader(ctx, GL_FRAGMENT_SHADER, frag_shader_text);
   vert= createShader(ctx, GL_VERTEX_SHADER, vert_shader_text);

   program= glCreateProgram();
   glAttachShader(program, frag);
   glAttachShader(program, vert);
   ctx->gl.pos= 0;
   ctx->gl.col= 1;
   glBindAttribLocation(program, ctx->gl.pos, "pos");
   glBindAttribLocation(program, ctx->gl.col, "color");
   glLinkProgram(program);

   glGetProgramiv(program, GL_LINK_STATUS, &status);
   if (!status)
   {
      char log[1000];
      GLsizei len;
      glGetProgramInfoLog(program, 1000, &len, log);
      fprintf(stderr, "Error: linking:\n%*s\n", len, log);
      goto exit;
   }

   ctx->gl.mvp= glGetUniformLocation(program, "mvp");
   glUseProgram(program);

   printf("GLES vendor: %s\n", glGetString(GL_VENDOR));
   printf("GLES renderer: %s\n", glGetString(GL_RENDERER));
   printf("GLES version: %s\n", glGetString(GL_VERSION));
   printf("GLES shading language: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

   result= true;

exit:
   return result;
}

static void drawRect( AppCtx *ctx, GLenum mode, const void* verts, const void* color, GLsizei count )
{
   glVertexAttribPointer(ctx->gl.pos, 2, GL_FLOAT, GL_FALSE, 0, verts);
   glVertexAttribPointer(ctx->gl.col, 4, GL_FLOAT, GL_FALSE, 0, color);
   glEnableVertexAttribArray(ctx->gl.pos);
   glEnableVertexAttribArray(ctx->gl.col);
   glDrawArrays(mode, 0, 4);
   glDisableVertexAttribArray(ctx->gl.pos);
   glDisableVertexAttribArray(ctx->gl.col);
   GLenum err= glGetError();
   if ( err != GL_NO_ERROR )
      printf( "renderGL: glGetError() = %X\n", err );
}

static void renderPopup( AppCtx *ctx, int width, int height, long long startTime, const float* c )
{
   const float BORDER_WIDTH= 5;
   float W = (float)width;
   float H = (float)height;
   float B = 5.0f;

   float ortho[16] = {
      2.0f/W,  0.0f,     0.0f,   0.0f,
      0.0f,    2.0f/H,   0.0f,   0.0f,
      0.0f,    0.0f,    -1.0f,   0.0f,
     -1.0f,   -1.0f,     0.0f,   1.0f
   };

   const GLfloat rectVerts[4][2] = {{0, 0}, {W, 0}, {W, H}, {0, H}};
   const GLfloat borderColor[4][4] = {{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}};
   const GLfloat backgroundColor[4][4] = {{c[0], c[1], c[2], c[3]}, {c[0], c[1], c[2], c[3]}, {c[0], c[1], c[2], c[3]}, {c[0], c[1], c[2], c[3]}};

   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
   glViewport(0, 0, width, height);
   glClearColor(0, 0, 0, 1);
   glClear(GL_COLOR_BUFFER_BIT);
   glUniformMatrix4fv(ctx->gl.mvp, 1, GL_FALSE, (GLfloat*)ortho);
   drawRect( ctx, GL_TRIANGLE_FAN, rectVerts, backgroundColor, 4 );
   glLineWidth(BORDER_WIDTH);
   drawRect( ctx, GL_LINE_LOOP, rectVerts, borderColor, 4 );
   renderGL ( ctx, width, height, startTime, false );//draw the spinning triangle inside
}

static void renderGL( AppCtx *ctx, int width, int height, long long startTime, bool clearScreen )
{
   if ( !ctx->haveMode ) return;

   static const GLfloat verts[3][2] = {
      { -0.5, -0.5 },
      {  0.5, -0.5 },
      {  0,    0.5 }
   };
   static const GLfloat colors[3][4] = {
      { 1, 0, 0, 1.0 },
      { 0, 1, 0, 1.0 },
      { 0, 0, 1, 1.0 }
   };
   GLfloat angle;
   GLfloat rotation[4][4] = {
      { 1, 0, 0, 0 },
      { 0, 1, 0, 0 },
      { 0, 0, 1, 0 },
      { 0, 0, 0, 1 }
   };

   if ( clearScreen )
   {
      glViewport( 0, 0, width, height );
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClear(GL_COLOR_BUFFER_BIT);
   }

   static const uint32_t speed_div = 5;
   angle = ctx->noAnimation ? 0.0 : ((currentTimeMillis()-startTime) / speed_div) % 360 * M_PI / 180.0;
   rotation[0][0] =  cos(angle);
   rotation[0][2] =  sin(angle);
   rotation[2][0] = -sin(angle);
   rotation[2][2] =  cos(angle);

   glUniformMatrix4fv(ctx->gl.mvp, 1, GL_FALSE, (GLfloat *) rotation);

   glVertexAttribPointer(ctx->gl.pos, 2, GL_FLOAT, GL_FALSE, 0, verts);
   glVertexAttribPointer(ctx->gl.col, 4, GL_FLOAT, GL_FALSE, 0, colors);
   glEnableVertexAttribArray(ctx->gl.pos);
   glEnableVertexAttribArray(ctx->gl.col);

   glDrawArrays(GL_TRIANGLES, 0, 3);

   glDisableVertexAttribArray(ctx->gl.pos);
   glDisableVertexAttribArray(ctx->gl.col);
   
   GLenum err= glGetError();
   if ( err != GL_NO_ERROR )
   {
      printf( "renderGL: glGetError() = %X\n", err );
   }
}
