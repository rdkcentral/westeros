/*
 * GLUT Mock Header for L1 Testing
 * Copyright 2024 RDK Management
 */
#ifndef __GLUT_H__
#define __GLUT_H__

#include "glew.h"

#ifdef __cplusplus
extern "C" {
#endif

// GLUT display mode bit masks
#define GLUT_RGB 0x0000
#define GLUT_RGBA 0x0000
#define GLUT_INDEX 0x0001
#define GLUT_SINGLE 0x0000
#define GLUT_DOUBLE 0x0002
#define GLUT_ACCUM 0x0004
#define GLUT_ALPHA 0x0008
#define GLUT_DEPTH 0x0010
#define GLUT_STENCIL 0x0020
#define GLUT_MULTISAMPLE 0x0080
#define GLUT_STEREO 0x0100
#define GLUT_LUMINANCE 0x0200

// Mouse buttons
#define GLUT_LEFT_BUTTON 0
#define GLUT_MIDDLE_BUTTON 1
#define GLUT_RIGHT_BUTTON 2

// Mouse button state
#define GLUT_DOWN 0
#define GLUT_UP 1

// Special keys
#define GLUT_KEY_F1 1
#define GLUT_KEY_F2 2
#define GLUT_KEY_F3 3
#define GLUT_KEY_F4 4
#define GLUT_KEY_F5 5
#define GLUT_KEY_F6 6
#define GLUT_KEY_F7 7
#define GLUT_KEY_F8 8
#define GLUT_KEY_F9 9
#define GLUT_KEY_F10 10
#define GLUT_KEY_F11 11
#define GLUT_KEY_F12 12
#define GLUT_KEY_LEFT 100
#define GLUT_KEY_UP 101
#define GLUT_KEY_RIGHT 102
#define GLUT_KEY_DOWN 103
#define GLUT_KEY_PAGE_UP 104
#define GLUT_KEY_PAGE_DOWN 105
#define GLUT_KEY_HOME 106
#define GLUT_KEY_END 107
#define GLUT_KEY_INSERT 108

// Entry/exit state
#define GLUT_LEFT 0
#define GLUT_ENTERED 1

// Menu usage state
#define GLUT_MENU_NOT_IN_USE 0
#define GLUT_MENU_IN_USE 1

// Visibility state
#define GLUT_NOT_VISIBLE 0
#define GLUT_VISIBLE 1

// Window status state
#define GLUT_HIDDEN 0
#define GLUT_FULLY_RETAINED 1
#define GLUT_PARTIALLY_RETAINED 2
#define GLUT_FULLY_COVERED 3

// Keyboard modifiers
#define GLUT_ACTIVE_SHIFT 0x0001
#define GLUT_ACTIVE_CTRL 0x0002
#define GLUT_ACTIVE_ALT 0x0004

// GLUT API functions (stubs)
void glutInit(int *argcp, char **argv);
void glutInitDisplayMode(unsigned int mode);
void glutInitWindowPosition(int x, int y);
void glutInitWindowSize(int width, int height);
int glutCreateWindow(const char *title);
void glutDestroyWindow(int win);
void glutPostRedisplay(void);
void glutSwapBuffers(void);
int glutGetWindow(void);
void glutSetWindow(int win);
void glutSetWindowTitle(const char *title);
void glutSetIconTitle(const char *title);
void glutPositionWindow(int x, int y);
void glutReshapeWindow(int width, int height);
void glutPopWindow(void);
void glutPushWindow(void);
void glutIconifyWindow(void);
void glutShowWindow(void);
void glutHideWindow(void);
void glutFullScreen(void);

void glutMainLoop(void);

// Callback registration
typedef void (*glutDisplayFunc_t)(void);
typedef void (*glutReshapeFunc_t)(int width, int height);
typedef void (*glutKeyboardFunc_t)(unsigned char key, int x, int y);
typedef void (*glutMouseFunc_t)(int button, int state, int x, int y);
typedef void (*glutMotionFunc_t)(int x, int y);
typedef void (*glutPassiveMotionFunc_t)(int x, int y);
typedef void (*glutIdleFunc_t)(void);
typedef void (*glutTimerFunc_t)(int value);
typedef void (*glutSpecialFunc_t)(int key, int x, int y);

void glutDisplayFunc(glutDisplayFunc_t func);
void glutReshapeFunc(glutReshapeFunc_t func);
void glutKeyboardFunc(glutKeyboardFunc_t func);
void glutMouseFunc(glutMouseFunc_t func);
void glutMotionFunc(glutMotionFunc_t func);
void glutPassiveMotionFunc(glutPassiveMotionFunc_t func);
void glutIdleFunc(glutIdleFunc_t func);
void glutTimerFunc(unsigned int millis, glutTimerFunc_t func, int value);
void glutSpecialFunc(glutSpecialFunc_t func);

// State queries
int glutGet(GLenum type);
int glutDeviceGet(GLenum type);
int glutGetModifiers(void);
int glutLayerGet(GLenum type);

// Font rendering
void glutBitmapCharacter(void *font, int character);
int glutBitmapWidth(void *font, int character);
void glutStrokeCharacter(void *font, int character);
int glutStrokeWidth(void *font, int character);
int glutBitmapLength(void *font, const unsigned char *string);
int glutStrokeLength(void *font, const unsigned char *string);

// Geometric object rendering
void glutWireSphere(GLdouble radius, GLint slices, GLint stacks);
void glutSolidSphere(GLdouble radius, GLint slices, GLint stacks);
void glutWireCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
void glutSolidCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
void glutWireCube(GLdouble size);
void glutSolidCube(GLdouble size);
void glutWireTorus(GLdouble innerRadius, GLdouble outerRadius, GLint sides, GLint rings);
void glutSolidTorus(GLdouble innerRadius, GLdouble outerRadius, GLint sides, GLint rings);

#ifdef __cplusplus
}
#endif

#endif /* __GLUT_H__ */
