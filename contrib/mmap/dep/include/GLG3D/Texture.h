/**
  @file Texture.h

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2001-02-28
  @edited  2007-07-18
*/

#ifndef GLG3D_TEXTURE_H
#define GLG3D_TEXTURE_H

#include "G3D/ReferenceCount.h"
#include "G3D/Array.h"
#include "G3D/Table.h"
#include "G3D/Vector2.h"
#include "G3D/WrapMode.h"
#include "GLG3D/glheaders.h"
#include "GLG3D/TextureFormat.h"
#include "G3D/Image1.h"
#include "G3D/Image1uint8.h"
#include "G3D/Image3.h"
#include "G3D/Image3uint8.h"
#include "G3D/Image4.h"
#include "G3D/Image4uint8.h"

namespace G3D {

class GImage;
class Rect2D;
class Matrix3;
class Texture;

/**
 Abstraction of OpenGL textures.  This class can be used with raw OpenGL, 
 without RenderDevice.  G3D::Texture supports all of the image formats
 that G3D::GImage can load, and DDS (DirectX textures), and Quake-style cube 
 maps.

 If you enable texture compression, textures will be compressed on the fly.
 This can be slow (up to a second).

 Unless DIM_2D_RECT, DIM_2D_NPOT, DIM_CUBE_MAP_NPOT are used, the texture is automatically
 scaled to the next power of 2 along each dimension to meet hardware requirements, if not
 already a power of 2.  However, DIM_2D_NPOT and DIM_CUBE_MAP_NPOT will safely fallback to
 POT requirements if the ARB_non_power_of_two extension is not supported. Develoeprs can 
 check if this will happen by calling GLCaps::supports_GL_ARB_texture_non_power_of_two().
 Note that the texture does not have to be a rectangle; the dimensions can be different powers of two.
 DIM_2D_RECT is provided primarily for older cards only and does not interact well with shaders.

 Textures are loaded so that (0, 0) is the upper-left corner of the image.
 If you set the invertY flag, RenderDevice will automatically turn them upside
 down when rendering to allow a (0, 0) <B>lower</B>-left corner.  If you
 aren't using RenderDevice, you must change the texture matrix to have
 a -1 in the Y column yourself.  If you replace the default vertex shader then
 the texture matrix transformation will not be performed.

 DIM_2D_RECT requires the GL_EXT_texture_rectangle extension.
 Texture compression requires the EXT_texture_compression_s3tc extions.
 You can either query OpenGL for whether these are supported or
 use the G3D::GLCaps facility for doing so.

 G3D::Texture can be used with straight OpenGL, without G3D::RenderDevice, as
 follows:

 <PRE>
  Texture::Ref texture = new Texture("logo.jpg");

  ...
    
  GLint u = texture->getOpenGLTextureTarget();
  glEnable(u);
  glBindTexture(u, texture->getOpenGLID());
 </PRE>

 To use Texture with RenderDevice:

  <PRE>
  Texture::Ref texture = new Texture("logo.jpg");
  ...
  renderDevice->setTexture(0, texture);
  // (to disable: renderDevice->setTexture(0, NULL);)
  </PRE>


  3D MIP Maps are not supported because gluBuild3DMipMaps is not in all GLU implementations.

  See G3D::RenderDevice::setBlendFunc for important information about turning on alpha blending. 
 */
class Texture : public ReferenceCountedObject {
public:

    /** Reference counted pointer to a Texture.*/
    typedef ReferenceCountedPointer<class Texture> Ref;

    /** DIM_2D_NPOT and DIM_CUBE_MAP_NPOT attempt to use
        ARB_non_power_of_two texture support with POT fallback. */
    enum Dimension       {DIM_2D = 2, DIM_3D = 3, DIM_2D_RECT = 4, 
                          DIM_CUBE_MAP = 5, DIM_2D_NPOT = 6, DIM_CUBE_MAP_NPOT = 7};

    /** 
      Returns true if this is a legal wrap mode for a G3D::Texture.
     */
    static bool supportsWrapMode(WrapMode m) {
        return (m == WrapMode::TILE) || (m == WrapMode::CLAMP) || (m == WrapMode::ZERO);
    }

    /**
     Trilinear mipmap is the best quality (and frequently fastest)
     mode.  The no-mipmap modes conserve memory.  Non-interpolating
     ("Nearest") modes are generally useful only when packing lookup
     tables into textures for shaders.

     3D textures do not support mipmap interpolation modes.
     */
    enum InterpolateMode {
        TRILINEAR_MIPMAP = 3, 
        BILINEAR_MIPMAP = 4,
        NEAREST_MIPMAP = 5,

