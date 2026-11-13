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
 * Mock header for DRM FOURCC formats
 */

#ifndef _DRM_FOURCC_MOCK_H
#define _DRM_FOURCC_MOCK_H

#include <stdint.h>

// DRM Format codes
#define DRM_FORMAT_XRGB8888  0x34325258
#define DRM_FORMAT_XBGR8888  0x34324258
#define DRM_FORMAT_RGBX8888  0x34325852
#define DRM_FORMAT_BGRX8888  0x34325842
#define DRM_FORMAT_ARGB8888  0x34325241
#define DRM_FORMAT_RGBA8888  0x34324152
#define DRM_FORMAT_ABGR8888  0x34324241
#define DRM_FORMAT_BGRA8888  0x34324142
#define DRM_FORMAT_RGB888    0x34324752

// YUV formats
#define DRM_FORMAT_NV12      0x3231564e
#define DRM_FORMAT_NV16      0x3631564e
#define DRM_FORMAT_NV21      0x3132564e
#define DRM_FORMAT_NV61      0x3136564e
#define DRM_FORMAT_YUV420    0x32315559
#define DRM_FORMAT_YVU420    0x32315659
#define DRM_FORMAT_YUV422    0x36315559
#define DRM_FORMAT_YUV444    0x34325559

#ifndef DRM_FORMAT_MOD_INVALID
#define DRM_FORMAT_MOD_INVALID ((1ULL<<56) - 1)
#endif

#ifndef DRM_FORMAT_MOD_LINEAR
#define DRM_FORMAT_MOD_LINEAR 0
#endif

#endif // _DRM_FOURCC_MOCK_H
