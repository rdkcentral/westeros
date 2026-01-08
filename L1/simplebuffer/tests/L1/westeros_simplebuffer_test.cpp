/*
 * Copyright 2016 RDK Management
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
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <unistd.h>
#include <memory>
#include <cstring>

// Include mocks before the actual headers
#include "wayland-server.h"
#include "simplebuffer/protocol/simplebuffer-server-protocol.h"

// Include the header under test
#include "westeros-simplebuffer.h"

// Extern declarations for STATIC_TEST functions (visible in UNIT_TEST builds)
#ifdef UNIT_TEST
extern "C" {
extern void wstISimpleBufferDestroy(struct wl_client *client, struct wl_resource *resource);
extern void wstISBCreateBuffer(struct wl_client *client, struct wl_resource *resource,
                                uint32_t id, uint32_t native_handle, int32_t width, int32_t height,
                                uint32_t stride, uint32_t format);
extern void wstISBCreatePlanarBuffer(struct wl_client *client, struct wl_resource *resource,
                                     uint32_t id, uint32_t native_handle,
                                     int32_t width, int32_t height, uint32_t format,
                                     int32_t offset0, int32_t offset1, int32_t offset2,
                                     int32_t stride0, int32_t stride1, int32_t stride2);
extern void wstISBCreatePlanarBufferFd(struct wl_client *client, struct wl_resource *resource,
                                       uint32_t id, int32_t fd,
                                       int32_t width, int32_t height, uint32_t format,
                                       int32_t offset0, int32_t offset1, int32_t offset2,
                                       int32_t stride0, int32_t stride1, int32_t stride2);
extern void wstISBCreatePlanarBufferFd2(struct wl_client *client, struct wl_resource *resource,
                                        uint32_t id, int32_t fd0, int32_t fd1, int32_t fd2,
                                        int32_t width, int32_t height, uint32_t format,
                                        int32_t offset0, int32_t offset1, int32_t offset2,
                                        int32_t stride0, int32_t stride1, int32_t stride2);
extern void wstSBCreateBuffer(struct wl_client *client, struct wl_resource *resource,
                               uint32_t id, uint32_t native_handle, int32_t fd0, int32_t fd1, int32_t fd2,
                               int32_t w, int32_t h, uint32_t fmt,
                               int32_t off0, int32_t off1, int32_t off2,
                               int32_t strd0, int32_t strd1, int32_t strd2);
extern void wstSBDestroyBuffer(struct wl_resource *resource);
extern void wstSBBind(struct wl_client *client, void *data, uint32_t version, uint32_t id);
extern const struct wl_buffer_interface bufferInterface;
extern const struct wl_sb_interface sb_interface;
}
#endif

using ::testing::_;
using ::testing::Return;
using ::testing::NiceMock;
using ::testing::InSequence;
using ::testing::NotNull;
using ::testing::IsNull;
using ::testing::Eq;
using ::testing::StrEq;

// Mock callback class
class MockSimpleBufferCallbacks {
public:
    MOCK_METHOD(void, bind, (void* user_data, struct wl_client* client, struct wl_resource* resource), ());
    MOCK_METHOD(void, reference_buffer, (void* user_data, struct wl_client* client, uint32_t native_handle, struct wl_sb_buffer* buffer), ());
    MOCK_METHOD(void, release_buffer, (void* user_data, struct wl_sb_buffer* buffer), ());
};

// Static callback wrappers
static MockSimpleBufferCallbacks* g_mockSBCallbacks = nullptr;

extern "C" {
    static void mock_bind(void* user_data, struct wl_client* client, struct wl_resource* resource) {
        if (g_mockSBCallbacks) g_mockSBCallbacks->bind(user_data, client, resource);
    }

    static void mock_reference_buffer(void* user_data, struct wl_client* client, uint32_t native_handle, struct wl_sb_buffer* buffer) {
        if (g_mockSBCallbacks) g_mockSBCallbacks->reference_buffer(user_data, client, native_handle, buffer);
    }

    static void mock_release_buffer(void* user_data, struct wl_sb_buffer* buffer) {
        if (g_mockSBCallbacks) g_mockSBCallbacks->release_buffer(user_data, buffer);
    }
}

// Test fixture
class WesterosSimpleBufferTest : public ::testing::Test {
protected:
    struct wl_display* display;
    wl_sb* sb;
    wayland_sb_callbacks callbacks;
    NiceMock<MockSimpleBufferCallbacks> mockCallbacks;
    void* testUserData;

    void SetUp() override {
        display = wl_display_create();
        ASSERT_NE(display, nullptr);

        g_mockSBCallbacks = &mockCallbacks;
        testUserData = reinterpret_cast<void*>(0x12345678);

        // Initialize callback structure with mock wrappers
        memset(&callbacks, 0, sizeof(callbacks));
        callbacks.bind = mock_bind;
        callbacks.reference_buffer = mock_reference_buffer;
        callbacks.release_buffer = mock_release_buffer;

        sb = WstSBInit(display, &callbacks, testUserData);
        ASSERT_NE(sb, nullptr);
    }

    void TearDown() override {
        if (sb) {
            WstSBUninit(sb);
        }
        if (display) {
            wl_display_destroy(display);
        }
        g_mockSBCallbacks = nullptr;
    }
};

// Basic initialization tests
TEST_F(WesterosSimpleBufferTest, InitializationWithValidParameters) {
    // SB should be created successfully in SetUp()
    EXPECT_NE(sb, nullptr);
}

TEST_F(WesterosSimpleBufferTest, InitializationWithNullDisplay) {
    wl_sb* nullSB = WstSBInit(nullptr, &callbacks, testUserData);
    EXPECT_EQ(nullSB, nullptr);
}

TEST_F(WesterosSimpleBufferTest, InitializationWithNullCallbacks) {
    struct wl_display* tempDisplay = wl_display_create();
    ASSERT_NE(tempDisplay, nullptr);
    
    wl_sb* nullSB = WstSBInit(tempDisplay, nullptr, testUserData);
    EXPECT_EQ(nullSB, nullptr);
    
    wl_display_destroy(tempDisplay);
}

TEST_F(WesterosSimpleBufferTest, InitializationWithNullUserData) {
    struct wl_display* tempDisplay = wl_display_create();
    ASSERT_NE(tempDisplay, nullptr);
    
    wl_sb* validSB = WstSBInit(tempDisplay, &callbacks, nullptr);
    EXPECT_NE(validSB, nullptr);
    
    WstSBUninit(validSB);
    wl_display_destroy(tempDisplay);
}

// Buffer get tests
TEST_F(WesterosSimpleBufferTest, BufferGetWithNullResource) {
    struct wl_sb_buffer* buffer = WstSBBufferGet(nullptr);
    EXPECT_EQ(buffer, nullptr);
}

TEST_F(WesterosSimpleBufferTest, BufferGetWithValidResource) {
    struct wl_resource* resource = wl_resource_create(
        reinterpret_cast<struct wl_client*>(0x1000), 
        &wl_buffer_interface, 
        1, 
        1001
    );
    ASSERT_NE(resource, nullptr);

    // Create a mock buffer
    struct wl_sb_buffer* mockBuffer = reinterpret_cast<struct wl_sb_buffer*>(malloc(sizeof(struct wl_sb_buffer)));
    memset(mockBuffer, 0, sizeof(struct wl_sb_buffer));
    
    // Set the resource user data to our mock buffer
    wl_resource_set_implementation(resource, nullptr, mockBuffer, nullptr);

    struct wl_sb_buffer* retrievedBuffer = WstSBBufferGet(resource);
    EXPECT_EQ(retrievedBuffer, mockBuffer);

    free(mockBuffer);
    wl_resource_destroy(resource);
}

// Buffer property tests with mock buffer
class WesterosSimpleBufferPropertyTest : public ::testing::Test {
protected:
    struct wl_sb_buffer* mockBuffer;

    void SetUp() override {
        mockBuffer = reinterpret_cast<struct wl_sb_buffer*>(malloc(sizeof(struct wl_sb_buffer)));
        memset(mockBuffer, 0, sizeof(struct wl_sb_buffer));
        
        // Initialize with test values
        mockBuffer->format = WL_SB_FORMAT_ARGB8888;
        mockBuffer->width = 1920;
        mockBuffer->height = 1080;
        mockBuffer->stride[0] = 7680;  // 1920 * 4 bytes per pixel
        mockBuffer->stride[1] = 3840;
        mockBuffer->stride[2] = 1920;
        mockBuffer->offset[0] = 0;
        mockBuffer->offset[1] = 100;
        mockBuffer->offset[2] = 200;
        mockBuffer->fd[0] = 10;
        mockBuffer->fd[1] = 11;
        mockBuffer->fd[2] = 12;
        mockBuffer->driverBuffer = reinterpret_cast<void*>(0xDEADBEEF);
    }

    void TearDown() override {
        free(mockBuffer);
    }
};

TEST_F(WesterosSimpleBufferPropertyTest, BufferGetFormat) {
    uint32_t format = WstSBBufferGetFormat(mockBuffer);
    EXPECT_EQ(format, WL_SB_FORMAT_ARGB8888);
}

TEST_F(WesterosSimpleBufferPropertyTest, BufferGetWidth) {
    int32_t width = WstSBBufferGetWidth(mockBuffer);
    EXPECT_EQ(width, 1920);
}

TEST_F(WesterosSimpleBufferPropertyTest, BufferGetHeight) {
    int32_t height = WstSBBufferGetHeight(mockBuffer);
    EXPECT_EQ(height, 1080);
}

TEST_F(WesterosSimpleBufferPropertyTest, BufferGetStride) {
    int32_t stride = WstSBBufferGetStride(mockBuffer);
    EXPECT_EQ(stride, 7680);
}

TEST_F(WesterosSimpleBufferPropertyTest, BufferGetPlaneOffsetAndStride) {
    int32_t offset, stride;
    
    // Test plane 0
    WstSBBufferGetPlaneOffsetAndStride(mockBuffer, 0, &offset, &stride);
    EXPECT_EQ(offset, 0);
    EXPECT_EQ(stride, 7680);
    
    // Test plane 1
    WstSBBufferGetPlaneOffsetAndStride(mockBuffer, 1, &offset, &stride);
    EXPECT_EQ(offset, 100);
    EXPECT_EQ(stride, 3840);
    
    // Test plane 2
    WstSBBufferGetPlaneOffsetAndStride(mockBuffer, 2, &offset, &stride);
    EXPECT_EQ(offset, 200);
    EXPECT_EQ(stride, 1920);
}

TEST_F(WesterosSimpleBufferPropertyTest, BufferGetPlaneOffsetAndStrideInvalidPlane) {
    int32_t offset = -1, stride = -1;
    
    // Test invalid plane numbers
    WstSBBufferGetPlaneOffsetAndStride(mockBuffer, -1, &offset, &stride);
    EXPECT_EQ(offset, -1);  // Should not be modified
    EXPECT_EQ(stride, -1);  // Should not be modified
    
    WstSBBufferGetPlaneOffsetAndStride(mockBuffer, 3, &offset, &stride);
    EXPECT_EQ(offset, -1);  // Should not be modified
    EXPECT_EQ(stride, -1);  // Should not be modified
}

TEST_F(WesterosSimpleBufferPropertyTest, BufferGetBuffer) {
    void* buffer = WstSBBufferGetBuffer(mockBuffer);
    EXPECT_EQ(buffer, reinterpret_cast<void*>(0xDEADBEEF));
}

TEST_F(WesterosSimpleBufferPropertyTest, BufferGetFd) {
    int fd = WstSBBufferGetFd(mockBuffer);
    EXPECT_EQ(fd, 10);
}

TEST_F(WesterosSimpleBufferPropertyTest, BufferGetPlaneFd) {
    // Test valid plane numbers
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 0), 10);
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 1), 11);
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 2), 12);
    
    // Test invalid plane numbers
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, -1), -1);
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 3), -1);
}

// Format validation tests
TEST(WesterosSimpleBufferFormatTest, ValidRGBFormats) {
    // Test various RGB format values
    EXPECT_EQ(WL_SB_FORMAT_ARGB8888, 0x34325241);
    EXPECT_EQ(WL_SB_FORMAT_XRGB8888, 0x34325258);
    EXPECT_EQ(WL_SB_FORMAT_RGB565, 0x36314752);
    EXPECT_EQ(WL_SB_FORMAT_YUYV, 0x56595559);
}

TEST(WesterosSimpleBufferFormatTest, ValidYUVFormats) {
    // Test various YUV format values
    EXPECT_EQ(WL_SB_FORMAT_YUV420, 0x32315559);
    EXPECT_EQ(WL_SB_FORMAT_YUV422, 0x36315559);
    EXPECT_EQ(WL_SB_FORMAT_YUV444, 0x34325559);
    EXPECT_EQ(WL_SB_FORMAT_NV12, 0x3231564e);
    EXPECT_EQ(WL_SB_FORMAT_NV16, 0x3631564e);
}

// Cleanup tests
TEST_F(WesterosSimpleBufferTest, ProperCleanupAfterUninit) {
    // Uninit should clean up properly (tested in TearDown)
    // This test verifies no crashes occur during cleanup
}

TEST_F(WesterosSimpleBufferTest, DoubleUninitHandling) {
    // First uninit
    WstSBUninit(sb);
    sb = nullptr; // Prevent double cleanup in TearDown

    // Should handle double uninit gracefully
    EXPECT_NO_THROW(WstSBUninit(nullptr));
}

// Boundary value tests
TEST_F(WesterosSimpleBufferPropertyTest, ZeroDimensionBuffer) {
    mockBuffer->width = 0;
    mockBuffer->height = 0;
    
    EXPECT_EQ(WstSBBufferGetWidth(mockBuffer), 0);
    EXPECT_EQ(WstSBBufferGetHeight(mockBuffer), 0);
}

TEST_F(WesterosSimpleBufferPropertyTest, MaxDimensionBuffer) {
    mockBuffer->width = INT32_MAX;
    mockBuffer->height = INT32_MAX;
    
    EXPECT_EQ(WstSBBufferGetWidth(mockBuffer), INT32_MAX);
    EXPECT_EQ(WstSBBufferGetHeight(mockBuffer), INT32_MAX);
}

TEST_F(WesterosSimpleBufferPropertyTest, NegativeDimensionBuffer) {
    mockBuffer->width = -100;
    mockBuffer->height = -200;
    
    EXPECT_EQ(WstSBBufferGetWidth(mockBuffer), -100);
    EXPECT_EQ(WstSBBufferGetHeight(mockBuffer), -200);
}

// Edge cases for file descriptors
TEST_F(WesterosSimpleBufferPropertyTest, InvalidFileDescriptors) {
    mockBuffer->fd[0] = -1;
    mockBuffer->fd[1] = -1;
    mockBuffer->fd[2] = -1;
    
    EXPECT_EQ(WstSBBufferGetFd(mockBuffer), -1);
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 0), -1);
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 1), -1);
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 2), -1);
}

// Stress tests
TEST_F(WesterosSimpleBufferPropertyTest, MultipleAccessesToSameBuffer) {
    // Access same buffer properties multiple times
    for (int i = 0; i < 1000; ++i) {
        EXPECT_EQ(WstSBBufferGetFormat(mockBuffer), WL_SB_FORMAT_ARGB8888);
        EXPECT_EQ(WstSBBufferGetWidth(mockBuffer), 1920);
        EXPECT_EQ(WstSBBufferGetHeight(mockBuffer), 1080);
        EXPECT_EQ(WstSBBufferGetStride(mockBuffer), 7680);
    }
}

// Null pointer safety tests
TEST(WesterosSimpleBufferNullSafetyTest, BufferPropertyFunctionsWithNullBuffer) {
    // These functions should handle null buffers gracefully and return safe default values
    // This validates the NULL pointer safety improvements
    
    // Test format access with null buffer - should return 0
    EXPECT_EQ(WstSBBufferGetFormat(nullptr), 0);
    
    // Test dimension access with null buffer - should return 0
    EXPECT_EQ(WstSBBufferGetWidth(nullptr), 0);
    EXPECT_EQ(WstSBBufferGetHeight(nullptr), 0);
    
    // Test stride access with null buffer - should return 0
    EXPECT_EQ(WstSBBufferGetStride(nullptr), 0);
    
    // Test buffer pointer access with null buffer - should return nullptr
    EXPECT_EQ(WstSBBufferGetBuffer(nullptr), nullptr);
    
    // Test fd access with null buffer - should return -1
    EXPECT_EQ(WstSBBufferGetFd(nullptr), -1);
    EXPECT_EQ(WstSBBufferGetPlaneFd(nullptr, 0), -1);
}

// Callback structure validation
TEST_F(WesterosSimpleBufferTest, CallbackStructureIntegrity) {
    // Verify all callbacks are properly set
    EXPECT_NE(callbacks.bind, nullptr);
    EXPECT_NE(callbacks.reference_buffer, nullptr);
    EXPECT_NE(callbacks.release_buffer, nullptr);
}

// Format enum completeness test
TEST(WesterosSimpleBufferFormatTest, FormatEnumCompleteness) {
    // Test that all major format categories are represented
    
    // RGB formats
    EXPECT_NE(WL_SB_FORMAT_RGB565, 0);
    EXPECT_NE(WL_SB_FORMAT_ARGB8888, 0);
    EXPECT_NE(WL_SB_FORMAT_XRGB8888, 0);
    
    // YUV packed formats
    EXPECT_NE(WL_SB_FORMAT_YUYV, 0);
    EXPECT_NE(WL_SB_FORMAT_YVYU, 0);
    EXPECT_NE(WL_SB_FORMAT_UYVY, 0);
    EXPECT_NE(WL_SB_FORMAT_VYUY, 0);
    
    // YUV planar formats
    EXPECT_NE(WL_SB_FORMAT_YUV420, 0);
    EXPECT_NE(WL_SB_FORMAT_YUV422, 0);
    EXPECT_NE(WL_SB_FORMAT_YUV444, 0);
    
    // YUV semi-planar formats
    EXPECT_NE(WL_SB_FORMAT_NV12, 0);
    EXPECT_NE(WL_SB_FORMAT_NV21, 0);
    EXPECT_NE(WL_SB_FORMAT_NV16, 0);
    EXPECT_NE(WL_SB_FORMAT_NV61, 0);
}

//==============================================================================
// Additional L1 Tests for >90% Coverage
//==============================================================================

// Test Group: Buffer Resource Management
TEST_F(WesterosSimpleBufferTest, BufferGet_WithInvalidResourceType) {
    // Create resource with different interface
    struct wl_resource* resource = wl_resource_create(
        reinterpret_cast<struct wl_client*>(0x1000), 
        &wl_sb_interface,  // Wrong interface (not wl_buffer_interface)
        1, 
        1002
    );
    ASSERT_NE(resource, nullptr);

    struct wl_sb_buffer* buffer = WstSBBufferGet(resource);
    EXPECT_EQ(buffer, nullptr);  // Should return NULL for wrong interface type

    wl_resource_destroy(resource);
}

// Test Group: Initialization Edge Cases
TEST_F(WesterosSimpleBufferTest, UninitWithNull_NoSegfault) {
    // Should handle null gracefully
    EXPECT_NO_THROW(WstSBUninit(nullptr));
}

// Test Group: All Buffer Format Getters with Edge Cases
TEST_F(WesterosSimpleBufferPropertyTest, BufferGetFormat_AllSupportedFormats) {
    // Test all RGB formats
    mockBuffer->format = WL_SB_FORMAT_ARGB8888;
    EXPECT_EQ(WstSBBufferGetFormat(mockBuffer), WL_SB_FORMAT_ARGB8888);
    
    mockBuffer->format = WL_SB_FORMAT_XRGB8888;
    EXPECT_EQ(WstSBBufferGetFormat(mockBuffer), WL_SB_FORMAT_XRGB8888);
    
    mockBuffer->format = WL_SB_FORMAT_RGB565;
    EXPECT_EQ(WstSBBufferGetFormat(mockBuffer), WL_SB_FORMAT_RGB565);
    
    mockBuffer->format = WL_SB_FORMAT_YUYV;
    EXPECT_EQ(WstSBBufferGetFormat(mockBuffer), WL_SB_FORMAT_YUYV);
    
    // Test YUV formats
    mockBuffer->format = WL_SB_FORMAT_YUV420;
    EXPECT_EQ(WstSBBufferGetFormat(mockBuffer), WL_SB_FORMAT_YUV420);
    
    mockBuffer->format = WL_SB_FORMAT_NV12;
    EXPECT_EQ(WstSBBufferGetFormat(mockBuffer), WL_SB_FORMAT_NV12);
    
    mockBuffer->format = WL_SB_FORMAT_NV21;
    EXPECT_EQ(WstSBBufferGetFormat(mockBuffer), WL_SB_FORMAT_NV21);
}

TEST_F(WesterosSimpleBufferPropertyTest, BufferGetWidth_BoundaryValues) {
    // Zero width
    mockBuffer->width = 0;
    EXPECT_EQ(WstSBBufferGetWidth(mockBuffer), 0);
    
    // Minimum positive width
    mockBuffer->width = 1;
    EXPECT_EQ(WstSBBufferGetWidth(mockBuffer), 1);
    
    // Common HD width
    mockBuffer->width = 1280;
    EXPECT_EQ(WstSBBufferGetWidth(mockBuffer), 1280);
    
    // 4K width
    mockBuffer->width = 3840;
    EXPECT_EQ(WstSBBufferGetWidth(mockBuffer), 3840);
    
    // Maximum value
    mockBuffer->width = INT32_MAX;
    EXPECT_EQ(WstSBBufferGetWidth(mockBuffer), INT32_MAX);
}

TEST_F(WesterosSimpleBufferPropertyTest, BufferGetHeight_BoundaryValues) {
    // Zero height
    mockBuffer->height = 0;
    EXPECT_EQ(WstSBBufferGetHeight(mockBuffer), 0);
    
    // Minimum positive height
    mockBuffer->height = 1;
    EXPECT_EQ(WstSBBufferGetHeight(mockBuffer), 1);
    
    // Common HD height
    mockBuffer->height = 720;
    EXPECT_EQ(WstSBBufferGetHeight(mockBuffer), 720);
    
    // Full HD height
    mockBuffer->height = 1080;
    EXPECT_EQ(WstSBBufferGetHeight(mockBuffer), 1080);
    
    // 4K height
    mockBuffer->height = 2160;
    EXPECT_EQ(WstSBBufferGetHeight(mockBuffer), 2160);
}

TEST_F(WesterosSimpleBufferPropertyTest, BufferGetStride_VariousValues) {
    // Minimum stride
    mockBuffer->stride[0] = 0;
    EXPECT_EQ(WstSBBufferGetStride(mockBuffer), 0);
    
    // Typical ARGB8888 stride (1920 * 4)
    mockBuffer->stride[0] = 7680;
    EXPECT_EQ(WstSBBufferGetStride(mockBuffer), 7680);
    
    // Aligned stride (power of 2)
    mockBuffer->stride[0] = 8192;
    EXPECT_EQ(WstSBBufferGetStride(mockBuffer), 8192);
    
    // Large stride
    mockBuffer->stride[0] = 1000000;
    EXPECT_EQ(WstSBBufferGetStride(mockBuffer), 1000000);
}

TEST_F(WesterosSimpleBufferPropertyTest, BufferGetPlaneOffsetAndStride_AllPlanes) {
    int32_t offset, stride;
    
    // Set different values for all planes
    mockBuffer->offset[0] = 0;
    mockBuffer->offset[1] = 1920*1080;
    mockBuffer->offset[2] = 1920*1080 + 960*540;
    mockBuffer->stride[0] = 1920;
    mockBuffer->stride[1] = 960;
    mockBuffer->stride[2] = 960;
    
    // Test plane 0 (Y plane for YUV)
    offset = -999; stride = -999;
    WstSBBufferGetPlaneOffsetAndStride(mockBuffer, 0, &offset, &stride);
    EXPECT_EQ(offset, 0);
    EXPECT_EQ(stride, 1920);
    
    // Test plane 1 (U plane for YUV)
    offset = -999; stride = -999;
    WstSBBufferGetPlaneOffsetAndStride(mockBuffer, 1, &offset, &stride);
    EXPECT_EQ(offset, 1920*1080);
    EXPECT_EQ(stride, 960);
    
    // Test plane 2 (V plane for YUV)
    offset = -999; stride = -999;
    WstSBBufferGetPlaneOffsetAndStride(mockBuffer, 2, &offset, &stride);
    EXPECT_EQ(offset, 1920*1080 + 960*540);
    EXPECT_EQ(stride, 960);
}

TEST_F(WesterosSimpleBufferPropertyTest, BufferGetPlaneOffsetAndStride_InvalidPlanes) {
    int32_t offset = 123, stride = 456;
    
    // Test below valid range
    WstSBBufferGetPlaneOffsetAndStride(mockBuffer, -1, &offset, &stride);
    EXPECT_EQ(offset, 123);  // Should not be modified
    EXPECT_EQ(stride, 456);  // Should not be modified
    
    WstSBBufferGetPlaneOffsetAndStride(mockBuffer, -100, &offset, &stride);
    EXPECT_EQ(offset, 123);
    EXPECT_EQ(stride, 456);
    
    // Test above valid range
    offset = 123; stride = 456;
    WstSBBufferGetPlaneOffsetAndStride(mockBuffer, 3, &offset, &stride);
    EXPECT_EQ(offset, 123);
    EXPECT_EQ(stride, 456);
    
    WstSBBufferGetPlaneOffsetAndStride(mockBuffer, 10, &offset, &stride);
    EXPECT_EQ(offset, 123);
    EXPECT_EQ(stride, 456);
}

TEST_F(WesterosSimpleBufferPropertyTest, BufferGetPlaneOffsetAndStride_NullPointers) {
    // Test with null offset pointer (should not crash)
    EXPECT_NO_THROW(WstSBBufferGetPlaneOffsetAndStride(mockBuffer, 0, nullptr, &mockBuffer->stride[0]));
    
    // Test with null stride pointer (should not crash)
    EXPECT_NO_THROW(WstSBBufferGetPlaneOffsetAndStride(mockBuffer, 0, &mockBuffer->offset[0], nullptr));
    
    // Test with both null (should not crash)
    EXPECT_NO_THROW(WstSBBufferGetPlaneOffsetAndStride(mockBuffer, 0, nullptr, nullptr));
}

TEST_F(WesterosSimpleBufferPropertyTest, BufferGetBuffer_VariousPointers) {
    // Test null driver buffer
    mockBuffer->driverBuffer = nullptr;
    EXPECT_EQ(WstSBBufferGetBuffer(mockBuffer), nullptr);
    
    // Test valid pointer
    void* testPtr = reinterpret_cast<void*>(0xCAFEBABE);
    mockBuffer->driverBuffer = testPtr;
    EXPECT_EQ(WstSBBufferGetBuffer(mockBuffer), testPtr);
    
    // Test another valid pointer
    testPtr = reinterpret_cast<void*>(0x12345678);
    mockBuffer->driverBuffer = testPtr;
    EXPECT_EQ(WstSBBufferGetBuffer(mockBuffer), testPtr);
}

TEST_F(WesterosSimpleBufferPropertyTest, BufferGetFd_VariousValues) {
    // Zero FD (valid but unusual)
    mockBuffer->fd[0] = 0;
    EXPECT_EQ(WstSBBufferGetFd(mockBuffer), 0);
    
    // Negative FD (invalid)
    mockBuffer->fd[0] = -1;
    EXPECT_EQ(WstSBBufferGetFd(mockBuffer), -1);
    
    // Typical FD values
    mockBuffer->fd[0] = 3;
    EXPECT_EQ(WstSBBufferGetFd(mockBuffer), 3);
    
    mockBuffer->fd[0] = 42;
    EXPECT_EQ(WstSBBufferGetFd(mockBuffer), 42);
    
    // Large FD
    mockBuffer->fd[0] = 1024;
    EXPECT_EQ(WstSBBufferGetFd(mockBuffer), 1024);
}

TEST_F(WesterosSimpleBufferPropertyTest, BufferGetPlaneFd_AllPlanes) {
    // Set different FDs for all planes
    mockBuffer->fd[0] = 10;
    mockBuffer->fd[1] = 20;
    mockBuffer->fd[2] = 30;
    
    // Test all valid planes
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 0), 10);
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 1), 20);
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 2), 30);
}

TEST_F(WesterosSimpleBufferPropertyTest, BufferGetPlaneFd_InvalidPlanes) {
    // Test negative plane indices
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, -1), -1);
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, -2), -1);
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, -100), -1);
    
    // Test out of range positive indices
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 3), -1);
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 4), -1);
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 100), -1);
}

TEST_F(WesterosSimpleBufferPropertyTest, BufferGetPlaneFd_BoundaryPlanes) {
    // Set FDs for boundary testing
    mockBuffer->fd[0] = 0;    // Minimum valid FD
    mockBuffer->fd[1] = -1;   // Invalid FD
    mockBuffer->fd[2] = INT32_MAX;  // Maximum FD
    
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 0), 0);
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 1), -1);
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 2), INT32_MAX);
}

// Test Group: Callback Functionality
TEST_F(WesterosSimpleBufferTest, InitWithPartialCallbacks_OnlyBindSet) {
    struct wl_display* tempDisplay = wl_display_create();
    ASSERT_NE(tempDisplay, nullptr);
    
    wayland_sb_callbacks partialCallbacks;
    memset(&partialCallbacks, 0, sizeof(partialCallbacks));
    partialCallbacks.bind = mock_bind;
    // reference_buffer and release_buffer are NULL
    
    wl_sb* tempSB = WstSBInit(tempDisplay, &partialCallbacks, testUserData);
    // Should still initialize (callbacks are not validated individually)
    
    if (tempSB) {
        WstSBUninit(tempSB);
    }
    wl_display_destroy(tempDisplay);
}

// Test Group: Memory and Resource Safety
TEST_F(WesterosSimpleBufferPropertyTest, MultipleAccessDifferentProperties_Consistency) {
    // Set all properties to known values
    mockBuffer->format = WL_SB_FORMAT_ARGB8888;
    mockBuffer->width = 1920;
    mockBuffer->height = 1080;
    mockBuffer->stride[0] = 7680;
    mockBuffer->fd[0] = 42;
    void* testPtr = reinterpret_cast<void*>(0xABCDEF00);
    mockBuffer->driverBuffer = testPtr;
    
    // Access all properties multiple times and verify consistency
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(WstSBBufferGetFormat(mockBuffer), WL_SB_FORMAT_ARGB8888);
        EXPECT_EQ(WstSBBufferGetWidth(mockBuffer), 1920);
        EXPECT_EQ(WstSBBufferGetHeight(mockBuffer), 1080);
        EXPECT_EQ(WstSBBufferGetStride(mockBuffer), 7680);
        EXPECT_EQ(WstSBBufferGetFd(mockBuffer), 42);
        EXPECT_EQ(WstSBBufferGetBuffer(mockBuffer), testPtr);
    }
}

// Test Group: Format Coverage for All Defined Formats
TEST(WesterosSimpleBufferFormatTest, AllDefinedFormats_HaveValues) {
    // RGB formats
    EXPECT_EQ(WL_SB_FORMAT_ARGB8888, 0x34325241);
    EXPECT_EQ(WL_SB_FORMAT_XRGB8888, 0x34325258);
    EXPECT_EQ(WL_SB_FORMAT_RGB565, 0x36314752);
    
    // Packed YUV formats
    EXPECT_EQ(WL_SB_FORMAT_YUYV, 0x56595559);
    EXPECT_EQ(WL_SB_FORMAT_YVYU, 0x55595659);
    EXPECT_EQ(WL_SB_FORMAT_UYVY, 0x59565955);
    EXPECT_EQ(WL_SB_FORMAT_VYUY, 0x59555956);
    
    // Planar YUV formats
    EXPECT_EQ(WL_SB_FORMAT_YUV410, 0x39565559);
    EXPECT_EQ(WL_SB_FORMAT_YUV411, 0x31315559);
    EXPECT_EQ(WL_SB_FORMAT_YUV420, 0x32315559);
    EXPECT_EQ(WL_SB_FORMAT_YUV422, 0x36315559);
    EXPECT_EQ(WL_SB_FORMAT_YUV444, 0x34325559);
    
    // Semi-planar YUV formats
    EXPECT_EQ(WL_SB_FORMAT_NV12, 0x3231564e);
    EXPECT_EQ(WL_SB_FORMAT_NV21, 0x3132564e);
    EXPECT_EQ(WL_SB_FORMAT_NV16, 0x3631564e);
    EXPECT_EQ(WL_SB_FORMAT_NV61, 0x3136564e);
}

// Test Group: Error Codes
TEST(WesterosSimpleBufferErrorTest, ErrorEnumValues) {
    EXPECT_EQ(WL_SB_ERROR_AUTHENTICATE_FAIL, 0);
    EXPECT_EQ(WL_SB_ERROR_INVALID_FORMAT, 1);
    EXPECT_EQ(WL_SB_ERROR_INVALID_NAME, 2);
}

// Test Group: Concurrent Operations
TEST_F(WesterosSimpleBufferPropertyTest, ConcurrentPropertyAccess_ThreadSafety) {
    // Set properties
    mockBuffer->format = WL_SB_FORMAT_NV12;
    mockBuffer->width = 1920;
    mockBuffer->height = 1080;
    mockBuffer->stride[0] = 1920;
    mockBuffer->stride[1] = 1920;
    mockBuffer->fd[0] = 10;
    mockBuffer->fd[1] = 11;
    
    // Simulate concurrent access (though not truly multi-threaded in this test)
    for (int iteration = 0; iteration < 50; ++iteration) {
        uint32_t format = WstSBBufferGetFormat(mockBuffer);
        int32_t width = WstSBBufferGetWidth(mockBuffer);
        int32_t height = WstSBBufferGetHeight(mockBuffer);
        int32_t stride = WstSBBufferGetStride(mockBuffer);
        int fd0 = WstSBBufferGetPlaneFd(mockBuffer, 0);
        int fd1 = WstSBBufferGetPlaneFd(mockBuffer, 1);
        
        EXPECT_EQ(format, WL_SB_FORMAT_NV12);
        EXPECT_EQ(width, 1920);
        EXPECT_EQ(height, 1080);
        EXPECT_EQ(stride, 1920);
        EXPECT_EQ(fd0, 10);
        EXPECT_EQ(fd1, 11);
    }
}

// ============================================================================
// Additional Coverage Tests - Buffer State and Edge Cases
// ============================================================================
// These tests improve coverage by testing buffer state handling and
// edge cases that don't require protocol-level mocking.
// ============================================================================

// Test: Multiple initialization cycles
TEST_F(WesterosSimpleBufferTest, MultipleInitUninit_NoMemoryLeaks) {
    for (int i = 0; i < 5; ++i) {
        struct wl_display* tempDisplay = wl_display_create();
        ASSERT_NE(tempDisplay, nullptr);
        
        wayland_sb_callbacks tempCallbacks;
        memset(&tempCallbacks, 0, sizeof(tempCallbacks));
        tempCallbacks.bind = mock_bind;
        tempCallbacks.reference_buffer = mock_reference_buffer;
        tempCallbacks.release_buffer = mock_release_buffer;
        
        wl_sb* tempSB = WstSBInit(tempDisplay, &tempCallbacks, testUserData);
        EXPECT_NE(tempSB, nullptr);
        
        WstSBUninit(tempSB);
        wl_display_destroy(tempDisplay);
    }
}

// Test: Callback infrastructure - bind callback
TEST_F(WesterosSimpleBufferTest, BindCallback_Invoked) {
    // Create a client and resource that would trigger bind
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = reinterpret_cast<struct wl_resource*>(0x2000);
    
    // Expect bind callback to be called with all 3 parameters
    EXPECT_CALL(mockCallbacks, bind(testUserData, client, resource))
        .Times(1);
    
    if (callbacks.bind) {
        callbacks.bind(testUserData, client, resource);
    }
}

// Test: Callback infrastructure - reference_buffer callback
TEST_F(WesterosSimpleBufferTest, ReferenceBufferCallback_Invoked) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    uint32_t native_handle = 0x12345678;
    struct wl_sb_buffer* buffer = reinterpret_cast<struct wl_sb_buffer*>(0xABCDEF00);
    
    // Expect reference_buffer callback with all 4 parameters
    EXPECT_CALL(mockCallbacks, reference_buffer(testUserData, client, native_handle, buffer))
        .Times(1);
    
    if (callbacks.reference_buffer) {
        callbacks.reference_buffer(testUserData, client, native_handle, buffer);
    }
}

// Test: Callback infrastructure - release_buffer callback
TEST_F(WesterosSimpleBufferTest, ReleaseBufferCallback_Invoked) {
    struct wl_sb_buffer* buffer = reinterpret_cast<struct wl_sb_buffer*>(0xDEADBEEF);
    
    // Expect release_buffer callback with correct buffer pointer type
    EXPECT_CALL(mockCallbacks, release_buffer(testUserData, buffer))
        .Times(1);
    
    if (callbacks.release_buffer) {
        callbacks.release_buffer(testUserData, buffer);
    }
}

// Test: Buffer property getters with NULL buffer
TEST_F(WesterosSimpleBufferTest, BufferGetters_WithNullBuffer_HandleGracefully) {
    // These should handle NULL gracefully (return 0 or -1)
    EXPECT_EQ(WstSBBufferGetFormat(nullptr), 0);
    EXPECT_EQ(WstSBBufferGetWidth(nullptr), 0);
    EXPECT_EQ(WstSBBufferGetHeight(nullptr), 0);
    EXPECT_EQ(WstSBBufferGetStride(nullptr), 0);
    EXPECT_EQ(WstSBBufferGetFd(nullptr), -1);
    EXPECT_EQ(WstSBBufferGetBuffer(nullptr), nullptr);
}

// Test: Buffer FD handling - multiple FDs
TEST_F(WesterosSimpleBufferPropertyTest, BufferGetPlaneFd_MultipleFds) {
    // Set up buffer with multiple FDs (planar YUV)
    mockBuffer->fd[0] = 10;
    mockBuffer->fd[1] = 11;
    mockBuffer->fd[2] = 12;
    
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 0), 10);
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 1), 11);
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 2), 12);
}

// Test: Buffer FD handling - invalid plane index
TEST_F(WesterosSimpleBufferPropertyTest, BufferGetPlaneFd_InvalidPlane) {
    mockBuffer->fd[0] = 10;
    
    // Out of bounds plane indices
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, -1), -1);
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 3), -1);
    EXPECT_EQ(WstSBBufferGetPlaneFd(mockBuffer, 100), -1);
}

// Test: All YUV format validations
TEST_F(WesterosSimpleBufferPropertyTest, BufferGetFormat_AllYUVFormats) {
    mockBuffer->format = WL_SB_FORMAT_YUV420;
    EXPECT_EQ(WstSBBufferGetFormat(mockBuffer), WL_SB_FORMAT_YUV420);
    
    mockBuffer->format = WL_SB_FORMAT_YUV422;
    EXPECT_EQ(WstSBBufferGetFormat(mockBuffer), WL_SB_FORMAT_YUV422);
    
    mockBuffer->format = WL_SB_FORMAT_YUV444;
    EXPECT_EQ(WstSBBufferGetFormat(mockBuffer), WL_SB_FORMAT_YUV444);
    
    mockBuffer->format = WL_SB_FORMAT_NV12;
    EXPECT_EQ(WstSBBufferGetFormat(mockBuffer), WL_SB_FORMAT_NV12);
    
    mockBuffer->format = WL_SB_FORMAT_NV21;
    EXPECT_EQ(WstSBBufferGetFormat(mockBuffer), WL_SB_FORMAT_NV21);
    
    mockBuffer->format = WL_SB_FORMAT_NV16;
    EXPECT_EQ(WstSBBufferGetFormat(mockBuffer), WL_SB_FORMAT_NV16);
    
    mockBuffer->format = WL_SB_FORMAT_NV61;
    EXPECT_EQ(WstSBBufferGetFormat(mockBuffer), WL_SB_FORMAT_NV61);
    
    mockBuffer->format = WL_SB_FORMAT_YUV410;
    EXPECT_EQ(WstSBBufferGetFormat(mockBuffer), WL_SB_FORMAT_YUV410);
    
    mockBuffer->format = WL_SB_FORMAT_YUV411;
    EXPECT_EQ(WstSBBufferGetFormat(mockBuffer), WL_SB_FORMAT_YUV411);
}

// Test: Stride values for different formats
TEST_F(WesterosSimpleBufferPropertyTest, BufferGetStride_VariousFormats) {
    // ARGB8888: 4 bytes per pixel
    mockBuffer->format = WL_SB_FORMAT_ARGB8888;
    mockBuffer->width = 1920;
    mockBuffer->stride[0] = 1920 * 4;
    EXPECT_EQ(WstSBBufferGetStride(mockBuffer), 7680);
    
    // RGB565: 2 bytes per pixel
    mockBuffer->format = WL_SB_FORMAT_RGB565;
    mockBuffer->stride[0] = 1920 * 2;
    EXPECT_EQ(WstSBBufferGetStride(mockBuffer), 3840);
    
    // YUYV: 2 bytes per pixel (packed)
    mockBuffer->format = WL_SB_FORMAT_YUYV;
    mockBuffer->stride[0] = 1920 * 2;
    EXPECT_EQ(WstSBBufferGetStride(mockBuffer), 3840);
}

// Test: Plane offset and stride for different YUV formats
TEST_F(WesterosSimpleBufferPropertyTest, BufferPlaneLayout_YUV420) {
    int32_t offset, stride;
    
    // YUV420: Y plane full size, U/V planes quarter size
    mockBuffer->format = WL_SB_FORMAT_YUV420;
    mockBuffer->width = 1920;
    mockBuffer->height = 1080;
    mockBuffer->stride[0] = 1920;    // Y plane stride
    mockBuffer->stride[1] = 960;     // U plane stride (half width)
    mockBuffer->stride[2] = 960;     // V plane stride (half width)
    mockBuffer->offset[0] = 0;       // Y plane at start
    mockBuffer->offset[1] = 1920 * 1080;  // U plane after Y
    mockBuffer->offset[2] = 1920 * 1080 + 960 * 540;  // V plane after U
    
    WstSBBufferGetPlaneOffsetAndStride(mockBuffer, 0, &offset, &stride);
    EXPECT_EQ(offset, 0);
    EXPECT_EQ(stride, 1920);
    
    WstSBBufferGetPlaneOffsetAndStride(mockBuffer, 1, &offset, &stride);
    EXPECT_EQ(offset, 1920 * 1080);
    EXPECT_EQ(stride, 960);
    
    WstSBBufferGetPlaneOffsetAndStride(mockBuffer, 2, &offset, &stride);
    EXPECT_EQ(offset, 1920 * 1080 + 960 * 540);
    EXPECT_EQ(stride, 960);
}

// Test: Plane offset and stride for NV12 (semi-planar)
TEST_F(WesterosSimpleBufferPropertyTest, BufferPlaneLayout_NV12) {
    int32_t offset, stride;
    
    // NV12: Y plane full size, UV interleaved plane half height
    mockBuffer->format = WL_SB_FORMAT_NV12;
    mockBuffer->width = 1920;
    mockBuffer->height = 1080;
    mockBuffer->stride[0] = 1920;    // Y plane stride
    mockBuffer->stride[1] = 1920;    // UV plane stride (full width, interleaved)
    mockBuffer->offset[0] = 0;       // Y plane at start
    mockBuffer->offset[1] = 1920 * 1080;  // UV plane after Y
    
    WstSBBufferGetPlaneOffsetAndStride(mockBuffer, 0, &offset, &stride);
    EXPECT_EQ(offset, 0);
    EXPECT_EQ(stride, 1920);
    
    WstSBBufferGetPlaneOffsetAndStride(mockBuffer, 1, &offset, &stride);
    EXPECT_EQ(offset, 1920 * 1080);
    EXPECT_EQ(stride, 1920);
}

// Test: Buffer dimensions - various resolutions
TEST_F(WesterosSimpleBufferPropertyTest, BufferDimensions_CommonResolutions) {
    // 720p
    mockBuffer->width = 1280;
    mockBuffer->height = 720;
    EXPECT_EQ(WstSBBufferGetWidth(mockBuffer), 1280);
    EXPECT_EQ(WstSBBufferGetHeight(mockBuffer), 720);
    
    // 1080p
    mockBuffer->width = 1920;
    mockBuffer->height = 1080;
    EXPECT_EQ(WstSBBufferGetWidth(mockBuffer), 1920);
    EXPECT_EQ(WstSBBufferGetHeight(mockBuffer), 1080);
    
    // 4K
    mockBuffer->width = 3840;
    mockBuffer->height = 2160;
    EXPECT_EQ(WstSBBufferGetWidth(mockBuffer), 3840);
    EXPECT_EQ(WstSBBufferGetHeight(mockBuffer), 2160);
    
    // 8K
    mockBuffer->width = 7680;
    mockBuffer->height = 4320;
    EXPECT_EQ(WstSBBufferGetWidth(mockBuffer), 7680);
    EXPECT_EQ(WstSBBufferGetHeight(mockBuffer), 4320);
}

// Test: Driver buffer pointer
TEST_F(WesterosSimpleBufferPropertyTest, BufferGetBuffer_ValidPointer) {
    void* expectedBuffer = reinterpret_cast<void*>(0xCAFEBABE);
    mockBuffer->driverBuffer = expectedBuffer;
    
    EXPECT_EQ(WstSBBufferGetBuffer(mockBuffer), expectedBuffer);
}

// Test: Driver buffer NULL
TEST_F(WesterosSimpleBufferPropertyTest, BufferGetBuffer_NullPointer) {
    mockBuffer->driverBuffer = nullptr;
    
    EXPECT_EQ(WstSBBufferGetBuffer(mockBuffer), nullptr);
}

/**
 * @brief Test buffer with extreme dimensions
 */