        BILINEAR_NO_MIPMAP = 2,
        NEAREST_NO_MIPMAP = 6};

    /** A m_depth texture can automatically perform the m_depth comparison used for shadow mapping
        on a texture lookup.  The result of a texture lookup is thus the shadowed amount
        (which will be percentage closer filtered on newer hardware) and <I>not</I> the 
        actual m_depth from the light's point of view.
       
        This combines GL_TEXTURE_COMPARE_MODE_ARB and GL_TEXTURE_COMPARE_FUNC_ARB from
        http://www.nvidia.com/dev_content/nvopenglspecs/GL_ARB_shadow.txt

        For best results on percentage closer hardware (GeForceFX and Radeon9xxx or better), 
        create shadow maps as m_depth textures with BILINEAR_NO_MIPMAP sampling.

        See also G3D::RenderDevice::configureShadowMap and the Collision_Demo.
     */
    enum DepthReadMode {DEPTH_NORMAL = 0, DEPTH_LEQUAL = 1, DEPTH_GEQUAL = 2};

    /**
     Splits a filename around the '*' character-- used by cube maps to generate all filenames.
     */
    static void splitFilenameAtWildCard(
        const std::string&  filename,
        std::string&        filenameBeforeWildCard,
        std::string&        filenameAfterWildCard);

	/**
	 Returns true if the specified filename exists and is an image that can be loaded as a Texture.
	*/
	static bool isSupportedImage(const std::string& filename);


    /**
     All parameters of a texture that are independent of the
     underlying image data.
     */
    class Settings {
    public:

        /** Default is TRILINEAR_MIPMAP */
        InterpolateMode             interpolateMode;

        /** Default is TILE */
        WrapMode                    wrapMode;

        /** Default is DEPTH_NORMAL */
        DepthReadMode               depthReadMode;

        /** Default is 2.0 */
        float                       maxAnisotropy;

        /** Default is true */
        bool                        autoMipMap;

        /**
         Highest MIP-map level that will be used during rendering.
         The highest level that actually exists will be L =
         log(max(m_width, m_height), 2)), although it is fine to set
         maxMipMap higher than this.  Must be larger than minMipMap.
         Default is 1000.

         Setting the max mipmap level is useful for preventing
         adjacent areas of a texture from being blurred together when
         viewed at a distance.  It may decrease performance, however,
         by forcing a larger texture into cache than would otherwise
         be required.
         */
        int                         maxMipMap;

        /**
         Lowest MIP-map level that will be used during rendering.
         Level 0 is the full-size image.  Default is -1000, matching
         the OpenGL spec.  @cite
         http://oss.sgi.com/projects/ogl-sample/registry/SGIS/texture_lod.txt
         */
        int                         minMipMap;

        Settings();

        static const Settings& defaults();

        /** 
          Useful defaults for video/image processing.
          BILINEAR_NO_MIPMAP / CLAMP / DEPTH_NORMAL / 1.0 / false
        */
        static const Settings& video();

        /** 
          Useful defaults for shadow maps.
          BILINEAR_NO_MIPMAP / CLAMP / DEPTH_LEQUAL / 1.0 / false
        */
        static const Settings& shadow();

        /*
         Coming in a future version...
        void serialize(class BinaryOutput& b);
        void deserialize(class BinaryInput& b);
        void serialize(class TextOutput& t);
        void deserialize(class TextInput& t);
        */

        bool operator==(const Settings& other) const;

        /** True if both Settings are identical, ignoring mipmap settings.*/
        bool equalsIgnoringMipMap(const Settings& other) const;
        size_t hashCode() const;
    };


    class PreProcess {
    public:

        /** Amount to brighten colors by (e.g., useful for Quake textures, which are dark). 
            Brightening happens first of all preprocessing.
         */
        float                       brighten;

        /** Amount to resize images by before loading onto the graphics card to save memory; 
            typically a negative power of 2 (e.g., 1.0, 0.5, 0.25). Scaling happens last of all preprocessing.*/
        float                       scaleFactor;

        /** If true, treat the input as a monochrome bump map and compute a normal map from
            it where the RGB channels are XYZ and the A channel is the input bump height.*/
        bool                        computeNormalMap;

        /** If computeNormalMap is true, then this blurs the elevation before computing normals.*/
        bool                        normalMapLowPassBump;

        /** See G3D::GImage::computeNormalMap() */
        float                       normalMapWhiteHeightInPixels;

        bool                        normalMapScaleHeightByNz;

