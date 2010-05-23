/** 
  @file Sky.cpp

  @author Morgan McGuire, matrix@graphics3d.com

  @created 2002-10-04
  @edited  2006-02-11
  */

#include "G3D/fileutils.h"
#include "GLG3D/glcalls.h"
#include "GLG3D/Sky.h"
#include "GLG3D/SkyParameters.h"
#include "G3D/BinaryInput.h"
#include "G3D/g3dmath.h"
#include "GLG3D/TextureFormat.h"
#include "GLG3D/getOpenGLState.h"

namespace G3D {

#define SHORT_TO_FLOAT(x) ((2.0f*x+1.0f)*(1.0f/65535.0f))


SkyRef Sky::fromCubeMap(
    Texture::Ref                          _cubeMap,
    const std::string&                  directory,
    bool                                _drawCelestialBodies,
    double                              quality) {

    debugAssertM(
        (directory == "") || 
        (directory[directory.size() - 1] == '/') || 
        (directory[directory.size() - 1] == '\\'), 
        "Directory must end in a slash");

    debugAssert( _cubeMap.notNull() );
    debugAssert( _cubeMap->dimension() == Texture::DIM_CUBE_MAP );

    Texture::Ref t[6];
    t[0] = _cubeMap;
    for (int i = 1; i < 6; ++i) {
        t[i] = NULL;
    }    

    return new Sky(t, directory, true, _drawCelestialBodies, quality);
}


SkyRef Sky::fromFile(
    const std::string&                  directory,
    const std::string&                  filename,
    bool                                _drawCelestialBodies,
    double                              quality,
    int                                 scaleDownFactor) {

    std::string f[6];
    f[0] = filename;
    for (int i = 1; i < 6; ++i) {
        f[i] = "";
    }
    return Sky::fromFile(directory, f, _drawCelestialBodies, quality, scaleDownFactor);
}


SkyRef Sky::fromFile(
    const std::string&                  directory,
    const std::string                   _filename[6],
    bool                                _drawCelestialBodies,
    double                              quality,
    int                                 scaleDownFactor) {

    debugAssertM(
        (directory == "") || 
        (directory[directory.size() - 1] == '/') || 
        (directory[directory.size() - 1] == '\\'), 
        "Directory must end in a slash");

    const TextureFormat* format;
    std::string filename = _filename[0];

    if (quality > 0.55) {
        format      = TextureFormat::RGB8();
    } else {
        format      = TextureFormat::RGB_DXT1();
    }

    // Look for the filename
    // Parse the filename into a base name and extension
    std::string filenameBase;
    std::string filenameExt;
    std::string fullFilename = filename;

    if (_filename[1] == "") {
        // First look relative to the current directory
        Texture::splitFilenameAtWildCard(fullFilename, filenameBase, filenameExt);

        if (! fileExists(filenameBase + "up" + filenameExt)) {
            // Look relative to the specified directory
            filenameBase = directory + filenameBase;
        }
    }

    Texture::Ref faceTextures[6];
    bool useCubeMap;

    // Even if there are cube map bugs, G3D knows how to work around them
    if (GLCaps::supports_GL_ARB_texture_cube_map()) {
        Texture::Settings   textureSettings;
        Texture::PreProcess texturePreProcess;
        
        textureSettings.wrapMode = WrapMode::CLAMP;
        textureSettings.interpolateMode = Texture::TRILINEAR_MIPMAP;
        texturePreProcess.scaleFactor = 1.0f / scaleDownFactor;

        if (_filename[1] == "") {
            // Specified one cube map
            faceTextures[0] = Texture::fromFile(
				filenameBase + "*" + filenameExt, 
				format, 
				Texture::DIM_CUBE_MAP,
				textureSettings,
				texturePreProcess);

        } else {
            // Specified six filenames
            faceTextures[0] = Texture::fromFile(
				_filename, 
				format, 
				Texture::DIM_CUBE_MAP,
				textureSettings,
				texturePreProcess);
        }

        // For the cube map case, we don't need the other five texture slots
        for (int t = 1; t < 6; ++t) {
            faceTextures[t] = NULL;
        }

        useCubeMap = true;

    } else {

        // This card doesn't support cube maps; it probably has low memory as well, so we
        // switch to bilinear instead of trilinear to save space on the mipmaps.
        Texture::Settings   textureSettings;
        Texture::PreProcess texturePreProcess;
   
        textureSettings.wrapMode = WrapMode::CLAMP;
        textureSettings.interpolateMode = Texture::BILINEAR_NO_MIPMAP;
        texturePreProcess.scaleFactor = 1.0f / scaleDownFactor;

        static const char* ext[] = {"up", "lf", "rt", "bk", "ft", "dn"};

        if (_filename[1] == "") {
            // Specified one cube map
            for (int t = 0; t < 6; ++t) {
                faceTextures[t] = Texture::fromFile(
                    filenameBase + ext[t] + filenameExt, 
                    format, 
                    Texture::DIM_2D, 
                    textureSettings, 
                    texturePreProcess);
            }
        } else {
            // Specified six textures
            for (int t = 0; t < 6; ++t) {
                faceTextures[t] = Texture::fromFile(
					_filename[t],
					format,
                    Texture::DIM_2D,
					textureSettings, 
					texturePreProcess);
            }
        }

        useCubeMap = false;
    }
    
    return new Sky(faceTextures, directory, useCubeMap, _drawCelestialBodies, quality);
}


Sky::Sky(
    Texture::Ref                          textures[6],
    const std::string&                  directory,
    bool                                useCubeMap,
    bool                                _drawCelestialBodies,
    double                              quality) :
    drawCelestialBodies(_drawCelestialBodies) {

    if (useCubeMap) {
        cubeMap = textures[0];
    } else {
        for (int i = 0; i < 6; ++i) {
            texture[i] = textures[i];
        }
    }

    const TextureFormat* format;
    const TextureFormat* alphaFormat;

    if (quality > .66) {
        format      = TextureFormat::RGB8();
        alphaFormat = TextureFormat::RGBA8();
    } else if (quality > .33) {
        format      = TextureFormat::RGBA_DXT1();
        alphaFormat = TextureFormat::RGBA_DXT5();
    } else {
        format      = TextureFormat::RGBA_DXT1();
        alphaFormat = TextureFormat::RGBA_DXT5();
    }

    if (drawCelestialBodies) {
        Texture::Settings textureSettings;

        textureSettings.wrapMode        = WrapMode::ZERO;
        textureSettings.interpolateMode = Texture::BILINEAR_NO_MIPMAP;

        moon     = Texture::fromTwoFiles(directory + "moon.jpg", directory + "moon-alpha.jpg", alphaFormat, Texture::DIM_2D, textureSettings);
        sun      = Texture::fromFile(directory + "sun.jpg", format, Texture::DIM_2D, textureSettings);		
        disk     = Texture::fromFile(directory + "lensflare.jpg", format, Texture::DIM_2D, textureSettings);
        sunRays  = Texture::fromFile(directory + "sun-rays.jpg", format, Texture::DIM_2D, textureSettings);
		
        int i = 0;

	    // If file exists, load the real starfield
        if (fileExists(directory + "real.str")) {
            BinaryInput in(directory + "real.str", G3D_LITTLE_ENDIAN, true);
            int16 numStars;
		    float32 x, y, z;

	        std::string header = in.readString(5);
	        debugAssert(header == "STARS"); (void)header;

	        numStars = in.readInt16();
	        star.resize(numStars);
	        starIntensity.resize(numStars);

                // Read X, Y, Z, and intensity
                for (i = 0; i < numStars; ++i) {
                    x = SHORT_TO_FLOAT(in.readInt16());
                    y = SHORT_TO_FLOAT(in.readInt16());
                    z = SHORT_TO_FLOAT(in.readInt16());

                    star[i] = Vector4(x, y, z, 0);

                    starIntensity[i] = square(SHORT_TO_FLOAT(in.readInt16())) + .3f;
	        }
        } else {
	    // Create a random starfield
            star.resize(3000);
    	    starIntensity.resize(star.size());
            for (i = star.size() - 1; i >= 0; --i) {
                star[i] = Vector4(Vector3::random(), 0);
                starIntensity[i] = square(uniformRandom()) + .3f;
            }
        }
    }
}


Sky::~Sky() {
}


/**
 Used to draw the sky, moon, and lens flare

 @param C center
 @param X a unit axis
 @param Y a unit axis perpendicular to X
 @param r radius of the sphere
 */
static void drawCelestialSphere(
    RenderDevice*                       renderDevice,
    const Vector4&                      C,
    const Vector4&                      X,
    const Vector4&                      Y,
    const double                        r,
    const Color4                        color) {

    renderDevice->setColor(color);
    renderDevice->beginPrimitive(RenderDevice::QUADS);
		renderDevice->setTexCoord(0, Vector2(0, 0));
        renderDevice->sendVertex(C + ( X + Y) * r);
        renderDevice->setTexCoord(0, Vector2(0, 1));
        renderDevice->sendVertex(C + ( X - Y) * r);
        renderDevice->setTexCoord(0, Vector2(1, 1));
		renderDevice->sendVertex(C + (-X - Y) * r);
        renderDevice->setTexCoord(0, Vector2(1, 0));
		renderDevice->sendVertex(C + (-X + Y) * r);
	renderDevice->endPrimitive();
}


static void hackProjectionMatrix(RenderDevice* renderDevice) {
	
    Matrix4 P = renderDevice->getProjectionMatrix();

    // Set the 3rd row (2nd index) so the depth always is in the middle of the depth range.

    P[2][0] = 0;
    P[2][1] = 0;
    P[2][2] = -0.5;
    P[2][3] = 0;

    renderDevice->setProjectionMatrix(P);
}


void Sky::vertex(RenderDevice* renderDevice, 
                 float x, float y, float z, float s, float t) const {
    const double w = 0;
    const bool cube = cubeMap.notNull();
    static bool explicitTexCoord = GLCaps::hasBug_normalMapTexGen();

    if (cube) {
        if (explicitTexCoord) {
            glTexCoord3f(x, y, z);
        } else {
            // Texcoord generation will move this normal to tex coord 0
            renderDevice->setNormal(Vector3(x,y,z));
        }
    } else {
        if (!GLCaps::supports_GL_EXT_texture_edge_clamp()) {
            // Move the edge coordinates towards the center just
            // enough that the black clamped border isn't sampled.
            if (s == 0) {
                s += (0.6f / (float)texture[0]->texelWidth());
            } else if (s == 1) {
                s -= (0.6f / (float)texture[0]->texelWidth());
            }
            if (t == 0) {
                t += (0.6f / (float)texture[0]->texelHeight());
            } else if (t == 1) {
                t -= (0.6f / (float)texture[0]->texelHeight());
            }
        }
        renderDevice->setTexCoord(0, Vector2(s, t));
    }
    
    renderDevice->sendVertex(Vector4(x,y,z,w));
}


void Sky::renderBox(RenderDevice* renderDevice) const {
    renderDevice->pushState();

    bool cube = (cubeMap.notNull());

    if (cube) {
        renderDevice->setTexture(0, cubeMap);

        if (! GLCaps::hasBug_normalMapTexGen()) {
            // On old Radeon Mobility, explicit cube map coordinates don't work right.
            // We instead put cube map coords in the normals and use texgen to copy
            // them over
            glActiveTextureARB(GL_TEXTURE0_ARB + 0);
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
            glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);
            glEnable(GL_TEXTURE_GEN_R);
        } else {
            // Hope that we're on a card where explicit texcoords work
        }

        CoordinateFrame cframe = renderDevice->getCameraToWorldMatrix();
        cframe.translation = Vector3::zero();
        renderDevice->setTextureMatrix(0, cframe);

    } else {
        // In the 6-texture case, the sky box is rotated 90 degrees
        // (this is because the textures are loaded incorrectly)
        CoordinateFrame cframe;
        cframe.rotation = Matrix3::fromAxisAngle(Vector3::unitY(), toRadians(-90));
        renderDevice->setObjectToWorldMatrix(cframe);
        renderDevice->setTexture(0, texture[BK]);
    }

