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
 */
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <new>

#include <dlfcn.h>

#include "westeros-nested.h"
#include "westeros-render.h"

#define DEFAULT_OUTPUT_WIDTH (1280)
#define DEFAULT_OUTPUT_HEIGHT (720)

#define WESTEROS_UNUSED(x) ((void)(x))

#ifdef UNIT_TEST
namespace {
struct WstTestRenderSurface {
   bool visible;
   int x;
   int y;
   int width;
   int height;
   float opacity;
   float zorder;

   WstTestRenderSurface(): visible(false), x(0), y(0), width(0), height(0), opacity(1.0f), zorder(0.0f) {}
};

void wstTestRenderUpdateScene(WstRenderer *) {}

WstRenderSurface* wstTestRenderSurfaceCreate(WstRenderer *)
{
   return reinterpret_cast<WstRenderSurface*>(new (std::nothrow) WstTestRenderSurface());
}

void wstTestRenderSurfaceDestroy(WstRenderer *, WstRenderSurface *surface)
{
   delete reinterpret_cast<WstTestRenderSurface*>(surface);
}

void wstTestRenderSurfaceCommit(WstRenderer *, WstRenderSurface *, struct wl_resource *) {}

void wstTestRenderSurfaceSetVisible(WstRenderer *, WstRenderSurface *surface, bool visible)
{
   if ( surface ) reinterpret_cast<WstTestRenderSurface*>(surface)->visible= visible;
}

bool wstTestRenderSurfaceGetVisible(WstRenderer *, WstRenderSurface *surface, bool *visible)
{
   if ( !surface ) return false;
   if ( visible ) *visible= reinterpret_cast<WstTestRenderSurface*>(surface)->visible;
   return true;
}

void wstTestRenderSurfaceSetGeometry(WstRenderer *, WstRenderSurface *surface, int x, int y, int width, int height)
{
   if ( surface )
   {
      WstTestRenderSurface *testSurface= reinterpret_cast<WstTestRenderSurface*>(surface);
      testSurface->x= x;
      testSurface->y= y;
      testSurface->width= width;
      testSurface->height= height;
   }
}

void wstTestRenderSurfaceGetGeometry(WstRenderer *, WstRenderSurface *surface, int *x, int *y, int *width, int *height)
{
   if ( !surface ) return;
   WstTestRenderSurface *testSurface= reinterpret_cast<WstTestRenderSurface*>(surface);
   if ( x ) *x= testSurface->x;
   if ( y ) *y= testSurface->y;
   if ( width ) *width= testSurface->width;
   if ( height ) *height= testSurface->height;
}

void wstTestRenderSurfaceSetOpacity(WstRenderer *, WstRenderSurface *surface, float opacity)
{
   if ( surface ) reinterpret_cast<WstTestRenderSurface*>(surface)->opacity= opacity;
}

float wstTestRenderSurfaceGetOpacity(WstRenderer *, WstRenderSurface *surface, float *opacity)
{
   if ( !surface ) return 0.0f;
   float value= reinterpret_cast<WstTestRenderSurface*>(surface)->opacity;
   if ( opacity ) *opacity= value;
   return value;
}

void wstTestRenderSurfaceSetZOrder(WstRenderer *, WstRenderSurface *surface, float zorder)
{
   if ( surface ) reinterpret_cast<WstTestRenderSurface*>(surface)->zorder= zorder;
}

float wstTestRenderSurfaceGetZOrder(WstRenderer *, WstRenderSurface *surface, float *zorder)
{
   if ( !surface ) return 0.0f;
   float value= reinterpret_cast<WstTestRenderSurface*>(surface)->zorder;
   if ( zorder ) *zorder= value;
   return value;
}

void wstTestRenderInit(WstRenderer *renderer)
{
   renderer->updateScene= wstTestRenderUpdateScene;
   renderer->surfaceCreate= wstTestRenderSurfaceCreate;
   renderer->surfaceDestroy= wstTestRenderSurfaceDestroy;
   renderer->surfaceCommit= wstTestRenderSurfaceCommit;
   renderer->surfaceSetVisible= wstTestRenderSurfaceSetVisible;
   renderer->surfaceGetVisible= wstTestRenderSurfaceGetVisible;
   renderer->surfaceSetGeometry= wstTestRenderSurfaceSetGeometry;
   renderer->surfaceGetGeometry= wstTestRenderSurfaceGetGeometry;
   renderer->surfaceSetOpacity= wstTestRenderSurfaceSetOpacity;
   renderer->surfaceGetOpacity= wstTestRenderSurfaceGetOpacity;
   renderer->surfaceSetZOrder= wstTestRenderSurfaceSetZOrder;
   renderer->surfaceGetZOrder= wstTestRenderSurfaceGetZOrder;
}
}
#endif


WstRenderer* WstRendererCreate( const char *moduleName, int argc, char **argv, struct wl_display *display, WstNestedConnection *nc )
{
   bool error= false;
   void *module= 0, *init;

   WstRenderer *renderer= new (std::nothrow) WstRenderer{};
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
      renderer->outputWidth= width;
      renderer->outputHeight= height;
      renderer->nativeWindow= nativeWindow;
      if ( nc )
      {
         renderer->displayNested= WstNestedConnectionGetDisplay( nc );
         renderer->surfaceNested= WstNestedConnectionGetCompositionSurface( nc );
      }
            
   #ifdef UNIT_TEST
      if ( moduleName && strcmp(moduleName, "mock") == 0 )
      {
         wstTestRenderInit(renderer);
      }
      else
   #endif
      {
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

      rc= ((WSTMethodRenderInit)init)( renderer, argc, argv );
      if ( rc )
      {
         printf("WstRendererCreate: module (%s) init failed: %d\n", moduleName, rc );
         error= true;
         goto exit;
      }
      
      printf("WstRendererCreate: module (%s) loaded and intialized\n", moduleName );
      }
   }
   
