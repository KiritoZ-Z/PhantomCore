/**
 @file GLG3D/ToneMap.cpp

 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2005-08-30
 @edited  2006-10-20
 */

#include "GLG3D/ToneMap.h"
#include "GLG3D/Draw.h"
#include "G3D/Rect2D.h"

namespace G3D {

ToneMap::Profile ToneMap::profile = ToneMap::UNINITIALIZED;

ShaderRef  ToneMap::bloomShader[3];
GLuint     ToneMap::gammaShaderPS14ATI;
Texture::Ref ToneMap::RG;
Texture::Ref ToneMap::B;

void ToneMap::beginFrame(RenderDevice* rd) {
    // Intentionally empty.  Future implementations might
    // use this opportunity to switch from the back buffer to a 
    // render buffer.
}


void ToneMap::endFrame(RenderDevice* rd) {
    if (! mEnabled) {
        return;
    }

    switch (profile) {
    case PS20:
        applyPS20(rd);
        break;
        
    case PS14ATI:
        applyPS14ATI(rd);
        break;
        
    default:
        // No tone mapping
        ;
    }
}


Texture::Ref ToneMap::getBloomMap(RenderDevice* rd) const {
    if (stereo && 
        ((rd->drawBuffer() == RenderDevice::BUFFER_FRONT_RIGHT) ||
         (rd->drawBuffer() == RenderDevice::BUFFER_BACK_RIGHT))) {
        return stereoBloomMap[1];
    } else {
        return stereoBloomMap[0];
    }
}


void ToneMap::applyPS14ATI(RenderDevice* rd) {

    // TODO: obey viewport

    // TODO: gamma correct
    // TODO: bloom
    resizeImages(rd);
    Texture::Ref bloomMap = getBloomMap(rd);

    // We're going to use combiners, which G3D does not preserve
    glPushAttrib(GL_TEXTURE_BIT);
    rd->push2D();
        rd->disableLighting();

        // Undo renderdevice's 0.35 translation
        rd->setCameraToWorldMatrix(CoordinateFrame(Matrix3::identity(), Vector3(0, 0, 0.0)));
        
        Rect2D rect = Rect2D::xywh(0, 0, rd->width(), rd->height());
        Rect2D smallRect = bloomMap->rect2DBounds();
        screenImage->copyFromScreen(rect);
        
        glBindFragmentShaderATI(gammaShaderPS14ATI);
        glEnable(GL_FRAGMENT_SHADER_ATI);

            rd->setTexture(0, screenImage);
            rd->setTexture(1, RG);
            rd->setTexture(2, B);

            Draw::rect2D(rect, rd, Color3::white());
    
        glDisable(GL_FRAGMENT_SHADER_ATI);
    rd->pop2D();
    glPopAttrib();
}

/*
        // On NVIDIA, use GL_NV_texture_shader OFFSET_TEXTURE_2D_NV

        // Combiner setup
        //
        // Unit 0:
        // Mode = replace
        // arg0 = texture (screen) 
        // 
        // Unit 1:
        // Mode = dependent read off 0
        // arg0 = texture (RG gamma) 
        //
        // Unit 2:
        // Mode = dependent read off 0
        // arg0 = texture (B gamma) 
        //
        // Unit 3:
        // Mode = add
        // arg0 = unit 1
        // arg1 = unit 2 

        // Apply gamma correction
        rd->setTexture(0, screenImage);
        glActiveTextureARB(GL_TEXTURE0_ARB + 0);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_COMBINE_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,   GL_MODULATE);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,   GL_PRIMARY_COLOR_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB,  GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,   GL_TEXTURE);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB,  GL_SRC_COLOR);

        rd->setTexture(1, RG);
        glActiveTextureARB(GL_TEXTURE0_ARB + 1);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_COMBINE_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,   BUMP_TARGET_ATI);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,   GL_PRIMARY_COLOR_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB,  GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,   GL_TEXTURE);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB,  GL_SRC_COLOR);
        
        rd->setTexture(2, B);
        glActiveTextureARB(GL_TEXTURE0_ARB + 2);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_COMBINE_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,   GL_MODULATE);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,   GL_PRIMARY_COLOR_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB,  GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,   GL_TEXTURE);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB,  GL_SRC_COLOR);

        rd->setTexture(3, B); // Bind B as a dummy texture
        glActiveTextureARB(GL_TEXTURE0_ARB + 3);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_COMBINE_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,   GL_ADD);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,   GL_TEXTURE1_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB,  GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,   GL_TEXTURE2_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB,  GL_SRC_COLOR);
        */


void ToneMap::applyPS20(RenderDevice* rd) {
    // TODO: obey viewport

    resizeImages(rd);
    Texture::Ref bloomMap = getBloomMap(rd);

    rd->push2D();
        rd->setAlphaTest(RenderDevice::ALPHA_ALWAYS_PASS, 0);    
        rd->setColor(Color3::white());
        // Undo renderdevice's 0.35 translation
        rd->setCameraToWorldMatrix(CoordinateFrame());
        rd->setObjectToWorldMatrix(CoordinateFrame());

        // Grab the image of the screen that was just rendered
        Rect2D rect = Rect2D::xywh(0, 0, rd->width(), rd->height());
        screenImage->copyFromScreen(rect);
        // Threshold, gaussian blur horizontally, and subsample so that we have an image 1/4 as wide.
        bloomShader[0]->args.set("screenImage", screenImage);
        rd->setShader(bloomShader[0]);
        Draw::fastRect2D(bloomMapIntermediate->rect2DBounds(), rd);
        bloomMapIntermediate->copyFromScreen(bloomMapIntermediate->rect2DBounds());

        // Gaussian filter vertically and subsample so that we have an image 1/4 as high and 
        // blend this result in with the previous frame's bloom map to produce motion blur in
        // bright areas.
        bloomShader[1]->args.set("bloomImage",  bloomMapIntermediate);
        bloomShader[1]->args.set("oldBloom",    bloomMap);
        rd->setShader(bloomShader[1]);
        Draw::fastRect2D(bloomMap->rect2DBounds(), rd);
        bloomMap->copyFromScreen(bloomMap->rect2DBounds());

        bool showBloomMap = false; // Set to true for debugging
        if (! showBloomMap) {
            // Now combine
            bloomShader[2]->args.set("screenImage", screenImage);
            bloomShader[2]->args.set("bloomImage",  bloomMap);
            bloomShader[2]->args.set("gamma",       RG);
            rd->setShader(bloomShader[2]);
            Draw::fastRect2D(rect, rd);
        }                
    rd->pop2D();
}


void ToneMap::makeGammaCorrectionTextures() {
    if (RG.notNull() || (profile == NO_TONE)) {
        return;
    }
    
    // The inverse gamma ramp function
    G3D::uint8 ramp[256];
    
    for (int i = 0; i < 256; ++i) {
        // Linear
        //ramp[i] = i;

        // Inverse power
        const double A = 1.9;
        // Brighten the screen image by 1.0 / 0.75 = 1.34, since we darkened the 
        // scene when rendering to avoid saturation.
        ramp[i] = iClamp(iRound((1.0 - pow(1.0 - i/255.0, A)) * 255.0 / 0.75), 0, 255);

        // Log
        // const double A = 10, B = 1; 
        // ramp[i] = iRound(((log(A*i/255.0 + B) - log(B)) /
        //                  (log(A+B) - log(B))) * 255.0);
    }
    
    GImage data(256, 256, 3);
    
    for (int g = 0; g < 256; ++g) {
        for (int r = 0; r < 256; ++r) {
            Color3uint8& p = data.pixel3(r, g);
            p.r = ramp[r];
            p.g = ramp[g];
            p.b = 0;
        }
    }
    
    // MIP-mapping causes bad interpolation for some reason
    RG = Texture::fromGImage("RG Gamma", data, TextureFormat::RGB8(), Texture::DIM_2D, Texture::Settings::video());
    
    if (profile != PS20) {
        // On PS20 we can re-use the original RG texture
        data.resize(256, 1, 3);
        for (int b = 0; b < 256; ++b) {
            Color3uint8& p = data.pixel3(b, 0);
            p.r = 0;
            p.g = 0;
            p.b = ramp[b];
        }
    
        B = Texture::fromGImage("B Gamma", data, TextureFormat::RGB8(), Texture::DIM_2D, Texture::Settings::video());
    }
}


void ToneMap::makeShadersPS14ATI() {

    // Really slow--appears to be implemented in software!
    gammaShaderPS14ATI = glGenFragmentShadersATI(1);
    glBindFragmentShaderATI(gammaShaderPS14ATI);
    glBeginFragmentShaderATI();

        // Pass 1

        // R0 = texture0[texcoord0]
        glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);  

