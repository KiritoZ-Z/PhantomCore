/**
  @file glcalls.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com
  @created 2002-08-07
  @edited  2006-02-01
*/

#include "G3D/Matrix3.h"
#include "G3D/Matrix4.h"
#include "G3D/CoordinateFrame.h"
#include "GLG3D/glcalls.h"
#include "GLG3D/getOpenGLState.h"
#include "GLG3D/GLCaps.h"


#if defined(G3D_OSX)
#include <mach-o/dyld.h>
#endif


#ifndef G3D_GL_ARB_multitexture_static
PFNGLMULTITEXCOORD1FARBPROC                 glMultiTexCoord1fARB		    = NULL;
PFNGLMULTITEXCOORD1DARBPROC                 glMultiTexCoord1dARB		    = NULL;

PFNGLMULTITEXCOORD2FARBPROC                 glMultiTexCoord2fARB		    = NULL;
PFNGLMULTITEXCOORD2FVARBPROC                glMultiTexCoord2fvARB		    = NULL;
PFNGLMULTITEXCOORD2DVARBPROC                glMultiTexCoord2dvARB		    = NULL;

PFNGLMULTITEXCOORD3FVARBPROC                glMultiTexCoord3fvARB		    = NULL;
PFNGLMULTITEXCOORD3DVARBPROC                glMultiTexCoord3dvARB		    = NULL;

PFNGLMULTITEXCOORD4FVARBPROC                glMultiTexCoord4fvARB		    = NULL;
PFNGLMULTITEXCOORD4DVARBPROC                glMultiTexCoord4dvARB		    = NULL;

PFNGLACTIVETEXTUREARBPROC                   glActiveTextureARB 			    = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC             glClientActiveTextureARB	    = NULL;
#endif


PFNGLBLENDEQUATIONEXTPROC                   glBlendEquationEXT              = NULL;
PFNGLTEXIMAGE3DEXTPROC                      glTexImage3DEXT                 = NULL;

#ifdef G3D_WIN32

PFNWGLSWAPINTERVALEXTPROC                   wglSwapIntervalEXT 			    = NULL;
PFNWGLCHOOSEPIXELFORMATARBPROC              wglChoosePixelFormatARB		    = NULL;
PFNWGLALLOCATEMEMORYNVPROC                  wglAllocateMemoryNV 		    = NULL;
PFNWGLFREEMEMORYNVPROC                      wglFreeMemoryNV 			    = NULL;
#endif

PFNGLVERTEXARRAYRANGENVPROC                 glVertexArrayRangeNV 		    = NULL;
PFNGLFLUSHVERTEXARRAYRANGENVPROC            glFlushVertexArrayRangeNV       = NULL;

PFNGLCOMPRESSEDTEXIMAGE2DARBPROC            glCompressedTexImage2DARB 	    = NULL;
PFNGLGETCOMPRESSEDTEXIMAGEARBPROC           glGetCompressedTexImageARB 	    = NULL;

PFNGLGENFENCESNVPROC				        glGenFencesNV				    = NULL;
PFNGLDELETEFENCESNVPROC				        glDeleteFencesNV			    = NULL;
PFNGLSETFENCENVPROC					        glSetFenceNV				    = NULL;
PFNGLFINISHFENCENVPROC	  			        glFinishFenceNV				    = NULL;

PFNGLGENPROGRAMSARBPROC                     glGenProgramsARB                = NULL;
PFNGLBINDPROGRAMARBPROC                     glBindProgramARB                = NULL;
PFNGLDELETEPROGRAMSARBPROC                  glDeleteProgramsARB             = NULL;
PFNGLPROGRAMSTRINGARBPROC                   glProgramStringARB              = NULL;
PFNGLPROGRAMENVPARAMETER4FARBPROC           glProgramEnvParameter4fARB      = NULL;
PFNGLPROGRAMLOCALPARAMETER4FARBPROC         glProgramLocalParameter4fARB    = NULL;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC        glProgramLocalParameter4fvARB   = NULL;
PFNGLPROGRAMENVPARAMETER4DVARBPROC          glProgramEnvParameter4dvARB     = NULL;
PFNGLPROGRAMLOCALPARAMETER4DVARBPROC        glProgramLocalParameter4dvARB   = NULL;