    float s = 1;
    renderDevice->beginPrimitive(RenderDevice::QUADS);
        vertex(renderDevice, -s, +s, -s, 0, 0);
        vertex(renderDevice, -s, -s, -s, 0, 1);
        vertex(renderDevice, +s, -s, -s, 1, 1);
        vertex(renderDevice, +s, +s, -s, 1, 0);
	renderDevice->endPrimitive();
        
    if (! cube) {
        renderDevice->setTexture(0, texture[LT]);
    }

    renderDevice->beginPrimitive(RenderDevice::QUADS);
        vertex(renderDevice, -s, +s, +s, 0, 0);
        vertex(renderDevice, -s, -s, +s, 0, 1);
        vertex(renderDevice, -s, -s, -s, 1, 1);
        vertex(renderDevice, -s, +s, -s, 1, 0);
	renderDevice->endPrimitive();

    
    if (! cube) {
        renderDevice->setTexture(0, texture[FT]);
    }

    renderDevice->beginPrimitive(RenderDevice::QUADS);
        vertex(renderDevice, +s, +s, +s, 0, 0);
        vertex(renderDevice, +s, -s, +s, 0, 1);
        vertex(renderDevice, -s, -s, +s, 1, 1);
        vertex(renderDevice, -s, +s, +s, 1, 0);
	renderDevice->endPrimitive();