        // Move the blue component of R0 into the red component of R3
        glColorFragmentOp1ATI(GL_MOV_ATI, GL_REG_3_ATI, GL_RED_BIT_ATI, GL_NONE, 
                              GL_REG_0_ATI, GL_BLUE, GL_NONE);
        // R1 = texture1[R0]
        // R2 = texture2[R3]
        glSampleMapATI(GL_REG_1_ATI, GL_REG_0_ATI, GL_SWIZZLE_STR_ATI);  
        glSampleMapATI(GL_REG_2_ATI, GL_REG_3_ATI, GL_SWIZZLE_STR_ATI);

        // At this point, R1 contains the gamma corrected red and green channels
        // and R2 contains the corrected blue channel.  Combine them:

        // R0 = R1 + R2
        glColorFragmentOp2ATI(GL_ADD_ATI, GL_REG_0_ATI, GL_NONE, GL_NONE, 
                              GL_REG_1_ATI, GL_NONE, GL_NONE,
                              GL_REG_2_ATI, GL_NONE, GL_NONE);
        
        // R0 = texcoord 0
        //glPassTexCoordATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
        // 
        //glPassTexCoordATI(GL_REG_1_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);

    glEndFragmentShaderATI();
}


void ToneMap::makeShadersPS20() {
    // Bloom filter pass 1 (horizontal).
    // Threshold and horizontal gaussian blur.

    // ATI's shader implementation does not support (yet) any way of initializing
    // array elements other than assigning each explicitly.

    bloomShader[0] = Shader::fromStrings("",
        STR(
        uniform sampler2D screenImage;

        vec3 threshold(vec3 v) {
            const float T = 0.88;
            return vec3((v.r > T) ? v.r : 0.0,
                        (v.g > T) ? v.g : 0.0,
                        (v.b > T) ? v.b : 0.0);
        }

        vec3 sample(vec2 p, int tap, vec2 stride) {
            return texture2D(screenImage, p + stride * float(tap)).rgb;
        }
        
        void main()) +  std::string("{\n"
        "const float scale = 3.0;\n"
        "const int kernelSize = 17;\n"
        "float gaussCoef[kernelSize];\n"
        "gaussCoef[0] = 0.013960189 * scale;\n"
        "gaussCoef[1] = 0.022308318 * scale;\n"
        "gaussCoef[2] = 0.033488752 * scale;\n"
        "gaussCoef[3] = 0.047226710 * scale;\n"
        "gaussCoef[4] = 0.062565226 * scale;\n"
        "gaussCoef[5] = 0.077863682 * scale;\n" 
        "gaussCoef[6] = 0.091031867 * scale;\n" 
        "gaussCoef[7] = 0.099978946 * scale;\n"
        "gaussCoef[8] = 0.103152619 * scale;\n"
        "gaussCoef[9] = 0.099978946 * scale;\n"
        "gaussCoef[10] = 0.091031867 * scale;\n"
        "gaussCoef[11] = 0.077863682 * scale;\n"
        "gaussCoef[12] = 0.062565226 * scale;\n"
        "gaussCoef[13] = 0.047226710 * scale;\n"
        "gaussCoef[14] = 0.033488752 * scale;\n"
        "gaussCoef[15] = 0.022308318 * scale;\n"
        "gaussCoef[16] = 0.013960189 * scale;\n") +

        STR(
            vec2 direction = vec2(1.0, 0.0);
            vec2 pixelSize = g3d_sampler2DInvSize(screenImage);
            vec2 stride    = pixelSize * 2.0 * direction;
            
            vec2 p = gl_TexCoord[0].xy + direction * pixelSize * (0.5 - float(kernelSize));
            
            vec3 color = threshold(sample(p, 0, stride)) * gaussCoef[0];
            for (int tap = 1; tap < kernelSize; ++tap) {
                color += threshold(sample(p, tap, stride)) * gaussCoef[tap];
            }
            
            gl_FragColor.rgb = color;
        }));

    // Bloom filter pass 2 (vertical).
    // Vertical gaussian blur and blend with the old bloom map.
    bloomShader[1] = Shader::fromStrings("", 
                    /*"#version 120\n"*/
        STR(
        uniform sampler2D bloomImage;
        uniform sampler2D oldBloom;

        vec3 sample(vec2 p, int tap, vec2 stride) {
            return texture2D(bloomImage, p + stride * vec2(0.0, tap)).rgb;
        }
        
        void main()) + std::string("{\n"
        "const float scale = 3.0;\n"
        "const int kernelSize = 17;\n"
        "float gaussCoef[kernelSize];\n"
        "gaussCoef[0] = 0.013960189 * scale;\n"
        "gaussCoef[1] = 0.022308318 * scale;\n"
        "gaussCoef[2] = 0.033488752 * scale;\n"
        "gaussCoef[3] = 0.047226710 * scale;\n"
        "gaussCoef[4] = 0.062565226 * scale;\n"
        "gaussCoef[5] = 0.077863682 * scale;\n" 
        "gaussCoef[6] = 0.091031867 * scale;\n" 
        "gaussCoef[7] = 0.099978946 * scale;\n"
        "gaussCoef[8] = 0.103152619 * scale;\n"
        "gaussCoef[9] = 0.099978946 * scale;\n"
        "gaussCoef[10] = 0.091031867 * scale;\n"
        "gaussCoef[11] = 0.077863682 * scale;\n"
        "gaussCoef[12] = 0.062565226 * scale;\n"
        "gaussCoef[13] = 0.047226710 * scale;\n"
        "gaussCoef[14] = 0.033488752 * scale;\n"
        "gaussCoef[15] = 0.022308318 * scale;\n"
        "gaussCoef[16] = 0.013960189 * scale;\n") +
        STR(vec2 direction = vec2(0.0, 1.0);
            vec2 pixelSize = g3d_sampler2DInvSize(bloomImage);
            vec2 stride    = 2.0 * pixelSize * direction;
            
            vec2 p = gl_TexCoord[0].xy + direction * (0.5 - float(kernelSize)) * pixelSize;
            
            vec3 bloom = sample(p, 0, stride) * gaussCoef[0];
            for (int tap = 1; tap < kernelSize; ++tap) {
                bloom += sample(p, tap, stride) * gaussCoef[tap];
            }
            
            vec3 old = texture2D(oldBloom, gl_TexCoord[0].xy).rgb;
            
            gl_FragColor.rgb = (bloom + old) * 0.7;
            ) + std::string("}"));

    // Bloom pass 3:
    // Gamma correct the screen image and
    // add it to a 2D blur of the bloom image.
    bloomShader[2] = Shader::fromStrings("", 
        STR(
        uniform sampler2D screenImage;
        uniform sampler2D bloomImage;
        uniform sampler2D gamma; 

        void main() {
                
            // Use a 3-vector so that swizzles give us the 4-neighbors
            vec3 s = vec3(g3d_sampler2DInvSize(bloomImage) * 2.0, 0.0);
            
            // Offset by half a texel to get bilinear
            vec2 p = gl_TexCoord[0].xy - s.xy * 0.5;
            
            vec3 bloom = 
                (texture2D(bloomImage, p) +
                 texture2D(bloomImage, p - s.xz) +
                 texture2D(bloomImage, p + s.xz) +
                 texture2D(bloomImage, p + s.zy) +
                 texture2D(bloomImage, p - s.zy)).rgb * 0.2;
            
            vec3 screen = texture2D(screenImage, gl_TexCoord[0].xy).rgb;

            // Apply gamma correction.  Use gamma lookup table.
            // Process red and green simultaneously to reduce 
            // the number of texture reads.
            screen.rg = texture2D(gamma, screen.rg).rg;
            screen.b  = texture2D(gamma, screen.rb).g; 
            
            gl_FragColor.rgb = screen + bloom;
        }
        ));
  
    // Shaders don't need to preserve state since tonemap is wrapped in
    // push2D
    for (int i = 0; i < 3; ++i) {
        bloomShader[i]->setPreserveState(false);
    }
}