PFNGLGENPROGRAMSNVPROC                      glGenProgramsNV                 = NULL;
PFNGLDELETEPROGRAMSNVPROC                   glDeleteProgramsNV              = NULL;
PFNGLBINDPROGRAMNVPROC                      glBindProgramNV                 = NULL;
PFNGLLOADPROGRAMNVPROC                      glLoadProgramNV                 = NULL;
PFNGLTRACKMATRIXNVPROC                      glTrackMatrixNV                 = NULL;
PFNGLPROGRAMPARAMETER4FVNVPROC              glProgramParameter4fvNV         = NULL;
PFNGLGETPROGRAMPARAMETERFVNVPROC            glGetProgramParameterfvNV       = NULL;
PFNGLGETPROGRAMPARAMETERDVNVPROC            glGetProgramParameterdvNV       = NULL;

PFNGLVERTEXATTRIBPOINTERARBPROC             glVertexAttribPointerARB        = NULL;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC         glEnableVertexAttribArrayARB    = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC        glDisableVertexAttribArrayARB   = NULL;

PFNGLPOINTPARAMETERFARBPROC                 glPointParameterfARB            = NULL;
PFNGLPOINTPARAMETERFVARBPROC                glPointParameterfvARB           = NULL;

PFNGLMULTIDRAWARRAYSEXTPROC                 glMultiDrawArraysEXT            = NULL;
PFNGLMULTIDRAWELEMENTSEXTPROC               glMultiDrawElementsEXT          = NULL;

PFNGLCOMBINERPARAMETERFVNVPROC              glCombinerParameterfvNV         = NULL;
PFNGLCOMBINERPARAMETERFNVPROC               glCombinerParameterfNV          = NULL;
PFNGLCOMBINERPARAMETERIVNVPROC              glCombinerParameterivNV         = NULL;
PFNGLCOMBINERPARAMETERINVPROC               glCombinerParameteriNV          = NULL;
PFNGLCOMBINERINPUTNVPROC                    glCombinerInputNV               = NULL;
PFNGLCOMBINEROUTPUTNVPROC                   glCombinerOutputNV              = NULL;
PFNGLFINALCOMBINERINPUTNVPROC               glFinalCombinerInputNV          = NULL;
PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC      glGetCombinerInputParameterfvNV        = NULL;
PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC      glGetCombinerInputParameterivNV        = NULL;
PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC     glGetCombinerOutputParameterfvNV       = NULL;
PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC     glGetCombinerOutputParameterivNV       = NULL;
PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC glGetFinalCombinerInputParameterfvNV   = NULL;
PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC glGetFinalCombinerInputParameterivNV   = NULL;
PFNGLCOMBINERSTAGEPARAMETERFVNVPROC         glCombinerStageParameterfvNV           = NULL;
PFNGLGETCOMBINERSTAGEPARAMETERFVNVPROC      glGetCombinerStageParameterfvNV        = NULL;

PFNGLACTIVESTENCILFACEEXTPROC               glActiveStencilFaceEXT          = NULL;

PFNGLBINDBUFFERARBPROC glBindBufferARB = NULL;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL;
PFNGLGENBUFFERSARBPROC glGenBuffersARB = NULL;
PFNGLISBUFFERARBPROC glIsBufferARB = NULL;
PFNGLBUFFERDATAARBPROC glBufferDataARB = NULL;
PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB = NULL;
PFNGLGETBUFFERSUBDATAARBPROC glGetBufferSubDataARB = NULL;
PFNGLMAPBUFFERARBPROC glMapBufferARB = NULL;
PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB = NULL;
PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB = NULL;
PFNGLGETBUFFERPOINTERVARBPROC glGetBufferPointervARB = NULL;
PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements = NULL;