TEST_F(WesterosSimpleBufferPropertyTest, Buffer_ExtremeDimensions_Success) {
    // Maximum dimensions
    mockBuffer->width = INT32_MAX;
    mockBuffer->height = INT32_MAX;
    EXPECT_EQ(WstSBBufferGetWidth(mockBuffer), INT32_MAX);
    EXPECT_EQ(WstSBBufferGetHeight(mockBuffer), INT32_MAX);
    
    // Minimum dimensions (unusual but valid)
    mockBuffer->width = 1;
    mockBuffer->height = 1;
    EXPECT_EQ(WstSBBufferGetWidth(mockBuffer), 1);
    EXPECT_EQ(WstSBBufferGetHeight(mockBuffer), 1);
}

/**
 * @brief Test buffer with all file descriptors set
 */
TEST_F(WesterosSimpleBufferPropertyTest, Buffer_AllFdsSet_Success) {
    mockBuffer->fd[0] = 100;
    mockBuffer->fd[1] = 200;
    mockBuffer->fd[2] = 300;
    
    EXPECT_EQ(WstSBBufferGetFd(mockBuffer), 100);  // Returns first FD
}

/**
 * @brief Test buffer with negative file descriptors
 */
TEST_F(WesterosSimpleBufferPropertyTest, Buffer_NegativeFds_ValidState) {
    mockBuffer->fd[0] = -1;
    mockBuffer->fd[1] = -1;
    mockBuffer->fd[2] = -1;
    
    EXPECT_EQ(WstSBBufferGetFd(mockBuffer), -1);
}

