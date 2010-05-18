/**
 @file GLCaps.h

 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2004-03-28
 @edited  2006-02-28

 Copyright 2000-2006, Morgan McGuire.
 All rights reserved.
*/

#ifndef G3D_GLCAPS_H
#define G3D_GLCAPS_H

#include "G3D/platform.h"
#include "G3D/Set.h"
#include "GLG3D/glheaders.h"
#include "G3D/NetworkDevice.h"
#include <string>

namespace G3D {

/**
 Low-level wrapper for OpenGL extension management.
 Can be used without G3D::RenderDevice to load and
 manage extensions.

 OpenGL has a base API and an extension API.  All OpenGL drivers
 must support the base API.  The latest features may not 
 be supported by some drivers, so they are in the extension API
 and are dynamically loaded at runtime using GLCaps::loadExtensions.  
 Before using a specific extension you must test for its presence
 using the GLCaps::supports method.
 
 For convenience, frequently used extensions have fast tests, e.g.,
 GLCaps::supports_GL_EXT_texture_rectangle.

 Note that GL_NV_texture_rectangle and GL_EXT_texture_rectangle
 have exactly the same constants, so supports_GL_EXT_texture_rectangle
 returns true if either is supported.

 GLCaps assumes all OpenGL contexts have the same capabilities.

  The following extensions are shortcutted with a method named
  similarly to GLCaps::supports_GL_EXT_texture_rectangle():

  <UL>
    <LI>GL_ARB_texture_non_power_of_two
    <LI>GL_EXT_texture_rectangle
    <LI>GL_ARB_vertex_program
    <LI>GL_NV_vertex_program2
    <LI>GL_ARB_vertex_buffer_object
    <LI>GL_ARB_fragment_program
    <LI>GL_ARB_multitexture
    <LI>GL_EXT_texture_edge_clamp
    <LI>GL_ARB_texture_border_clamp
    <LI>GL_EXT_texture_r
    <LI>GL_EXT_stencil_wrap
    <LI>GL_EXT_stencil_two_side
    <LI>GL_ATI_separate_stencil
    <LI>GL_EXT_texture_compression_s3tc
    <LI>GL_EXT_texture_cube_map, GL_ARB_texture_cube_map
    <LI>GL_ARB_shadow
    <LI>GL_ARB_shader_objects
    <LI>GL_ARB_shading_language_100
    <LI>GL_ARB_fragment_shader
    <LI>GL_ARB_vertex_shader
    <LI>GL_EXT_geometry_shader4
    <LI>GL_EXT_framebuffer_object
	</UL>

  These methods do not appear in the documentation because they
  are generated using macros.

  The <code>hasBug_</code> methods detect specific common bugs on
  graphics cards.  They can be used to switch to fallback rendering
  paths.
 */
class GLCaps {
public:

    enum Vendor {ATI, NVIDIA, MESA, ARB};

private:

    /** True when init has been called */
    static bool         _initialized;

    /** True when loadExtensions has already been called */
    static bool         _loadedExtensions;

    /** True if this is GL 2.0 or greater, which mandates certain extensions.*/
    static bool         _hasGLMajorVersion2;

    /** True when checkAllBugs has been called. */
    static bool         _checkedForBugs;

    static int          _numTextureCoords;
    static int          _numTextures;
    static int          _numTextureUnits;

    static Vendor computeVendor();

    /**
       Returns the version string for the video driver.
       
       @cite Based in part on code by Ted Peck tpeck@roundwave.com http://www.codeproject.com/dll/ShowVer.asp      
    */
    static std::string getDriverVersion();
  
 // We're going to need exactly the same code for each of 
 // several extensions, so we abstract the boilerplate into
 // a macro that declares a private variable and public accessor.
#define DECLARE_EXT(extname)                    \
private:                                        \
    static bool     _supports_##extname;        \
public:                                         \
    static bool inline supports_##extname() {   \
        return _supports_##extname;             \
    }                                           \
private:

    // New extensions must be added in 3 places: 1. here.
    // 2. at the top of GLCaps.cpp.  3. underneath the LOAD_EXTENSION code.
    DECLARE_EXT(GL_ARB_texture_non_power_of_two);
    DECLARE_EXT(GL_EXT_texture_rectangle);
    DECLARE_EXT(GL_ARB_vertex_program);
    DECLARE_EXT(GL_NV_vertex_program2);
    DECLARE_EXT(GL_ARB_vertex_buffer_object);
    DECLARE_EXT(GL_ARB_fragment_program);
    DECLARE_EXT(GL_ARB_multitexture);
    DECLARE_EXT(GL_EXT_texture_edge_clamp);
    DECLARE_EXT(GL_ARB_texture_border_clamp);
    DECLARE_EXT(GL_EXT_texture3D);
    DECLARE_EXT(GL_EXT_stencil_wrap);
    DECLARE_EXT(GL_EXT_stencil_two_side);
    DECLARE_EXT(GL_ATI_separate_stencil);    
    DECLARE_EXT(GL_EXT_texture_compression_s3tc);
    DECLARE_EXT(GL_EXT_texture_cube_map);
    DECLARE_EXT(GL_ARB_shadow);
    DECLARE_EXT(GL_ARB_shader_objects);
    DECLARE_EXT(GL_ARB_shading_language_100);
    DECLARE_EXT(GL_ARB_fragment_shader);
    DECLARE_EXT(GL_ARB_vertex_shader);
    DECLARE_EXT(GL_EXT_geometry_shader4);
    DECLARE_EXT(GL_EXT_framebuffer_object);
    
#undef DECLARE_EXT