    if (! cube) {
        renderDevice->setTexture(0, texture[RT]);
    }

    renderDevice->beginPrimitive(RenderDevice::QUADS);
        vertex(renderDevice, +s, +s, +s, 1, 0);
        vertex(renderDevice, +s, +s, -s, 0, 0);
        vertex(renderDevice, +s, -s, -s, 0, 1);
        vertex(renderDevice, +s, -s, +s, 1, 1);
	renderDevice->endPrimitive();

    if (! cube) {
        renderDevice->setTexture(0, texture[UP]);
    }

    renderDevice->beginPrimitive(RenderDevice::QUADS);
        vertex(renderDevice, +s, +s, +s, 1, 1);
        vertex(renderDevice, -s, +s, +s, 1, 0);
        vertex(renderDevice, -s, +s, -s, 0, 0);
        vertex(renderDevice, +s, +s, -s, 0, 1);
	renderDevice->endPrimitive();

    if (! cube) {
        renderDevice->setTexture(0, texture[DN]);
    }

    renderDevice->beginPrimitive(RenderDevice::QUADS);
        vertex(renderDevice, +s, -s, -s, 0, 0);
        vertex(renderDevice, -s, -s, -s, 0, 1);
        vertex(renderDevice, -s, -s, +s, 1, 1);
        vertex(renderDevice, +s, -s, +s, 1, 0);
	renderDevice->endPrimitive();

