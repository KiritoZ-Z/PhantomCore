/**
 @file Framebuffer.cpp

 @maintainer Morgan McGuire
 @cite Initial implementation by Daniel Hilferty, djhilferty@users.sourceforge.net

 Notes:
 <UL>
 <LI>http://oss.sgi.com/projects/ogl-sample/registry/EXT/framebuffer_object.txt
 </UL>

 @created 2006-01-07
 @edited  2006-10-30
*/

#ifndef GLG3D_FRAMEBUFFER_H
#define GLG3D_FRAMEBUFFER_H

#include "G3D/Array.h"
#include "G3D/Table.h"
#include "G3D/Rect2D.h"
#include "GLG3D/Texture.h"
#include "GLG3D/Renderbuffer.h"
#include <string.h>

namespace G3D {

class Framebuffer;
typedef ReferenceCountedPointer<Framebuffer> FramebufferRef;

/**
 Abstraction of OpenGL's Framebuffer Object extension.  This is a fast and 
 efficient way of rendering to textures.  This class can be used with raw OpenGL, 
 without RenderDevice or SDL.

 Basic Framebuffer Theory:
	Every OpenGL program has at least one Framebuffer.  This framebuffer is
 setup by the windowing system and its image format is that specified by the
 OS.  With the Framebuffer Object extension, OpenGL gives the developer
 the ability to create offscreen framebuffers that can be used to render 
 to textures of any specified format.
    The Framebuffer class is used in conjunction with the RenderDevice to
 set a render target.  The RenderDevice method setFramebuffer performs this
 action.  If a NULL argument is passed to setFramebuffer, the render target
 defaults to the window display framebuffer.
    Framebuffer works in conjunction with the push/pop RenderDevice state, but
 in a limited form.   The state will save the current Framebuffer, but will not
 save the state of the Framebuffer itself.  If the attachment points are changed
 in the push/pop block, these will not be restored by a pop.
    The following example shows how to create a texture and bind it to Framebuffer
 for rendering.

 Framebuffer Example:

 <PRE>
    // Create Texture
	static Texture::Ref tex = Texture::createEmpty(256, 256, "Rendered Texture", TextureFormat::RGB8, Texture::CLAMP, Texture::NEAREST_NO_MIPMAP, Texture::DIM_2D);

	// Create a framebuffer that uses this texture as the color buffer
	static FramebufferRef fb = Framebuffer::create("Offscreen target");
	bool init = false;

	if (! init) {
		fb->set(Framebuffer::COLOR_ATTACHMENT0, tex);
		init = true;
	}

	rd->pushState();
		rd->setFramebuffer(fb);
		rd->push2D(fb->rect2DBounds());

			// Set framebuffer as the render target

			// Draw on the texture
			Draw::rect2D(Rect2D::xywh(0,0,128,256), rd, Color3::white());
			Draw::rect2D(Rect2D::xywh(128,0,128,256), rd, Color3::red());

			// Restore renderdevice state (old frame buffer)
		rd->pop2D();
	rd->popState();

	app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

	// Remove the texture from the framebuffer
	//	fb->set(Framebuffer::COLOR_ATTACHMENT0, NULL);

	// Can now render from the texture

    
    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1f, .5f, 1));
    app->renderDevice->clear();

    app->renderDevice->push2D();
		rd->setTexture(0, tex);
		Draw::rect2D(Rect2D::xywh(10,10,256,256), rd);
	app->renderDevice->pop2D();
  </PRE>

 In addition to Textures, Renderbuffers may also be bound to the
 Framebuffer.   This is done in a very similar manner to the Texture
 object in the example.  Renderbuffer class contains an example of this
 operation.

 Note:  Not any combination of images may be attached to a Framebuffer.
 OpenGL lays out some restrictions that must be considered:

 <ol>
	<li> In order to render to a Framebuffer, there must be at least
	one image (Renderbuffer or Texture) attached to an attachment point.
	<li> All images must have the same height and width.
	<li> All images attached to a COLOR_ATTACHMENT[n] point must have
	the same internal format (RGBA8, RGBA16...etc)
	<li> If RenderDevice->setDrawBuffer is used then the specified 
	attachment point must have a bound image.
	<li> The combination of internal formats of attached images does not
	violate some implementation-dependent set of restrictions (i.e., Your
	graphics card must completely implement all combinations that you
	plan to use!)
 </ol>

 If you create a Framebuffer with a single, depth Renderbuffer attached
 (e.g., for shadow map rendering)
 it is complete in the OpenGL sense, however you will receive a completeness
 error because the glDrawBuffer and glReadBuffer attached to that 
 Framebuffer have incorrect defaults.  To fix this, call <code>glDrawBuffer(GL_NONE);glReadBuffer(GL_NONE);</code>
 <b>after</b> binding the Framebuffer to the RenderDevice but before rendering.

	<B>BETA API</B> -- Subject to change
*/
class Framebuffer : public ReferenceCountedObject {
public:

