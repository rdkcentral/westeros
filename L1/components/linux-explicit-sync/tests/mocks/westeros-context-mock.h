#ifndef WESTEROS_CONTEXT_MOCK_H
#define WESTEROS_CONTEXT_MOCK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "wayland-stubs.h"
#include "westeros-linux-expsync.h"

/* WstExplicitSync types are defined in westeros-linux-expsync.h */

/* Mock WstContext structure */
typedef struct _WstContext {
    struct wl_display *display;
    void *lexpsync;  /* pointer to wl_lexpsync */
    int initialized;
} WstContext;

/* Mock WstSurface structure */
typedef struct _WstSurface {
    struct wl_resource *resource;
    struct wl_resource *syncRes;
    WstExplicitSync createdBufferSync;
    WstExplicitSync attachedBufferSync;
    WstExplicitSync detachedBufferSync;
    int surfaceId;
    int destroyed;
} WstSurface;

/* Mock WstRenderer structure */
typedef struct _WstRenderer {
    WstContext *ctx;
    int initialized;
} WstRenderer;

/* Mock functions for test support */
WstContext* mock_create_context(void);
void mock_destroy_context(WstContext *ctx);

WstSurface* mock_create_surface(WstContext *ctx);
void mock_destroy_surface(WstSurface *surface);

WstRenderer* mock_create_renderer(WstContext *ctx);
void mock_destroy_renderer(WstRenderer *renderer);

/* Helper for creating mock resources */
struct wl_resource* mock_create_resource(struct wl_client *client, uint32_t id);

#ifdef __cplusplus
}
#endif

#endif /* WESTEROS_CONTEXT_MOCK_H */