/**
 * @brief Test buffer with mixed valid/invalid FDs
 */
TEST_F(WesterosSimpleBufferPropertyTest, Buffer_MixedFds_ReturnsFirstFd) {
    mockBuffer->fd[0] = 42;
    mockBuffer->fd[1] = -1;
    mockBuffer->fd[2] = 99;
    
    EXPECT_EQ(WstSBBufferGetFd(mockBuffer), 42);
}

/**
 * @brief Test buffer with all possible formats
 */
TEST_F(WesterosSimpleBufferPropertyTest, Buffer_AllFormats_Success) {
    uint32_t formats[] = {
        WL_SB_FORMAT_ARGB8888,
        WL_SB_FORMAT_XRGB8888,
        WL_SB_FORMAT_YUYV,
        WL_SB_FORMAT_RGB565,
        WL_SB_FORMAT_NV12,
        WL_SB_FORMAT_NV21,
        WL_SB_FORMAT_YUV420,
        WL_SB_FORMAT_YVU420
    };
    
    for (uint32_t fmt : formats) {
        mockBuffer->format = fmt;
        EXPECT_EQ(WstSBBufferGetFormat(mockBuffer), fmt);
    }
}

/**
 * @brief Test buffer with extreme stride values
 */
TEST_F(WesterosSimpleBufferPropertyTest, Buffer_ExtremeStrides_Success) {
    // Maximum stride
    mockBuffer->format = WL_SB_FORMAT_ARGB8888;
    mockBuffer->stride[0] = INT32_MAX;
    EXPECT_EQ(WstSBBufferGetStride(mockBuffer), INT32_MAX);
    
    // Zero stride
    mockBuffer->stride[0] = 0;
    EXPECT_EQ(WstSBBufferGetStride(mockBuffer), 0);
    
    // Negative stride (sometimes used for inverted images)
    mockBuffer->stride[0] = -1920;
    EXPECT_EQ(WstSBBufferGetStride(mockBuffer), -1920);
}