PFNGLDELETEOBJECTARBPROC glDeleteObjectARB = NULL;
PFNGLGETHANDLEARBPROC glGetHandleARB = NULL;
PFNGLDETACHOBJECTARBPROC glDetachObjectARB = NULL;
PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB = NULL;
PFNGLSHADERSOURCEARBPROC glShaderSourceARB = NULL;
PFNGLCOMPILESHADERARBPROC glCompileShaderARB = NULL;
PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB = NULL;
PFNGLATTACHOBJECTARBPROC glAttachObjectARB = NULL;
PFNGLLINKPROGRAMARBPROC glLinkProgramARB = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB = NULL;
PFNGLVALIDATEPROGRAMARBPROC glValidateProgramARB = NULL;
PFNGLUNIFORM1FVARBPROC glUniform1fvARB = NULL;
PFNGLUNIFORM2FVARBPROC glUniform2fvARB = NULL;
PFNGLUNIFORM3FVARBPROC glUniform3fvARB = NULL;
PFNGLUNIFORM4FVARBPROC glUniform4fvARB = NULL;
PFNGLUNIFORM1IARBPROC glUniform1iARB = NULL;
PFNGLUNIFORM2IARBPROC glUniform2iARB = NULL;
PFNGLUNIFORM3IARBPROC glUniform3iARB = NULL;
PFNGLUNIFORM4IARBPROC glUniform4iARB = NULL;
PFNGLUNIFORMMATRIX2FVARBPROC glUniformMatrix2fvARB = NULL;
PFNGLUNIFORMMATRIX3FVARBPROC glUniformMatrix3fvARB = NULL;
PFNGLUNIFORMMATRIX4FVARBPROC glUniformMatrix4fvARB = NULL;
PFNGLGETINFOLOGARBPROC glGetInfoLogARB = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB = NULL;
PFNGLGETATTRIBLOCATIONARBPROC glGetAttribLocationARB = NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB = NULL;
PFNGLGETACTIVEUNIFORMARBPROC glGetActiveUniformARB = NULL;

PFNGLGENFRAGMENTSHADERSATIPROC glGenFragmentShadersATI = NULL;
PFNGLDELETEFRAGMENTSHADERATIPROC glDeleteFragmentShaderATI = NULL;
PFNGLBINDFRAGMENTSHADERATIPROC glBindFragmentShaderATI = NULL;
PFNGLBEGINFRAGMENTSHADERATIPROC glBeginFragmentShaderATI = NULL;
PFNGLENDFRAGMENTSHADERATIPROC glEndFragmentShaderATI = NULL;
PFNGLPASSTEXCOORDATIPROC         glPassTexCoordATI = NULL;
PFNGLSAMPLEMAPATIPROC            glSampleMapATI = NULL;
PFNGLSETFRAGMENTSHADERCONSTANTATIPROC glSetFragmentShaderConstantATI = NULL;
PFNGLCOLORFRAGMENTOP1ATIPROC glColorFragmentOp1ATI = NULL;
PFNGLCOLORFRAGMENTOP2ATIPROC glColorFragmentOp2ATI = NULL;
PFNGLCOLORFRAGMENTOP3ATIPROC glColorFragmentOp3ATI = NULL;
PFNGLALPHAFRAGMENTOP1ATIPROC glAlphaFragmentOp1ATI = NULL;
PFNGLALPHAFRAGMENTOP2ATIPROC glAlphaFragmentOp2ATI = NULL;
PFNGLALPHAFRAGMENTOP3ATIPROC glAlphaFragmentOp3ATI = NULL;

#ifdef G3D_WIN32
PFNWGLENABLEGENLOCKI3D wglEnableGenlockI3D = NULL;
#elif defined(G3D_LINUX) 
PFNGLXJOINSWAPGROUPNV glXJoinSwapGroupNV = NULL;
PFNGLXBINDSWAPBARRIERNV glXBindSwapBarrierNV = NULL;
PFNGLXQUERYSWAPGROUPNV glXQuerySwapGroupNV = NULL;
PFNGLXQUERYMAXSWAPGROUPSNV glXQueryMaxSwapGroupsNV = NULL;
PFNGLXQUERYFRAMECOUNTNV glXQueryFrameCountNV = NULL;
PFNGLXRESETFRAMECOUNTNV glXResetFrameCountNV = NULL;

PFNGLXCREATEGLXPBUFFERSGIX glXCreateGLXPbufferSGIX = NULL;
PFNGLXDESTROYGLXPBUFFERSGIX glXDestroyGLXPbufferSGIX = NULL;
PFNGLXQUERYGLXPBUFFERSGIX glXQueryGLXPbufferSGIX = NULL;
PFNGLXCHOOSEFBCONFIGSGIX glXChooseFBConfigSGIX = NULL;
PFNGLXCREATECONTEXTWITHCONFIGSGIX glXCreateContextWithConfigSGIX = NULL;

#endif

#ifdef G3D_WIN32
PFNWGLCREATEPBUFFEREXTPROC wglCreatePbufferEXT = NULL;
PFNWGLDESTROYPBUFFEREXTPROC wglDestroyPbufferEXT = NULL;
#endif

