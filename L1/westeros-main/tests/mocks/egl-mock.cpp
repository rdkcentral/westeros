/*
 * EGL Mock Implementation
 * Copyright 2024 RDK Management
 */
#include <EGL/egl.h>
#include <cstddef>

// EGL Mock implementations
EGLDisplay eglGetDisplay(EGLNativeDisplayType display_id) {
    (void)display_id;
    return (EGLDisplay)0x12345678; // Mock display handle
}

__eglMustCastToProperFunctionPointerType eglGetProcAddress(const char *procname) {
    (void)procname;
    return nullptr; // Mock returns null for all proc addresses
}

EGLBoolean eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor) {
    (void)dpy;
    if (major) *major = 1;
    if (minor) *minor = 4;
    return EGL_TRUE;
}

EGLBoolean eglTerminate(EGLDisplay dpy) {
    (void)dpy;
    return EGL_TRUE;
}

EGLBoolean eglBindAPI(EGLenum api) {
    (void)api;
    return EGL_TRUE;
}

EGLBoolean eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config) {
    (void)dpy;
    (void)attrib_list;
    (void)configs;
    (void)config_size;
    if (num_config) *num_config = 0;
    return EGL_TRUE;
}

EGLContext eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list) {
    (void)dpy;
    (void)config;
    (void)share_context;
    (void)attrib_list;
    return (EGLContext)0x87654321;
}

EGLBoolean eglDestroyContext(EGLDisplay dpy, EGLContext ctx) {
    (void)dpy;
    (void)ctx;
    return EGL_TRUE;
}

EGLBoolean eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx) {
    (void)dpy;
    (void)draw;
    (void)read;
    (void)ctx;
    return EGL_TRUE;
}

EGLBoolean eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    (void)dpy;
    (void)surface;
    return EGL_TRUE;
}
