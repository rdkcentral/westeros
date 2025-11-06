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
#include <memory>
#include <cstring>

// Include mocks before the actual headers
#include "wayland-server.h"
#include "simpleshell/simplebuffer-server-protocol.h"

// Include the header under test
#include "westeros-simplebuffer.h"

// Extern declarations for UNIT_TEST access to internal functions
#ifdef UNIT_TEST
extern void wstISimpleBufferDestroy(struct wl_client *client, struct wl_resource *resource);
extern void wstSBDestroyBuffer(struct wl_resource *resource);
extern void wstSBBind(struct wl_client *client, void *data, uint32_t version, uint32_t id);
extern void wstISBCreateBuffer(struct wl_client *client, struct wl_resource *resource,
                                uint32_t id, uint32_t native_handle, int32_t width, int32_t height,
                                uint32_t stride, uint32_t format);
extern void wstISBCreatePlanarBuffer(struct wl_client *client,
                                     struct wl_resource *resource,
                                     uint32_t id, uint32_t native_handle,
                                     int32_t width, int32_t height, uint32_t format,
                                     int32_t offset0, int32_t offset1, int32_t offset2,
                                     int32_t stride0, int32_t stride1, int32_t stride2);
extern void wstISBCreatePlanarBufferFd(struct wl_client *client,
                                       struct wl_resource *resource,
                                       uint32_t id, int32_t fd,
                                       int32_t width, int32_t height, uint32_t format,
                                       int32_t offset0, int32_t offset1, int32_t offset2,
                                       int32_t stride0, int32_t stride1, int32_t stride2);
extern void wstISBCreatePlanarBufferFd2(struct wl_client *client,
                                        struct wl_resource *resource,
                                        uint32_t id, int32_t fd0, int32_t fd1, int32_t fd2,
                                        int32_t width, int32_t height, uint32_t format,
                                        int32_t offset0, int32_t offset1, int32_t offset2,
                                        int32_t stride0, int32_t stride1, int32_t stride2);
extern void wstSBCreateBuffer(struct wl_client *client, struct wl_resource *resource,
                               uint32_t id, uint32_t native_handle, int32_t fd0, int32_t fd1, int32_t fd2,
                               int32_t w, int32_t h, uint32_t fmt, int32_t off0, int32_t off1, int32_t off2,
                               int32_t strd0, int32_t strd1, int32_t strd2);
extern const struct wl_buffer_interface bufferInterface;
extern const struct wl_sb_interface sb_interface;
#endif

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;
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
    StrictMock<MockSimpleBufferCallbacks> mockCallbacks;
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
    // These functions should handle null buffers gracefully
    // Note: The actual implementation may crash, but this documents the behavior
    
    // Test format access with null buffer
    EXPECT_DEATH_IF_SUPPORTED(WstSBBufferGetFormat(nullptr), ".*");
    
    // Test dimension access with null buffer
    EXPECT_DEATH_IF_SUPPORTED(WstSBBufferGetWidth(nullptr), ".*");
    EXPECT_DEATH_IF_SUPPORTED(WstSBBufferGetHeight(nullptr), ".*");
    
    // Test stride access with null buffer
    EXPECT_DEATH_IF_SUPPORTED(WstSBBufferGetStride(nullptr), ".*");
    
    // Test buffer pointer access with null buffer
    EXPECT_DEATH_IF_SUPPORTED(WstSBBufferGetBuffer(nullptr), ".*");
    
    // Test fd access with null buffer
    EXPECT_DEATH_IF_SUPPORTED(WstSBBufferGetFd(nullptr), ".*");
    EXPECT_DEATH_IF_SUPPORTED(WstSBBufferGetPlaneFd(nullptr, 0), ".*");
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

#ifdef UNIT_TEST
// Internal function tests
TEST_F(WesterosSimpleBufferTest, InternalBufferInterfaceExists) {
    // Verify the buffer interface structure is accessible
    EXPECT_NE(bufferInterface.destroy, nullptr);
    EXPECT_EQ(bufferInterface.destroy, wstISimpleBufferDestroy);
}