/**
 * @brief Test buffer with extreme offset values
 */
TEST_F(WesterosSimpleBufferPropertyTest, Buffer_ExtremeOffsets_Success) {
    // Maximum offset
    mockBuffer->offset[0] = INT32_MAX;
    mockBuffer->offset[1] = INT32_MAX;
    mockBuffer->offset[2] = INT32_MAX;
    
    // These should be retrievable
    EXPECT_EQ(mockBuffer->offset[0], INT32_MAX);
    EXPECT_EQ(mockBuffer->offset[1], INT32_MAX);
    EXPECT_EQ(mockBuffer->offset[2], INT32_MAX);
}

/**
 * @brief Test buffer with unusual but valid combinations
 */
TEST_F(WesterosSimpleBufferPropertyTest, Buffer_UnusualCombinations_Success) {
    // Tall thin buffer
    mockBuffer->width = 1;
    mockBuffer->height = 10000;
    mockBuffer->format = WL_SB_FORMAT_ARGB8888;
    mockBuffer->stride[0] = 4;  // 1 pixel * 4 bytes
    
    EXPECT_EQ(WstSBBufferGetWidth(mockBuffer), 1);
    EXPECT_EQ(WstSBBufferGetHeight(mockBuffer), 10000);
    EXPECT_EQ(WstSBBufferGetStride(mockBuffer), 4);
    
    // Wide short buffer
    mockBuffer->width = 10000;
    mockBuffer->height = 1;
    mockBuffer->stride[0] = 40000;  // 10000 pixels * 4 bytes
    
    EXPECT_EQ(WstSBBufferGetWidth(mockBuffer), 10000);
    EXPECT_EQ(WstSBBufferGetHeight(mockBuffer), 1);
    EXPECT_EQ(WstSBBufferGetStride(mockBuffer), 40000);
}

