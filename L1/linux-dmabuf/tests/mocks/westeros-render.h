/*
 * Mock header for Westeros Renderer
 * Used for testing westeros-linux-dmabuf
 */

#ifndef _WESTEROS_RENDER_MOCK_H
#define _WESTEROS_RENDER_MOCK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _WstRenderer WstRenderer;

void WstRendererQueryDmabufFormats(WstRenderer *renderer, int **formats, int *numFormats);

void WstRendererQueryDmabufModifiers(WstRenderer *renderer, int format,
                                     uint64_t **modifiers, int *numModifiers);

#ifdef __cplusplus
}
#endif

#endif // _WESTEROS_RENDER_MOCK_H
