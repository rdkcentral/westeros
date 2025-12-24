#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#endif
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>

/* Linux sync_file mock implementation */

/* Define SYNC_IOC_FILE_INFO if not already defined */
#ifndef SYNC_IOC_FILE_INFO
#define SYNC_IOC_FILE_INFO 0xc0303e04
#endif

/* Mock sync_file_info structure */
struct sync_file_info {
    char name[32];
    int32_t status;
    uint32_t flags;
    uint32_t num_fences;
    uint32_t pad;
    uint64_t sync_fence_info;
};

struct sync_fence_info {
    char obj_name[32];
    char driver_name[32];
    int32_t status;
    uint32_t flags;
    uint64_t timestamp_ns;
};

/* IOCTL mock state */
static int mock_ioctl_should_fail = 0;
static int mock_ioctl_num_fences = 1;  /* Default to 1 for valid fence */

void mock_set_ioctl_failure(int should_fail) {
    mock_ioctl_should_fail = should_fail;
}

void mock_set_num_fences(int num_fences) {
    mock_ioctl_num_fences = num_fences;
}

/* Mock ioctl implementation */
#ifdef LINUX_PLATFORM
/* Mock ioctl - wrapped at link time with -Wl,--wrap=ioctl */
int __wrap_ioctl(int fd, unsigned long request, ...)
{
    // Check if we should fail
    if (mock_ioctl_should_fail) {
        errno = EINVAL;
        return -1;
    }
    
    // Mock successful ioctl for SYNC_IOC_FILE_INFO
    if (request == SYNC_IOC_FILE_INFO || request == 0x40083e00) {
        // Get the sync_file_info pointer from varargs
        va_list args;
        va_start(args, request);
        struct sync_file_info *info = va_arg(args, struct sync_file_info*);
        va_end(args);
        
        // Fill in mock data
        snprintf(info->name, sizeof(info->name), "mock_fence_%d", fd);
        info->status = 1; // signaled
        info->flags = 0;
        info->num_fences = mock_ioctl_num_fences;
        info->pad = 0;
        info->sync_fence_info = 0;
        return 0;
    }
    
    // For other ioctl requests, just return success
    return 0;
}
#elif defined(WINDOWS_PLATFORM) || (defined(_WIN32) && !defined(LINUX_PLATFORM))
/* Direct mock ioctl for Windows and other platforms */
int ioctl(int fd, unsigned long request, ...)
{
    // Check if we should fail
    if (mock_ioctl_should_fail) {
        errno = EINVAL;
        return -1;
    }
    
    // Mock successful ioctl for SYNC_IOC_FILE_INFO
    if (request == SYNC_IOC_FILE_INFO || request == 0x40083e00) {
        // Get the sync_file_info pointer from varargs
        va_list args;
        va_start(args, request);
        struct sync_file_info *info = va_arg(args, struct sync_file_info*);
        va_end(args);
        
        if (info) {
            // Fill in mock data
            snprintf(info->name, sizeof(info->name), "mock_fence_%d", fd);
            info->status = 1; // signaled
            info->flags = 0;
            info->num_fences = mock_ioctl_num_fences;
            info->pad = 0;
            info->sync_fence_info = 0;
        }
        return 0;
    }
    
    // For other ioctl requests, just return success
    return 0;
}
#endif

/* Mock close implementation */
#ifdef LINUX_PLATFORM
/* Mock close - wrapped at link time with -Wl,--wrap=close */
int __wrap_close(int fd)
{
    // Mock: always succeed
    // In real implementation, this would close the file descriptor
    return 0;
}
#elif defined(WINDOWS_PLATFORM) || (defined(_WIN32) && !defined(LINUX_PLATFORM))
/* Direct mock close for Windows and other platforms */
int close(int fd)
{
    // Mock: always succeed
    // In real implementation, this would close the file descriptor
    return 0;
}
#endif

/* Utility function to create a mock fence fd for testing */
int mock_create_fence_fd(void)
{
    // In mock, we just return a fake fd number
    // Real implementation would create actual fence fd
    static int mock_fd_counter = 100;
    return mock_fd_counter++;
}

/* Utility function to check if fd is valid in mock context */
int mock_is_valid_fence_fd(int fd)
{
    struct sync_file_info fInfo;
    int rc;
    
    /* Initialize to zero */
    memset(&fInfo, 0, sizeof(fInfo));
    
    /* On Linux, call __wrap_ioctl directly since we're in the same compilation unit
     * and linker wrapping only works across different object files */
#ifdef LINUX_PLATFORM
    rc = __wrap_ioctl(fd, SYNC_IOC_FILE_INFO, &fInfo);
#else
    rc = ioctl(fd, SYNC_IOC_FILE_INFO, &fInfo);
#endif
    if (rc < 0)
    {
        return 0;  // false
    }
    
    return (fInfo.num_fences > 0 ? 1 : 0);  // true if num_fences > 0
}