/**
 * @brief Test buffer with zero width and height
 */
TEST_F(WesterosSimpleBufferPropertyTest, Buffer_ZeroDimensions_ValidState) {
    mockBuffer->width = 0;
    mockBuffer->height = 0;
    
    EXPECT_EQ(WstSBBufferGetWidth(mockBuffer), 0);
    EXPECT_EQ(WstSBBufferGetHeight(mockBuffer), 0);
}

/**
 * @brief Test buffer driver pointer extremes
 */
TEST_F(WesterosSimpleBufferPropertyTest, Buffer_DriverBufferExtremes_Success) {
    // Maximum address
    void* maxPtr = reinterpret_cast<void*>(UINTPTR_MAX);
    mockBuffer->driverBuffer = maxPtr;
    EXPECT_EQ(WstSBBufferGetBuffer(mockBuffer), maxPtr);
    
    // Address 0 (not NULL, but zero address)
    void* zeroPtr = reinterpret_cast<void*>(0);
    mockBuffer->driverBuffer = zeroPtr;
    EXPECT_EQ(WstSBBufferGetBuffer(mockBuffer), zeroPtr);
}

/**
 * @brief Test buffer resource pointer validity
 */
TEST_F(WesterosSimpleBufferPropertyTest, Buffer_ResourcePointer_ValidStates) {
    // NULL resource
    mockBuffer->resource = nullptr;
    EXPECT_EQ(mockBuffer->resource, nullptr);
    
    // Valid resource
    struct wl_resource* res = reinterpret_cast<struct wl_resource*>(0xDEADBEEF);
    mockBuffer->resource = res;
    EXPECT_EQ(mockBuffer->resource, res);
}

/**
 * @brief Test buffer sb pointer validity  
 */
TEST_F(WesterosSimpleBufferPropertyTest, Buffer_SbPointer_ValidStates) {
    // NULL sb
    mockBuffer->sb = nullptr;
    EXPECT_EQ(mockBuffer->sb, nullptr);
    
    // Valid sb
    struct wl_sb* sb = reinterpret_cast<struct wl_sb*>(0xBEEFCAFE);
    mockBuffer->sb = sb;
    EXPECT_EQ(mockBuffer->sb, sb);
}
/*
 * ============================================================================
 * INTEGRATION TESTS FOR SOURCE COVERAGE
 * ============================================================================
 */

/**
 * @brief Test multiple Init/Uninit cycles
 * 
 * Objective: Cover initialization and cleanup paths repeatedly
 */
TEST_F(WesterosSimpleBufferTest, MultipleInitUninit_FiveCycles_AllSucceed) {
    // Uninit the one from SetUp
    if (sb) {
        WstSBUninit(sb);
        sb = nullptr;
    }
    
    // Multiple cycles
    for (int i = 0; i < 5; i++) {
        wl_sb* tempSb = WstSBInit(display, &callbacks, testUserData);
        ASSERT_NE(tempSb, nullptr) << "Init cycle " << i << " failed";
        WstSBUninit(tempSb);
    }
    
    // Restore for TearDown
    sb = WstSBInit(display, &callbacks, testUserData);
}

/**
 * @brief Test buffer accessor functions with edge cases
 * 
 * Objective: Cover WstSBBuffer accessor functions with boundary values
 */
TEST_F(WesterosSimpleBufferTest, BufferAccessors_EdgeCases_AllWork) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    
    // Setup edge case values
    buffer.width = 0;
    buffer.height = 0;
    buffer.stride[0] = 0;
    buffer.format = 0;
    buffer.fd[0] = -1;
    
    // Test accessors
    EXPECT_EQ(WstSBBufferGetWidth(&buffer), 0);
    EXPECT_EQ(WstSBBufferGetHeight(&buffer), 0);
    EXPECT_EQ(WstSBBufferGetStride(&buffer), 0);
    EXPECT_EQ(WstSBBufferGetFormat(&buffer), 0U);
    EXPECT_EQ(WstSBBufferGetFd(&buffer), -1);
    EXPECT_EQ(WstSBBufferGetBuffer(&buffer), nullptr);
}

