/**
 @file GFont.h
 
 @maintainer Morgan McGuire, morgan@graphics3d.com

 @created 2002-11-02
 @edited  2007-05-19
 */

#ifndef G3D_GFONT_H
#define G3D_GFONT_H

#include "GLG3D/Texture.h"
#include "G3D/BinaryInput.h"
#include "G3D/CoordinateFrame.h"
#include <string>

namespace G3D {

typedef ReferenceCountedPointer<class GFont> GFontRef;

/**
 Font class for use with RenderDevice.  Renders variable size and color 
 fonts from high-resolution bitmaps.

 Font rendering is (inherently) slow-- you can achieve better performance
 for static text by creating bitmap textures with whole words and 
 sentences on them.
 <P>

 The following fonts are provided with G3D in the
 <CODE>data/font</CODE> directory.  See the <CODE>copyright.txt</CODE>
 file in that directory for information about the source of these
 files and rules for distribution. 
 */
class GFont : public ReferenceCountedObject {
public:

    typedef ReferenceCountedPointer<class GFont> Ref;

    /** Constant for draw2D.  Specifies the horizontal alignment
	of an entire string relative to the supplied x,y position */
    enum XAlign {XALIGN_RIGHT, XALIGN_LEFT, XALIGN_CENTER};

    /** Constant for draw2D.  Specifies the vertical alignment
	of the characters relative to the supplied x,y position.
      */
    enum YAlign {YALIGN_TOP, YALIGN_BASELINE, YALIGN_CENTER, YALIGN_BOTTOM};

    /** Constant for draw2D.  Proportional width (default)
        spaces characters based on their size.  Fixed spacing gives
        uniform spacing regardless of character width. */
    enum Spacing {PROPORTIONAL_SPACING, FIXED_SPACING};

private:

    /** Must be a power of 2.  Number of characters in the set (typically 128 or 256)*/
    int charsetSize;

    /** The actual width of the character. */ 
    Array<int> subWidth;

    /** The width of the box, in texels, around the character. */
    int charWidth;
    int charHeight;

    /** Y distance from top of the bounding box to the font baseline. */
    int baseline;

    Texture::Ref m_texture;

    /** Assumes you are already inside of beginPrimitive(QUADS) */
    Vector2 drawString(
        class RenderDevice*     renderDevice,
        const std::string&      s,
        float                  x,
        float                  y,
        float                  w,
        float                  h,
        Spacing                 spacing) const;

    /** Packs vertices for rendering the string
        into the array as tex/vertex, tex/vertex, ...
     */
    Vector2 computePackedArray(
        const std::string&  s,
        float              x,
        float              y,
        float              w,
        float              h,
        Spacing             spacing,
        Array<Vector2>&     array) const;

    GFont(const std::string& filename, BinaryInput& b);

    float m_textureMatrix[16];

public:
    
    /** Returns the underlying texture used by the font.  This is rarely needed by applications */
    inline Texture::Ref texture() const {
        return m_texture;
    }

    /** 
        4x4 matrix transforming texel coordinates to the range [0,1]. Rarely needed by applications.
     */
    inline const float* textureMatrix() const {
        return m_textureMatrix;
    }

    /** Load a new font from disk (fonts are be cached in memory, so repeatedly loading
        the same font is fast as long as the first was not garbage collected).

        The filename must be a G3D .fnt file.

        <P> If a font file is not found, an assertion will fail, an
        exception will be thrown, and texelSize() will return (0, 0).
        <P> Several fonts in this format at varying resolutions are
        available in the font of the G3D data module.

        See GFont::makeFont for creating new fonts in the FNT format.
    */
    static GFontRef fromFile(const std::string& filename);

    /** see GFont::fromFile */
    static GFontRef fromMemory(const std::string& name, const uint8* bytes, const int size);

