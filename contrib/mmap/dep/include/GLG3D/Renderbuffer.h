/**
  @file Renderbuffer.h

  @maintainer Daniel Hilferty, djhilferty@users.sourceforge.net

  @created 2006-01-06
  @edited  2006-11-14
*/

#ifndef GLG3D_RENDERBUFFER_H
#define GLG3D_RENDERBUFFER_H

#include <string>
#include "G3D/ReferenceCount.h"
#include "G3D/Vector2.h"
#include "G3D/Rect2D.h"
#include "GLG3D/glheaders.h"

namespace G3D {

class Renderbuffer;

typedef ReferenceCountedPointer<Renderbuffer> RenderbufferRef;

/**
 Abstraction of the OpenGL renderbuffer object.  A Framebuffer has many
 buffers inside it; typically one for colors, one for stencil, one for depth.
 Those can be either Textures or Renderbuffers.

 A Renderbuffer object is essentially an image that cannot be used as a
 texture.  It may be rendered to, but not used for rendering. On most hardware
 there is no advantage to using a Renderbuffer instead of a G3D::Texture, although
 theoretically hardware could support some G3D::TextureFormat s as Renderbuffers
 but not as Textures.  

 Note that G3D allows you to spell the class name either as G3D::Renderbuffer (matching
 OpenGL conventions) or G3D::RenderBuffer (matching G3D conventions).

 See G3D::Framebuffer for a detailed example.

 Not all graphics cards support all renderbuffer formats.  For example, pre-GeForce 8800 NVIDIA cards
 require packed DEPTH24_STENCIL8 and do not allow STENCIL8 textures alone.

 <B>BETA API</B> -- Subject to change

  @cite http://oss.sgi.com/projects/ogl-sample/registry/EXT/framebuffer_object.txt
*/
class Renderbuffer : public ReferenceCountedObject {
private:

	/** Renderbuffer name */
    std::string						mName;

	/** OpenGL Object ID */
	GLuint							mImageID;

	/** Texel format */
    const class TextureFormat*      mFormat;

	/** Buffer width */
    int								mWidth;

	/** Buffer height */
    int								mHeight;

	/** Constructor */
	Renderbuffer (	
		const std::string&			_name,
		const GLuint				_renderbufferID,
		const G3D::TextureFormat*	_format, 
		const int					_width, 
		const int					_height);

public:

	/** Destroys the underlying OpenGL id */
	~Renderbuffer();

	/**
	 Create a renderbuffer object from a previously initialized OpenGL
	 renderbuffer context.

	 @param _name			Name of renderbuffer
	 @param _renderbufferID OpenGL context of prior Renderbuffer
	 @param _format			Texel format
	 @param _width			Width
	 @param _height			Height
	 @param _depth			Image depth
	 */
	static RenderbufferRef fromGLRenderbuffer(
		const std::string&			name, 
		const GLuint				imageID,
		const G3D::TextureFormat*   format);

	/**
	 Creates an empty renderbuffer object.

	 @param _name	Name of renderbuffer
	 @param _format	Texel format
	 @param _width  Width
	 @param _height Height
	 @param _depth  Image depth
	 */
    static RenderbufferRef createEmpty(
		const std::string&			name, 
		const int					width, 
		const int					height,
		const class TextureFormat*  format); 

	/**
	 Get OpenGL renderbuffer ID, useful for accessing
     functionality not directly exposed by G3D.
	*/
    inline unsigned int openGLID() const {
        return mImageID;
	}

    inline const TextureFormat* format() const {
        return mFormat;
    }

    inline const std::string& name() const {
        return mName;
    }

	inline int width() const {
        return mWidth;
    }

	inline int height() const {
        return mHeight;
    }
   
    inline Vector2 vector2Bounds() const {
        return Vector2((float)mWidth, (float)mHeight);
    }

    inline Rect2D rect2DBounds() const {
        return Rect2D::xywh(0.0f, 0.0f, (float)mWidth, (float)mHeight);
    }

}; // class Renderbuffer

typedef Renderbuffer RenderBuffer;
typedef RenderbufferRef RenderBufferRef;

} // G3D

#endif // GLG3D_RENDERBUFFER_H 