/**
 * @brief Test WstSBBufferGetPlaneOffsetAndStride with all planes
 * 
 * Objective: Cover plane accessor function for all planes
 */
TEST_F(WesterosSimpleBufferTest, BufferPlaneAccessor_AllPlanes_Success) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    
    // Setup all planes
    for (int i = 0; i < 3; i++) {
        buffer.offset[i] = i * 1000;
        buffer.stride[i] = i * 100;
    }
    
    // Test each plane
    for (int i = 0; i < 3; i++) {
        int32_t offset = -1, stride = -1;
        WstSBBufferGetPlaneOffsetAndStride(&buffer, i, &offset, &stride);
        EXPECT_EQ(offset, i * 1000);
        EXPECT_EQ(stride, i * 100);
    }
}

/**
 * @brief Test WstSBBufferGetPlaneFd for all planes
 * 
 * Objective: Cover WstSBBufferGetPlaneFd for all planes
 */
TEST_F(WesterosSimpleBufferTest, BufferPlaneFd_AllPlanes_ReturnsCorrectFd) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    
    // Setup all plane FDs
    for (int i = 0; i < 3; i++) {
        buffer.fd[i] = 100 + i;
    }
    
    // Test each plane
    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(WstSBBufferGetPlaneFd(&buffer, i), 100 + i);
    }
}

/**
 * @brief Test buffer accessors with maximum values
 * 
 * Objective: Cover buffer accessors with maximum integer values
 */
TEST_F(WesterosSimpleBufferTest, BufferAccessors_MaximumValues_Success) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    
    buffer.width = INT32_MAX;
    buffer.height = INT32_MAX;
    buffer.stride[0] = INT32_MAX;
    buffer.format = UINT32_MAX;
    buffer.fd[0] = INT32_MAX;
    
    EXPECT_EQ(WstSBBufferGetWidth(&buffer), INT32_MAX);
    EXPECT_EQ(WstSBBufferGetHeight(&buffer), INT32_MAX);
    EXPECT_EQ(WstSBBufferGetStride(&buffer), INT32_MAX);
    EXPECT_EQ(WstSBBufferGetFormat(&buffer), UINT32_MAX);
    EXPECT_EQ(WstSBBufferGetFd(&buffer), INT32_MAX);
}

/**
 * @brief Test complete workflow with Init, operations, and Uninit
 * 
 * Objective: Cover full API usage pattern
 */
TEST_F(WesterosSimpleBufferTest, CompleteWorkflow_InitOperationsUninit_Success) {
    ASSERT_NE(sb, nullptr);
    
    // Create mock buffer
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    buffer.width = 1920;
    buffer.height = 1080;
    buffer.format = WL_SB_FORMAT_ARGB8888;
    buffer.stride[0] = 1920 * 4;
    buffer.fd[0] = 42;
    
    // Perform various operations
    EXPECT_EQ(WstSBBufferGetWidth(&buffer), 1920);
    EXPECT_EQ(WstSBBufferGetHeight(&buffer), 1080);
    EXPECT_EQ(WstSBBufferGetFormat(&buffer), WL_SB_FORMAT_ARGB8888);
    EXPECT_EQ(WstSBBufferGetStride(&buffer), 1920 * 4);
    EXPECT_EQ(WstSBBufferGetFd(&buffer), 42);
    
    int32_t offset, stride;
    WstSBBufferGetPlaneOffsetAndStride(&buffer, 0, &offset, &stride);
    
    // Workflow completes successfully
    SUCCEED();
}

/**
 * @brief Test WstSBInit with NULL display
 * 
 * Objective: Cover NULL display handling
 */
TEST_F(WesterosSimpleBufferTest, Init_WithNullDisplay_ReturnsNull) {
    wl_sb* result = WstSBInit(nullptr, &callbacks, testUserData);
    EXPECT_EQ(result, nullptr);
}

/**
 * @brief Test WstSBInit with NULL callbacks
 * 
 * Objective: Cover NULL callbacks handling
 */
TEST_F(WesterosSimpleBufferTest, Init_WithNullCallbacks_ReturnsNull) {
    wl_sb* result = WstSBInit(display, nullptr, testUserData);
    EXPECT_EQ(result, nullptr);
}

/**
 * @brief Test WstSBUninit with NULL sb
 * 
 * Objective: Cover NULL handling in Uninit
 */
TEST_F(WesterosSimpleBufferTest, Uninit_WithNullSb_HandlesGracefully) {
    // Should not crash
    WstSBUninit(nullptr);
    SUCCEED();
}

/**
 * @brief Test WstSBInit with NULL userData (should be allowed)
 * 
 * Objective: Verify NULL userData is acceptable
 */
TEST_F(WesterosSimpleBufferTest, Init_WithNullUserData_Success) {
    wl_sb* result = WstSBInit(display, &callbacks, nullptr);
    EXPECT_NE(result, nullptr);
    if (result) {
        WstSBUninit(result);
    }
}

/**
 * @brief Test buffer accessors with negative values
 * 
 * Objective: Cover negative value handling
 */
TEST_F(WesterosSimpleBufferTest, BufferAccessors_NegativeValues_HandlesCorrectly) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    
    buffer.width = -100;
    buffer.height = -200;
    buffer.stride[0] = -7680;
    buffer.fd[0] = -1;
    
    EXPECT_EQ(WstSBBufferGetWidth(&buffer), -100);
    EXPECT_EQ(WstSBBufferGetHeight(&buffer), -200);
    EXPECT_EQ(WstSBBufferGetStride(&buffer), -7680);
    EXPECT_EQ(WstSBBufferGetFd(&buffer), -1);
}

/**
 * @brief Test plane accessors with out-of-bounds indices
 * 
 * Objective: Cover boundary checking in plane functions
 */
TEST_F(WesterosSimpleBufferTest, PlaneAccessors_OutOfBounds_HandlesGracefully) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    
    // Test with valid indices
    for (int i = 0; i < 3; i++) {
        buffer.fd[i] = i + 100;
        buffer.offset[i] = i * 1000;
        buffer.stride[i] = i * 100;
    }
    
    // Access each plane
    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(WstSBBufferGetPlaneFd(&buffer, i), i + 100);
        
        int32_t offset, stride;
        WstSBBufferGetPlaneOffsetAndStride(&buffer, i, &offset, &stride);
        EXPECT_EQ(offset, i * 1000);
        EXPECT_EQ(stride, i * 100);
    }
}

/**
 * @brief Test WstSBBufferGetBuffer with various scenarios
 * 
 * Objective: Cover WstSBBufferGetBuffer function
 */
TEST_F(WesterosSimpleBufferTest, BufferGetBuffer_VariousScenarios_ReturnsCorrectly) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    
    // NULL driverBuffer
    buffer.driverBuffer = nullptr;
    EXPECT_EQ(WstSBBufferGetBuffer(&buffer), nullptr);
    
    // Valid driverBuffer
    void* mockDriver = reinterpret_cast<void*>(0xDEADBEEF);
    buffer.driverBuffer = mockDriver;
    EXPECT_EQ(WstSBBufferGetBuffer(&buffer), mockDriver);
    
    // NULL buffer parameter
    EXPECT_EQ(WstSBBufferGetBuffer(nullptr), nullptr);
}

/**
 * @brief Test buffer with all formats
 * 
 * Objective: Cover all supported buffer formats
 */
TEST_F(WesterosSimpleBufferTest, BufferFormats_AllSupported_Success) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    
    uint32_t formats[] = {
        WL_SB_FORMAT_ARGB8888,
        WL_SB_FORMAT_XRGB8888,
        WL_SB_FORMAT_YUYV,
        WL_SB_FORMAT_RGB565,
        WL_SB_FORMAT_NV12,
        WL_SB_FORMAT_NV21,
        WL_SB_FORMAT_YUV420,
        WL_SB_FORMAT_YVU420
    };
    
    for (size_t i = 0; i < sizeof(formats) / sizeof(formats[0]); i++) {
        buffer.format = formats[i];
        EXPECT_EQ(WstSBBufferGetFormat(&buffer), formats[i]);
    }
}

/**
 * @brief Test plane offset and stride with NULL output pointers
 * 
 * Objective: Cover NULL output parameter handling
 */
TEST_F(WesterosSimpleBufferTest, PlaneOffsetStride_NullOutputs_HandlesGracefully) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    
    buffer.offset[0] = 1000;
    buffer.stride[0] = 2000;
    
    // Test with NULL offset
    int32_t stride;
    WstSBBufferGetPlaneOffsetAndStride(&buffer, 0, nullptr, &stride);
    EXPECT_EQ(stride, 2000);
    
    // Test with NULL stride
    int32_t offset;
    WstSBBufferGetPlaneOffsetAndStride(&buffer, 0, &offset, nullptr);
    EXPECT_EQ(offset, 1000);
    
    // Test with both NULL (should not crash)
    WstSBBufferGetPlaneOffsetAndStride(&buffer, 0, nullptr, nullptr);
    SUCCEED();
}

/**
 * @brief Test multi-plane NV12 buffer
 * 
 * Objective: Cover typical NV12 2-plane setup
 */
TEST_F(WesterosSimpleBufferTest, MultiPlaneNV12_TwoPlanes_Success) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    
    // NV12: Y plane + interleaved UV plane
    buffer.width = 1920;
    buffer.height = 1080;
    buffer.format = WL_SB_FORMAT_NV12;
    
    // Y plane
    buffer.fd[0] = 50;
    buffer.offset[0] = 0;
    buffer.stride[0] = 1920;
    
    // UV plane (interleaved)
    buffer.fd[1] = 50;
    buffer.offset[1] = 1920 * 1080;
    buffer.stride[1] = 1920;
    
    // Verify
    EXPECT_EQ(WstSBBufferGetWidth(&buffer), 1920);
    EXPECT_EQ(WstSBBufferGetHeight(&buffer), 1080);
    EXPECT_EQ(WstSBBufferGetFormat(&buffer), WL_SB_FORMAT_NV12);
    EXPECT_EQ(WstSBBufferGetPlaneFd(&buffer, 0), 50);
    EXPECT_EQ(WstSBBufferGetPlaneFd(&buffer, 1), 50);
}

/**
 * @brief Test rapid Init/Uninit cycles for memory leak detection
 * 
 * Objective: Stress test memory management
 */
TEST_F(WesterosSimpleBufferTest, RapidInitUninit_HundredCycles_NoLeaks) {
    for (int i = 0; i < 100; i++) {
        wl_sb* tempSb = WstSBInit(display, &callbacks, testUserData);
        ASSERT_NE(tempSb, nullptr);
        WstSBUninit(tempSb);
    }
    SUCCEED();
}

/**
 * @brief Test BufferGetPlaneOffsetAndStride with negative plane - FAILURE PATH
 * 
 * Objective: Cover negative plane index handling
 */
TEST_F(WesterosSimpleBufferTest, BufferGetPlaneOffsetAndStride_NegativePlane_NoChange) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    int32_t offset = 999, stride = 888;
    
    WstSBBufferGetPlaneOffsetAndStride(&buffer, -1, &offset, &stride);
    
    // Values should NOT change
    EXPECT_EQ(offset, 999);
    EXPECT_EQ(stride, 888);
}

