#include "GLG3D/GaussianBlur.h"
#include "GLG3D/RenderDevice.h"
#include "G3D/filter.h"
#include "G3D/Rect2D.h"
#include "GLG3D/Draw.h"

namespace G3D {

Table<int, ShaderRef> GaussianBlur::shaderCache;

void GaussianBlur::apply(RenderDevice* rd, const Texture::Ref& source, const Vector2& direction, int N) {
    apply(rd, source, direction, N, source->vector2Bounds());
}

void GaussianBlur::apply(RenderDevice* rd, const Texture::Ref& source, const Vector2& direction, int N, const Vector2& destSize) {
    debugAssert(isOdd(N));

    ShaderRef gaussian1DShader = getShader(17);

    gaussian1DShader->args.set("source", source);
    gaussian1DShader->args.set("pixelStep", direction / source->vector2Bounds());
    rd->setShader(gaussian1DShader);

    Rect2D dest = Rect2D::xywh(Vector2(0, 0), destSize);
    Draw::rect2D(dest, rd);
}


ShaderRef GaussianBlur::getShader(int N) {
    if (! shaderCache.containsKey(N)) {
        if (shaderCache.size() >= MAX_CACHE_SIZE) {
            // Remove a random element to keep the cache
            // size limited.
            Array<int> key;
            shaderCache.getKeys(key);
            shaderCache.remove(key.randomElement());
        }

        shaderCache.set(N, makeShader(N));
    }

    return shaderCache[N];
}

ShaderRef GaussianBlur::makeShader(int N) {

    // Make a string of the coefficients to insert into the shader
    Array<float> coeff;
    float stddev = N / 2.0f;
    gaussian1D(coeff, N, stddev);

    std::string coefDecl;

    if (GLCaps::enumVendor() == GLCaps::ATI) {
        // ATI doesn't yet support the new array syntax

        coefDecl = format("  const int kernelSize = %d;\n  float gaussCoef[%d];\n", N, N);

        for (int i = 0; i < N; ++i) {
            coefDecl += format("gaussCoef[%d] = %10.8f;\n", i, coeff[i]);
        }

    } else {
        // NVIDIA has their own array syntax   
        coefDecl = format("  const int kernelSize = %d;\n  const float gaussCoef[] = {", N);
        for (int i = 0; i < N; ++i) {
            coefDecl += format("%10.8f", coeff[i]);
            if (i < N - 1) {
                coefDecl += ", ";
            }
        }

        coefDecl += "};\n";
    }

    std::string pixelSource =
        "uniform sampler2D source;\n"
        "\n"
        "// vec2(dx, dy) / (source.width, source.height)\n"
        "uniform vec2      pixelStep;\n"
        "\n"
        "void main() {\n" +
            
            coefDecl + 
            
            STR(                
                vec2 pixel = gl_TexCoord[0].xy;         
                vec4 sum = texture2D(source, pixel) * gaussCoef[0];
                
                for (int tap = 1; tap < kernelSize; ++tap) {
                    sum += texture2D(source, pixelStep * (float(tap) - float(kernelSize - 1) * 0.5) + pixel) * gaussCoef[tap];
                }
                
                gl_FragColor = sum;) + "}";
    
    //debugPrintf("%s\n", pixelSource.c_str());
    return Shader::fromStrings("", pixelSource);
}
    
}
