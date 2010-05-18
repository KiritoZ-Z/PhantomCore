/**
 @file Draw.h
  
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2003-10-29
 @edited  2005-08-06
 */

#ifndef G3D_DRAW_H
#define G3D_DRAW_H

#include "G3D/Color3.h"
#include "G3D/Color4.h"
#include "G3D/Vector3.h"
#include "G3D/MeshAlg.h"
#include "G3D/Rect2D.h"
#include "G3D/GCamera.h"

namespace G3D {

class RenderDevice;
class Sphere;
class LineSegment;
class Box;
class AABox;
class Line;
class Capsule;
class Cylinder;
class Plane;

/**
 Useful routines for rendering primitives when debugging.  Compared
 to the optimized RenderDevice::beginIndexedPrimitives calls used
 by IFSModel, these routines are slow.

 When rendering translucent bounding objects, depth write is automatically
 disabled.  Render from back to front for proper transparency.
 */
class Draw {
private:

    static const int WIRE_SPHERE_SECTIONS;
    static const int SPHERE_SECTIONS;
    static const int SPHERE_YAW_SECTIONS;
    static const int SPHERE_PITCH_SECTIONS;

    /** Called from wireSphere, wireCapsule */
    static void wireSphereSection(
        const Sphere&               sphere,
        class RenderDevice*         renderDevice,
        const class Color4&         color,
        bool                        top,
        bool                        bottom);


    static void sphereSection(
        const Sphere&       sphere,
        RenderDevice*       renderDevice,
        const Color4&       color,
        bool                top,
        bool                bottom);

    /**
     Returns the scale due to perspective at
     a point for a line.
     */
    static float perspectiveLineThickness(
        RenderDevice*       rd,
        const class Vector3&      pt);

public:

    /**
    Renders exact corners of a 2D polygon using lines.
    Assumes you already called push2D(). 
     */
    static void poly2DOutline(const Array<Vector2>& polygon, RenderDevice* renderDevice, const Color4& color = Color3::yellow());
    static void poly2D(const Array<Vector2>& polygon, RenderDevice* renderDevice, const Color4& color = Color3::yellow());


    /**
     Set the solid color or wire color to Color4::clear() to
     prevent rendering of surfaces or lines.
     */
    static void box(
        const Box&          box,
        RenderDevice*       rd,
        const Color4&       solidColor = Color4(1,.2f,.2f,.5f),
        const Color4&       wireColor  = Color3::black());

    static void box(
        const AABox&        box,
        RenderDevice*       rd,
        const Color4&       solidColor = Color4(1,.2f,.2f,.5f),
        const Color4&       wireColor  = Color3::black());

    static void sphere(
        const Sphere&       sphere,
        RenderDevice*       rd,
        const Color4&       solidColor = Color4(1, 1, 0, .5f),
        const Color4&       wireColor  = Color3::black());

    static void plane(
        const Plane&        plane,
        RenderDevice*       rd,
        const Color4&       solidColor = Color4(.2f, .2f, 1, .5f),
        const Color4&       wireColor  = Color3::black());

    static void line(
        const Line&         line,
        RenderDevice*       rd,
        const Color4&       color = Color3::black());

    static void lineSegment(
        const LineSegment&  lineSegment,
        RenderDevice*       rd,
        const Color4&       color = Color3::black(),
        float               scale = 1);

    /**
     Renders per-vertex normals as thin arrows.  The length
     of the normals is scaled inversely to the number of normals
     rendered.
     */
    static void vertexNormals(
        const G3D::MeshAlg::Geometry&    geometry,
        RenderDevice*               renderDevice,
        const Color4&               color = Color3::green() * .5,
        float                       scale = 1);

    /**
     Convenient for rendering tangent space basis vectors.
     */
    static void vertexVectors(
        const Array<Vector3>&       vertexArray,
        const Array<Vector3>&       directionArray,
        RenderDevice*               renderDevice,
        const Color4&               color = Color3::red() * 0.5,
        float                       scale = 1);

    static void capsule(
       const Capsule&       capsule, 
       RenderDevice*        renderDevice,
       const Color4&        solidColor = Color4(1,0,1,.5),
       const Color4&        wireColor = Color3::black());

    static void cylinder(
       const Cylinder&      cylinder, 
       RenderDevice*        renderDevice,
       const Color4&        solidColor = Color4(1,1,0,.5),
       const Color4&        wireColor = Color3::black());

    static void ray(
        const class Ray&    ray,
        RenderDevice*       renderDevice,
        const Color4&       color = Color3::orange(),
        float               scale = 1);
    
    static void arrow(
        const Vector3&      start,
        const Vector3&      direction,
        RenderDevice*       renderDevice,
        const Color4&       color = Color3::orange(),
        float               scale = 1.0f);

    static void axes(
        const class CoordinateFrame& cframe,
        RenderDevice*       renderDevice,
        const Color4&       xColor = Color3::red(),
        const Color4&       yColor = Color3::green(),
        const Color4&       zColor = Color3::blue(),
        float               scale = 1.0f);

    static void axes(
        RenderDevice*       renderDevice,
        const Color4&       xColor = Color3::red(),
        const Color4&       yColor = Color3::green(),
        const Color4&       zColor = Color3::blue(),
        float               scale = 1.0f);

    /**
	Draws a rectangle using the current textures and texture coordinates that by default stretch the textures
	to fill the rectangle.
    Provided texture coordinates are upper bounds for each of four textures.  The actual tex
    coords will vary from (0, 0) to those bounds.
    */
    static void rect2D(
        const class Rect2D& rect,
        RenderDevice* rd,
        const Color4& color = Color3::white(),
        const Vector2& texCoord0 = Vector2(1,1),
        const Vector2& texCoord1 = Vector2(1,1),
        const Vector2& texCoord2 = Vector2(1,1),
        const Vector2& texCoord3 = Vector2(1,1));

    /** Leaves the renderDevice color and texture coordinates modified. Does not allow
        custom texture coordinates and only sets texture coordinate 0.*/
    static void fastRect2D(
        const Rect2D& rect,
        RenderDevice* rd,
        const Color4& color = Color3::white());


    /** Draws the specified rectangle, setting coordinates for
      the first four texture units as specified. */
    static void rect2D(
        const class Rect2D& rect,
        RenderDevice* rd,
        const Color4& color,
        const Rect2D& texCoord0,
        const Rect2D& texCoord1 = Rect2D::xywh(0,0,1,1),
        const Rect2D& texCoord2 = Rect2D::xywh(0,0,1,1),
        const Rect2D& texCoord3 = Rect2D::xywh(0,0,1,1));

    /** Draws a border of size width about the rectangle
        using polygons (since RenderDevice::LINE_STRIP doesn't 
        guarantee pixel widths). */
    static void rect2DBorder(
        const class Rect2D& rect,
        RenderDevice* rd,
        const Color4& color = Color3::black(),
        float innerBorder = 0,
        float outerBorder = 1);

    static void frustum(
        const class GCamera::Frustum& frustum,
        RenderDevice* rd,
        const Color4& color = Color4(1,.4f,.4f,0.2f),
        const Color4& wire = Color3::black());

    /**
     This method is as hideously slow as it is convenient.  If you care
     about performance, make a rectangular texture (without MIPMaps) 
     and draw using that.
     */
    static void fullScreenImage(
        const class GImage& im,
        RenderDevice*       renderDevice);
};

}

#endif