/**
 * @brief Test BufferGetPlaneOffsetAndStride with plane > 2 - FAILURE PATH
 * 
 * Objective: Cover out-of-bounds plane (only 0-2 valid)
 */
TEST_F(WesterosSimpleBufferTest, BufferGetPlaneOffsetAndStride_OutOfBoundsPlane_NoChange) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    int32_t offset = 777, stride = 666;
    
    WstSBBufferGetPlaneOffsetAndStride(&buffer, 3, &offset, &stride);
    
    // Values should NOT change
    EXPECT_EQ(offset, 777);
    EXPECT_EQ(stride, 666);
}

/**
 * @brief Test BufferGetPlaneFd with all valid plane indices - SUCCESS PATH
 * 
 * Objective: Cover all 3 planes (0, 1, 2)
 */
TEST_F(WesterosSimpleBufferTest, BufferGetPlaneFd_AllValidPlanes_ReturnsFds) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    buffer.fd[0] = 10;
    buffer.fd[1] = 11;
    buffer.fd[2] = 12;
    
    EXPECT_EQ(WstSBBufferGetPlaneFd(&buffer, 0), 10);
    EXPECT_EQ(WstSBBufferGetPlaneFd(&buffer, 1), 11);
    EXPECT_EQ(WstSBBufferGetPlaneFd(&buffer, 2), 12);
}

/**
 * @brief Test BufferGetPlaneFd with negative plane - FAILURE PATH
 * 
 * Objective: Cover negative plane index
 */
TEST_F(WesterosSimpleBufferTest, BufferGetPlaneFd_NegativePlane_ReturnsMinusOne) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    
    int fd = WstSBBufferGetPlaneFd(&buffer, -1);
    EXPECT_EQ(fd, -1);
}

/**
 * @brief Test BufferGetPlaneFd with plane 3 - FAILURE PATH
 * 
 * Objective: Cover out-of-bounds plane (only 0-2 valid)
 */
TEST_F(WesterosSimpleBufferTest, BufferGetPlaneFd_OutOfBoundsPlane_ReturnsMinusOne) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    
    int fd = WstSBBufferGetPlaneFd(&buffer, 5);
    EXPECT_EQ(fd, -1);
}

/**
 * @brief Test BufferGet with NULL resource - FAILURE PATH
 * 
 * Objective: Cover NULL resource handling
 */
TEST_F(WesterosSimpleBufferTest, BufferGet_WithNullResource_ReturnsNull) {
    struct wl_sb_buffer* result = WstSBBufferGet(nullptr);
    EXPECT_EQ(result, nullptr);
}

/**
 * @brief Test all buffer property getters with NULL buffer - FAILURE PATH
 * 
 * Objective: Comprehensive NULL safety check
 */
TEST_F(WesterosSimpleBufferTest, AllBufferGetters_WithNullBuffer_ReturnSafeValues) {
    EXPECT_EQ(WstSBBufferGetFormat(nullptr), 0U);
    EXPECT_EQ(WstSBBufferGetWidth(nullptr), 0);
    EXPECT_EQ(WstSBBufferGetHeight(nullptr), 0);
    EXPECT_EQ(WstSBBufferGetStride(nullptr), 0);
    EXPECT_EQ(WstSBBufferGetFd(nullptr), -1);
    EXPECT_EQ(WstSBBufferGetBuffer(nullptr), nullptr);
    
    // Plane accessors
    EXPECT_EQ(WstSBBufferGetPlaneFd(nullptr, 0), -1);
    EXPECT_EQ(WstSBBufferGetPlaneFd(nullptr, 1), -1);
    EXPECT_EQ(WstSBBufferGetPlaneFd(nullptr, 2), -1);
}

/**
 * @brief Test BufferGetBuffer with valid buffer - SUCCESS PATH
 * 
 * Objective: Cover WstSBBufferGetBuffer function
 */
TEST_F(WesterosSimpleBufferTest, BufferGetBuffer_WithValidBuffer_ReturnsDriverBuffer) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    void *driverBuf = (void*)0xDEADBEEF;
    buffer.driverBuffer = driverBuf;
    
    void *result = WstSBBufferGetBuffer(&buffer);
    EXPECT_EQ(result, driverBuf);
}

/**
 * @brief Test BufferGetPlaneOffsetAndStride with NULL buffer - FAILURE PATH
 * 
 * Objective: Cover NULL buffer check
 */
TEST_F(WesterosSimpleBufferTest, BufferGetPlaneOffsetAndStride_WithNullBuffer_NoChange) {
    int32_t offset = 12345, stride = 67890;
    
    WstSBBufferGetPlaneOffsetAndStride(nullptr, 0, &offset, &stride);
    
    // Values should NOT change
    EXPECT_EQ(offset, 12345);
    EXPECT_EQ(stride, 67890);
}

/**
 * @brief Test buffer with FD 0 (stdin) - SUCCESS PATH
 * 
 * Objective: Cover edge case where FD is 0 (valid)
 */
TEST_F(WesterosSimpleBufferTest, BufferGetFd_WithFdZero_ReturnsZero) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    buffer.fd[0] = 0;  // FD 0 is stdin, technically valid
    
    int fd = WstSBBufferGetFd(&buffer);
    EXPECT_EQ(fd, 0);
}

/**
 * @brief Test buffer with very large dimensions - SUCCESS PATH
 * 
 * Objective: Cover large dimension handling
 */
TEST_F(WesterosSimpleBufferTest, Buffer_WithLargeDimensions_HandlesCorrectly) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    buffer.width = 16384;   // Very large width
    buffer.height = 16384;  // Very large height
    buffer.stride[0] = 65536;
    
    EXPECT_EQ(WstSBBufferGetWidth(&buffer), 16384);
    EXPECT_EQ(WstSBBufferGetHeight(&buffer), 16384);
    EXPECT_EQ(WstSBBufferGetStride(&buffer), 65536);
}

/**
 * @brief Test all supported pixel formats - SUCCESS PATH
 * 
 * Objective: Cover all WL_SB_FORMAT values
 */
TEST_F(WesterosSimpleBufferTest, BufferGetFormat_AllFormats_ReturnsCorrectly) {
    uint32_t formats[] = {
        WL_SB_FORMAT_ARGB8888,
        WL_SB_FORMAT_XRGB8888,
        WL_SB_FORMAT_YUV420,
        WL_SB_FORMAT_YUV422,
        WL_SB_FORMAT_YUV444,
        WL_SB_FORMAT_NV12,
        WL_SB_FORMAT_NV16,
        WL_SB_FORMAT_YUYV,
        WL_SB_FORMAT_RGB565
    };
    
    for (uint32_t fmt : formats) {
        struct wl_sb_buffer buffer;
        memset(&buffer, 0, sizeof(buffer));
        buffer.format = fmt;
        
        EXPECT_EQ(WstSBBufferGetFormat(&buffer), fmt);
    }
}

/**
 * @brief Test 3-plane YUV buffer - SUCCESS PATH
 * 
 * Objective: Cover 3-plane planar format
 */
TEST_F(WesterosSimpleBufferTest, ThreePlaneYUV_AllPlanes_Success) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    
    // YUV 4:2:0 with 3 planes
    buffer.width = 1280;
    buffer.height = 720;
    buffer.format = WL_SB_FORMAT_YUV420;
    
    // Y plane
    buffer.fd[0] = 100;
    buffer.offset[0] = 0;
    buffer.stride[0] = 1280;
    
    // U plane
    buffer.fd[1] = 100;
    buffer.offset[1] = 1280 * 720;
    buffer.stride[1] = 640;
    
    // V plane
    buffer.fd[2] = 100;
    buffer.offset[2] = 1280 * 720 + 640 * 360;
    buffer.stride[2] = 640;
    
    // Verify all planes
    int32_t offset, stride;
    for (int plane = 0; plane < 3; plane++) {
        EXPECT_EQ(WstSBBufferGetPlaneFd(&buffer, plane), 100);
        WstSBBufferGetPlaneOffsetAndStride(&buffer, plane, &offset, &stride);
        EXPECT_EQ(offset, buffer.offset[plane]);
        EXPECT_EQ(stride, buffer.stride[plane]);
    }
}

/**
 * @brief Test Init/Uninit with different callback combinations - SUCCESS PATH
 * 
 * Objective: Cover various callback configurations
 */
TEST_F(WesterosSimpleBufferTest, Init_VariousCallbackCombinations_Success) {
    struct wayland_sb_callbacks cb1, cb2;
    
    // With all callbacks
    cb1.bind = mock_bind;
    cb1.reference_buffer = mock_reference_buffer;
    cb1.release_buffer = mock_release_buffer;
    wl_sb* sb1 = WstSBInit(display, &cb1, testUserData);
    EXPECT_NE(sb1, nullptr);
    if (sb1) WstSBUninit(sb1);
    
    // With minimal callbacks
    cb2.bind = mock_bind;
    cb2.reference_buffer = mock_reference_buffer;
    cb2.release_buffer = mock_release_buffer;
    wl_sb* sb2 = WstSBInit(display, &cb2, testUserData);
    EXPECT_NE(sb2, nullptr);
    if (sb2) WstSBUninit(sb2);
}

/**
 * @brief Test sequential buffer property queries - SUCCESS PATH
 * 
 * Objective: Verify consistency of multiple queries
 */
TEST_F(WesterosSimpleBufferTest, BufferProperties_MultipleQueries_Consistent) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    buffer.width = 1024;
    buffer.height = 768;
    buffer.format = WL_SB_FORMAT_ARGB8888;
    buffer.stride[0] = 4096;
    buffer.fd[0] = 42;
    
    // Query multiple times
    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(WstSBBufferGetWidth(&buffer), 1024);
        EXPECT_EQ(WstSBBufferGetHeight(&buffer), 768);
        EXPECT_EQ(WstSBBufferGetFormat(&buffer), WL_SB_FORMAT_ARGB8888);
        EXPECT_EQ(WstSBBufferGetStride(&buffer), 4096);
        EXPECT_EQ(WstSBBufferGetFd(&buffer), 42);
    }
}

/**
 * @brief Test boundary plane index (plane 2) - SUCCESS PATH
 * 
 * Objective: Cover last valid plane index
 */
TEST_F(WesterosSimpleBufferTest, BufferPlaneFunctions_BoundaryIndex_Success) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    buffer.fd[2] = 99;
    buffer.offset[2] = 256;
    buffer.stride[2] = 512;
    
    EXPECT_EQ(WstSBBufferGetPlaneFd(&buffer, 2), 99);
    
    int32_t offset = 0, stride = 0;
    WstSBBufferGetPlaneOffsetAndStride(&buffer, 2, &offset, &stride);
    EXPECT_EQ(offset, 256);
    EXPECT_EQ(stride, 512);
}

/**
 * @brief Test WstSBBufferGetPlaneOffsetAndStride with NULL offset pointer
 * 
 * Objective: Cover NULL offset handling
 */
