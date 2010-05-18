/**
 @file glcalls.h

 Maps OpenGL calls to G3D data types

 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2002-08-07
 @edited  2006-05-05

 Copyright 2002-2006, Morgan McGuire.
 All rights reserved.
*/

#ifndef G3D_GLCALLS_H
#define G3D_GLCALLS_H

#include "G3D/platform.h"
#include "G3D/Vector2.h"
#include "G3D/Vector3.h"
#include "G3D/Vector3int16.h"
#include "G3D/Vector4.h"
#include "G3D/Color3.h"
#include "G3D/Color3uint8.h"
#include "G3D/Color4.h"
#include "G3D/Color4uint8.h"
#include "G3D/debugAssert.h"
#include "GLG3D/glheaders.h"
#ifdef _DEBUG
// Needed for debugAssertGLOk
#   include "GLG3D/getOpenGLState.h"
#endif

namespace G3D {

class CoordinateFrame;
class Matrix3;
class Matrix4;


/**
 Produces a debugAssert that no OpenGL error has been produced.
 */
#ifdef _DEBUG
#   define debugAssertGLOk() {GLenum e = glGetError(); debugAssertM(e == GL_NO_ERROR, G3D::GLenumToString(e));}
#else
#   define debugAssertGLOk()
#endif

/**
 A functional version of glGetIntegerv
 */
GLint glGetInteger(GLenum which);

/**
 A functional version of glGetFloatv
 */
GLfloat glGetFloat(GLenum which);

/**
 A functional version of glGetDoublev
 */
GLboolean glGetBoolean(GLenum which);

/**
 A functional version of glGetDoublev
 */
GLdouble glGetDouble(GLenum which);

/** Safe to call even if the glGet would return more than 2 values */
Vector2 glGetVector2(GLenum which);
Vector3 glGetVector3(GLenum which);
Vector4 glGetVector4(GLenum which);

#ifdef G3D_WIN32

    /**
     OpenGL context (argument to glMakeCurrent, return value of glGetCurrentContext).
     <B>BETA API-- subject to change</B>
     */
    typedef HGLRC GLContext;

    /**
     Returns the current OpenGL context if more than one is in use.
     <B>BETA API-- subject to change</B>
     */
    inline GLContext glGetCurrentContext() {
        return wglGetCurrentContext();
    }

#elif defined(G3D_LINUX) || defined(G3D_FREEBSD)

    typedef GLXContext GLContext;
    inline GLContext glGetCurrentContext() {
        return glXGetCurrentContext();
    }

#elif defined(G3D_OSX)

    typedef CGLContextObj GLContext;
    inline GLContext glGetCurrentContext() {
        return CGLGetCurrentContext();
    }
#endif


inline void glMultiTexCoord(GLint unit, float v) {
	glMultiTexCoord1fARB(unit, v);
}


inline void glMultiTexCoord(GLint unit, const G3D::Vector2& v) {
	glMultiTexCoord2fvARB(unit, (const float*)&v);
}


inline void glMultiTexCoord(GLint unit, const G3D::Vector2int16& v) {
	glMultiTexCoord(unit, Vector2(v.x, v.y));
}


inline void glMultiTexCoord(GLint unit, const G3D::Vector3& v) {
	glMultiTexCoord3fvARB(unit, (const float*)&v);
}


inline void glMultiTexCoord(GLint unit, const G3D::Vector3int16& v) {
	glMultiTexCoord(unit, Vector3(v.x, v.y, v.z));
}


inline void glMultiTexCoord(GLint unit, const G3D::Vector4& v) {
	glMultiTexCoord4fvARB(unit, (const float*)&v);
}


inline void glVertex(const G3D::Vector2& v) {
	glVertex2fv((const float*)&v);
}


inline void glVertex(const G3D::Vector2int16& v) {
    glVertex2i(v.x, v.y);
}


inline void glVertex(const G3D::Vector3& v) {
	glVertex3fv((const float*)&v);
}


inline void glVertex(const G3D::Vector3int16& v) {
	glVertex3i(v.x, v.y, v.z);
}


inline void glVertex(const G3D::Vector4& v) {
	glVertex4fv((const float*)&v);
}


inline void glColor(const G3D::Color3 &c) {
	glColor3fv((const float*)&c);
}


inline void glColor(const G3D::Color4& c) {
	glColor4fv((const float*)&c);
}


inline void glColor(float r, float g, float b, float a) {
	glColor4f(r, g, b, a);
}


inline void glColor(float r, float g, float b) {
	glColor3f(r, g, b);
}


inline void glNormal(const G3D::Vector3& n) {
	glNormal3fv((const float*)&n);
}


inline void glTexCoord(const G3D::Vector4& t) {
	glTexCoord4fv((const float*)&t);
}


inline void glTexCoord(const G3D::Vector3& t) {
	glTexCoord3fv((const float*)&t);
}


inline void glTexCoord(const G3D::Vector2& t) {
	glTexCoord2fv((const float*)&t);
}


inline void glTexCoord(const float t) {
	glTexCoord1f(t);
}


/**
 Loads a coordinate frame into the current OpenGL matrix slot.
 */
void glLoadMatrix(const CoordinateFrame& cf);

void glLoadMatrix(const Matrix4& m);

void glGetMatrix(GLenum name, Matrix4& m);

Matrix4 glGetMatrix(GLenum name);

/**
 Loads the inverse of a coordinate frame into the current OpenGL matrix slot.
 */
void glLoadInvMatrix(const CoordinateFrame& cf);

/**
 Multiplies the current GL matrix slot by the inverse of a matrix.
 */
void glMultInvMatrix(const CoordinateFrame& cf);

/**
 Multiplies the current GL matrix slot by this matrix.
 */
void glMultMatrix(const CoordinateFrame& cf);

/** Platform independent version of 
    wglGetProcAddress/glXGetProcAddress/NSGLGetProcAddress */
void* glGetProcAddress(const char * name);

/**
 Takes an object space point to screen space using the current MODELVIEW and
 PROJECTION matrices. The resulting xy values are in <B>pixels</B>, the z 
 value is on the glDepthRange scale, and the w value contains rhw (-1/z for
 camera space z), which is useful for scaling line and point size.
 */
Vector4 glToScreen(const Vector4& v);

/** Calls glDisable() for all supported texture types: 1D, 2D, 3D,
    cube, rectangle. */
void glDisableAllTextures();


} // namespace

#endif
