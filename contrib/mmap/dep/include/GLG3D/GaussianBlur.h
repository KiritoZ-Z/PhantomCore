/**
 @file GaussianBlur.h

 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2007-03-10
 @edited  2007-03-20

 Copyright 2002-2007, Morgan McGuire.
 All rights reserved.
*/
#ifndef G3D_GAUSSIANBLUR_H
#define G3D_GAUSSIANBLUR_H

#include "G3D/Table.h"
#include "G3D/Vector2.h"
#include "GLG3D/Texture.h"
#include "GLG3D/Shader.h"

namespace G3D {

/**
 1D Gaussian blur.  Call twice to produce a 2D blur.

 Operates on the graphics card; this requires a RenderDevice.  See G3D::gaussian for
 gaussian filter coefficients on the CPU.
 */
class GaussianBlur {
private:

    /** Maximum number of cached Gaussian shaders. A random replacement
        strategy is used, and shaders that are not in use even if not
        replaced should be swapped to main memory by the graphics driver. */
    enum {MAX_CACHE_SIZE = 10};

    /**
     Cached shaders, indexed by filter width.  This cache does not need threadsafe access
     since OpenGL code must already be externally threadsafed.
     */
    static Table<int, ShaderRef> shaderCache;

    /** Create a 1D gaussian shader of width N */
    static ShaderRef makeShader(int N);

    /** Fetch a shader of width N from the cache, creating it if necessary.*/
    static ShaderRef getShader(int N);

public:

    enum Direction {VERTICAL, HORIZONTAL};

    /** 
     Blurs the source to the current G3D::Framebuffer.  Assumes RenderDevice::push2D rendering mode is already set.
     Blurs the alpha channel the same as any color channel, however, you must have alphaWrite
     enabled to obtain that result.

     2D blur is not directly supported because handling of the intermediate texture is different
     for Framebuffer and backbuffer rendering.

     @param N Number of taps in the filter (filter kernel width)
     @param direction Direction of the blur.  For best results, use Vector2(1,0) and Vector(0,1).
     @param destSize output dimensions
    */
    static void apply(class RenderDevice* rd, const Texture::Ref& source, const Vector2& direction, int N, const Vector2& destSize);
    static void apply(class RenderDevice* rd, const Texture::Ref& source, const Vector2& direction = Vector2(1.0f, 0.0f), int N = 17);

};

}

#endif

