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
#ifndef L1_SYNC_FILE_STUBS_H
#define L1_SYNC_FILE_STUBS_H

#include <stdint.h>

#ifndef SYNC_IOC_FILE_INFO
#define SYNC_IOC_FILE_INFO 0xc0303e04
#endif

struct sync_file_info {
    char name[32];
    int32_t status;
    uint32_t flags;
    uint32_t num_fences;
    uint32_t pad;
    uint64_t sync_fence_info;
};

#ifdef __cplusplus
extern "C" {
#endif
int ioctl(int fd, unsigned long request, ...);
#ifdef __cplusplus
}
#endif

#endif /* L1_SYNC_FILE_STUBS_H */