    if (! GLCaps::hasBug_normalMapTexGen() && cube) {
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_GEN_R);
    }
    renderDevice->popState();

}


void Sky::render(
    RenderDevice*                   renderDevice,
    const SkyParameters&            lighting) {

    debugAssertGLOk();
    debugAssert(renderDevice != NULL);

    renderDevice->pushState();
        // Ignore depth, make sure we're not clipped by the far plane
        hackProjectionMatrix(renderDevice);

        // Eliminate the translation of the camera
        CoordinateFrame matrix(Vector3::zero());
	    matrix.rotation = renderDevice->getCameraToWorldMatrix().rotation;
        renderDevice->setCameraToWorldMatrix(matrix);
        renderDevice->setObjectToWorldMatrix(CoordinateFrame());

        renderDevice->disableLighting();
        renderDevice->setColor(lighting.skyAmbient);
        renderDevice->setCullFace(RenderDevice::CULL_BACK);
        renderDevice->setDepthWrite(false);
        renderDevice->setDepthTest(RenderDevice::DEPTH_ALWAYS_PASS);

        // Draw the sky box
        renderDevice->resetTextureUnit(0);
        debugAssertGLOk();
        renderBox(renderDevice);
        debugAssertGLOk();

        if (drawCelestialBodies) {   
            drawMoonAndStars(renderDevice, lighting);

            drawSun(renderDevice, lighting);
        }

    renderDevice->popState();
}


