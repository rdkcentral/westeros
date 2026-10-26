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