TEST_F(WesterosSimpleBufferTest, InternalSBInterfaceExists) {
    // Verify the sb_interface structure is accessible
    EXPECT_NE(sb_interface.create_buffer, nullptr);
    EXPECT_NE(sb_interface.create_planar_buffer, nullptr);
    EXPECT_NE(sb_interface.create_planar_buffer_fd, nullptr);
    EXPECT_NE(sb_interface.create_planar_buffer_fd2, nullptr);
    EXPECT_EQ(sb_interface.create_buffer, wstISBCreateBuffer);
    EXPECT_EQ(sb_interface.create_planar_buffer, wstISBCreatePlanarBuffer);
    EXPECT_EQ(sb_interface.create_planar_buffer_fd, wstISBCreatePlanarBufferFd);
    EXPECT_EQ(sb_interface.create_planar_buffer_fd2, wstISBCreatePlanarBufferFd2);
}

TEST_F(WesterosSimpleBufferTest, InternalSimpleBufferDestroy) {
    // Create a test resource
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = wl_resource_create(client, &wl_buffer_interface, 1, 1001);
    ASSERT_NE(resource, nullptr);

    // Call the internal destroy function
    wstISimpleBufferDestroy(client, resource);
    
    // Resource should be destroyed (can't verify directly, but no crash is success)
}

TEST_F(WesterosSimpleBufferTest, InternalSBDestroyBuffer) {
    // Create a mock buffer with all required fields
    struct wl_sb_buffer* buffer = (struct wl_sb_buffer*)calloc(1, sizeof(struct wl_sb_buffer));
    ASSERT_NE(buffer, nullptr);
    
    buffer->sb = sb;
    buffer->fd[0] = -1;  // No fd to close
    buffer->fd[1] = -1;
    buffer->fd[2] = -1;
    
    // Create a resource for the buffer
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = wl_resource_create(client, &wl_buffer_interface, 1, 1002);
    ASSERT_NE(resource, nullptr);
    resource->data = buffer;

    // Expect the release callback to be called
    EXPECT_CALL(mockCallbacks, release_buffer(testUserData, buffer)).Times(1);
    
    // Call internal destroy function
    wstSBDestroyBuffer(resource);
    
    // Buffer is freed by the function, so we don't clean it up
}

TEST_F(WesterosSimpleBufferTest, InternalSBBind) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    
    // Expect bind callback to be called
    EXPECT_CALL(mockCallbacks, bind(testUserData, client, NotNull())).Times(1);
    
    // Call internal bind function
    wstSBBind(client, sb, 1, 2001);
    
    // Successful bind should create a resource and call the callback
}

TEST_F(WesterosSimpleBufferTest, InternalISBCreateBufferARGB8888) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = wl_resource_create(client, &wl_sb_interface, 1, 3001);
    ASSERT_NE(resource, nullptr);
    
    // Set resource data to point to sb (required by wstSBCreateBuffer)
    resource->data = sb;

    // Expect reference_buffer callback to be called
    EXPECT_CALL(mockCallbacks, reference_buffer(testUserData, client, 0xABCDEF, NotNull())).Times(1);
    
    // Call internal create buffer for ARGB8888
    wstISBCreateBuffer(client, resource, 4001, 0xABCDEF, 1920, 1080, 7680, WL_SB_FORMAT_ARGB8888);
}

TEST_F(WesterosSimpleBufferTest, InternalISBCreateBufferXRGB8888) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = wl_resource_create(client, &wl_sb_interface, 1, 3002);
    ASSERT_NE(resource, nullptr);
    
    // Set resource data to point to sb
    resource->data = sb;

    // Expect reference_buffer callback
    EXPECT_CALL(mockCallbacks, reference_buffer(testUserData, client, 0x123456, NotNull())).Times(1);
    
    // Call internal create buffer for XRGB8888
    wstISBCreateBuffer(client, resource, 4002, 0x123456, 1280, 720, 5120, WL_SB_FORMAT_XRGB8888);
}