void Sky::drawMoonAndStars(
    RenderDevice*               renderDevice, 
    const SkyParameters&   lighting) {

    Vector3 moonPosition = lighting.physicallyCorrect ? lighting.trueMoonPosition : lighting.moonPosition;

    Vector4 L(moonPosition,0);
    Vector3 LcrossZ = moonPosition.cross(Vector3::unitZ()).direction();
	Vector4 X(LcrossZ, 0);
    Vector4 Y(moonPosition.cross(LcrossZ), 0);

    // Draw stars
    if (lighting.moonPosition.y > -0.3f) {

        float k = (3.001 - square(lighting.skyAmbient.length()));
		float s = k;
		renderDevice->pushState();
            // Rotate stars
			renderDevice->setObjectToWorldMatrix((lighting.physicallyCorrect ? lighting.trueStarFrame : lighting.starFrame));
			renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE);

            // We use raw GL calls here for performance since the changing point size
            // precludes the use of a vertex array.
            glPushAttrib(GL_POINT_BIT|GL_CURRENT_BIT);

                for (int i = star.size() - 1; i >= 0; --i) {
                    const float b = starIntensity[i] * k;
					glPointSize(starIntensity[i] * s);
					glBegin(GL_POINTS);
						glColor3f(b * lighting.emissiveScale.r, b * lighting.emissiveScale.g, b * lighting.emissiveScale.b);
						glVertex3fv(star[i]);
                    glEnd();
                }

            glPopAttrib();

        renderDevice->popState();
    }

    renderDevice->setTexture(0, moon);
    renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
    renderDevice->setAlphaTest(RenderDevice::ALPHA_GEQUAL, 0.05);
    drawCelestialSphere(renderDevice, L, X, Y, .06, Color4(lighting.emissiveScale, min(1.0f, max(0.0f, moonPosition.y * 4.0f))));
}


void Sky::drawSun(
    RenderDevice*               renderDevice, 
    const SkyParameters&   lighting) {

    Vector3 sunPosition = lighting.physicallyCorrect ? lighting.trueSunPosition : lighting.sunPosition;
	
    // Sun vector
    Vector4 L(sunPosition,0);
    Vector3 LcrossZ = sunPosition.cross(Vector3::unitZ()).direction();
    Vector4 X(LcrossZ, 0);
    Vector4 Y(sunPosition.cross(LcrossZ), 0);
    
    renderDevice->setTexture(0, sun);
    renderDevice->setBlendFunc(RenderDevice::BLEND_ONE, RenderDevice::BLEND_ONE);
    Color3 c(lighting.emissiveScale * .8f);

    if (sunPosition.y < 0) {
        // Fade out the sun as it goes below the horizon
        c *= max(0.0f, (sunPosition.y + 0.1f) * 10.0f);
    }

    drawCelestialSphere(renderDevice, L, X, Y, 0.12f, c);
}