ToneMap::ToneMap() : mEnabled(true) {

    // Determine shading language profile

    if (profile == UNINITIALIZED) {
        profile = NO_TONE;
        
        if (GLCaps::supports_GL_ARB_texture_non_power_of_two()) {
            if (Shader::supportsPixelShaders()) {
                profile = PS20;
            } else if (GLCaps::supports("GL_ARB_texture_env_crossbar") &&
                GLCaps::supports("GL_ARB_texture_env_combine") &&
                GLCaps::supports("GL_EXT_texture_env_add") &&
                GLCaps::supports("GL_NV_texture_shader") &&
                (GLCaps::numTextureUnits() >= 4)) {

                profile = PS14NVIDIA;

                // TODO: remove (not currently supporting PS14NIVIDA)
                profile = NO_TONE;
            } else if (GLCaps::supports("GL_ATI_fragment_shader") &&
                (GLCaps::numTextureUnits() >= 4)) {

                // profile = PS14ATI;
                profile = NO_TONE;
            }
        }
    }

    if (profile != NO_TONE) {
        makeGammaCorrectionTextures();
        
        if (profile == PS20) {
            makeShadersPS20();
        } else if (profile == PS14ATI) {
            makeShadersPS14ATI();
        }
    }
}


void ToneMap::resizeImages(RenderDevice* rd) {
    
    const Rect2D viewport = rd->viewport();
    
    if (screenImage.isNull() ||
        (viewport.wh() != screenImage->vector2Bounds())) {

        screenImage = Texture::createEmpty
          ("Copied Screen Image",
           (int)viewport.width(), (int)viewport.height(), 
           TextureFormat::RGB8(),
           Texture::DIM_2D_NPOT, 
           Texture::Settings::video());

        bloomMapIntermediate = Texture::createEmpty
          ("Bloom map intermediate",
           (int)viewport.width() / 4, (int)viewport.height(), 
           TextureFormat::RGB8(), 
           Texture::DIM_2D_NPOT, 
           Texture::Settings::video());

        resizeBloomMap((int)viewport.width() / 4, (int)viewport.height() / 4);
    }
}


