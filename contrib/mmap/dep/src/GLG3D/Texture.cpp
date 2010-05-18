/**
 @file Texture.cpp

 @author Morgan McGuire, morgan3d@graphics3d.com

 Notes:
 <UL>
 <LI>http://developer.apple.com/opengl/extensions/ext_texture_rectangle.html
 </UL>

 @created 2001-02-28
 @edited  2006-08-08
*/

#include "G3D/Log.h"
#include "G3D/Matrix3.h"
#include "G3D/Rect2D.h"
#include "G3D/GImage.h"
#include "G3D/fileutils.h"
#include "GLG3D/glcalls.h"
#include "GLG3D/TextureFormat.h"
#include "GLG3D/Texture.h"
#include "GLG3D/getOpenGLState.h"
#include "GLG3D/GLCaps.h"


namespace G3D {

static const char* cubeMapString[] = {"ft", "bk", "up", "dn", "rt", "lf"};

size_t Texture::m_sizeOfAllTexturesInMemory = 0;

/**
 Returns true if the system supports automatic MIP-map generation.
 */
static bool hasAutoMipMap();

/**
 Pushes all OpenGL texture state.
 */
// TODO: this is slow; push only absolutely necessary state
// or back up state that will be corrupted.
static void glStatePush();

/**
 Pops all OpenGL texture state.
 */
static void glStatePop() {
    glPopClientAttrib();
    glPopAttrib();
}

/**
 Scales the intensity up or down of an entire image.
 */
static void brightenImage(TextureFormat::Code fmt, void* byte, int n, float brighten);

static GLenum dimensionToTarget(Texture::Dimension d);

static void createTexture(
    GLenum          target,
    const uint8*    rawBytes,
    GLenum          bytesFormat,
    GLenum          bytesActualFormat,
    int             m_width,
    int             m_height,
    int             depth,
    GLenum          textureFormat,
    int             bytesPerPixel,
    int             mipLevel,
    bool            compressed,
    bool            useNPOT,
    float           rescaleFactor,
    GLenum          dataType);



static void createMipMapTexture(    
    GLenum          target,
    const uint8*    _bytes,
    int             bytesFormat,
    int             m_width,
    int             m_height,
    GLenum          textureFormat,
    size_t          bytesFormatBytesPerPixel,
    float           rescaleFactor,
    GLenum          dataType);


/**
 Overrides the current wrap and interpolation parameters for the
 current texture.
 */
static void setTexParameters(
    GLenum                          target,
    const Texture::Settings&        settings);

/////////////////////////////////////////////////////////////////////////////

Texture::Texture(
    const std::string&      name,
    GLuint                  textureID,
    Dimension               dimension,
    const TextureFormat*    format,
	bool					opaque,
	const Settings&			settings) :
    m_textureID(textureID),
	m_settings(settings),
    m_name(name),
    m_dimension(dimension),
    m_opaque(opaque),
    m_format(format) {

    debugAssert(m_format);
    debugAssertGLOk();

    glStatePush();

        GLenum target = dimensionToTarget(m_dimension);
        glBindTexture(target, m_textureID);

		// For cube map, we can't read "cube map" but must choose a face
		GLenum readbackTarget = target;
		if ((m_dimension == DIM_CUBE_MAP) || (m_dimension == DIM_CUBE_MAP_NPOT)) {
			readbackTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
		}

        glGetTexLevelParameteriv(readbackTarget, 0, GL_TEXTURE_WIDTH, &m_width);
        glGetTexLevelParameteriv(readbackTarget, 0, GL_TEXTURE_HEIGHT, &m_height);

        m_depth = 1;
        invertY = false;
        
        debugAssertGLOk();
        setTexParameters(target, m_settings);
        debugAssertGLOk();
    glStatePop();
    debugAssertGLOk();

    m_sizeOfAllTexturesInMemory += sizeInMemory();
}


Texture::Ref Texture::fromMemory(
    const std::string&              name,
    const void*                     bytes,
    const class TextureFormat*      bytesFormat,
    int                             m_width,
    int                             m_height,
    int                             depth,
    const class TextureFormat*      desiredFormat,
    Dimension                       dimension,
    const Settings&                 settings,
    const PreProcess&               preprocess) {

    Array< Array<const void*> > data(1);
    data[0].append(bytes);

    return fromMemory(
                      name, 
                      data,
                      bytesFormat, 
                      m_width, 
                      m_height, 
                      depth, 
                      desiredFormat,
                      dimension,
                      settings,
                      preprocess);
}


void Texture::setAutoMipMap(bool b) {
    m_settings.autoMipMap = b;

    // Update the OpenGL state
    GLenum target = dimensionToTarget(m_dimension);

    glPushAttrib(GL_TEXTURE_BIT);
    glBindTexture(target, m_textureID);

    if (hasAutoMipMap()) {
        glTexParameteri(target, GL_GENERATE_MIPMAP_SGIS, b ? GL_TRUE : GL_FALSE);
    }

    // Restore the old texture
    glPopAttrib();
}


Texture::Ref Texture::fromGLTexture(
    const std::string&      name,
    GLuint                  textureID,
    const TextureFormat*    textureFormat,
    Dimension               dimension,
	const Settings&			settings) {

    debugAssert(textureFormat);

    return new Texture(
		name, 
		textureID, 
		dimension,
		textureFormat, 
        textureFormat->opaque, 
		settings);
}


Texture::Ref Texture::fromFile(
    const std::string               filename[6],
    const class TextureFormat*      desiredFormat,
    Dimension                       dimension,
    const Settings&                 settings,
    const PreProcess&               preProcess) {
    
    std::string realFilename[6];

    const TextureFormat* format = TextureFormat::RGB8();
    bool opaque = true;

    // The six cube map faces, or the one texture and 5 dummys.
    GImage image[6];
    Array< Array< const void* > > byteMipMapFaces;

    const int numFaces = 
        ((dimension == DIM_CUBE_MAP) || (dimension == DIM_CUBE_MAP_NPOT)) ?
        6 : 1;

    // Check for DDS file and load separately.
    std::string ddsExt;

    // Find the period
    size_t period = filename[0].rfind('.');

    // Make sure it is before a slash!
    size_t j = iMax(filename[0].rfind('/'), filename[0].rfind('\\'));
    if ((period != std::string::npos) && (period > j)) {
        ddsExt = filename[0].substr(period + 1, filename[0].size() - period - 1);
    }

    if (G3D::toUpper(ddsExt) == "DDS") {

        debugAssertM(GLCaps::supports_GL_EXT_texture_compression_s3tc(),
            "This device does not support s3tc compression formats.");

        DDSTexture ddsTexture(filename[0]);

        uint8* byteStart = ddsTexture.getBytes();
        debugAssert( byteStart != NULL );

        const TextureFormat* bytesFormat = ddsTexture.getBytesFormat();
        debugAssert( bytesFormat );

        // Assert that we are loading a cubemap DDS
        debugAssert( numFaces == ddsTexture.getNumFaces() );

        int numMipMaps = ddsTexture.getNumMipMaps();
        int mapWidth   = ddsTexture.getWidth();
        int mapHeight  = ddsTexture.getHeight();

        byteMipMapFaces.resize(numMipMaps);

        for (int i = 0; i < numMipMaps; ++i) {
            
            byteMipMapFaces[i].resize(numFaces);

            for (int face = 0; face < numFaces; ++face) {
                byteMipMapFaces[i][face] = byteStart;
                byteStart += ((bytesFormat->packedBitsPerTexel / 8) * ((mapWidth + 3) / 4) * ((mapHeight + 3) / 4));
            }
            mapWidth = iMax(1, iFloor(mapWidth/2));
            mapHeight = iMax(1,iFloor(mapHeight/2));
        }

        return fromMemory(
			filename[0], 
			byteMipMapFaces,
			bytesFormat,
			ddsTexture.getWidth(), 
			ddsTexture.getHeight(), 
			1,
			desiredFormat,
			dimension,
			settings,
			preProcess);
    }

    // Single mip-map level
    byteMipMapFaces.resize(1);
    
    Array<const void*>& array = byteMipMapFaces[0];
    array.resize(numFaces);
    
    // Test for both DIM_CUBE_MAP and DIM_CUBE_MAP_NPOT
    if (numFaces == 6) {
        if (filename[1] == "") {
            // Wildcard format
            // Parse the filename into a base name and extension
            std::string filenameBase, filenameExt;
            splitFilenameAtWildCard(filename[0], filenameBase, filenameExt);
            for (int f = 0; f < 6; ++f) {
                realFilename[f] = filenameBase + cubeMapString[f] + filenameExt;
            }
        } else {
            // Separate filenames have been provided
            realFilename[0] = filename[0];
            for (int f = 1; f < 6; ++f) {
                debugAssert(filename[f] != "");
                realFilename[f] = filename[f];
            }
        }
    } else {
        debugAssertM(filename[1] == "",
            "Can't specify more than one filename unless loading a cube map");
        realFilename[0] = filename[0];
    }

    for (int f = 0; f < numFaces; ++f) {

        image[f].load(realFilename[f]);

        if (image[f].channels == 4) {
            format = TextureFormat::RGBA8();
            opaque = false;
        }

        if (desiredFormat == NULL) {
            desiredFormat = format;
        }

        array[f] = image[f].byte();
    }

    Texture::Ref t =
        fromMemory(
                   filename[0], 
                   byteMipMapFaces, 
                   format,
                   image[0].width, 
                   image[0].height, 
                   1,
                   desiredFormat, 
                   dimension,
                   settings,
                   preProcess);

    return t;
}


Texture::Ref Texture::fromFile(
    const std::string&      filename,
    const TextureFormat*    desiredFormat,
    Dimension               dimension,
    const Settings&         settings,
    const PreProcess&       preProcess) {

    std::string f[6];
    f[0] = filename;
    f[1] = "";
    f[2] = "";
    f[3] = "";
    f[4] = "";
    f[5] = "";

    return fromFile(f, desiredFormat, dimension, settings, preProcess);
}


Texture::Ref Texture::fromTwoFiles(
    const std::string&      filename,
    const std::string&      alphaFilename,
    const TextureFormat*    desiredFormat,
    Dimension               dimension,
	const Settings&			settings,
	const PreProcess&		preProcess) {

    debugAssert(desiredFormat);

    // The six cube map faces, or the one texture and 5 dummys.
    Array< Array<const void*> > mip(1);
	Array<const void*>& array = mip[0];

    const int numFaces = 
		((dimension == DIM_CUBE_MAP) || (dimension == DIM_CUBE_MAP_NPOT)) ? 
        6 : 1;

    array.resize(numFaces);
    for (int i = 0; i < numFaces; ++i) {
        array[i] = NULL;
    }

    // Parse the filename into a base name and extension
    std::string filenameBase = filename;
    std::string filenameExt;
    std::string alphaFilenameBase = alphaFilename;
    std::string alphaFilenameExt;

    // Test for both DIM_CUBE_MAP and DIM_CUBE_MAP_NPOT
    if (numFaces == 6) {
        splitFilenameAtWildCard(filename, filenameBase, filenameExt);
    }
    
    GImage color[6];
    GImage alpha[6];
    Texture::Ref t;

    try {
		for (int f = 0; f < numFaces; ++f) {

			std::string fn = filename;
			std::string an = alphaFilename;

			// Test for both DIM_CUBE_MAP and DIM_CUBE_MAP_NPOT
			if (numFaces == 6) {
				fn = filenameBase + cubeMapString[f] + filenameExt;
				an = alphaFilenameBase + cubeMapString[f] + alphaFilenameExt;
			}

			// Compose the two images to a single RGBA
			color[f].load(fn);
			alpha[f].load(an);
			uint8* data = NULL;

			if (color[f].channels == 4) {
				data = color[f].byte();
				// Write the data inline
				for (int i = 0; i < color[f].width * color[f].height; ++i) {
					data[i * 4 + 3] = alpha[f].byte()[i * alpha[f].channels];
				}
			} else {
				debugAssert(color[f].channels == 3);
				data = new uint8[color[f].width * color[f].height * 4];
				// Write the data inline
				for (int i = 0; i < color[f].width * color[f].height; ++i) {
					data[i * 4 + 0] = color[f].byte()[i * 3 + 0];
					data[i * 4 + 1] = color[f].byte()[i * 3 + 1];
					data[i * 4 + 2] = color[f].byte()[i * 3 + 2];
					data[i * 4 + 3] = alpha[f].byte()[i * alpha[f].channels];
				}
			}

			array[f] = data;
		}

		t = fromMemory(
				filename, 
				mip, 
				TextureFormat::RGBA8(),
				color[0].width, 
				color[0].height, 
				1, 
				desiredFormat, 
				dimension,
				settings,
				preProcess);

		if (color[0].channels == 3) {
			// Delete the data if it was dynamically allocated
			for (int f = 0; f < numFaces; ++f) {
				delete[] static_cast<uint8*>(const_cast<void*>(array[f]));
			}
		}

    } catch (const GImage::Error& e) {
        Log::common()->printf("\n**************************\n\n"
            "Loading \"%s\" failed. %s\n", e.filename.c_str(),
            e.reason.c_str());
    }

    return t;
}

static const GLenum cubeFaceTarget[] =
    {GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
     GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
     GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,
     GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
     GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,
     GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB};


static bool isMipMapformat(Texture::InterpolateMode i) {
    switch (i) {
    case Texture::TRILINEAR_MIPMAP:
    case Texture::BILINEAR_MIPMAP:
    case Texture::NEAREST_MIPMAP:
        return true;

    case Texture::BILINEAR_NO_MIPMAP:
    case Texture::NEAREST_NO_MIPMAP:
        return false;
    }

    alwaysAssertM(false, "Illegal interpolate mode");
    return false;
}


Texture::Ref Texture::fromMemory(
    const std::string&                  name,
    const Array< Array<const void*> >&  _bytes,
    const TextureFormat*                bytesFormat,
    int                                 m_width,
    int                                 m_height,
    int                                 depth,
    const TextureFormat*                desiredFormat,
    Dimension                           dimension,
    const Settings&                     settings,
    const PreProcess&                   preProcess) {

    typedef Array< Array<const void*> > MipArray;
    // Used for normal map computation
    GImage normal;
    
    // Indirection needed in case we have to reallocate our own
    // data for preprocessing.
    MipArray* bytesPtr = const_cast<MipArray*>(&_bytes);

    if (dimension == DIM_3D) {
        debugAssertM(
                     (settings.interpolateMode == BILINEAR_NO_MIPMAP) ||
                     (settings.interpolateMode == NEAREST_NO_MIPMAP), 
                     "DIM_3D textures do not support mipmaps");
        debugAssertM(_bytes.size() == 1,                    
                     "DIM_3D textures do not support mipmaps");
    } else {
        debugAssertM(depth == 1, "Depth must be 1 for all textures that are not DIM_3D");
    }

    if (preProcess.brighten != 1.0f) {

        debugAssert(
            (bytesFormat->code == TextureFormat::CODE_RGB8) ||
            (bytesFormat->code == TextureFormat::CODE_RGBA8));

        // Allow brightening to fail silently in release mode
        if (
            ( bytesFormat->code == TextureFormat::CODE_RGB8) ||
            ( bytesFormat->code == TextureFormat::CODE_RGBA8)) {

            bytesPtr = new MipArray;
            bytesPtr->resize(_bytes.size());

            for (int m = 0; m < bytesPtr->size(); ++m) {
                Array<const void*>& face = (*bytesPtr)[m]; 
                face.resize(_bytes[m].size());
            
                for (int f = 0; f < face.size(); ++f) {

                    size_t numBytes = iCeil(m_width * m_height * depth * bytesFormat->packedBitsPerTexel / 8.0f);

                    // Allocate space for the converted image
                    face[f] = System::alignedMalloc(numBytes, 16);

                    // Copy the original data
                    System::memcpy(const_cast<void*>(face[f]), _bytes[m][f], numBytes);

                    // Apply the processing to the copy
                    brightenImage(
                        bytesFormat->code,
                        const_cast<void*>(face[f]),
                        numBytes,
                        preProcess.brighten);
                }
            }
        }
    }

    if (preProcess.computeNormalMap) {
        debugAssertM(bytesFormat->redBits == 8 || bytesFormat->luminanceBits == 8, "To preprocess a texture with normal maps, 8-bit channels are required");
        debugAssertM(bytesFormat->compressed == false, "Cannot compute normal maps from compressed textures");
        debugAssertM(bytesFormat->floatingPoint == false, "Cannot compute normal maps from floating point textures");
        debugAssertM(bytesFormat->numComponents == 1 || bytesFormat->numComponents == 3 || bytesFormat->numComponents == 4, "1, 3, or 4 channels needed to compute normal maps");
        debugAssertM(bytesPtr->size() == 1, "Cannot specify mipmaps when computing normal maps automatically");

        GImage::computeNormalMap(m_width, m_height, bytesFormat->numComponents, 
                                 reinterpret_cast<const uint8*>((*bytesPtr)[0][0]),
                                 normal, preProcess.normalMapWhiteHeightInPixels, 
                                 preProcess.normalMapLowPassBump, preProcess.normalMapScaleHeightByNz);
        
        // Replace the previous array with the data from our normal map
        bytesPtr = new MipArray;
        bytesPtr->resize(1);
        (*bytesPtr)[0].append(normal.byte());
        
        bytesFormat = TextureFormat::RGBA8();
    }

    debugAssert(bytesFormat);
    (void)depth;
    
    // Check for at least one miplevel on the incoming data
    int numMipMaps = bytesPtr->length();
    debugAssert( numMipMaps > 0 );

    // Create the texture
    GLuint textureID = newGLTextureID();
    GLenum target = dimensionToTarget(dimension);

    if ((desiredFormat == TextureFormat::AUTO()) || (bytesFormat->compressed)) {
        desiredFormat = bytesFormat;
    }

    if (GLCaps::hasBug_redBlueMipmapSwap() && (desiredFormat == TextureFormat::RGB8())) {
        desiredFormat = TextureFormat::RGBA8();
    }

    debugAssertM(GLCaps::supports(desiredFormat), "Unsupported texture format.");

    glStatePush();

        // Set unpacking alignment
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glEnable(target);
        glBindTexture(target, textureID);
        debugAssertGLOk();
        if (isMipMapformat(settings.interpolateMode) && hasAutoMipMap() && (numMipMaps == 1)) {
            // Enable hardware MIP-map generation.
            // Must enable before setting the level 0 image (we'll set it again
            // in setTexParameters, but that is intended primarily for 
            // the case where that function is called for a pre-existing GL texture ID).
            glTexParameteri(target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        }

        int mipWidth = m_width;
        int mipHeight = m_height;
        for (int mipLevel = 0; mipLevel < numMipMaps; ++mipLevel) {

            const int numFaces = (*bytesPtr)[mipLevel].length();
            
            debugAssert( (((dimension == DIM_CUBE_MAP) || (dimension == DIM_CUBE_MAP_NPOT)) ? 6 : 1) == numFaces);
        
            for (int f = 0; f < numFaces; ++f) {
        
                // Test for both DIM_CUBE_MAP and DIM_CUBE_MAP_NPOT
                if (numFaces == 6) {
                    // Choose the appropriate face target
                    target = cubeFaceTarget[f];
                }

                if (isMipMapformat(settings.interpolateMode) && ! hasAutoMipMap() && (numMipMaps == 1)) {

                    debugAssertM((bytesFormat->compressed == false), 
                                 "Cannot manually generate Mip-Maps for compressed textures.");

                    createMipMapTexture(
                                        target, 
                                        reinterpret_cast<const uint8*>((*bytesPtr)[mipLevel][f]),
                                        bytesFormat->openGLFormat,
                                        mipWidth, 
                                        mipHeight, 
                                        desiredFormat->openGLFormat,
                                        desiredFormat->packedBitsPerTexel / 8, 
                                        preProcess.scaleFactor,
                                        bytesFormat->openGLDataFormat);
                    
                } else {

                    const bool useNPOT = (dimension == DIM_2D_NPOT) || (dimension == DIM_CUBE_MAP_NPOT);

                    createTexture(
                                  target, 
                                  reinterpret_cast<const uint8*>((*bytesPtr)[mipLevel][f]), 
                                  bytesFormat->openGLBaseFormat,
                                  bytesFormat->openGLFormat, 
                                  mipWidth, 
                                  mipHeight, 
                                  depth,
                                  desiredFormat->openGLFormat, 
                                  bytesFormat->packedBitsPerTexel / 8, 
                                  mipLevel, 
                                  bytesFormat->compressed, 
                                  useNPOT, 
                                  preProcess.scaleFactor,
                                  bytesFormat->openGLDataFormat);
                }

                debugAssertGLOk();
            }

            mipWidth = iMax(1, mipWidth / 2);
            mipHeight = iMax(1, mipHeight / 2);
        }
    glStatePop();

    if ((dimension != DIM_2D_RECT) &&
        ((dimension != DIM_2D_NPOT && (dimension != DIM_CUBE_MAP_NPOT)))) {
        m_width  = ceilPow2(m_width);
        m_height = ceilPow2(m_height);
    }

    debugAssertGLOk();
    Texture::Ref t = fromGLTexture(name, textureID, desiredFormat, dimension, settings);
    debugAssertGLOk();

    t->m_width = m_width;
    t->m_height = m_height;

    if (bytesPtr != &_bytes) {

        // We must free our own data
        if (normal.width != 0) {
            // The normal GImage is holding the data; do not free it because 
            // the GImage destructor will do so at the end of the method.
        } else {
            for (int m = 0; m < bytesPtr->size(); ++m) {
                Array<const void*>& face = (*bytesPtr)[m]; 
                for (int f = 0; f < face.size(); ++f) {
                    System::alignedFree(const_cast<void*>(face[f]));
                }
            }
        }
        delete bytesPtr;
        bytesPtr = NULL;
    }

    return t;
}


Texture::Ref Texture::fromGImage(
    const std::string&              name,
    const GImage&                   image,
    const class TextureFormat*      desiredFormat,
    Dimension                       dimension,
	const Settings&					settings,
	const PreProcess&				preProcess) {

    const TextureFormat* format = TextureFormat::RGB8();
    bool opaque = true;

    switch (image.channels) {
    case 4:
        format = TextureFormat::RGBA8();
        opaque = false;
        break;

    case 3:
        format = TextureFormat::RGB8();
        opaque = true;
        break;

    case 1:
        format = TextureFormat::L8();
        opaque = true;
        break;

    default:
        alwaysAssertM(
            false,
            G3D::format("GImage has an unexpected number of channels (%d)", image.channels));
    }

    if (desiredFormat == NULL) {
        desiredFormat = format;
    }

    Texture::Ref t =
        fromMemory(
			name, 
			image.byte(), 
			format,
            image.width, 
			image.height, 
			1,
            desiredFormat, 
			dimension, 
			settings,
			preProcess);

    return t;
}


Texture::Ref Texture::createEmpty(
    const std::string&               name,
    int                              w,
    int                              h,
    const TextureFormat*             desiredFormat,
    Dimension                        dimension,
	const Settings&					 settings) {

    debugAssertGLOk();
    debugAssertM(desiredFormat, "desiredFormat may not be TextureFormat::AUTO()");

    // We must pretend the input is in the desired format otherwise 
    // OpenGL might refuse to negotiate formats for us.
    Array<uint8> data(w * h * desiredFormat->packedBitsPerTexel / 8);

    Texture::Ref t = 
		fromMemory(
			name, 
			data.getCArray(), 
			desiredFormat, 
			w, 
			h, 
			1, 
			desiredFormat, 
			dimension, 
			settings);

    // The only purpose of creating an empty texture is to render to it with FBO/readback,
    // so clearly the caller will want Y inverted.
    t->invertY = true;

    debugAssertGLOk();
    return t;
}


Rect2D Texture::rect2DBounds() const {
    return Rect2D::xywh(0, 0, (float)m_width, (float)m_height);
}


const Texture::Settings& Texture::settings() const {
    return m_settings;
}


void Texture::getImage(GImage& dst, const TextureFormat* outFormat) const {
    alwaysAssertM(outFormat == TextureFormat::AUTO() ||
                  outFormat == TextureFormat::RGB8() ||
                  outFormat == TextureFormat::RGBA8() ||
                  outFormat == TextureFormat::L8() ||
                  outFormat == TextureFormat::A8(), "Illegal texture format.");

    if (outFormat == TextureFormat::AUTO()) {
        switch(format()->openGLBaseFormat) { 
        case GL_ALPHA:
            outFormat = TextureFormat::A8();
            break;

        case GL_LUMINANCE:
            outFormat = TextureFormat::L8();
            break;

        case GL_RGB:
            outFormat = TextureFormat::RGB8();
            break;

        case GL_RGBA:
            // Fall through intentionally
        default:
            outFormat = TextureFormat::RGBA8();
            break;
        }
    }

    int channels = 0;


    switch(outFormat->openGLBaseFormat) {
    case GL_LUMINANCE:
    case GL_ALPHA:
        channels = 1;
        break;

    case GL_RGB:
        channels = 3;
        break;

    case GL_RGBA:
        channels = 4;
        break;

    default:
        alwaysAssertM(false, "This texture format is not appropriate for reading to an image.");
    }

    dst.resize(m_width, m_height, channels);

	getTexImage(dst.byte(), outFormat);
}


void Texture::getTexImage(void* data, const TextureFormat* desiredFormat) const {
    GLenum target = dimensionToTarget(m_dimension);

    glPushAttrib(GL_TEXTURE_BIT);
    glBindTexture(target, m_textureID);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    glGetTexImage(
       target,
       0,
       desiredFormat->openGLBaseFormat,
       desiredFormat->openGLDataFormat,
       data);

    glPopAttrib();
}

Image4Ref Texture::toImage4() const {
	Image4Ref im = Image4::createEmpty(m_width, m_height); 
	getTexImage(im->getCArray(), TextureFormat::RGBA32F());
	return im;
}

Image4uint8Ref Texture::toImage4uint8() const {
	Image4uint8Ref im = Image4uint8::createEmpty(m_width, m_height); 
	getTexImage(im->getCArray(), TextureFormat::RGBA8());
	return im;
}

Image3Ref Texture::toImage3() const {	
	Image3Ref im = Image3::createEmpty(m_width, m_height); 
	getTexImage(im->getCArray(), TextureFormat::RGB32F());
	return im;
}

Image3uint8Ref Texture::toImage3uint8() const {	
	Image3uint8Ref im = Image3uint8::createEmpty(m_width, m_height); 
	getTexImage(im->getCArray(), TextureFormat::RGB8());
	return im;
}

Map2D<float>::Ref Texture::toDepthMap() const {
	Map2D<float>::Ref im = Map2D<float>::create(m_width, m_height); 
	getTexImage(im->getCArray(), TextureFormat::DEPTH32F());
	return im;
}

Image1Ref Texture::toDepthImage1() const {
	Image1Ref im = Image1::createEmpty(m_width, m_height);
	getTexImage(im->getCArray(), TextureFormat::DEPTH32F());
	return im;
}

Image1uint8Ref Texture::toDepthImage1uint8() const {
	Image1Ref src = toDepthImage1();
	Image1uint8Ref dst = Image1uint8::createEmpty(m_width, m_height);

	const Color1* s = src->getCArray();
	Color1uint8* d = dst->getCArray();

	// Float to int conversion
	for (int i = m_width * m_height - 1; i >= 0; --i) {
		d[i] = s[i];
	}

	return dst;
}

Image1Ref Texture::toImage1() const {
	Image1Ref im = Image1::createEmpty(m_width, m_height); 
	getTexImage(im->getCArray(), TextureFormat::L32F());
	return im;
}

Image1uint8Ref Texture::toImage1uint8() const {
	Image1uint8Ref im = Image1uint8::createEmpty(m_width, m_height); 
	getTexImage(im->getCArray(), TextureFormat::L8());
	return im;
}

void Texture::splitFilenameAtWildCard(
    const std::string&  filename,
    std::string&        filenameBase,
    std::string&        filenameExt) {

    const std::string splitter = "*";

    int i = filename.rfind(splitter);
    if (i != -1) {
        filenameBase = filename.substr(0, i);
        filenameExt  = filename.substr(i + 1, filename.size() - i - splitter.length()); 
    } else {
        throw GImage::Error("Cube map filenames must contain \"*\" as a "
                            "placeholder for up/lf/rt/bk/ft/dn", filename);
    }
}


bool Texture::isSupportedImage(const std::string& filename) {
	// Reminder: this looks in zipfiles as well
	if (!fileExists(filename)) {
		return false;
	}

	std::string ext = toLower(filenameExt(filename));

	if ((ext == "jpg") ||	
		(ext == "ico") ||
		(ext == "dds") ||
		(ext == "png") ||
		(ext == "tga") || 
		(ext == "bmp") ||
		(ext == "ppm") ||
		(ext == "pcx")) {
			return true;
	} else {
		return false;
	}
}


Texture::~Texture() {
    m_sizeOfAllTexturesInMemory -= sizeInMemory();
	glDeleteTextures(1, &m_textureID);
	m_textureID = 0;
}


uint32 Texture::newGLTextureID() {
    uint32 t;

    // Clear the OpenGL error flag
    glGetError();

    glGenTextures(1, &t);

    alwaysAssertM(glGetError() != GL_INVALID_OPERATION, 
         "GL_INVALID_OPERATION: Probably caused by invoking "
         "glGenTextures between glBegin and glEnd.");

    return t;
}

/** Returns the buffer constant that matches the current draw buffer (left vs. right) */
static GLenum getCurrentBuffer(bool useBack) {
    GLenum draw = glGetInteger(GL_DRAW_BUFFER);

    if (useBack) {
        switch (draw) {
        case GL_FRONT_LEFT:
        case GL_BACK_LEFT:
            return GL_BACK_LEFT;

        case GL_FRONT_RIGHT:
        case GL_BACK_RIGHT:
            return GL_BACK_RIGHT;

        default:
			// FBO goes here
            return draw;
        }
    } else {
        switch (draw) {
        case GL_FRONT_LEFT:
        case GL_BACK_LEFT:
            return GL_FRONT_LEFT;

        case GL_FRONT_RIGHT:
        case GL_BACK_RIGHT:
            return GL_FRONT_RIGHT;

        default:
			// FBO goes here
            return draw;
        }
    }
}


void Texture::copyFromScreen(
    const Rect2D& rect,
    bool useBackBuffer) {

    glStatePush();

    glReadBuffer(getCurrentBuffer(useBackBuffer));

    m_sizeOfAllTexturesInMemory -= sizeInMemory();

    // Set up new state
    m_width   = (int)rect.width();
    m_height  = (int)rect.height();
    m_depth   = 1;
    debugAssert(m_dimension == DIM_2D || m_dimension == DIM_2D_RECT || m_dimension == DIM_2D_NPOT);

    if (GLCaps::supports_GL_ARB_multitexture()) {
        glActiveTextureARB(GL_TEXTURE0_ARB);
    }
    glDisableAllTextures();
    GLenum target = dimensionToTarget(m_dimension);
    glEnable(target);

    glBindTexture(target, m_textureID);
    int e = glGetError();
    alwaysAssertM(e == GL_NONE, 
        std::string("Error encountered during glBindTexture: ") + GLenumToString(e));

    double viewport[4];
    glGetDoublev(GL_VIEWPORT, viewport);
    double viewportHeight = viewport[3];
    debugAssertGLOk();
    
    glCopyTexImage2D(target, 0, format()->openGLFormat,
                     iRound(rect.x0()), 
                     iRound(viewportHeight - rect.y1()), 
                     iRound(rect.width()), iRound(rect.height()), 
                     0);

    debugAssertGLOk();
    // Reset the original properties
    setTexParameters(target, m_settings);

    debugAssertGLOk();
    glDisable(target);

    // Once copied from the screen, the direction will be reversed.
    invertY = true;

    glStatePop();

    m_sizeOfAllTexturesInMemory += sizeInMemory();
}


void Texture::copyFromScreen(
    const Rect2D&       rect,
    CubeFace            face,
    bool                useBackBuffer) {

    glStatePush();

    glReadBuffer(getCurrentBuffer(useBackBuffer));

    // Set up new state
    debugAssertM(m_width == rect.width(), "Cube maps require all six faces to have the same dimensions");
    debugAssertM(m_height == rect.height(), "Cube maps require all six faces to have the same dimensions");
    debugAssert(m_dimension == DIM_CUBE_MAP || m_dimension == DIM_CUBE_MAP_NPOT);
    debugAssert(face >= 0);
    debugAssert(face < 6);

    if (GLCaps::supports_GL_ARB_multitexture()) {
        glActiveTextureARB(GL_TEXTURE0_ARB);
    }
    glDisableAllTextures();

    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, m_textureID);

    GLenum target = cubeFaceTarget[(int)face];

    int e = glGetError();
    alwaysAssertM(e == GL_NONE, 
        std::string("Error encountered during glBindTexture: ") + GLenumToString(e));

    double viewport[4];
    glGetDoublev(GL_VIEWPORT, viewport);
    double viewportHeight = viewport[3];
    debugAssertGLOk();

    glCopyTexImage2D(target, 0, format()->openGLFormat,
                     iRound(rect.x0()), 
                     iRound(viewportHeight - rect.y1()), 
                     iRound(rect.width()), 
                     iRound(rect.height()), 0);

    debugAssertGLOk();
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    glStatePop();
}


void Texture::getCameraRotation(CubeFace face, Matrix3& outMatrix) {
    switch (face) {
    case CUBE_POS_X:
        outMatrix = Matrix3::fromEulerAnglesYXZ((float)halfPi(), (float)pi(), 0);
        break;

    case CUBE_NEG_X:
        outMatrix = Matrix3::fromEulerAnglesYXZ(-(float)halfPi(), (float)pi(), 0);
        break;

    case CUBE_POS_Y:
        outMatrix = Matrix3::fromEulerAnglesXYZ((float)halfPi(), 0, 0);
        break;

    case CUBE_NEG_Y:
        outMatrix = Matrix3::fromEulerAnglesXYZ(-(float)halfPi(), 0, 0);
        break;

    case CUBE_POS_Z:
        outMatrix = Matrix3::fromEulerAnglesYZX((float)pi(), (float)pi(), 0);
        break;

    case CUBE_NEG_Z:
        outMatrix = Matrix3::fromAxisAngle(Vector3::unitZ(), (float)pi());
        break;
    }
}


size_t Texture::sizeInMemory() const {

    int base = (m_width * m_height * m_depth * m_format->hardwareBitsPerTexel) / 8;

    int total = 0;

    if (m_settings.interpolateMode == TRILINEAR_MIPMAP) {
        int w = m_width;
        int h = m_height;

        while ((w > 2) && (h > 2)) {
            total += base;
            base /= 4;
            w /= 2;
            h /= 2;
        }

    } else {
        total = base;
    }

    if (m_dimension == DIM_CUBE_MAP) {
        total *= 6;
    }

    return total;
}


unsigned int Texture::openGLTextureTarget() const {
    switch (m_dimension) {
    case DIM_CUBE_MAP_NPOT:
    case DIM_CUBE_MAP:
        return GL_TEXTURE_CUBE_MAP_ARB;

    case DIM_2D_NPOT:
    case DIM_2D:
        return GL_TEXTURE_2D;

    case Texture::DIM_2D_RECT:
        return GL_TEXTURE_RECTANGLE_EXT;

    default:
        debugAssertM(false, "Fell through switch");
    }
    return 0;
}


Texture::Ref Texture::alphaOnlyVersion() const {
    if (opaque()) {
        return NULL;
    }

    debugAssert(m_settings.depthReadMode == DEPTH_NORMAL);
    debugAssertM(
        m_dimension == DIM_2D ||
        m_dimension == DIM_2D_RECT ||
        m_dimension == DIM_2D_NPOT,
        "alphaOnlyVersion only supported for 2D textures");

    int numFaces = 1;

	Array< Array<const void*> > mip(1);
	Array<const void*>& bytes = mip[0];
	bytes.resize(numFaces);
    const TextureFormat* bytesFormat = TextureFormat::A8();

    glStatePush();
    // Setup to later implement cube faces
    for (int f = 0; f < numFaces; ++f) {
        GLenum target = dimensionToTarget(m_dimension);
        glBindTexture(target, m_textureID);
        bytes[f] = (const void*)System::malloc(m_width * m_height);
        glGetTexImage(target, 0, GL_ALPHA, GL_UNSIGNED_BYTE, const_cast<void*>(bytes[f]));
    }

    glStatePop();

    Texture::Ref ret = 
        fromMemory(
			m_name + " Alpha", 
			mip,
			bytesFormat,
            m_width, 
			m_height, 
			1, 
			TextureFormat::A8(),
            m_dimension, 
			m_settings);

    for (int f = 0; f < numFaces; ++f) {
        System::free(const_cast<void*>(bytes[f]));
    }

    return ret;
}

//////////////////////////////////////////////////////////////////////////////////

static void setTexParameters(
    GLenum                          target,
    const Texture::Settings&        settings) {
    debugAssert(
        target == GL_TEXTURE_2D ||
        target == GL_TEXTURE_RECTANGLE_EXT ||
        target == GL_TEXTURE_CUBE_MAP_ARB);

    debugAssertGLOk();

    // Set the wrap and interpolate state

    bool supports3D = GLCaps::supports_GL_EXT_texture3D();
    GLenum mode = GL_NONE;
    
    switch (settings.wrapMode) {
    case WrapMode::TILE:
        mode = GL_REPEAT;
        break;

    case WrapMode::CLAMP:  
        if (GLCaps::supports_GL_EXT_texture_edge_clamp()) {
            mode = GL_CLAMP_TO_EDGE;
        } else {
            mode = GL_CLAMP;
        }
      break;

    case WrapMode::ZERO:
        if (GLCaps::supports_GL_ARB_texture_border_clamp()) {
            mode = GL_CLAMP_TO_BORDER_ARB;
        } else {
            mode = GL_CLAMP;
        }
        glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, Color4::clear());
        debugAssertGLOk();
      break;

    default:
        debugAssertM(Texture::supportsWrapMode(settings.wrapMode), "Unsupported wrap mode for Texture");
    }
    