void Sky::renderLensFlare(
    RenderDevice* renderDevice, 
    const SkyParameters&           lighting) {
	
    debugAssert(renderDevice != NULL);

    if (! drawCelestialBodies) {
        return;
    }

    Vector3 sunPosition = lighting.physicallyCorrect ? lighting.trueSunPosition : lighting.sunPosition;

    if (sunPosition.y < -.1) {
        return;
    }

    debugAssertGLOk();
    renderDevice->pushState();
        renderDevice->disableLighting();

        CoordinateFrame camera = renderDevice->getCameraToWorldMatrix();
	    CoordinateFrame matrix;
    	matrix.rotation = camera.rotation;
        renderDevice->setCameraToWorldMatrix(matrix);
	    renderDevice->setObjectToWorldMatrix(CoordinateFrame());

        renderDevice->setColor(lighting.skyAmbient);
        renderDevice->setCullFace(RenderDevice::CULL_BACK);
        renderDevice->setDepthWrite(false);
        renderDevice->setDepthTest(RenderDevice::DEPTH_ALWAYS_PASS);
        renderDevice->resetTextureUnit(0);
        debugAssertGLOk();

        // Compute the sun's position using the 3D transformation
        Vector4 pos = renderDevice->project(Vector4(sunPosition, 0));
        debugAssertGLOk();

        if (sunPosition.dot(camera.lookVector()) > 0) {

            // Number of visible points on the sun
            int visible = 0;
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    double distanceToSun = 
                        renderDevice->getDepthBufferValue(
                         iRound(pos.x + dx * 15.0),
                         iRound(pos.y + dy * 15.0));
                    visible += (distanceToSun >= .99999) ? 1 : 0;
                }
            }

            double fractionOfSunVisible = visible / 9.0;

            if (fractionOfSunVisible > 0.0) {

                // We need to switch to an infinite projection matrix
                // to draw the flares.  Note that we must make this
                // change *after* the depth buffer values have been
                // read back.
                hackProjectionMatrix(renderDevice);

                renderDevice->setBlendFunc(RenderDevice::BLEND_ONE,
                                           RenderDevice::BLEND_ONE);

                // Make flares fade out near sunset and sunrise
                float flareBrightness = sqrtf(max(sunPosition.y * 4.0f, 0.0f));

                // Sun position
                Vector4 L(sunPosition,0);
                Vector3 LcrossZ = sunPosition.cross(Vector3::unitZ()).direction();
                Vector4 X(LcrossZ, 0);
				Vector4 Y(sunPosition.cross(LcrossZ), 0);
                debugAssertGLOk();

                // Sun rays at dawn
                if ((sunPosition.x > 0) && 
                    (sunPosition.y >= -.1)) {

                    renderDevice->setTexture(0, sunRays);
                    double occlusionAttenuation = 
                        (1 - square(2*fractionOfSunVisible - 1));

                    Color4 col =
                        Color4(1,1,1,1) * (occlusionAttenuation *
                            0.4f * max(0.0f, min(1.0f, 1.0f - sunPosition.y * 2.0f / sqrtf(2.0f))));
                    drawCelestialSphere(renderDevice, L, X , Y, 0.6, col);
                }

                renderDevice->setTexture(0, sun);
                drawCelestialSphere(renderDevice, L, X, Y, .13f,
                                    lighting.emissiveScale * fractionOfSunVisible * .5f);

                // Lens flare
                Vector4 C(camera.lookVector(), 0);
                static const double position[] = { .5,                    .5,                    -.25,                     -.75,                .45,                      .6,                    -.5,                   -.1,                   .55,                     -1.5,                       -2,                         1};
                static const double size[]     = { .12,                   .05,                    .02,                      .02,                .02,                      .02,                    .01,                  .01,                   .01,                     .01,                        .01,                        0.05}; 
                static const Color3 color[]    = {Color3(6, 4, 0) / 255, Color3(6, 4, 0) / 255, Color3(0, 12, 0) / 255, Color3(0, 12, 0) / 255, Color3(0, 12, 0) / 255, Color3(0, 12, 0) / 255, Color3(10, 0, 0) /255,  Color3(0, 12, 0) / 255, Color3(10,0,0) / 255, Color3::fromARGB(0x192125)/10,   Color3::fromARGB(0x1F2B1D)/10, Color3::fromARGB(0x1F2B1D)/10};
                int numFlares     = 12;
                debugAssertGLOk();

                renderDevice->setTexture(0, disk);
                for (int i = 0; i < numFlares; ++i) {
                    drawCelestialSphere(renderDevice, 
                         C + (C - L) * (float)position[i], X, Y, (float)size[i], 
                         Color4(color[i] * lighting.emissiveScale * flareBrightness, 1));

                }
            }
        }

    renderDevice->popState();
}

} // namespace