void ToneMap::resizeBloomMap(int w, int h) {
    stereo = glGetBoolean(GL_STEREO) != 0;
    
    for (int i = 0; i < (stereo ? 2 : 1); ++i) {
        stereoBloomMap[i] = Texture::createEmpty(
			"Bloom map", w, h, 
            TextureFormat::RGB8(), Texture::DIM_2D_NPOT, 
			Texture::Settings::video());
    }
}


SkyParameters ToneMap::prepareSkyParameters(const SkyParameters& L) const {

    bool on = mEnabled && (profile != NO_TONE);

    double lightScale = on ? 0.75 : 1.0;
    SkyParameters params = L;

    params.emissiveScale *= lightScale;
    params.skyAmbient   *= on ? 0.5 : 1.0;
    params.diffuseAmbient *= lightScale;
    params.lightColor   *= lightScale;
    params.ambient      *= lightScale;
    
    return params;
}


LightingRef ToneMap::prepareLighting(const LightingRef& L) const {

    double lightScale = (mEnabled && (profile != NO_TONE)) ? 0.75 : 1.0;

    LightingRef lighting = Lighting::create();
    *lighting = *L;

    lighting->environmentMapColor *= lightScale;
    lighting->emissiveScale *= lightScale;

    lighting->ambientTop *= lightScale;
    lighting->ambientBottom *= lightScale;

    for (int i = 0; i < lighting->lightArray.size(); ++i) {
        lighting->lightArray[i].color *= lightScale;
    }

    for (int i = 0; i < lighting->shadowedLightArray.size(); ++i) {
        lighting->shadowedLightArray[i].color *= lightScale;
    }
 
    return lighting;
}


void ToneMap::setEnabled(bool e) {
    if (e == mEnabled) {
        return;
    }

    if (! mEnabled && stereoBloomMap[0].notNull()) {
        // Wipe old bloomMap out on re-enable
        // (this implementation leaves noise; we should really color them black)
        //resizeBloomMap(stereoBloomMap[0]->getTexelWidth(), stereoBloomMap[0]->getTexelHeight());
    }

    mEnabled = e;
}

} // namespace G3D
