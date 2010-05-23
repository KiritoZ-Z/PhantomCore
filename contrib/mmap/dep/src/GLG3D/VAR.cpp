/**
 @file VAR.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com
 
 @created 2003-04-08
 @edited  2005-06-20
 */

#include "GLG3D/VAR.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/VARArea.h"
#include "G3D/Log.h"
#include "GLG3D/getOpenGLState.h"

namespace G3D {

VAR::VAR() : area(NULL), _pointer(NULL), elementSize(0), 
    numElements(0), generation(0), underlyingRepresentation(GL_FLOAT), _maxSize(0) {
}


bool VAR::valid() const {
    return
        (! area.isNull()) && 
        (area->currentGeneration() == generation) &&
        // If we're in VBO_MEMORY mode, the pointer can be null.  Otherwise
        // it shouldn't be
        (VARArea::mode == VARArea::VBO_MEMORY || _pointer);
}


void VAR::init(
    const void*         sourcePtr,
    int                 _numElements,
    VARAreaRef          _area,
    GLenum              glformat,
    size_t              eltSize) {

	alwaysAssertM(! _area.isNull(), "Bad VARArea");

	numElements              = _numElements;
	area		             = _area;
    underlyingRepresentation = glformat;
	elementSize              = eltSize;

    size_t size              = elementSize * numElements;
    _maxSize                 = size;

    debugAssertM(
        (elementSize % sizeOfGLFormat(underlyingRepresentation)) == 0,
        "Sanity check failed on OpenGL data format; you may"
        " be using an unsupported type in a vertex array.");

	generation = area->currentGeneration();

    _pointer = (uint8*)area->gl_basePointer() + area->allocatedSize();

	// Ensure that the next memory address is 8-byte aligned
	size_t pointerOffset = ((8 - (size_t)_pointer % 8) % 8);

    if (_numElements == 0) {
        pointerOffset = 0;
    }

    // Adjust pointer to new 8-byte alignment
    _pointer = (uint8*)_pointer + pointerOffset;

    size_t newAlignedSize = size + pointerOffset;

	alwaysAssertM(newAlignedSize <= area->freeSize(),
        "VARArea too small to hold new VAR (possibly due to rounding to 8-byte boundaries).");

	// Upload the data
    if (size > 0) {
        // Update VARArea values
        area->updateAllocation(newAlignedSize);

        uploadToCard(sourcePtr, 0, size);
    }
}


void VAR::update(
    const void*         sourcePtr,
    int                 _numElements,
    GLenum              glformat,
    size_t              eltSize) {

	size_t size = eltSize * _numElements;

    alwaysAssertM(size <= _maxSize,
        "A VAR can only be updated with an array that is smaller "
        "or equal size (in bytes) to the original array.");

    alwaysAssertM(generation == area->currentGeneration(),
        "The VARArea has been reset since this VAR was created.");

	numElements              = _numElements;
    underlyingRepresentation = glformat;
	elementSize              = eltSize;

    debugAssertM(
        (elementSize % sizeOfGLFormat(underlyingRepresentation)) == 0,
        "Sanity check failed on OpenGL data format; you may"
        " be using an unsupported type in a vertex array.");
	
	// Upload the data
    if (size > 0) {
        uploadToCard(sourcePtr, 0, size);
    }
}


void VAR::set(int index, const void* value, GLenum glformat, size_t eltSize) {
    (void)glformat;
    debugAssertM(index < numElements && index >= 0, 
        "Cannot call VAR::set with out of bounds index");
    
    debugAssertM(glformat == underlyingRepresentation, 
        "Value argument to VAR::set must match the intialization type.");

    debugAssertM(eltSize == elementSize, 
        "Value argument to VAR::set must match the intialization type's memory footprint.");

    uploadToCard(value, index * eltSize, eltSize);
}


void VAR::uploadToCard(const void* sourcePtr, int dstPtrOffset, size_t size) {
    void* ptr = (void*)(reinterpret_cast<intptr_t>(_pointer) + dstPtrOffset);

    switch (VARArea::mode) {
    case VARArea::VBO_MEMORY:
        // Don't destroy any existing bindings; this call can
        // be made at any time and the program might also
        // use VBO on its own.
        glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, area->glbuffer);
            glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, (GLintptrARB)ptr, size, sourcePtr);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        glPopClientAttrib();
        break;

    case VARArea::MAIN_MEMORY:
        System::memcpy(ptr, sourcePtr, size);
        break;

    default:
        alwaysAssertM(false, "Fell through switch");
    }
}


// The following are called by the VARSystem.
void VAR::vertexPointer() const {
	debugAssert(valid());
	glEnableClientState(GL_VERTEX_ARRAY);
    debugAssertM(underlyingRepresentation != GL_UNSIGNED_INT, 
              "OpenGL does not support GL_UNSIGNED_INT as a vertex format.");
    debugAssertM(underlyingRepresentation != GL_UNSIGNED_SHORT, 
              "OpenGL does not support GL_UNSIGNED_SHORT as a vertex format.");
    debugAssertM(underlyingRepresentation != GL_UNSIGNED_BYTE, 
              "OpenGL does not support GL_UNSIGNED_BYTE as a vertex format.");
	glVertexPointer(elementSize / sizeOfGLFormat(underlyingRepresentation), 
                    underlyingRepresentation, elementSize, _pointer);
}


void VAR::normalPointer() const {
	debugAssert(valid());
	debugAssert((double)elementSize / sizeOfGLFormat(underlyingRepresentation) == 3.0);
    debugAssertM(underlyingRepresentation != GL_UNSIGNED_INT, 
              "OpenGL does not support GL_UNSIGNED_INT as a normal format.");
    debugAssertM(underlyingRepresentation != GL_UNSIGNED_SHORT, 
              "OpenGL does not support GL_UNSIGNED_SHORT as a normal format.");
    debugAssertM(underlyingRepresentation != GL_UNSIGNED_BYTE,
              "OpenGL does not support GL_UNSIGNED_BYTE as a normal format.");
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(underlyingRepresentation, elementSize, _pointer); 
}


void VAR::colorPointer() const {
	debugAssert(valid());
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(elementSize / sizeOfGLFormat(underlyingRepresentation),
                   underlyingRepresentation, elementSize, _pointer); 
}


void VAR::texCoordPointer(uint unit) const {
	debugAssert(valid());
    debugAssertM(GLCaps::supports_GL_ARB_multitexture() || (unit == 0),
        "Graphics card does not support multitexture");

    if (GLCaps::supports_GL_ARB_multitexture()) {
	    glClientActiveTextureARB(GL_TEXTURE0_ARB + unit);
    }
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(elementSize / sizeOfGLFormat(underlyingRepresentation),
                      underlyingRepresentation, elementSize, _pointer);

    if (GLCaps::supports_GL_ARB_multitexture()) {
        glClientActiveTextureARB(GL_TEXTURE0_ARB);
    }
}


void VAR::vertexAttribPointer(uint attribNum, bool normalize) const {
	debugAssert(valid());
    if (GLCaps::supports_GL_ARB_vertex_program()) {
	    glEnableVertexAttribArrayARB(attribNum);
	    glVertexAttribPointerARB(attribNum, elementSize / sizeOfGLFormat(underlyingRepresentation),
                          underlyingRepresentation, normalize, elementSize, _pointer);
    }
}

}
