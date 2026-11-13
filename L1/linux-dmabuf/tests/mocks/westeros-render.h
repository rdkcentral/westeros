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
