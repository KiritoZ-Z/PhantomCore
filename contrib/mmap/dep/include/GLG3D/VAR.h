/**
  @file GLG3D/VAR.h

  @maintainer Morgan McGuire, morgan@graphics3d.com
  @created 2001-05-29
  @edited  2004-01-06
*/

#ifndef GLG3D_VAR_H
#define GLG3D_VAR_H

#include "GLG3D/RenderDevice.h"
#include "GLG3D/getOpenGLState.h"
#include "GLG3D/glFormat.h"
#include "GLG3D/VARArea.h"

namespace G3D {

/**
 A pointer to an array of vertices, colors, or normals in video memory.
 It is safe to copy these (the pointer will be copied, not the video 
 memory).  
 
 There is no destructor because the memory referenced is freed when
 the parent VARArea is reset or freed.
 */
class VAR {
private:

    friend class RenderDevice;

	VARAreaRef	        area;

	/** For VBO_MEMORY, this is the offset.  For
        MAIN_MEMORY, this is a pointer to the block
        of uploaded memory */
	void*				_pointer;

	/** Size of one element */
	size_t				elementSize;

	int					numElements;

	uint64				generation;

    GLenum              underlyingRepresentation;

    /**
     The initial size this VAR was allocated with.
     */
    size_t              _maxSize;

	void init(const void* sourcePtr, int _numElements, VARAreaRef _area,
        GLenum glformat, size_t eltSize);

	void update(const void* sourcePtr, int _numElements,
        GLenum glformat, size_t eltSize);

    /** Performs the actual memory transfer (like memcpy).  The dstPtrOffset is the number of 
        <B>bytes</B> to add to _pointer when performing the transfer. */
    void uploadToCard(const void* sourcePtr, int dstPtrOffset, size_t size);

    void set(int index, const void* value, GLenum glformat, size_t eltSize);

	// The following are called by RenderDevice
	void vertexPointer() const;

	void normalPointer() const;

	void colorPointer() const;

	void texCoordPointer(unsigned int unit) const;

    void vertexAttribPointer(unsigned int attribNum, bool normalize) const;

public:

    /** Creates an invalid VAR */
	VAR();

	/**
	 Uploads the memory.  The element type is inferred from the
	 pointer type by the preprocessor.  Sample usage:

     
	  <PRE>
	    // Once at the beginning of the program
        VARAreaRef area = VARArea::create(1024 * 1024);

        //----------
        // Store data in main memory
	    Array<Vector3> vertex;
        Array<int>     index;
		
        //... fill out vertex & index arrays

        //------------
        // Upload to graphics card every frame
        area.reset();
		VAR varray(vertex, area);
        renderDevice->beginIndexedPrimitives();
    		renderDevice->setVertexArray(varray);
            renderDevice->sendIndices(RenderDevice::TRIANGLES, index);
        renderDevice->endIndexedPrimitives();
	  </PRE>
		See GLG3D_Demo for examples.
    */
	template<class T>
	VAR(const T* sourcePtr, int _numElements, VARAreaRef _area) {
		init(sourcePtr, _numElements, _area, glFormatOf(T), sizeof(T));
	}		

	template<class T>
	VAR(const Array<T>& source, VARAreaRef _area) {
		init(source.getCArray(), source.size(), _area, glFormatOf(T), sizeof(T));
	}

	template<class T>
	void update(const T* sourcePtr, int _numElements) {
		update(sourcePtr, _numElements, glFormatOf(T), sizeof(T));
	}

    /**
     Overwrites existing data with data of the same size or smaller.
     Convenient for changing part of a G3D::VARArea without 
     reseting the area (and thereby deallocating the other G3D::VAR
     arrays in it).
     */
    template<class T>
	void update(const Array<T>& source) {
		update(source.getCArray(), source.size(), glFormatOf(T), sizeof(T));
	}

    /**
     Overwrites a single element of an existing array without changing
     the number of elements.  This is faster than calling update for
     large arrays, but slow if many set calls are made.  Typically used
     to change a few key vertices, e.g., the single dark cap point of a
     directional light's shadow volume.
     */
    template<class T>
    void set(int index, const T& value) {
        set(index, &value, glFormatOf(T), sizeof(T));
    }

    /**
     Returns true if this VAR can be used for rendering
     (i.e. contains data and the parent VARArea has not been
     reset).
     */
    bool valid() const;

    /** Maximum size that can be loaded via update into this VAR. */
    inline size_t maxSize() const {
        if (valid()) {
            return _maxSize;
        } else {
            return 0;
        }
    }
};

}

#endif