        PreProcess() : brighten(1.0f), scaleFactor(1.0f), computeNormalMap(false), normalMapLowPassBump(false),
                       normalMapWhiteHeightInPixels(-1), normalMapScaleHeightByNz(false) {}

        static const PreProcess& defaults() {
            static const PreProcess p;
            return p;
        }

        static const PreProcess& quake() {
            static PreProcess p;
            p.brighten = 2.0f;
            return p;
        }

        static const PreProcess& normalMap() {
            static bool initialized = false;
            static PreProcess p;
            if (! initialized) {
                p.computeNormalMap = true;
                p.normalMapLowPassBump = false;
                p.normalMapScaleHeightByNz = false;
                p.normalMapWhiteHeightInPixels = -1.0f;
                initialized = true;
            }

            return p;
        }
    };

private:

    /** OpenGL texture ID */
    GLuint                          m_textureID;

    /** Set in the base constructor. */
    Settings                        m_settings;

    std::string                     m_name;
    Dimension                       m_dimension;
    bool                            m_opaque;

    const class TextureFormat*      m_format;
    int                             m_width;
    int                             m_height;
    int                             m_depth;

    static size_t                   m_sizeOfAllTexturesInMemory;

    Texture(
        const std::string&          name,
        GLuint                      textureID,
        Dimension                   dimension,
        const class TextureFormat*  format,
		bool		      	        opaque,
		const Settings&             settings);

	/** Call glGetTexImage with appropriate target */
	void getTexImage(void* data, const TextureFormat* desiredFormat) const;

public:

    /**
     Creates an empty texture (useful for later reading from the screen).
     */
    static Texture::Ref createEmpty(
        const std::string&              name,
        int                             m_width,
        int                             m_height,
        const class TextureFormat*      desiredFormat  = TextureFormat::RGBA8(),
        Dimension                       dimension      = DIM_2D,
        const Settings&                 settings       = Settings::defaults());

    /**
     Wrap and interpolate will override the existing parameters on the
     GL texture.

     @param name Arbitrary name for this texture to identify it
     @param textureID Set to newGLTextureID() to create an empty texture.
     */
    static Texture::Ref fromGLTexture(
        const std::string&              name,
        GLuint                          textureID,
        const class TextureFormat*      textureFormat,
        Dimension                       dimension      = DIM_2D,
        const Settings&                 settings       = Settings::defaults());

    /**
     Creates a texture from a single image.  The image must have a format understood
     by G3D::GImage or a DirectDraw Surface (DDS).  If dimension is DIM_CUBE_MAP, this loads the 6 files with names
     _ft, _bk, ... following the G3D::Sky documentation.
     */    
    static Texture::Ref fromFile(
        const std::string&              filename,
        const class TextureFormat*      desiredFormat  = TextureFormat::AUTO(),
        Dimension                       dimension      = DIM_2D,
        const Settings&                 settings       = Settings::defaults(),
        const PreProcess&               process        = PreProcess());

    /**
     Creates a cube map from six independently named files.  The first
     becomes the name of the texture.
     */
    static Texture::Ref fromFile(
        const std::string               filename[6],
        const class TextureFormat*      desiredFormat  = TextureFormat::AUTO(),
        Dimension                       dimension      = DIM_2D,
        const Settings&                 settings       = Settings::defaults(),
        const PreProcess&               process        = PreProcess());

    /**
     Creates a texture from the colors of filename and takes the alpha values
     from the red channel of alpha filename. See G3D::RenderDevice::setBlendFunc
	 for important information about turning on alpha blending. 
     */
    static Texture::Ref fromTwoFiles(
        const std::string&              filename,
        const std::string&              alphaFilename,
        const class TextureFormat*      desiredFormat  = TextureFormat::AUTO(),
        Dimension                       dimension      = DIM_2D,
        const Settings&                 settings       = Settings::defaults(),
        const PreProcess&               process        = PreProcess());

    /**
    Construct from an explicit set of (optional) mipmaps and (optional) cubemap faces.

    bytes[miplevel][cubeface] is a pointer to the bytes
    for that miplevel and cube
    face. If the outer array has only one element and the
    interpolation mode is
    TRILINEAR_MIPMAP, mip-maps are automatically generated from
	the level 0 mip-map.

    There must be exactly
    6 cube faces per mip-level if the dimensions are
    DIM_CUBE and and 1 per
    mip-level otherwise. You may specify compressed and
    uncompressed formats for
    both the bytesformat and the desiredformat.

    3D Textures map not use mip-maps.
    */
    static Texture::Ref fromMemory(
        const std::string&                  name,
        const Array< Array<const void*> >&  bytes,
        const TextureFormat*                bytesFormat,
        int                                 m_width,
        int                                 m_height,
        int                                 m_depth,
        const TextureFormat*                desiredFormat  = TextureFormat::AUTO(),
        Dimension                           dimension      = DIM_2D,
        const Settings&                     settings       = Settings::defaults(),
        const PreProcess&                   preProcess     = PreProcess::defaults());