// Frame buffer object
PFNGLISRENDERBUFFEREXTPROC glIsRenderBufferEXT = NULL;
PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT = NULL;
PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT = NULL;
PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT = NULL;
PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC glGetRenderbufferParameterivEXT = NULL;
PFNGLISFRAMEBUFFEREXTPROC glIsFramebufferEXT = NULL;
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT = NULL;
PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC glFramebufferTexture1DEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC glFramebufferTexture3DEXT = NULL;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT = NULL;
PFNGLGENERATEMIPMAPEXTPROC glGenerateMipmapEXT = NULL;

PFNGLDRAWBUFFERSARBPROC glDrawBuffersARB = NULL;


// Query
PFNGLBEGINQUERYARBPROC glBeginQueryARB = NULL;
PFNGLDELETEQUERIESARBPROC glDeleteQueriesARB = NULL;
PFNGLENDQUERYARBPROC glEndQueryARB = NULL;
PFNGLGENQUERIESARBPROC glGenQueriesARB = NULL;
PFNGLGETQUERYIVARBPROC glGetQueryivARB = NULL;
PFNGLGETQUERYOBJECTIVARBPROC glGetQueryObjectivARB = NULL;
PFNGLGETQUERYOBJECTUIVARBPROC glGetQueryObjectuivARB = NULL;
PFNGLISQUERYARBPROC glIsQueryARB = NULL;

// ATI separate stencil
PFNGLSTENCILFUNCSEPARATEATIPROC glStencilFuncSeparateATI = NULL;
PFNGLSTENCILOPSEPARATEATIPROC glStencilOpSeparateATI = NULL;


// Depth bounds
PFNGLDEPTHBOUNDSEXTPROC glDepthBoundsEXT = NULL;


namespace G3D {

/**
 Sets up matrix m from rot and trans
 */
static void _getGLMatrix(GLfloat* m, const Matrix3& rot, const Vector3& trans) {
    // GL wants a column major matrix
    m[0] = rot[0][0];
    m[1] = rot[1][0];
    m[2] = rot[2][0];
    m[3] = 0.0f;

    m[4] = rot[0][1];
    m[5] = rot[1][1];
    m[6] = rot[2][1];
    m[7] = 0.0f;

    m[8] = rot[0][2];
    m[9] = rot[1][2];
    m[10] = rot[2][2];
    m[11] = 0.0f;

    m[12] = trans[0];
    m[13] = trans[1];
    m[14] = trans[2];
    m[15] = 1.0f;
}


void glGetMatrix(GLenum name, Matrix4& m) {
    float f[16];
    glGetFloatv(name, f);
    debugAssertGLOk();
    m = Matrix4(f).transpose();
}


Matrix4 glGetMatrix(GLenum name) {
    Matrix4 m;
    glGetMatrix(name, m);
    return m;
}


void glLoadMatrix(const CoordinateFrame &cf) {
    GLfloat matrix[16];
    _getGLMatrix(matrix, cf.rotation, cf.translation);
    glLoadMatrixf(matrix);
}


void glLoadMatrix(const Matrix4& m) {
    GLfloat matrix[16];
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            // Transpose
            matrix[c * 4 + r] = m[r][c];
        }
    }

    glLoadMatrixf(matrix);
}


void glLoadInvMatrix(const CoordinateFrame &cf) {
    Matrix3 rotInv = cf.rotation.transpose();

    GLfloat matrix[16];
    _getGLMatrix(matrix, rotInv, rotInv * -cf.translation);
    glLoadMatrixf(matrix);
}

void glMultInvMatrix(const CoordinateFrame &cf) {
    Matrix3 rotInv = cf.rotation.transpose();

    GLfloat matrix[16];
    _getGLMatrix(matrix, rotInv, rotInv * -cf.translation);
    glMultMatrixf(matrix);
}

void glMultMatrix(const CoordinateFrame &cf) {
    GLfloat matrix[16];
    _getGLMatrix(matrix, cf.rotation, cf.translation);
    glMultMatrixf(matrix);
}


Vector2 glGetVector2(GLenum which) {
    float v[4];
    glGetFloatv(which, v);
    return Vector2(v[0], v[1]);
}


Vector3 glGetVector3(GLenum which) {
    float v[4];
    glGetFloatv(which, v);
    return Vector3(v[0], v[1], v[2]);
}