TEST_F(WesterosSimpleBufferTest, InternalISBCreateBufferYUYV) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = wl_resource_create(client, &wl_sb_interface, 1, 3003);
    ASSERT_NE(resource, nullptr);
    
    // Set resource data to point to sb
    resource->data = sb;

    // Expect reference_buffer callback
    EXPECT_CALL(mockCallbacks, reference_buffer(testUserData, client, 0x789ABC, NotNull())).Times(1);
    
    // Call internal create buffer for YUYV
    wstISBCreateBuffer(client, resource, 4003, 0x789ABC, 640, 480, 1280, WL_SB_FORMAT_YUYV);
}

TEST_F(WesterosSimpleBufferTest, InternalISBCreateBufferRGB565) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = wl_resource_create(client, &wl_sb_interface, 1, 3004);
    ASSERT_NE(resource, nullptr);
    
    // Set resource data to point to sb
    resource->data = sb;

    // Expect reference_buffer callback
    EXPECT_CALL(mockCallbacks, reference_buffer(testUserData, client, 0xDEF123, NotNull())).Times(1);
    
    // Call internal create buffer for RGB565
    wstISBCreateBuffer(client, resource, 4004, 0xDEF123, 800, 600, 1600, WL_SB_FORMAT_RGB565);
}

TEST_F(WesterosSimpleBufferTest, InternalISBCreatePlanarBufferYUV420) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = wl_resource_create(client, &wl_sb_interface, 1, 3005);
    ASSERT_NE(resource, nullptr);
    
    // Set resource data to point to sb
    resource->data = sb;

    // Expect reference_buffer callback
    EXPECT_CALL(mockCallbacks, reference_buffer(testUserData, client, 0xABCDEF, NotNull())).Times(1);
    
    // Call internal create planar buffer for YUV420
    wstISBCreatePlanarBuffer(client, resource, 4005, 0xABCDEF, 1920, 1080, WL_SB_FORMAT_YUV420,
                             0, 100, 200, 1920, 960, 960);
}

TEST_F(WesterosSimpleBufferTest, InternalISBCreatePlanarBufferNV12) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = wl_resource_create(client, &wl_sb_interface, 1, 3006);
    ASSERT_NE(resource, nullptr);
    
    // Set resource data to point to sb
    resource->data = sb;

    // Expect reference_buffer callback
    EXPECT_CALL(mockCallbacks, reference_buffer(testUserData, client, 0x123456, NotNull())).Times(1);
    
    // Call internal create planar buffer for NV12
    wstISBCreatePlanarBuffer(client, resource, 4006, 0x123456, 1280, 720, WL_SB_FORMAT_NV12,
                             0, 50, 100, 1280, 640, 640);
}

TEST_F(WesterosSimpleBufferTest, InternalISBCreatePlanarBufferFdARGB8888) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = wl_resource_create(client, &wl_sb_interface, 1, 3007);
    ASSERT_NE(resource, nullptr);
    
    // Set resource data to point to sb
    resource->data = sb;

    // Expect reference_buffer callback (no native_handle, so 0)
    EXPECT_CALL(mockCallbacks, reference_buffer(testUserData, client, 0, NotNull())).Times(1);
    
    // Call internal create planar buffer fd for ARGB8888
    wstISBCreatePlanarBufferFd(client, resource, 4007, 10, 1920, 1080, WL_SB_FORMAT_ARGB8888,
                               0, 100, 200, 7680, 3840, 1920);
}

TEST_F(WesterosSimpleBufferTest, InternalISBCreatePlanarBufferFdYUV420) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = wl_resource_create(client, &wl_sb_interface, 1, 3008);
    ASSERT_NE(resource, nullptr);
    
    // Set resource data to point to sb
    resource->data = sb;

    // Expect reference_buffer callback
    EXPECT_CALL(mockCallbacks, reference_buffer(testUserData, client, 0, NotNull())).Times(1);
    
    // Call internal create planar buffer fd for YUV420
    wstISBCreatePlanarBufferFd(client, resource, 4008, 15, 1280, 720, WL_SB_FORMAT_YUV420,
                               0, 50, 100, 1280, 640, 640);
}