	 /** Construct from a single packed 2D or 3D data set.  For 3D
         textures, the interpolation mode must be one that does not
         use MipMaps. */
    static Texture::Ref fromMemory(
        const std::string&              name,
        const void*                     bytes,
        const class TextureFormat*      bytesFormat,
        int                             m_width,
        int                             m_height,
        int				                m_depth,
        const class TextureFormat*      desiredFormat  = TextureFormat::AUTO(),
        Dimension                       dimension      = DIM_2D,
        const Settings&                 settings       = Settings::defaults(),
        const PreProcess&               preProcess     = PreProcess::defaults());

    static Texture::Ref fromGImage(
        const std::string&              name,
        const GImage&                   image,
        const class TextureFormat*      desiredFormat  = TextureFormat::AUTO(),
        Dimension                       dimension      = DIM_2D,
        const Settings&                 settings	   = Settings::defaults(),
        const PreProcess&               preProcess     = PreProcess::defaults());

    /** Creates another texture that is the same as this one but contains only
        an alpha channel.  Alpha-only textures are useful as mattes.  
        
        If the current texture is opaque(), returns NULL (since it is not useful
        to construct an alpha-only version of a texture without an alpha channel).
        
        Like all texture construction methods, this is fairly
        slow and should not be called every frame during interactive rendering.*/
    Texture::Ref alphaOnlyVersion() const;

    /**
     Helper method. Returns a new OpenGL texture ID that is not yet managed by a G3D Texture.
     */
    static uint32 newGLTextureID();

    /**
     Copies data from screen into an existing texture (replacing whatever was
     previously there).  The dimensions must be powers of two or a texture 
     rectangle will be created (not supported on some cards).

     The (x, y) coordinates are in real screen pixels.  (0, 0) is the top left
     of the screen.

     The texture dimensions will be updated but all other properties will be preserved:
     The previous wrap mode will be preserved.
     The interpolation mode will be preserved (unless it required a mipmap,
     in which case it will be set to BILINEAR_NO_MIPMAP).  The previous color m_depth
     and alpha m_depth will be preserved.  Texture compression is not supported for
     textures copied from the screen.

     To copy a m_depth texture, first create an empty m_depth texture then copy into it.

     If you invoke this method on a texture that is currently set on RenderDevice,
     the texture will immediately be updated (there is no need to rebind).

     @param useBackBuffer If true, the texture is created from the back buffer.
     If false, the texture is created from the front buffer.

     @param rect The rectangle to copy (relative to the viewport)
     See also RenderDevice::screenShotPic
     */
    void copyFromScreen(const Rect2D& rect, bool useBackBuffer = true);

    /**
     Argument for copyFromScreen
     */
    enum CubeFace {
        CUBE_POS_X = 0,
        CUBE_NEG_X = 1,
        CUBE_POS_Y = 2,
        CUBE_NEG_Y = 3,
        CUBE_POS_Z = 4,
        CUBE_NEG_Z = 5};

    /**
     Copies into the specified face of a cube map.  Because cube maps can't have
     the Y direction inverted (and still do anything useful), you should render
     the cube map faces <B>upside-down</B> before copying them into the map.  This
     is an unfortunate side-effect of OpenGL's cube map convention.  
     
     Use G3D::Texture::getCameraRotation to generate the (upside-down) camera
     orientations.
     */
    void copyFromScreen(const Rect2D& rect, CubeFace face, bool useBackBuffer = true);

    /**
     Returns the rotation matrix that should be used for rendering the
     given cube map face.
     */
    static void getCameraRotation(CubeFace face, Matrix3& outMatrix);

    /**
     When true, rendering code that uses this texture is respondible for
     flipping texture coordinates applied to this texture vertically (initially,
     this is false).
     
     RenderDevice watches this flag and performs the appropriate transformation.
     If you are not using RenderDevice (or are writing shaders), you must do it yourself.
     */
    bool invertY;

    /**
     How much (texture) memory this texture occupies.  OpenGL backs
     video memory textures with main memory, so the total memory 
     is actually twice this number.
     */
    size_t sizeInMemory() const;

    /**
     Video memory occupied by all OpenGL textures allocated using Texture
     or maintained by pointers to a Texture.
     */
    inline static size_t sizeOfAllTexturesInMemory() {
        return m_sizeOfAllTexturesInMemory;
    }