    /**
	 Specifies which channels of the framebuffer the renderbuffer or texture will 
     define. These mirror
	 the OpenGL definition as do their values.

     A DEPTH_STENCIL format renderbuffer or texture can be attached to either the 
     DEPTH_ATTACHMENT or the STENCIL_ATTACHMENT, or both simultaneously; Framebuffer will
     understand the format and use the appropriate channels.
	 */
	enum AttachmentPoint {
		COLOR_ATTACHMENT0  = 0x8CE0,
        COLOR_ATTACHMENT1  = 0x8CE1,
        COLOR_ATTACHMENT2  = 0x8CE2,
        COLOR_ATTACHMENT3  = 0x8CE3,
        COLOR_ATTACHMENT4  = 0x8CE4,
        COLOR_ATTACHMENT5  = 0x8CE5,
        COLOR_ATTACHMENT6  = 0x8CE6,
        COLOR_ATTACHMENT7  = 0x8CE7,
        COLOR_ATTACHMENT8  = 0x8CE8,
        COLOR_ATTACHMENT9  = 0x8CE9,
        COLOR_ATTACHMENT10 = 0x8CEA,
        COLOR_ATTACHMENT11 = 0x8CEB,
        COLOR_ATTACHMENT12 = 0x8CEC,
        COLOR_ATTACHMENT13 = 0x8CED,
        COLOR_ATTACHMENT14 = 0x8CEE,
        COLOR_ATTACHMENT15 = 0x8CEF,
        DEPTH_ATTACHMENT   = 0x8D00,
		STENCIL_ATTACHMENT = 0x8D20,
        };

private:

    class Attachment {
    public:
        enum Type {TEXTURE, RENDERBUFFER};
        Type                        type;

        RenderbufferRef             renderbuffer;
        Texture::Ref                  texture;

		/** True if the texture had autoMipMap on when it was set. */
		bool						hadAutoMipMap;

        Attachment() {}

        Attachment(const RenderbufferRef& r) : 
            type(RENDERBUFFER), 
            renderbuffer(r) {}

        Attachment(const Texture::Ref& r) : 
            type(TEXTURE), 
            texture(r),
            hadAutoMipMap(r->settings().autoMipMap) {

			if (hadAutoMipMap) {
				texture->setAutoMipMap(false);
			}
		}
    };

    /**
     Current attachments.
     Slots are not specified if they correspond to NULL elements.

     Note: the uint32 key corresponds to an AttachmentPoint value
     */
    Table<uint32, Attachment>  attachmentTable;

	/** OpenGL Object ID */
	GLuint							framebufferID;

	/** Framebuffer name */
	std::string                     m_name;

	/** 
	 Not yet implemented yet -- for non-gl error checking to pre-check
	 for Framebuffer completeness.  Width & Height should also be
	 implemented for this check.
	*/
    const class TextureFormat*      format;

    /**
     Framebuffer Height
     */
    GLuint                          m_height;
    GLuint                          m_width;

    /**
     Number of currently bound attachments.  When this hits zero we can
     add attachments with new sizes.
     */
    inline int numAttachments() const {
        return static_cast<int>(attachmentTable.size());
    }

	/** Default Constructor. */
	Framebuffer(const std::string& name, GLuint framebufferID);

public:


	/** Reclaims OpenGL ID.  All buffers/textures are automatically
        detacted on destruction. */
	~Framebuffer();

	/**
	 Creates a framebuffer object from an OpenGL context.

	 @param name			Name of framebuffer
	 @param framebufferID	OpenGL id of ramebuffer
	 */
	static FramebufferRef fromGLFramebuffer
    (const std::string& name, GLuint framebufferID);

	/**
	 Creates a framebuffer object from scratch.

	 @param name			Name of framebuffer
	 */
	static FramebufferRef create(const std::string& name);

    /** Overload used when setting attachment points to NULL */
    void set(AttachmentPoint ap, const void* n);

	/**
	 Set one of the attachment points to reference a texture.  Set to
	 NULL to unset.  Auto-mipmap will automatically be disabled on
	 set.  It will be re-enabled when the texture is unbound.

	 Do not use a texture that is bound to the *current* framebuffer
	 for rendering, however, you can render using a texture that is
	 bound on a different frame buffer.

	 @param texture		Texture to bind to the framebuffer.
	 @param ap	Attachment point to bind texture to.
	 */
	void set(AttachmentPoint ap, const Texture::Ref& texture);

	/**
	 Set one of the attachment points to reference a renderbuffer.
	 Set to NULL to unset.

	 @param renderbuffer	Renderbuffer to bind to the framebuffer
	 @param slot		Attachment point to bind renderbuffer to.
	 */
	void set(AttachmentPoint ap, const RenderbufferRef& renderbuffer);

    /** Returns true if this attachment is currently non-null.*/
    bool has(AttachmentPoint ap) const;

    /**
     Gets the OpenGL ID of the framebuffer object.
     */
	inline unsigned int openGLID() const {
        return framebufferID;
    }

	inline int width() const {
        return m_width;
    }

    /**
     Gets the OpenGL ID of the framebuffer object.
     */
	inline int height() const {
        return m_height;
    }

	inline Rect2D rect2DBounds() const {
		return Rect2D::xywh(0.0f, 0.0f, (float)m_width, (float)m_height);
	}

    inline Vector2 vector2Bounds() const {
        return Vector2((float)m_width, (float)m_height);
    }

    inline const std::string& name() const {
        return m_name;
    }

}; // class Framebuffer 

typedef Framebuffer FrameBuffer;
typedef FramebufferRef FrameBufferRef;

} //  G3D

#endif // GLG3D_FRAMEBUFFER_H