exit:

   if ( error )
   {
      if ( renderer )
      {
         WstRendererDestroy( renderer );
      }
      if ( module )
      {
         dlclose( module );
      }
      renderer= 0;
   }
   
   return renderer;
}

void WstRendererDestroy( WstRenderer *renderer )
{
   if ( renderer )
   {
   if ( renderer->renderer && renderer->renderTerm )
      {
         renderer->renderTerm( renderer );
         renderer->renderer= 0;
      }
      delete renderer;
   }
}

void WstRendererUpdateScene( WstRenderer *renderer )
{
   if ( renderer && renderer->updateScene ) renderer->updateScene( renderer );
}

WstRenderSurface* WstRendererSurfaceCreate( WstRenderer *renderer )
{
   return renderer && renderer->surfaceCreate ? renderer->surfaceCreate( renderer ) : 0;
}

void WstRendererSurfaceDestroy( WstRenderer *renderer, WstRenderSurface *surface )
{
   if ( renderer && renderer->surfaceDestroy ) renderer->surfaceDestroy( renderer, surface );
}

#ifdef ENABLE_LEXPSYNCPROTOCOL
void WstRendererSurfaceImportSync( WstRenderer *renderer, WstRenderSurface *surface, WstExplicitSync *sync)
{
   if( renderer && renderer->surfaceImportSync)
   {
      renderer->surfaceImportSync( renderer, surface, sync );
   }
}
#endif

void WstRendererSurfaceCommit( WstRenderer *renderer, WstRenderSurface *surface, struct wl_resource *resource )
{
   if ( renderer && renderer->surfaceCommit ) renderer->surfaceCommit( renderer, surface, resource );
}

void WstRendererSurfaceSetVisible( WstRenderer *renderer, WstRenderSurface *surface, bool visible )
{
   if ( renderer && renderer->surfaceSetVisible ) renderer->surfaceSetVisible( renderer, surface, visible );
}

bool WstRendererSurfaceGetVisible( WstRenderer *renderer, WstRenderSurface *surface, bool *visible )
{
   return renderer && renderer->surfaceGetVisible ? renderer->surfaceGetVisible( renderer, surface, visible ) : false;
}

void WstRendererSurfaceSetGeometry( WstRenderer *renderer, WstRenderSurface *surface, int x, int y, int width, int height )
{
   if ( renderer && renderer->surfaceSetGeometry ) renderer->surfaceSetGeometry( renderer, surface, x, y, width, height );
}

void WstRendererSurfaceGetGeometry( WstRenderer *renderer, WstRenderSurface *surface, int *x, int *y, int *width, int *height )
{
   if ( renderer && renderer->surfaceGetGeometry ) renderer->surfaceGetGeometry( renderer, surface, x, y, width, height );
}

void WstRendererSurfaceSetOpacity( WstRenderer *renderer, WstRenderSurface *surface, float opacity )
{
   if ( renderer && renderer->surfaceSetOpacity ) renderer->surfaceSetOpacity( renderer, surface, opacity );
}

float WstRendererSurfaceGetOpacity( WstRenderer *renderer, WstRenderSurface *surface, float *opacity )
{
   return renderer && renderer->surfaceGetOpacity ? renderer->surfaceGetOpacity( renderer, surface, opacity ) : 0.0f;
}

void WstRendererSurfaceSetZOrder( WstRenderer *renderer, WstRenderSurface *surface, float z )
{
   if ( renderer && renderer->surfaceSetZOrder ) renderer->surfaceSetZOrder( renderer, surface, z );
}

float WstRendererSurfaceGetZOrder( WstRenderer *renderer, WstRenderSurface *surface, float *z )
{
   return renderer && renderer->surfaceGetZOrder ? renderer->surfaceGetZOrder( renderer, surface, z ) : 0.0f;
}

void WstRendererSurfaceSetCrop( WstRenderer *renderer, WstRenderSurface *surface, float x, float y, float width, float height )
{
   if ( renderer && renderer->surfaceSetCrop )
   {
      renderer->surfaceSetCrop( renderer, surface, x, y, width, height );
   }
}

void WstRendererQueryDmabufFormats( WstRenderer *renderer, int **formats, int *num_formats )
{
   if ( renderer && renderer->queryDmabufFormats )
   {
      renderer->queryDmabufFormats( renderer, formats, num_formats);
   }
   else
   {
      if ( num_formats ) *num_formats= 0;
   }
}

void WstRendererQueryDmabufModifiers( WstRenderer *renderer, int format, uint64_t **modifiers, int *num_modifiers )
{
   if ( renderer && renderer->queryDmabufModifiers )
   {
      renderer->queryDmabufModifiers( renderer, format, modifiers, num_modifiers );
   }
   else
   {
      if ( num_modifiers ) *num_modifiers= 0;
   }
}

void WstRendererDelegateUpdateScene( WstRenderer *renderer, std::vector<WstRect> &rects )
{
   if ( renderer && renderer->delegateUpdateScene )
   {
      renderer->delegateUpdateScene( renderer, rects );
   }
}

void WstRendererResolutionChangeBegin( WstRenderer *renderer )
{
   if ( renderer && renderer->resolutionChangeBegin )
   {
      renderer->resolutionChangeBegin( renderer );
   }
}

void WstRendererResolutionChangeEnd( WstRenderer *renderer )
{
   if ( renderer && renderer->resolutionChangeEnd )
   {
      renderer->resolutionChangeEnd( renderer );
   }
}