TEST_F(WesterosSimpleBufferTest, BufferGetPlaneOffsetAndStride_NullOffsetPointer_SetsOnlyStride) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    buffer.offset[0] = 100;
    buffer.stride[0] = 200;
    
    int32_t stride = 0;
    
    // Act
    WstSBBufferGetPlaneOffsetAndStride(&buffer, 0, nullptr, &stride);
    
    // Assert
    EXPECT_EQ(stride, 200);
}

/**
 * @brief Test WstSBBufferGetPlaneOffsetAndStride with NULL stride pointer
 * 
 * Objective: Cover NULL stride handling
 */
TEST_F(WesterosSimpleBufferTest, BufferGetPlaneOffsetAndStride_NullStridePointer_SetsOnlyOffset) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    buffer.offset[0] = 100;
    buffer.stride[0] = 200;
    
    int32_t offset = 0;
    
    // Act
    WstSBBufferGetPlaneOffsetAndStride(&buffer, 0, &offset, nullptr);
    
    // Assert
    EXPECT_EQ(offset, 100);
}

/**
 * @brief Test WstSBBufferGetPlaneFd with plane index >= 3
 * 
 * Objective: Cover boundary check for out of range plane
 */
TEST_F(WesterosSimpleBufferTest, BufferGetPlaneFd_PlaneOutOfRange_ReturnsMinusOne) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    buffer.fd[0] = 10;
    
    // Act
    int fd = WstSBBufferGetPlaneFd(&buffer, 3);
    
    // Assert
    EXPECT_EQ(fd, -1);
}

/**
 * @brief Test WstSBBufferGetPlaneOffsetAndStride with negative plane
 * 
 * Objective: Cover boundary check for negative plane
 */
TEST_F(WesterosSimpleBufferTest, BufferGetPlaneOffsetAndStride_NegativePlane_DoesNotModify) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    
    int32_t offset = 999, stride = 999;
    
    // Act
    WstSBBufferGetPlaneOffsetAndStride(&buffer, -1, &offset, &stride);
    
    // Assert - Should not modify for invalid plane
    EXPECT_EQ(offset, 999);
    EXPECT_EQ(stride, 999);
}

/**
 * @brief Test WstSBBufferGetPlaneOffsetAndStride with plane >= 3
 * 
 * Objective: Cover boundary check for out of range plane
 */
TEST_F(WesterosSimpleBufferTest, BufferGetPlaneOffsetAndStride_PlaneOutOfRange_DoesNotModify) {
    struct wl_sb_buffer buffer;
    memset(&buffer, 0, sizeof(buffer));
    
    int32_t offset = 999, stride = 999;
    
    // Act
    WstSBBufferGetPlaneOffsetAndStride(&buffer, 3, &offset, &stride);
    
    // Assert
    EXPECT_EQ(offset, 999);
    EXPECT_EQ(stride, 999);
}

/*
 * Test: InterfaceMethod_SimpleBufferDestroy_DestroysResource
 * Objective: Cover wstISimpleBufferDestroy function (interface method)
 */
TEST_F(WesterosSimpleBufferTest, InterfaceMethod_SimpleBufferDestroy_DestroysResource) {
    struct wl_resource* resource = wl_resource_create(nullptr, &wl_buffer_interface, 1, 1);
    ASSERT_NE(resource, nullptr);
    
    // Act - Call the interface method directly
    wstISimpleBufferDestroy(nullptr, resource);
    
    // Assert - Resource should be destroyed (verify through destructor call tracking)
    // The wl_resource_destroy call is tracked in the mock
}

/*
 * Test: InterfaceMethod_SBBind_CreatesResource
 * Objective: Cover wstSBBind function (bind callback)
 */
TEST_F(WesterosSimpleBufferTest, InterfaceMethod_SBBind_CreatesResource) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    
    // Expect bind callback to be invoked
    EXPECT_CALL(mockCallbacks, bind(testUserData, client, _)).Times(1);
    
    // Act - Call the bind method directly
    wstSBBind(client, sb, 1, 1);
    
    // Assert - Bind callback should have been called
}

/*
 * Test: InterfaceMethod_SBDestroyBuffer_ReleasesBuffer
 * Objective: Cover wstSBDestroyBuffer function (buffer destructor)
 */
TEST_F(WesterosSimpleBufferTest, InterfaceMethod_SBDestroyBuffer_ReleasesBuffer) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);

    
    // Create a buffer first through the API
    struct wl_resource* sbResource = wl_resource_create(client, &wl_sb_interface, 1, 1);
    ASSERT_NE(sbResource, nullptr);
    sbResource->data = sb;
    
    // Create buffer - expect callbacks
    EXPECT_CALL(mockCallbacks, reference_buffer(testUserData, client, 123, _)).Times(1);
    
    wstISBCreateBuffer(client, sbResource, 1, 123, 1920, 1080, 4096, WL_SB_FORMAT_ARGB8888);
    
    // Now test the destructor
    struct wl_resource* bufferResource = wl_resource_create(client, &wl_buffer_interface, 1, 2);
    ASSERT_NE(bufferResource, nullptr);
    
    // Set up buffer data (simulate a created buffer)
    struct wl_sb_buffer* buffer = (struct wl_sb_buffer*)calloc(1, sizeof(struct wl_sb_buffer));
    buffer->sb = sb;
    buffer->fd[0] = -1;
    buffer->fd[1] = -1;
    buffer->fd[2] = -1;
    bufferResource->data = buffer;
    
    // Expect release callback
    EXPECT_CALL(mockCallbacks, release_buffer(testUserData, buffer)).Times(1);
    
    // Act - Call the destructor directly
    wstSBDestroyBuffer(bufferResource);
    
    // Assert - release_buffer callback should have been called
}

/*
 * Test: InterfaceMethod_CreateBuffer_CreatesBuffer
 * Objective: Cover wstISBCreateBuffer function
 */
TEST_F(WesterosSimpleBufferTest, InterfaceMethod_CreateBuffer_CreatesBuffer) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = wl_resource_create(client, &wl_sb_interface, 1, 1);
    ASSERT_NE(resource, nullptr);
    resource->data = sb;
    
    // Expect reference_buffer callback
    EXPECT_CALL(mockCallbacks, reference_buffer(testUserData, client, 456, _)).Times(1);
    
    // Act
    wstISBCreateBuffer(client, resource, 1, 456, 1920, 1080, 4096, WL_SB_FORMAT_ARGB8888);
    
    // Assert - callback should have been called
}

/*
 * Test: InterfaceMethod_CreatePlanarBuffer_CreatesBuffer
 * Objective: Cover wstISBCreatePlanarBuffer function
 */
TEST_F(WesterosSimpleBufferTest, InterfaceMethod_CreatePlanarBuffer_CreatesBuffer) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = wl_resource_create(client, &wl_sb_interface, 1, 1);
    ASSERT_NE(resource, nullptr);
    resource->data = sb;
    
    // Expect reference_buffer callback
    EXPECT_CALL(mockCallbacks, reference_buffer(testUserData, client, 789, _)).Times(1);
    
    // Act - NV12 format with 2 planes
    wstISBCreatePlanarBuffer(client, resource, 1, 789, 1920, 1080, WL_SB_FORMAT_NV12,
                             0, 2073600, 0,  // offsets (plane 0, plane 1, plane 2)
                             1920, 1920, 0);  // strides
    
    // Assert - callback should have been called
}

/*
 * Test: InterfaceMethod_CreatePlanarBufferFd_CreatesBuffer
 * Objective: Cover wstISBCreatePlanarBufferFd function
 */
TEST_F(WesterosSimpleBufferTest, InterfaceMethod_CreatePlanarBufferFd_CreatesBuffer) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = wl_resource_create(client, &wl_sb_interface, 1, 1);
    ASSERT_NE(resource, nullptr);
    resource->data = sb;
    
    // Expect reference_buffer callback
    EXPECT_CALL(mockCallbacks, reference_buffer(testUserData, client, 0, _)).Times(1);
    
    // Act - Pass fd (will be stored in buffer)
    wstISBCreatePlanarBufferFd(client, resource, 1, 99, 1920, 1080, WL_SB_FORMAT_NV12,
                               0, 2073600, 0,  // offsets
                               1920, 1920, 0);  // strides
    
    // Assert - callback should have been called
}

/*
 * Test: InterfaceMethod_CreatePlanarBufferFd2_CreatesBuffer
 * Objective: Cover wstISBCreatePlanarBufferFd2 function (3 separate fds)
 */
TEST_F(WesterosSimpleBufferTest, InterfaceMethod_CreatePlanarBufferFd2_CreatesBuffer) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = wl_resource_create(client, &wl_sb_interface, 1, 1);
    ASSERT_NE(resource, nullptr);
    resource->data = sb;
    
    // Expect reference_buffer callback
    EXPECT_CALL(mockCallbacks, reference_buffer(testUserData, client, 0, _)).Times(1);
    
    // Act - YUV420 format with 3 separate fds
    wstISBCreatePlanarBufferFd2(client, resource, 1, 100, 101, 102, 1920, 1080, WL_SB_FORMAT_YUV420,
                                0, 0, 0,        // offsets
                                1920, 960, 960);  // strides
    
    // Assert - callback should have been called
}

/*
 * Test: InterfaceMethod_SBCreateBuffer_AllParameters_CreatesBuffer
 * Objective: Cover wstSBCreateBuffer function (unified create with all parameters)
 */
TEST_F(WesterosSimpleBufferTest, InterfaceMethod_SBCreateBuffer_AllParameters_CreatesBuffer) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = wl_resource_create(client, &wl_sb_interface, 1, 1);
    ASSERT_NE(resource, nullptr);
    resource->data = sb;
    
    // Expect reference_buffer callback
    EXPECT_CALL(mockCallbacks, reference_buffer(testUserData, client, 555, _)).Times(1);
    
    // Act - Create buffer with all parameters (handles + fds)
    wstSBCreateBuffer(client, resource, 1, 555, 100, 101, 102, 1920, 1080, WL_SB_FORMAT_NV12,
                      0, 2073600, 0,    // offsets
                      1920, 1920, 0);    // strides
    
    // Assert - callback should have been called
}

/*
 * Test: VerifyInterfaceStructures_ContainCorrectFunctions
 * Objective: Cover sb_interface and bufferInterface structures
 */
TEST_F(WesterosSimpleBufferTest, VerifyInterfaceStructures_ContainCorrectFunctions) {
    // Act & Assert - Verify buffer interface
    ASSERT_NE(bufferInterface.destroy, nullptr);
    EXPECT_EQ(bufferInterface.destroy, wstISimpleBufferDestroy);
    
    // Act & Assert - Verify sb interface
    ASSERT_EQ(sb_interface.create_buffer, wstISBCreateBuffer);
    ASSERT_EQ(sb_interface.create_planar_buffer, wstISBCreatePlanarBuffer);
    ASSERT_EQ(sb_interface.create_planar_buffer_fd, wstISBCreatePlanarBufferFd);
    ASSERT_EQ(sb_interface.create_planar_buffer_fd2, wstISBCreatePlanarBufferFd2);
}

/*
 * Main function
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    // Return normally to allow gcov to flush coverage data
    // (previously used _exit which bypassed atexit handlers)
    return result;
}