    /**
     Converts an 8-bit RAW font texture and INI file as produced by
     the Bitmap Font Builder program to a graphics3d PWF font.  inFile
     should have no extension-- .raw and .ini will be appended to it.
     outfile should end with ".FNT" or be "" for the default.  <P>

      The Bitmap Font Builder program can be downloaded from http://www.lmnopc.com/bitmapfontbuilder/

      Use the full ASCII character set; the conversion will strip
	  infrequently used characters automatically. Write out RAW files
	  with characters CENTER aligned and right side up using this
	  program.  Then, also write out an INI file; this contains the
	  width of each character in the font.  Example: 

      <PRE>
	  GFont::convertRAWINItoPWF("c:/tmp/g3dfont/news",
	                            "d:/graphics3d/book/cpp/data/font/news.fnt"); 
      </PRE> 

      @param infileBase The name of the raw/ini files @param outfile Defaults
	  to infileBase + ".fnt"

      @param charsetSize Must be 128 or 256; indicates whether the "extended" characters
      should be represented in the final texture.
     */
    static void makeFont(
        int charsetSize,
		const std::string& infileBase, 
		std::string outfile = "");


    /** Returns the natural character width and height of this font. */
    Vector2 texelSize() const;

    /**
     Draws a proportional width font string.  Assumes device->push2D()
     has been called.  Leaves all rendering state as it was, except for the
     texture coordinate on unit 0.

     @param size The distance between successive lines of text.  Specify
     texelSize().y / 1.5 to get 1:1 texel to pixel

     @param outline If this color has a non-zero alpha, a 1 pixel border of
     this color is drawn about the text.

     @param spacing Fixed width fonts are spaced based on the width of the 'M' character.

     @return Returns the x and y bounds (ala get2DStringBounds) of the printed string.

     You can draw rotated text by setting the RenderDevice object to world matrix
     manually.  The following example renders the word "ANGLE" on a 45-degree angle
     at (100, 100).
     <PRE>
    app->renderDevice->push2D();
        CoordinateFrame cframe(
            Matrix3::fromAxisAngle(Vector3::unitZ(), toRadians(45)),
            Vector3(100, 100, 0));
        app->renderDevice->setObjectToWorldMatrix(cframe);
        app->debugFont->draw2D("ANGLE", Vector2(0, 0), 20);
    app->renderDevice->pop2D();
     </PRE>
     */
    Vector2 draw2D(
        RenderDevice*       renderDevice,
        const std::string&  s,
        const Vector2&      pos2D,
        float              size    = 12,
        const Color4&       color   = Color3::black(),
        const Color4&       outline = Color4::clear(),
        XAlign              xalign  = XALIGN_LEFT,
        YAlign              yalign  = YALIGN_TOP,
        Spacing             spacing = PROPORTIONAL_SPACING) const;


    /**
     Text is visible from behind.  The text is oriented so that it
     reads "forward" when the pos3D z-axis points towards the viewer.

     Note that text, like all transparent objects, should be rendered
     in back to front sorted order to achieve proper alpha blending.

     @param size In meters.
     */
    Vector2 draw3D(
        RenderDevice*               renderDevice,
        const std::string&          s,
        const CoordinateFrame&      pos3D,
        float              size    = .1,
        const Color4&       color   = Color3::black(),
        const Color4&       outline = Color4::clear(),
        XAlign              xalign  = XALIGN_LEFT,
        YAlign              yalign  = YALIGN_TOP,
        Spacing             spacing = PROPORTIONAL_SPACING) const;

		
    /**
     Useful for drawing centered text and boxes around text.
     */
    Vector2 bounds(
        const std::string&  s,
        float               size = 12,
        Spacing             spacing = PROPORTIONAL_SPACING) const;

    /**
       For high performance when rendering substantial amounts of text,
       call:

       <pre>
       rd->push2D();
          font->configureRenderDevice(rd);
          for (...) {
             font->send2DQuads(...);
          }
       rd->pop2D();
       </pre>

       This amortizes the cost of the font setup across multiple calls.
     */
    void configureRenderDevice(RenderDevice* rd) const;

    /** For high-performance rendering of substantial amounts of text. */
    Vector2 send2DQuads(
        RenderDevice*       renderDevice,
        const std::string&  s,
        const Vector2&      pos2D,
        float              size    = 12,
        const Color4&       color   = Color3::black(),
        const Color4&       outline = Color4::clear(),
        XAlign              xalign  = XALIGN_LEFT,
        YAlign              yalign  = YALIGN_TOP,
        Spacing             spacing = PROPORTIONAL_SPACING) const;

};

}
#endif