    /**
     True if this texture was created with an alpha channel.  Note that
     a texture may have a format that is not opaque (e.g. RGBA8) yet still
     have a completely opaque alpha channel, causing texture->opaque to
     be true.  This is just a flag set for the user's convenience-- it does
     not affect rendering in any way.
	 See G3D::RenderDevice::setBlendFunc
	 for important information about turning on alpha blending. 
     */
    inline bool opaque() const {
        return m_opaque;
    }

    /**
     Returns the level 0 mip-map data in the format that most closely matches
     outFormat.
     @param outFormat Must be one of: TextureFormat::AUTO, TextureFormat::RGB8, TextureFormat::RGBA8, TextureFormat::L8, TextureFormat::A8
     */
    void getImage(GImage& dst, const TextureFormat* outFormat = TextureFormat::AUTO()) const;

	/** Extracts the data as TextureFormat::RGBA32F */
	Image4Ref toImage4() const;

	/** Extracts the data as TextureFormat::RGBA8 */
	Image4uint8Ref toImage4uint8() const;

	/** Extracts the data as TextureFormat::RGB32F */
	Image3Ref toImage3() const;

	/** Extracts the data as TextureFormat::RGB8 */
	Image3uint8Ref toImage3uint8() const;

	/** Extracts the data as TextureFormat::L32F
	 */
	Image1Ref toImage1() const;

	/** Extracts the data as TextureFormat::L8 */
	Image1uint8Ref toImage1uint8() const;

	/** Extracts the data as TextureFormat::DEPTH32F */
	Image1Ref toDepthImage1() const;

	/** Extracts the data as TextureFormat::DEPTH32F */
	Map2D<float>::Ref toDepthMap() const;

	/** Extracts the data as TextureFormat::DEPTH32F and converts to 8-bit. */
	Image1uint8Ref toDepthImage1uint8() const;

    inline unsigned int openGLID() const {
        return m_textureID;
    }

    /** @deprecated Use width() */
    inline int texelWidth() const {
        return m_width;
    }

    /** @deprecated Use height() */
    inline int texelHeight() const {
        return m_height;
    }

    /** Number of horizontal texels in the level 0 mipmap */
    inline int width() const {
        return m_width;
    }

    /** Number of horizontal texels in the level 0 mipmap */
    inline int height() const {
        return m_height;
    }

    inline int depth() const {
        return m_depth;
    }


    inline Vector2 vector2Bounds() const {
        return Vector2((float)m_width, (float)m_height);
    }

    /** Returns a rectangle whose m_width and m_height match the dimensions of the texture. */
    Rect2D rect2DBounds() const;

    /**
     For 3D textures.
     @deprecated use m_depth()
     */
    inline int texelDepth() const {
        return m_depth;
    }

    inline const std::string& name() const {
        return m_name;
    }

    inline const TextureFormat* format() const {
        return m_format;
    }
    
    inline Dimension dimension() const {
        return m_dimension;
    }

    /**
     Deallocates the OpenGL texture.
     */
    virtual ~Texture();

    /**
     The OpenGL texture target this binds (e.g. GL_TEXTURE_2D)
     */
    unsigned int openGLTextureTarget() const;

    const Settings& settings() const;

    /** Set the autoMipMap value, which only affects textures when they are rendered 
        to or copied from the screen.

        You can read the automipmap value from <code>settings().autoMipMap</code>. 
        */
    void setAutoMipMap(bool b);

    /** For a texture with automipmap off that supports the FrameBufferObject extension, 
       generate mipmaps from the level 0 mipmap immediately.  For other textures, does nothing.*/
    void generateMipMaps();

private:

    class DDSTexture {
    private:
                                    
        uint8*                      bytes;
        const TextureFormat*        bytesFormat;
        int                         m_width;
        int                         m_height;
        int                         numMipMaps;
        int                         numFaces;

    public:

        DDSTexture(const std::string& filename);

        ~DDSTexture();

        int getWidth() {
            return m_width;
        }

        int getHeight() {
            return m_height;
        }

        const TextureFormat* getBytesFormat() {
            return bytesFormat;
        }

        int getNumMipMaps() {
            return numMipMaps;
        }

        int getNumFaces() {
            return numFaces;
        }

        uint8* getBytes() {
            return bytes;
        }
    };
};

/** For backwards compatibility to 6.xx*/
typedef Texture::Ref TextureRef;

} // namespace

template <>
struct GHashCode<G3D::Texture::Settings>
{
    size_t operator()(const G3D::Texture::Settings& key) const { return key.hashCode(); }
};


#endif
