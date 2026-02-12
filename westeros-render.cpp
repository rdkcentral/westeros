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
#include <pthread.h>
#include <unistd.h>

#include <dlfcn.h>

#include "westeros-nested.h"
#include "westeros-render.h"

#define DEFAULT_OUTPUT_WIDTH (1280)
#define DEFAULT_OUTPUT_HEIGHT (720)

#define WESTEROS_UNUSED(x) ((void)(x))

#ifdef UNIT_TEST
// Render surface structure for unit testing
struct _WstRenderSurface {
    int x, y, width, height;
    float opacity;
    float zorder;
    bool visible;
    void *nativePixmap;
};

// Complete stub renderer functions for unit testing
static void stub_updateScene( WstRenderer *renderer ) {
   WESTEROS_UNUSED(renderer);
}

static WstRenderSurface* stub_surfaceCreate( WstRenderer *renderer ) {
   WESTEROS_UNUSED(renderer);
   WstRenderSurface *surface = (WstRenderSurface*)calloc(1, sizeof(WstRenderSurface));
   if (surface) {
      surface->opacity = 1.0f;
      surface->zorder = 0.0f;
      surface->visible = true;
      surface->x = 0;
      surface->y = 0;
      surface->width = 1280;
      surface->height = 720;
   }
   return surface;
}

static void stub_surfaceDestroy( WstRenderer *renderer, WstRenderSurface *surface ) {
   WESTEROS_UNUSED(renderer);
   if (surface) {
      free(surface);
   }
}

static void stub_surfaceCommit( WstRenderer *renderer, WstRenderSurface *surface, struct wl_resource *resource ) {
   WESTEROS_UNUSED(renderer);
   WESTEROS_UNUSED(surface);
   WESTEROS_UNUSED(resource);
}

static void stub_surfaceSetVisible( WstRenderer *renderer, WstRenderSurface *surface, bool visible ) {
   WESTEROS_UNUSED(renderer);
   if (surface) {
      surface->visible = visible;
   }
}

static bool stub_surfaceGetVisible( WstRenderer *renderer, WstRenderSurface *surface, bool *visible ) {
   WESTEROS_UNUSED(renderer);
   if (surface) {
      if (visible) {
         *visible = surface->visible;
      }
      return true;
   }
   return false;
}

static void stub_surfaceSetGeometry( WstRenderer *renderer, WstRenderSurface *surface, int x, int y, int width, int height ) {
   WESTEROS_UNUSED(renderer);
   if (surface) {
      surface->x = x;
      surface->y = y;
      surface->width = width;
      surface->height = height;
   }
}

static void stub_surfaceGetGeometry( WstRenderer *renderer, WstRenderSurface *surface, int *x, int *y, int *width, int *height ) {
   WESTEROS_UNUSED(renderer);
   if (surface) {
      if (x) *x = surface->x;
      if (y) *y = surface->y;
      if (width) *width = surface->width;
      if (height) *height = surface->height;
   }
}

static void stub_surfaceSetOpacity( WstRenderer *renderer, WstRenderSurface *surface, float opacity ) {
   WESTEROS_UNUSED(renderer);
   if (surface) {
      surface->opacity = opacity;
   }
}

static float stub_surfaceGetOpacity( WstRenderer *renderer, WstRenderSurface *surface, float *opacity ) {
   WESTEROS_UNUSED(renderer);
   if (surface) {
      if (opacity) {
         *opacity = surface->opacity;
      }
      return surface->opacity;
   }
   return 0.0f;
}

static void stub_surfaceSetZOrder( WstRenderer *renderer, WstRenderSurface *surface, float z ) {
   WESTEROS_UNUSED(renderer);
   if (surface) {
      surface->zorder = z;
   }
}

static float stub_surfaceGetZOrder( WstRenderer *renderer, WstRenderSurface *surface, float *z ) {
   WESTEROS_UNUSED(renderer);
   if (surface) {
      if (z) {
         *z = surface->zorder;
      }
      return surface->zorder;
   }
   return 0.0f;
}

static void stub_renderTerm( WstRenderer *renderer ) {
   WESTEROS_UNUSED(renderer);
}
#endif

