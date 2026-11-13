/**
* If not stated otherwise in this file or this component's LICENSE
* file the following copyright and licenses apply:
*
* Copyright 2024 RDK Management
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
**/
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