    glTexParameteri(target, GL_TEXTURE_WRAP_S, mode);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, mode);
    if (supports3D) {
        glTexParameteri(target, GL_TEXTURE_WRAP_R, mode);
    }

    debugAssertGLOk();

    bool hasMipMaps = 
        (target != GL_TEXTURE_RECTANGLE_EXT) &&
        (settings.interpolateMode != Texture::BILINEAR_NO_MIPMAP) &&
        (settings.interpolateMode != Texture::NEAREST_NO_MIPMAP);

    if (hasMipMaps &&
        (GLCaps::supports("GL_EXT_texture_lod") || 
         GLCaps::supports("GL_SGIS_texture_lod"))) {

        // I can find no documentation for GL_EXT_texture_lod even though many cards claim to support it - Morgan 6/30/06
        glTexParameteri(target, GL_TEXTURE_MAX_LOD_SGIS, settings.maxMipMap);
        glTexParameteri(target, GL_TEXTURE_MIN_LOD_SGIS, settings.minMipMap);
    }


    switch (settings.interpolateMode) {
    case Texture::TRILINEAR_MIPMAP:
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        if (hasAutoMipMap()) {  
            glTexParameteri(target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        }
        break;

    case Texture::BILINEAR_MIPMAP:
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

        if (hasAutoMipMap() && settings.autoMipMap) {  
            glTexParameteri(target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        }
        break;

    case Texture::NEAREST_MIPMAP:
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

        if (hasAutoMipMap() && settings.autoMipMap) {  
            glTexParameteri(target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        }
        break;

    case Texture::BILINEAR_NO_MIPMAP:
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
        break;

    case Texture::NEAREST_NO_MIPMAP:
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        break;

    default:
        debugAssert(false);
    }
    debugAssertGLOk();

    static const bool anisotropic = GLCaps::supports("GL_EXT_texture_filter_anisotropic");

    if (anisotropic) {
        glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, settings.maxAnisotropy);
    }

    if (GLCaps::supports_GL_ARB_shadow()) {
        if (settings.depthReadMode == Texture::DEPTH_NORMAL) {
            glTexParameteri(target, GL_TEXTURE_COMPARE_MODE_ARB, GL_NONE);
        } else {
            //glTexParameteri(target, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
            glTexParameteri(target, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
            glTexParameteri(target, GL_TEXTURE_COMPARE_FUNC_ARB, 
                (settings.depthReadMode == Texture::DEPTH_LEQUAL) ? GL_LEQUAL : GL_GEQUAL);
        }

    }
    debugAssertGLOk();
}


static void glStatePush() {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_ALL_CLIENT_ATTRIB_BITS);

    if (GLCaps::supports_GL_ARB_multitexture()) {
        glActiveTextureARB(GL_TEXTURE0_ARB);
    }
}


static bool hasAutoMipMap() {
    static bool initialized = false;
    static bool ham = false;

    if (! initialized) {
        initialized = true;
        std::string ext = (char*)glGetString(GL_EXTENSIONS);
        ham = (ext.find("GL_SGIS_generate_mipmap") != std::string::npos) &&
            ! GLCaps::hasBug_mipmapGeneration() &&
            ! GLCaps::hasBug_redBlueMipmapSwap();
    }

    return ham;
}



static GLenum dimensionToTarget(Texture::Dimension d) {
    switch (d) {
    case Texture::DIM_CUBE_MAP_NPOT:
    case Texture::DIM_CUBE_MAP:
        return GL_TEXTURE_CUBE_MAP_ARB;
        
    case Texture::DIM_3D:
        return GL_TEXTURE_3D;

    case Texture::DIM_2D_NPOT:
    case Texture::DIM_2D:
        return GL_TEXTURE_2D;

    case Texture::DIM_2D_RECT:
        return GL_TEXTURE_RECTANGLE_EXT;

    default:
        debugAssert(false);
        return GL_TEXTURE_2D;
    }
}

/** 
   @param bytesFormat OpenGL base format.
   @param bytesActualFormat OpenGL true format.  For compressed data, distinguishes the format that the data has due to compression.
 
    @param dataType Type of the incoming data from the CPU, e.g. GL_UNSIGNED_BYTES */
static void createTexture(
    GLenum          target,
    const uint8*    rawBytes,
    GLenum          bytesFormat,
    GLenum          bytesActualFormat,
    int             m_width,
    int             m_height,
    int             depth,
    GLenum          textureFormat,
    int             bytesPerPixel,
    int             mipLevel,
    bool            compressed,
    bool            useNPOT,
    float           rescaleFactor,
    GLenum          dataType) {

    uint8* bytes = const_cast<uint8*>(rawBytes);

    // If true, we're supposed to free the byte array at the end of
    // the function.
    bool   freeBytes = false; 

    switch (target) {
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB:
    case GL_TEXTURE_2D:
        if ((! isPow2(m_width) || ! isPow2(m_height)) &&
            (! useNPOT || ! GLCaps::supports_GL_ARB_texture_non_power_of_two())) {
            // NPOT texture with useNPOT disabled: resize to a power of two

            debugAssertM(! compressed,
                "This device does not support NPOT compressed textures.");

            int oldWidth = m_width;
            int oldHeight = m_height;
            m_width  = ceilPow2(static_cast<unsigned int>(m_width * rescaleFactor));
            m_height = ceilPow2(static_cast<unsigned int>(m_height * rescaleFactor));

            bytes = new uint8[m_width * m_height * bytesPerPixel];
            freeBytes = true;

            // Rescale the image to a power of 2
            gluScaleImage(
                bytesFormat,
                oldWidth,
                oldHeight,
                dataType,
                rawBytes,
                m_width,
                m_height,
                dataType,
                bytes);

        }

        // Intentionally fall through for power of 2 case

    case GL_TEXTURE_RECTANGLE_EXT:

        // Note code falling through from above

        if (compressed) {
            
            debugAssertM((target != GL_TEXTURE_RECTANGLE_EXT),
                "Compressed textures must be DIM_2D.");

            glCompressedTexImage2DARB(target, mipLevel, bytesActualFormat, m_width, 
                m_height, 0, (bytesPerPixel * ((m_width + 3) / 4) * ((m_height + 3) / 4)), rawBytes);

        } else {

            debugAssert(isValidPointer(bytes));
            debugAssertM(isValidPointer(bytes + (m_width * m_height - 1) * bytesPerPixel), 
                "Byte array in Texture creation was too small");

            // 2D texture, level of detail 0 (normal), internal format, x size from image, y size from image, 
            // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
            glTexImage2D(target, mipLevel, textureFormat, m_width, m_height, 0, bytesFormat, dataType, bytes);
        }
        break;

    case GL_TEXTURE_3D:
        // Can't rescale, so ensure that the texture is a power of two in each dimension
        debugAssertM(isPow2(m_width) && isPow2(m_height) && isPow2(depth),
                     "DIM_3D textures must be a power of two size in each dimension.");

        debugAssert(isValidPointer(bytes));
        glTexImage3DEXT(target, mipLevel, textureFormat, m_width, m_height, depth, 0, bytesFormat, dataType, bytes);
        break;

    default:
        debugAssertM(false, "Fell through switch");
    }

    if (freeBytes) {
        // Texture was resized; free the temporary.
        delete[] bytes;
    }
}


static void createMipMapTexture(    
    GLenum          target,
    const uint8*    _bytes,
    int             bytesFormat,
    int             m_width,
    int             m_height,
    GLenum          textureFormat,
    size_t          bytesFormatBytesPerPixel,
    float           rescaleFactor,
    GLenum          dataType) {

    switch (target) {
    case GL_TEXTURE_2D:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB:
        {
            bool freeBytes = false;
            const uint8* bytes = _bytes;

            if (rescaleFactor != 1.0f) {
                int oldWidth = m_width;
                int oldHeight = m_height;
                m_width  = ceilPow2(static_cast<unsigned int>(m_width * rescaleFactor));
                m_height = ceilPow2(static_cast<unsigned int>(m_height * rescaleFactor));

                bytes = new uint8[m_width * m_height * bytesFormatBytesPerPixel];
                freeBytes = true;

                // Rescale the image to a power of 2
                gluScaleImage(
                    bytesFormat,
                    oldWidth,
                    oldHeight,
                    dataType,
                    _bytes,
                    m_width,
                    m_height,
                    dataType,
                    (void*)bytes);
            }

            int r = gluBuild2DMipmaps(target, textureFormat, m_width, m_height, bytesFormat, dataType, bytes);
            debugAssertM(r == 0, (const char*)gluErrorString(r)); (void)r;

            if (freeBytes) {
                delete[] const_cast<uint8*>(bytes);
            }
            break;
        }

    default:
        debugAssertM(false, "Mipmaps not supported for this texture target");
    }
}


static void brightenImage(TextureFormat::Code fmt, void* _byte, int n, float brighten) {

    debugAssert(
        (fmt == TextureFormat::CODE_RGB8) ||
        (fmt == TextureFormat::CODE_RGBA8));

    uint8* byte = static_cast<uint8*>(_byte);

    // Make a lookup table
    uint8 bright[256];
    for (int i = 0; i < 256; ++i) {
        bright[i] = iClamp(iRound(i * brighten), 0, 255);
    }

    int skipAlpha = 0;
    if (fmt == TextureFormat::CODE_RGBA8) {
        skipAlpha = 1;
    }

    for (int i = 0; i < n; i += skipAlpha) {
        for (int c = 0; c < 3; ++c, ++i) {
            byte[i] = bright[byte[i]];
        }
    }
}


/////////////////////////////////////////////////////

Texture::Settings::Settings() : 
    interpolateMode(TRILINEAR_MIPMAP),
    wrapMode(WrapMode::TILE),
    depthReadMode(DEPTH_NORMAL),
    maxAnisotropy(2.0),
    autoMipMap(true),
    maxMipMap(1000),
    minMipMap(-1000){
}


const Texture::Settings& Texture::Settings::defaults() {
    static Settings param;
    return param;
}


const Texture::Settings& Texture::Settings::video() {

    static bool initialized = false;
    static Settings param;

    if (! initialized) {
        initialized = true;
        param.interpolateMode = BILINEAR_NO_MIPMAP;
        param.wrapMode = WrapMode::CLAMP;
        param.depthReadMode = DEPTH_NORMAL;
        param.maxAnisotropy = 1.0;
        param.autoMipMap = false;
    }

    return param;
}


const Texture::Settings& Texture::Settings::shadow() {

    static bool initialized = false;
    static Settings param;

    if (! initialized) {
        initialized = true;
        param.interpolateMode = BILINEAR_NO_MIPMAP;
        param.wrapMode = WrapMode::CLAMP;
        param.depthReadMode = DEPTH_LEQUAL;
        param.maxAnisotropy = 1.0;
        param.autoMipMap = false;
    }

    return param;
}

/*
void Texture::Settings::serialize(class BinaryOutput& b) {
    // TODO: use chunk format

}

void Texture::Settings::deserialize(class BinaryInput& b) {

}

void Texture::Settings::serialize(class TextOutput& t) {
    // TODO: ini-like format
}

void Texture::Settings::deserialize(class TextInput& t) {
}
*/

size_t Texture::Settings::hashCode() const {
    return 
        (uint32)interpolateMode + 
        16 * (uint32)wrapMode + 
        256 * (uint32)depthReadMode + 
        (autoMipMap ? 512 : 0) +
        (uint32)(1024 * maxAnisotropy) +
        (minMipMap ^ (maxMipMap << 16));
}

bool Texture::Settings::operator==(const Settings& other) const {
    return 
        (interpolateMode == other.interpolateMode) &&
        (wrapMode == other.wrapMode) &&
        (depthReadMode == other.depthReadMode) &&
        (maxAnisotropy == other.maxAnisotropy) &&
        (autoMipMap == other.autoMipMap) &&
        (maxMipMap == other.maxMipMap) &&
        (minMipMap == other.minMipMap);
}

bool Texture::Settings::equalsIgnoringMipMap(const Settings& other) const {
    return 
        (interpolateMode == other.interpolateMode) &&
        (wrapMode == other.wrapMode) &&
        (depthReadMode == other.depthReadMode);
}



} // G3D