    static Set<std::string>         extensionSet;

    static bool bug_glMultiTexCoord3fvARB;
    static bool bug_normalMapTexGen;
    static bool bug_redBlueMipmapSwap;
    static bool bug_mipmapGeneration;
    static bool bug_slowVBO;

    /** Tests for hasBug_glMultiTexCoord3fvARB and hasBug_glNormalMapTexGenARB */
    static void checkBug_cubeMapBugs();
    static void checkBug_redBlueMipmapSwap();
    static void checkBug_mipmapGeneration();
    static void checkBug_slowVBO();

    /** Runs all of the checkBug_ methods. Called from loadExtensions(). */
    static void checkAllBugs();

    /** Loads OpenGL extensions (e.g. glBindBufferARB).
    Call this once at the beginning of the program,
    after a video device is created.  This is called
    for you if you use G3D::RenderDevice.    
    */
    static void loadExtensions(class Log* debugLog = NULL);

public:

    /** Maximum number of texture coordinates supported
        by G3D and RenderDevice; used to preallocate
        some static arrays */
    enum {G3D_MAX_TEXTURE_UNITS = 8};

    /** Loads OpenGL extensions (e.g. glBindBufferARB).
        Call this once at the beginning of the program,
        after a video device is created.  This is called
        for you if you use G3D::RenderDevice.*/
    static void init();

    static bool supports(const std::string& extName);

    /** Returns true if the given texture format is supported on this device.*/
    static bool supports(const class TextureFormat* fmt);

    static const std::string& glVersion();

    static const std::string& driverVersion();

    static const std::string& vendor();

    static Vendor enumVendor();

    static const std::string& renderer();

    /** Returns true if either GL_EXT_stencil_two_side or GL_ATI_separate_stencil is supported.
        Convenient becaused G3D::RenderDevice unifies those extensions. */
    static bool supports_two_sided_stencil();
 
    inline static int numTextureCoords() {
        return _numTextureCoords;
    }

    /** Some devices (e.g. NVIDIA cards) support more textures than
        texture matrices */
    static int numTextures() {
        return _numTextures;
    }

    static int numTextureUnits() {
        return _numTextureUnits;
    }

    static inline bool supports_GL_ARB_texture_cube_map() {
        return supports_GL_EXT_texture_cube_map();
    }

    /**
     Returns true if cube map support has a specific known bug on this card.
     Returns false if cube maps are not supported at all on this card.

     Call after OpenGL is intialized.  Will render on the backbuffer but not make
     the backbuffer visible.  Safe to call multiple times; the result is memoized.

     On some Radeon Mobility cards (up to Mobility 9200), glMultiTexCoord3fvARB and glVertex4fv together
     create incorrect texture lookups from cube maps.  Using glVertex3fv or glTexCoord
     with glActiveTextureARB avoids this problem, as does using normal map generation.
     */
    static bool hasBug_glMultiTexCoord3fvARB();

    /**
     Returns true if cube map support has a specific known bug on this card that
     prevents correct normal map coordinate generation.
     Returns false if cube maps are not supported at all on this card.

     Call after OpenGL is intialized.  Will render on the backbuffer but not make
     the backbuffer visible.  Safe to call multiple times; the result is memoized.

     Radeon Mobility 7500 has been shown to have a bug where not only does
     hasBug_glMultiTexCoord3fvARB() exist, but normal maps can't work around 
     the problem.

     If detected, G3D::Sky will revert to non-cube map textures.
     */
    static bool hasBug_normalMapTexGen();

    /**
    Radeon mobility 7500 occasionally flips the red and blue channels
    when auto-generating mipmaps.  This has proven to be a reliable test
    for this bug. 

    If this bug is detected, G3D::Texture switches to RGBA8 formats for
    RGB8 data.
    */
    static bool hasBug_redBlueMipmapSwap();

    /**
      Returns true if SGIS auto mip-map generation occasionally
      produces buggy results (usually, pieces of other textures in
      the low-level mipmaps).

      Radeon Mobility 9200 has this bug for some drivers.

      If this bug is detected, G3D::Texture reverts to software mipmap generation.
     */
    static bool hasBug_mipmapGeneration();

    /**
	 Some graphics cards (e.g. Radeon Mobility 7500) support the VBO extension
	 but it is slower than main memory in most cases due to poor cache behavior.
	 This method performs a speed test the first time it is invoked and identifies
	 those cards.
	*/
	static bool hasBug_slowVBO();

};


/**
 Prints a human-readable description of this machine
 to the text output stream.  Either argument may be NULL.
 */
void describeSystem(
    class RenderDevice*  rd, 
    class NetworkDevice* nd, 
    class TextOutput& t);

void describeSystem(
    class RenderDevice*  rd, 
    class NetworkDevice* nd, 
    std::string&        s);

} // namespace

#endif