TEST_F(WesterosSimpleBufferTest, InternalISBCreatePlanarBufferFd2ARGB8888) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = wl_resource_create(client, &wl_sb_interface, 1, 3009);
    ASSERT_NE(resource, nullptr);
    
    // Set resource data to point to sb
    resource->data = sb;

    // Expect reference_buffer callback
    EXPECT_CALL(mockCallbacks, reference_buffer(testUserData, client, 0, NotNull())).Times(1);
    
    // Call internal create planar buffer fd2 for ARGB8888
    wstISBCreatePlanarBufferFd2(client, resource, 4009, 10, 11, 12, 640, 480, WL_SB_FORMAT_ARGB8888,
                                0, 100, 200, 2560, 1280, 640);
}

TEST_F(WesterosSimpleBufferTest, InternalISBCreatePlanarBufferFd2YUV422) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = wl_resource_create(client, &wl_sb_interface, 1, 3010);
    ASSERT_NE(resource, nullptr);
    
    // Set resource data to point to sb
    resource->data = sb;

    // Expect reference_buffer callback
    EXPECT_CALL(mockCallbacks, reference_buffer(testUserData, client, 0, NotNull())).Times(1);
    
    // Call internal create planar buffer fd2 for YUV422
    wstISBCreatePlanarBufferFd2(client, resource, 4010, 20, 21, 22, 800, 600, WL_SB_FORMAT_YUV422,
                                0, 50, 100, 800, 400, 400);
}

TEST_F(WesterosSimpleBufferTest, InternalSBCreateBufferWithNativeHandle) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = wl_resource_create(client, &wl_sb_interface, 1, 3011);
    ASSERT_NE(resource, nullptr);
    
    // Set resource data to point to sb
    resource->data = sb;

    // Expect reference_buffer callback with native handle
    EXPECT_CALL(mockCallbacks, reference_buffer(testUserData, client, 0xCAFEBABE, NotNull())).Times(1);
    
    // Call internal create buffer with native handle
    wstSBCreateBuffer(client, resource, 4011, 0xCAFEBABE, -1, -1, -1, 1920, 1080,
                      WL_SB_FORMAT_ARGB8888, 0, 0, 0, 7680, 0, 0);
}

TEST_F(WesterosSimpleBufferTest, InternalSBCreateBufferWithFds) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = wl_resource_create(client, &wl_sb_interface, 1, 3012);
    ASSERT_NE(resource, nullptr);
    
    // Set resource data to point to sb
    resource->data = sb;

    // Expect reference_buffer callback (no native handle when fds are used)
    EXPECT_CALL(mockCallbacks, reference_buffer(testUserData, client, 0, NotNull())).Times(1);
    
    // Call internal create buffer with file descriptors
    wstSBCreateBuffer(client, resource, 4012, 0, 10, 11, 12, 1280, 720,
                      WL_SB_FORMAT_YUV420, 0, 100, 200, 1280, 640, 640);
}

TEST_F(WesterosSimpleBufferTest, InternalSBCreateBufferMultiplePlanes) {
    struct wl_client* client = reinterpret_cast<struct wl_client*>(0x1000);
    struct wl_resource* resource = wl_resource_create(client, &wl_sb_interface, 1, 3013);
    ASSERT_NE(resource, nullptr);
    
    // Set resource data to point to sb
    resource->data = sb;

    // Expect reference_buffer callback
    EXPECT_CALL(mockCallbacks, reference_buffer(testUserData, client, 0, NotNull())).Times(1);
    
    // Call internal create buffer with all three planes
    wstSBCreateBuffer(client, resource, 4013, 0, 30, 31, 32, 640, 480,
                      WL_SB_FORMAT_YUV444, 0, 307200, 614400, 640, 640, 640);
}
#endif // UNIT_TEST

/*
 * Main function
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}