WstRenderer* WstRendererCreate( const char *moduleName, int argc, char **argv, struct wl_display *display, WstNestedConnection *nc )
{
   bool error= false;
   void *module= 0, *init;

   WstRenderer *renderer= (WstRenderer*)calloc( 1, sizeof(WstRenderer) );
   if ( renderer )
   {
      int rc;
      int i= 0;
      int len, value;
      int width= DEFAULT_OUTPUT_WIDTH;
      int height= DEFAULT_OUTPUT_HEIGHT;
      void *nativeWindow= 0;
      
      while ( i < argc )
      {
         len= strlen(argv[i]);
         if ( (len == 7) && (strncmp( argv[i], "--width", len) == 0) )
         {
            if ( i+1 < argc )
            {
               ++i;
               value= atoi(argv[i]);
               if ( value > 0 )
               {
                  width= value;
               }
            }
         }
         else
         if ( (len == 8) && (strncmp( argv[i], "--height", len) == 0) )
         {
            if ( i+1 < argc )
            {
               ++i;
               value= atoi(argv[i]);
               if ( value > 0 )
               {
                  height= value;
               }
            }
         }
         else
         if ( (len == 14) && (strncmp( argv[i], "--nativeWindow", len) == 0) )
         {
            if ( i+1 < argc )
            {
               void *value= 0;
               ++i;
               if ( sscanf( argv[i], "%p", &value ) == 1 )
               {
                  nativeWindow= value;
               }
            }
         }
         ++i;
      }
      
      renderer->display= display;
      renderer->nc= nc;
      if ( nc )
      {
         renderer->displayNested= WstNestedConnectionGetDisplay( nc );
         renderer->surfaceNested= WstNestedConnectionGetCompositionSurface( nc );
      }

#ifdef UNIT_TEST
      // In unit test mode, use stub renderer functions instead of loading a module
      renderer->outputWidth= width;
      renderer->outputHeight= height;
      renderer->nativeWindow= nativeWindow;
      renderer->renderer= (void*)1;  // Non-NULL to indicate initialized
      renderer->updateScene= stub_updateScene;
      renderer->surfaceCreate= stub_surfaceCreate;
      renderer->surfaceDestroy= stub_surfaceDestroy;
      renderer->surfaceCommit= stub_surfaceCommit;
      renderer->surfaceSetVisible= stub_surfaceSetVisible;
      renderer->surfaceGetVisible= stub_surfaceGetVisible;
      renderer->surfaceSetGeometry= stub_surfaceSetGeometry;
      renderer->surfaceGetGeometry= stub_surfaceGetGeometry;
      renderer->surfaceSetOpacity= stub_surfaceSetOpacity;
      renderer->surfaceGetOpacity= stub_surfaceGetOpacity;
      renderer->surfaceSetZOrder= stub_surfaceSetZOrder;
      renderer->surfaceGetZOrder= stub_surfaceGetZOrder;
      renderer->renderTerm= stub_renderTerm;
      printf("WstRendererCreate: stub renderer initialized for unit tests\n");
      WESTEROS_UNUSED(moduleName);
#else      
      module= dlopen( moduleName, RTLD_NOW );
      if ( !module )
      {
         printf("WstRendererCreate: failed to load module (%s)\n", moduleName);
         printf("  detail: %s\n", dlerror() );
         error= true;
         goto exit;
      }
      
      init= dlsym( module, RENDERER_MODULE_INIT );
      if ( !init )
      {
         printf("WstRendererCreate: failed to find module (%s) method (%s)\n", moduleName, RENDERER_MODULE_INIT );
         printf("  detail: %s\n", dlerror() );
         error= true;
         goto exit;
      }

      renderer->outputWidth= width;
      renderer->outputHeight= height;
      renderer->nativeWindow= nativeWindow;
      renderer->module= module;
      
      rc= ((WSTMethodRenderInit)init)( renderer, argc, argv );
      if ( rc )
      {
         printf("WstRendererCreate: module (%s) init failed: %d\n", moduleName, rc );
         error= true;
         goto exit;
      }
      
      printf("WstRendererCreate: module (%s) loaded and intialized\n", moduleName );
#endif
   }
   
exit:

   if ( error )
   {
      if ( renderer )
      {
         WstRendererDestroy( renderer );
         renderer= NULL;  // Prevent double-free in caller
      }
      if ( module )
      {
         dlclose( module );
         module= 0;
      }
   }
   
   return renderer;
}

void WstRendererDestroy( WstRenderer *renderer )
{
   if ( renderer )
   {
      if ( renderer->renderer )
      {
         renderer->renderTerm( renderer );
         renderer->renderer= 0;
      }
      if ( renderer->module )
      {
         dlclose( renderer->module );
         renderer->module= 0;
      }
      free( renderer );      
   }
}

void WstRendererUpdateScene( WstRenderer *renderer )
{
   if (renderer && renderer->updateScene)
   {
      renderer->updateScene( renderer );
   }
}

WstRenderSurface* WstRendererSurfaceCreate( WstRenderer *renderer )
{
   if (renderer && renderer->surfaceCreate)
   {
      return renderer->surfaceCreate( renderer );
   }
   return NULL;
}