Vector4 glGetVector4(GLenum which) {
    float v[4];
    glGetFloatv(which, v);
    return Vector4(v[0], v[1], v[2], v[3]);
}


/**
 Takes an object space point to screen space using the current MODELVIEW and
 PROJECTION matrices. The resulting xy values are in <B>pixels</B> and
 are relative to the viewport origin, the z 
 value is on the glDepthRange scale, and the w value contains rhw (-1/z for
 camera space z), which is useful for scaling line and point size.
 */
Vector4 glToScreen(const Vector4& v) {
    
    // Get the matrices and viewport
    double modelView[16];
    double projection[16];
    double viewport[4];
    double depthRange[2];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetDoublev(GL_VIEWPORT, viewport);
    glGetDoublev(GL_DEPTH_RANGE, depthRange);

    // Compose the matrices into a single row-major transformation
    Vector4 T[4];
    int r, c, i;
    for (r = 0; r < 4; ++r) {
        for (c = 0; c < 4; ++c) {
            T[r][c] = 0;
            for (i = 0; i < 4; ++i) {
                // OpenGL matrices are column major
                T[r][c] += projection[r + i * 4] * modelView[i + c * 4];
            }
        }
    }

    // Transform the vertex
    Vector4 result;
    for (r = 0; r < 4; ++r) {
        result[r] = T[r].dot(v);
    }

    // Homogeneous divide
    const double rhw = 1 / result.w;

    return Vector4(
        (1 + result.x * rhw) * viewport[2] / 2 + viewport[0],
        (1 - result.y * rhw) * viewport[3] / 2 + viewport[1],
        (result.z * rhw) * (depthRange[1] - depthRange[0]) + depthRange[0],
        rhw);
}

void glDisableAllTextures() {
    glDisable(GL_TEXTURE_2D);
    if (GLCaps::supports_GL_EXT_texture3D()) {
        glDisable(GL_TEXTURE_3D);
    }
    if (GLCaps::supports_GL_EXT_texture_cube_map()) {
        glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    }
    glDisable(GL_TEXTURE_1D);
    
    if (GLCaps::supports_GL_EXT_texture_rectangle()) {
        glDisable(GL_TEXTURE_RECTANGLE_ARB);
    }
}

} // namespace

extern void (*glXGetProcAddress (const GLubyte *))( void );
extern void (*glXGetProcAddressARB (const GLubyte *))( void );

namespace G3D {
void* glGetProcAddress(const char * name) {
    #if defined(G3D_WIN32)
	    return (void *)wglGetProcAddress(name);
    #elif defined(G3D_LINUX)
	    //#ifndef GLX_VERSION_1_4
            return (void *)glXGetProcAddressARB((const GLubyte*)name);
	    //#else
            //return (void *)glXGetProcAddress((const GLubyte*)name);
	    //#endif
    #elif defined(G3D_OSX)
        return G3D::NSGLGetProcAddress((const char*)name);
    #else
        return NULL;
    #endif
}

  

#if defined(G3D_OSX)
void* NSGLGetProcAddress(const char *name)
{
	/* This code is from Apple's tech note QA1188.
	    Apple states that this can be called from Cocoa
	    or Carbon applications, as long as they use Mach-O
	    executable rather than CFM.  G3D certainly used
	    Mach-O.
	 */

	    NSSymbol symbol;
	    char *symbolName;

	    // Prepend a '_' for the Unix C symbol mangling convention
	    symbolName = (char*)malloc(strlen (name) + 2);
	    strcpy(symbolName + 1, name);
	    symbolName[0] = '_';
	    symbol = NULL;

		const mach_header* mh = 
			NSAddImage("/System/Library/Frameworks/OpenGL.Framework/Libraries/libGL.dylib",
					   NSADDIMAGE_OPTION_RETURN_ON_ERROR);
		  
		bool rc = NSIsSymbolNameDefinedInImage(mh, symbolName);
		
		if (rc)
		{
			symbol = NSLookupSymbolInImage(mh, symbolName,
										NSLOOKUPSYMBOLINIMAGE_OPTION_RETURN_ON_ERROR);
		
		}
	    
		free (symbolName);
	    
		if (!symbol)
			return NULL;
		
		void* addr = NSAddressOfSymbol(symbol);
		 
		return addr; 
}
#endif

} // namespace 

