/**
  @file VARArea.h
  @maintainer Morgan McGuire, morgan@graphics3d.com
  @created 2003-08-09
  @edited  2006-02-06
*/

#ifndef GLG3D_VARAREA_H
#define GLG3D_VARAREA_H

#include "G3D/ReferenceCount.h"
#include "G3D/Table.h"
#include "GLG3D/Milestone.h"

namespace G3D {

// Forward-declaration because including RenderDevice.h would include VARArea.h also
// This kills the VARAreaRef typedef
class RenderDevice;

typedef ReferenceCountedPointer<class VARArea> VARAreaRef;

/**
 Wrapper for OpenGL Vertex Buffer Object
 http://oss.sgi.com/projects/ogl-sample/registry/ARB/vertex_buffer_object.txt
 http://developer.nvidia.com/docs/IO/8230/GDC2003_OGL_BufferObjects.ppt

 Allocate a VARArea, then allocate VARs within it.  VARAreas are garbage
 collected.  When no pointers remain to VARs inside it or the VARArea itself,
 it will automatically be reclaimed by the system.

 You cannot mix pointers from different VARAreas when rendering.  For
 example, if the vertex VAR is in one VARArea, the normal VAR and color
 VAR must come from the same area.


 You can't find out how much space is left for VARAreas in video memory,
 except by checking the VARArea::create value and seeing if it is NULL.
 */
class VARArea : public ReferenceCountedObject {
public:

    /**
     These values are <B>hints</B>. Your program will work correctly
     regardless of which you use, but using the appropriate value
     lets the renderer optimize for your useage patterns and can
     increase performance.

     Use WRITE_EVERY_FRAME if you write <I>at least</I> once per frame
     (e.g. software animation).

     Use WRITE_EVERY_FEW_FRAMES if you write to the area as part of
     the rendering loop, but not every frame (e.g. impostors, deformable
     data).

     Use WRITE_ONCE if you do not write to the area inside the rendering
     loop (e.g. rigid bodies loaded once at the beginning of a game level).  
     This does not mean you can't write multiple times
     to the area, just that writing might be very slow compared to rendering.

     Correspond to OpenGL hints: 
      WRITE_ONCE : GL_STATIC_DRAW_ARB
      WRITE_EVERY_FRAME : GL_STREAM_DRAW_ARB
      WRITE_EVERY_FEW_FRAMEs : DYNAMIC_DRAW_ARB
     */
    enum UsageHint {
        WRITE_ONCE,
        WRITE_EVERY_FEW_FRAMES,
        WRITE_EVERY_FRAME};

private:

	friend class VAR;
    friend class RenderDevice;

    /**
     The milestone is used for finish().  It is created
     by RenderDevice::setVARAreaMilestones.  If NULL, there
     is no milestone pending.
     */
    MilestoneRef        milestone;

	/** Number of bytes currently allocated out of size total. */
	size_t				allocated;

	/**
	 This count prevents vertex arrays that have been freed from
	 accidentally being used-- it is incremented every time
     the VARArea is reset.
	 */
	uint64				generation;

	/** The maximum size of this area that was ever used. */
	size_t				peakAllocated;

    /** Set by RenderDevice */
    RenderDevice*       renderDevice;


	/** Total  number of bytes in this area.  May be zero if resources have been freed.*/
	size_t				size;

    /**
     The OpenGL buffer object associated with this area
     (only used when mode == VBO_MEMORY)
     */
    uint32              glbuffer;

	/** Pointer to the memory (NULL when
        the VBO extension is not present). */
	void*				basePointer;

    enum Mode {UNINITIALIZED, VBO_MEMORY, MAIN_MEMORY};
    static Mode         mode;

    /** Updates allocation and peakAllocation based off of new allocation. */
    inline void updateAllocation(size_t newAllocation) {
        allocated += newAllocation;
        peakAllocated = (size_t)iMax((int)peakAllocated, (int)allocated);
    }

    static size_t       _sizeOfAllVARAreasInMemory;

	VARArea(size_t _size, UsageHint h);

    static Array<VARAreaRef>    allVARAreas;

    /** Removes elements of allVARAreas that are not externally referenced.
        Called whenever a new VARArea is created.*/
    static void cleanCache();

public:

    /**
     You should always create your VARAreas at least 8 bytes larger
     than needed for each individual VAR because VARArea tries to 
     align VAR starts in memory with dword boundaries.
     */
    static VARAreaRef create(size_t s , UsageHint h = WRITE_EVERY_FRAME);

    ~VARArea();

    inline size_t totalSize() const {
	    return size;
    }


    inline size_t freeSize() const {
	    return size - allocated;
    }


    inline size_t allocatedSize() const {
	    return allocated;
    }


    inline size_t peakAllocatedSize() const {
	    return peakAllocated;
    }

    inline uint64 currentGeneration() const {
        return generation;
    }

    /**
     Provided for breaking the VARArea abstraction; use G3D::VAR and 
     G3D::RenderDevice in general.

     When using the OpenGL vertex buffer API, this is the underlying 
     OpenGL vertex buffer object.  It is zero when using system memory.
     The caller cannot control whether VBO is used or not; G3D selects
     the best method automatically.
     */
    inline uint32 gl_vertexBufferObject() const {
        return glbuffer;
    }

    /**
     Provided for breaking the VARArea abstraction; use G3D::VAR and 
     G3D::RenderDevice in general.

     When using system memory, this is a pointer to the beginning of 
     the system memory block in which data is stored.  Null when using VBO.
     */
    inline void* gl_basePointer() const {
        return basePointer;
    }

    /**
     Blocks the CPU until all rendering calls referencing 
     this area have completed.
     */
    void finish();

	/** Finishes, then frees all VAR memory inside this area.*/ 
	void reset();

    /** Returns the total size of all VARAreas allocated.  Note that not all
        will be in video memory, and some will be backed by main memory 
        even if nominally stored in video memory, so the total size may
        exceed the video memory size.*/
    static size_t sizeOfAllVARAreasInMemory() {
        return _sizeOfAllVARAreasInMemory;
    }

    /** Releases all VARAreas. Called before shutdown by RenderDevice. */
    static void cleanupAllVARAreas();

};
}

template <>
struct GHashCode<G3D::VARArea*>
{
    size_t operator()(const G3D::VARArea* key) const { return reinterpret_cast<size_t>(key); }
};

#endif