void WstRendererSurfaceDestroy( WstRenderer *renderer, WstRenderSurface *surface )
{
   if (renderer && renderer->surfaceDestroy)
   {
      renderer->surfaceDestroy( renderer, surface );
   }
}

#ifdef ENABLE_LEXPSYNCPROTOCOL
void WstRendererSurfaceImportSync( WstRenderer *renderer, WstRenderSurface *surface, WstExplicitSync *sync)
{
   if( renderer->surfaceImportSync)
   {
      renderer->surfaceImportSync( renderer, surface, sync );
   }
}
#endif

void WstRendererSurfaceCommit( WstRenderer *renderer, WstRenderSurface *surface, struct wl_resource *resource )
{
   if (renderer && renderer->surfaceCommit)
   {
      renderer->surfaceCommit( renderer, surface, resource );
   }
}

void WstRendererSurfaceSetVisible( WstRenderer *renderer, WstRenderSurface *surface, bool visible )
{
   if (renderer && renderer->surfaceSetVisible)
   {
      renderer->surfaceSetVisible( renderer, surface, visible );
   }
}

bool WstRendererSurfaceGetVisible( WstRenderer *renderer, WstRenderSurface *surface, bool *visible )
{
   if (renderer && renderer->surfaceGetVisible)
   {
      return renderer->surfaceGetVisible( renderer, surface, visible );
   }
   return false;
}

void WstRendererSurfaceSetGeometry( WstRenderer *renderer, WstRenderSurface *surface, int x, int y, int width, int height )
{
   if (renderer && renderer->surfaceSetGeometry)
   {
      renderer->surfaceSetGeometry( renderer, surface, x, y, width, height );
   }
}

void WstRendererSurfaceGetGeometry( WstRenderer *renderer, WstRenderSurface *surface, int *x, int *y, int *width, int *height )
{
   if (renderer && renderer->surfaceGetGeometry)
   {
      renderer->surfaceGetGeometry( renderer, surface, x, y, width, height );
   }
}

void WstRendererSurfaceSetOpacity( WstRenderer *renderer, WstRenderSurface *surface, float opacity )
{
   if (renderer && renderer->surfaceSetOpacity)
   {
      renderer->surfaceSetOpacity( renderer, surface, opacity );
   }
}

float WstRendererSurfaceGetOpacity( WstRenderer *renderer, WstRenderSurface *surface, float *opacity )
{
   if (renderer && renderer->surfaceGetOpacity)
   {
      return renderer->surfaceGetOpacity( renderer, surface, opacity );
   }
   return 0.0f;
}

void WstRendererSurfaceSetZOrder( WstRenderer *renderer, WstRenderSurface *surface, float z )
{
   if (renderer && renderer->surfaceSetZOrder)
   {
      renderer->surfaceSetZOrder( renderer, surface, z );
   }
}

float WstRendererSurfaceGetZOrder( WstRenderer *renderer, WstRenderSurface *surface, float *z )
{
   if (renderer && renderer->surfaceGetZOrder)
   {
      return renderer->surfaceGetZOrder( renderer, surface, z );
   }
   return 0.0f;
}

void WstRendererSurfaceSetCrop( WstRenderer *renderer, WstRenderSurface *surface, float x, float y, float width, float height )
{
   if ( renderer->surfaceSetCrop )
   {
      renderer->surfaceSetCrop( renderer, surface, x, y, width, height );
   }
}

void WstRendererQueryDmabufFormats( WstRenderer *renderer, int **formats, int *num_formats )
{
   if ( renderer->queryDmabufFormats )
   {
      renderer->queryDmabufFormats( renderer, formats, num_formats);
   }
   else
   {
      *num_formats= 0;
   }
}

void WstRendererQueryDmabufModifiers( WstRenderer *renderer, int format, uint64_t **modifiers, int *num_modifiers )
{
   if ( renderer->queryDmabufModifiers )
   {
      renderer->queryDmabufModifiers( renderer, format, modifiers, num_modifiers );
   }
   else
   {
      *num_modifiers= 0;
   }
}

void WstRendererDelegateUpdateScene( WstRenderer *renderer, std::vector<WstRect> &rects )
{
   if ( renderer->delegateUpdateScene )
   {
      renderer->delegateUpdateScene( renderer, rects );
   }
}

void WstRendererResolutionChangeBegin( WstRenderer *renderer )
{
   if ( renderer->resolutionChangeBegin )
   {
      renderer->resolutionChangeBegin( renderer );
   }
}

void WstRendererResolutionChangeEnd( WstRenderer *renderer )
{
   if ( renderer->resolutionChangeEnd )
   {
      renderer->resolutionChangeEnd( renderer );
   }
}

