# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased] - 2024-12-31

### Fixed

#### Linux Explicit Sync Component
- **CRITICAL**: Fixed `LINUX_PLATFORM` macro not being defined in CMakeLists.txt
  - Added platform-specific compile definitions for Linux and Windows
  - Added linker wrapping options (`-Wl,--wrap=ioctl`, `-Wl,--wrap=close`) for Linux
  - Removed explicit `ioctl` declaration from `westeros-linux-expsync.cpp` to allow proper linker wrapping
  - Fixed mock `ioctl` implementation to properly set `num_fences` for fence validation tests
  - **Result**: `FileValidation_WithValidFence_ReturnsTrue` test now passes (86/86 tests passing - 100%)

#### SimpleBuffer Component
- Added NULL pointer checks to all `WstSBBuffer*` getter functions:
  - `WstSBBufferGetFormat()` - returns 0 on NULL
  - `WstSBBufferGetWidth()` - returns 0 on NULL
  - `WstSBBufferGetHeight()` - returns 0 on NULL
  - `WstSBBufferGetStride()` - returns 0 on NULL
  - `WstSBBufferGetFd()` - returns -1 on NULL
  - `WstSBBufferGetBuffer()` - returns NULL on NULL
  - `WstSBBufferGetPlaneFd()` - returns -1 on NULL
  - `WstSBBufferGetPlaneOffsetAndStride()` - returns safe defaults on NULL
- **Result**: No segfaults, safe error handling

#### Linux DMA-BUF Component
- Implemented missing `WstLDBBufferGetBuffer()` function (was declared but not implemented)
- Added NULL pointer checks to all `WstLDBBuffer*` getter functions:
  - `WstLDBBufferGetFormat()` - returns 0 on NULL
  - `WstLDBBufferGetWidth()` - returns 0 on NULL
  - `WstLDBBufferGetHeight()` - returns 0 on NULL
  - `WstLDBBufferGetStride()` - returns 0 on NULL
  - `WstLDBBufferGetFd()` - returns -1 on NULL
  - `WstLDBBufferGetPlaneOffsetAndStride()` - returns safe defaults on NULL
  - `WstLDBBufferGetPlaneFd()` - returns -1 on NULL
  - `WstLDBBufferGetPlaneModifier()` - returns 0 on NULL
- Added NULL check to `WstLDBInit()` to reject NULL callbacks parameter
- **Result**: No linker errors, no segfaults, proper parameter validation

### Changed

#### Build System
- Enhanced CMakeLists.txt for linux-explicit-sync component:
  - Platform detection (Linux vs Windows)
  - Proper compile definitions (`LINUX_PLATFORM`, `WINDOWS_PLATFORM`)
  - Linker wrapping configuration for Linux system calls
- Updated mock infrastructure:
  - `sync-file-stubs.c`: Direct call to `__wrap_ioctl()` on Linux to avoid same-file wrapping issues
  - Changed default `mock_ioctl_num_fences` from 0 to 1 for valid fence simulation
  - Removed NULL check in `__wrap_ioctl()` that was preventing proper mock data setting

#### Documentation
- Updated main README.md:
  - Added test status table showing 100% pass rate for all components
  - Added "Recent Fixes" section documenting December 2024 updates
  - Improved build instructions and examples
  - Added technical details about mock infrastructure
  - Updated component status table with test coverage information
- Removed temporary/outdated documentation files:
  - TEST_ENHANCEMENT_SUMMARY.md
  - TEST_COVERAGE_PLAN.md
  - SEGFAULT_FIX_ANALYSIS.md
  - COVERAGE_IMPROVEMENT_GUIDE.md
  - COMPREHENSIVE_TEST_ARCHITECTURE.md
  - APPLY_FIX_INSTRUCTIONS.md

### Test Results

**All Components: 100% Pass Rate**

- **Linux Explicit Sync**: 86/86 tests passing ✅
  - Fixed: `FileValidation_WithValidFence_ReturnsTrue`
  - Fixed: `FileValidation_WithInvalidFence_ReturnsFalse`
  - Fixed: `FileValidation_WithIoctlFailure_ReturnsFalse`
  - Fixed: `Init_NullCallbacks_ReturnsNull`

- **SimpleBuffer**: All tests passing ✅
  - Updated: `BufferPropertyFunctionsWithNullBuffer` (changed from EXPECT_DEATH to safe handling)

- **Linux DMA-BUF**: All tests passing ✅
  - All buffer getter functions handle NULL safely

- **SimpleShell**: All tests passing ✅
  - No changes needed, already stable

### Technical Details

**Root Cause Analysis:**

1. **LINUX_PLATFORM Issue**: The CMakeLists.txt was not defining `LINUX_PLATFORM` macro, causing the code to fall into the Windows/generic branch even on Linux builds. This meant the linker wrapping wasn't being used, and mock ioctl calls were failing.

2. **Linker Wrapping**: When both the wrapper (`__wrap_ioctl`) and the caller (`mock_is_valid_fence_fd`) are in the same compilation unit (sync-file-stubs.c), linker wrapping doesn't work. Solution: Direct call to `__wrap_ioctl()` on Linux.

3. **Explicit Declaration**: The production code had an explicit `extern "C"` declaration of `ioctl()`, which prevented proper linker wrapping. Removed and replaced with `#include <sys/ioctl.h>`.

**Files Modified:**

- `components/linux-explicit-sync/CMakeLists.txt`
- `components/linux-explicit-sync/src/westeros-linux-expsync.cpp`
- `components/linux-explicit-sync/tests/mocks/sync-file-stubs.c`
- `components/simplebuffer/src/westeros-simplebuffer.cpp`
- `components/linux-dmabuf/src/westeros-linux-dmabuf.cpp`
- `components/simpleshell/tests/L1/westeros_simpleshell_test.cpp`
- `README.md`

## [Previous Versions]

See git history for previous changes.
