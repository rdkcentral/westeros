/*
 * FreeGLUT Mock Header for L1 Testing
 * Copyright 2024 RDK Management
 */
#ifndef __FREEGLUT_H__
#define __FREEGLUT_H__

#include "glut.h"

#ifdef __cplusplus
extern "C" {
#endif

// FreeGLUT extensions
#define GLUT_ACTION_ON_WINDOW_CLOSE 0x01F9
#define GLUT_ACTION_EXIT 0
#define GLUT_ACTION_GLUTMAINLOOP_RETURNS 1
#define GLUT_ACTION_CONTINUE_EXECUTION 2

#define GLUT_WINDOW_BORDER_WIDTH 0x01FA
#define GLUT_WINDOW_HEADER_HEIGHT 0x01FB

// Rendering context options
#define GLUT_RENDERING_CONTEXT 0x01FC
#define GLUT_USE_CURRENT_CONTEXT 0x01FD

// FreeGLUT specific functions
void glutMainLoopEvent(void);
void glutLeaveMainLoop(void);
void glutExit(void);
void glutSetOption(GLenum option_flag, int value);

// Additional callbacks
typedef void (*glutWMCloseFunc_t)(void);
void glutWMCloseFunc(glutWMCloseFunc_t func);

#ifdef __cplusplus
}
#endif

#endif /* __FREEGLUT_H__ */
