/*
 * Mock Implementations for Westeros Renderer Functions
 */

#include "westeros-render.h"
#include <stdlib.h>
#include "drm_fourcc.h"

struct _WstRenderer {
    int *formats;
    int numFormats;
    uint64_t **modifiers;
    int *numModifiers;
};

void WstRendererQueryDmabufFormats(WstRenderer *renderer, int **formats, int *numFormats)
{
    if (renderer && formats && numFormats) {
        *formats = renderer->formats;
        *numFormats = renderer->numFormats;
    } else if (formats && numFormats) {
        // Default formats if no renderer
        static int defaultFormats[] = {
            DRM_FORMAT_XRGB8888,
            DRM_FORMAT_ARGB8888,
            DRM_FORMAT_RGB888,
            0
        };
        *formats = defaultFormats;
        *numFormats = 0;
    }
}

void WstRendererQueryDmabufModifiers(WstRenderer *renderer, int format,
                                     uint64_t **modifiers, int *numModifiers)
{
    if (renderer && modifiers && numModifiers) {
        // Return modifiers for the format
        *modifiers = NULL;
        *numModifiers = 0;
    } else if (modifiers && numModifiers) {
        *modifiers = NULL;
        *numModifiers = 0;
    }
}
