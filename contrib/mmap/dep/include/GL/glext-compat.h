/**
   This file defines OpenGL entry points that are needed for pre-2.0
   OpenGL; this is primarily needed on Windows where the system OpenGL
   version is very old.
 */

#ifndef GLEXT_COMPAT_H
#define GLEXT_COMPAT_H

#ifndef GL_STENCIL_INDEX_EXT
#define GL_STENCIL_INDEX_EXT              0x8D45
#endif


#ifdef GL_ARB_multitexture
#undef GL_ARB_multitexture
#endif

#ifndef GL_ARB_multitexture
#define GL_ARB_multitexture 1
#ifdef GL_GLEXT_PROTOTYPES
extern void APIENTRY glActiveTextureARB (GLenum);
extern void APIENTRY glClientActiveTextureARB (GLenum);
extern void APIENTRY glMultiTexCoord1dARB (GLenum, GLdouble);
extern void APIENTRY glMultiTexCoord1dvARB (GLenum, const GLdouble *);
extern void APIENTRY glMultiTexCoord1fARB (GLenum, GLfloat);
extern void APIENTRY glMultiTexCoord1fvARB (GLenum, const GLfloat *);
extern void APIENTRY glMultiTexCoord1iARB (GLenum, GLint);
extern void APIENTRY glMultiTexCoord1ivARB (GLenum, const GLint *);
extern void APIENTRY glMultiTexCoord1sARB (GLenum, GLshort);
extern void APIENTRY glMultiTexCoord1svARB (GLenum, const GLshort *);
extern void APIENTRY glMultiTexCoord2dARB (GLenum, GLdouble, GLdouble);
extern void APIENTRY glMultiTexCoord2dvARB (GLenum, const GLdouble *);
extern void APIENTRY glMultiTexCoord2fARB (GLenum, GLfloat, GLfloat);
extern void APIENTRY glMultiTexCoord2fvARB (GLenum, const GLfloat *);
extern void APIENTRY glMultiTexCoord2iARB (GLenum, GLint, GLint);
extern void APIENTRY glMultiTexCoord2ivARB (GLenum, const GLint *);
extern void APIENTRY glMultiTexCoord2sARB (GLenum, GLshort, GLshort);
extern void APIENTRY glMultiTexCoord2svARB (GLenum, const GLshort *);
extern void APIENTRY glMultiTexCoord3dARB (GLenum, GLdouble, GLdouble, GLdouble);
extern void APIENTRY glMultiTexCoord3dvARB (GLenum, const GLdouble *);
extern void APIENTRY glMultiTexCoord3fARB (GLenum, GLfloat, GLfloat, GLfloat);
extern void APIENTRY glMultiTexCoord3fvARB (GLenum, const GLfloat *);
extern void APIENTRY glMultiTexCoord3iARB (GLenum, GLint, GLint, GLint);
extern void APIENTRY glMultiTexCoord3ivARB (GLenum, const GLint *);
extern void APIENTRY glMultiTexCoord3sARB (GLenum, GLshort, GLshort, GLshort);
extern void APIENTRY glMultiTexCoord3svARB (GLenum, const GLshort *);
extern void APIENTRY glMultiTexCoord4dARB (GLenum, GLdouble, GLdouble, GLdouble, GLdouble);
extern void APIENTRY glMultiTexCoord4dvARB (GLenum, const GLdouble *);
extern void APIENTRY glMultiTexCoord4fARB (GLenum, GLfloat, GLfloat, GLfloat, GLfloat);
extern void APIENTRY glMultiTexCoord4fvARB (GLenum, const GLfloat *);
extern void APIENTRY glMultiTexCoord4iARB (GLenum, GLint, GLint, GLint, GLint);
extern void APIENTRY glMultiTexCoord4ivARB (GLenum, const GLint *);
extern void APIENTRY glMultiTexCoord4sARB (GLenum, GLshort, GLshort, GLshort, GLshort);
extern void APIENTRY glMultiTexCoord4svARB (GLenum, const GLshort *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum texture);
typedef void (APIENTRY * PFNGLCLIENTACTIVETEXTUREARBPROC) (GLenum texture);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DARBPROC) (GLenum target, GLdouble s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FARBPROC) (GLenum target, GLfloat s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1IARBPROC) (GLenum target, GLint s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SARBPROC) (GLenum target, GLshort s);
typedef void (APIENTRY * PFNGLMULTITEXCOORD1SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DARBPROC) (GLenum target, GLdouble s, GLdouble t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2IARBPROC) (GLenum target, GLint s, GLint t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SARBPROC) (GLenum target, GLshort s, GLshort t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DARBPROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FARBPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3IARBPROC) (GLenum target, GLint s, GLint t, GLint r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SARBPROC) (GLenum target, GLshort s, GLshort t, GLshort r);
typedef void (APIENTRY * PFNGLMULTITEXCOORD3SVARBPROC) (GLenum target, const GLshort *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DARBPROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4DVARBPROC) (GLenum target, const GLdouble *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FARBPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4FVARBPROC) (GLenum target, const GLfloat *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4IARBPROC) (GLenum target, GLint s, GLint t, GLint r, GLint q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4IVARBPROC) (GLenum target, const GLint *v);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SARBPROC) (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
typedef void (APIENTRY * PFNGLMULTITEXCOORD4SVARBPROC) (GLenum target, const GLshort *v);
#endif

#endif
