#include "westeros-context-mock.h"
#include <stdlib.h>
#include <string.h>

/* Mock context functions */
WstContext* mock_create_context(void)
{
    WstContext *ctx = (WstContext*)calloc(1, sizeof(WstContext));
    if (ctx) {
        ctx->display = (struct wl_display*)calloc(1, sizeof(struct wl_display));
        ctx->initialized = 1;
    }
    return ctx;
}

void mock_destroy_context(WstContext *ctx)
{
    if (ctx) {
        if (ctx->display) {
            free(ctx->display);
        }
        free(ctx);
    }
}

/* Mock surface functions */
WstSurface* mock_create_surface(WstContext *ctx)
{
    WstSurface *surface = (WstSurface*)calloc(1, sizeof(WstSurface));
    if (surface) {
        surface->surfaceId = 0;
        surface->destroyed = 0;
        surface->resource = NULL;
        surface->syncRes = NULL;
        
        // Initialize sync structures to safe defaults
        surface->createdBufferSync.acquireFenceFd = -1;
        surface->createdBufferSync.bufferRelease = NULL;
        
        surface->attachedBufferSync.acquireFenceFd = -1;
        surface->attachedBufferSync.bufferRelease = NULL;
        
        surface->detachedBufferSync.acquireFenceFd = -1;
        surface->detachedBufferSync.bufferRelease = NULL;
    }
    return surface;
}

void mock_destroy_surface(WstSurface *surface)
{
    if (surface) {
        // Clean up any open fence file descriptors
        if (surface->createdBufferSync.acquireFenceFd >= 0 && 
            surface->createdBufferSync.acquireFenceFd > 2) {
            close(surface->createdBufferSync.acquireFenceFd);
        }
        if (surface->attachedBufferSync.acquireFenceFd >= 0 && 
            surface->attachedBufferSync.acquireFenceFd > 2) {
            close(surface->attachedBufferSync.acquireFenceFd);
        }
        if (surface->detachedBufferSync.acquireFenceFd >= 0 && 
            surface->detachedBufferSync.acquireFenceFd > 2) {
            close(surface->detachedBufferSync.acquireFenceFd);
        }
        
        surface->destroyed = 1;
        if (surface->resource) {
            // Don't free resource - it's managed by wayland stubs
            surface->resource = NULL;
        }
        if (surface->syncRes) {
            // Don't free syncRes - it's managed by wayland stubs
            surface->syncRes = NULL;
        }
        free(surface);
    }
}

/* Mock renderer functions */
WstRenderer* mock_create_renderer(WstContext *ctx)
{
    WstRenderer *renderer = (WstRenderer*)calloc(1, sizeof(WstRenderer));
    if (renderer) {
        renderer->ctx = ctx;
        renderer->initialized = 1;
    }
    return renderer;
}

void mock_destroy_renderer(WstRenderer *renderer)
{
    if (renderer) {
        free(renderer);
    }
}

/* Helper for creating mock resources */
struct wl_resource* mock_create_resource(struct wl_client *client, uint32_t id)
{
    struct wl_resource *resource = (struct wl_resource*)calloc(1, sizeof(struct wl_resource));
    if (resource) {
        resource->client = client;
        resource->destroy = NULL;
        resource->data = NULL;
        /* Note: wl_resource doesn't have object.id or object.implementation in our mock */
    }
    return resource;
}
