/**
  @file GLG3D/Shape.h

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2005-08-10
  @edited  2006-03-25
*/
#ifndef G3D_SHAPE_H
#define G3D_SHAPE_H

#include "G3D/platform.h"
#include "G3D/ReferenceCount.h"
#include "G3D/Triangle.h"
#include "G3D/Sphere.h"
#include "G3D/Plane.h"
#include "G3D/Ray.h"
#include "G3D/Cylinder.h"
#include "G3D/Capsule.h"
#include "G3D/Color4.h"
#include "G3D/Box.h"
#include "G3D/AABSPTree.h"

namespace G3D {

typedef ReferenceCountedPointer<class Shape> ShapeRef;

/** 
   Base class for other shapes.  G3D primitives like Box and Cylinder
   have no base class so that their implementations are maximally efficient,
   and have no "render" method because they are lower-level than the
   rendering API.  
   
   The Shape classes provide a parallel set of classes to the G3D primitives
   that support more functionality.

   Mesh shapes are intentionally immutable because they precompute data.

   <b>BETA API</b> This API is subject to change in future releases.
  */
class Shape : public ReferenceCountedObject {
public:

    enum Type {NONE = 0, MESH = 1, BOX, CYLINDER, SPHERE, RAY, CAPSULE, PLANE, AXES, POINT};

    static std::string typeToString(Type t);

    virtual Type type() const = 0;
   
    virtual void render(
        class RenderDevice* rd, 
        const CoordinateFrame& cframe, 
        Color4 solidColor = Color4(.5,.5,0,.5), 
        Color4 wireColor = Color3::black()) = 0;

    virtual Box& box() { 
        debugAssertM(false, "Not a box");
        static Box b;
        return b;
    }

    virtual CoordinateFrame& axes() {
        debugAssertM(false, "Not axes");
        static CoordinateFrame c;
        return c;
    }

    virtual const Box& box() const { 
        debugAssertM(false, "Not a box");
        static Box b;
        return b;
    }

    virtual Ray& ray() {
        debugAssertM(false, "Not a ray");
        static Ray r;
        return r; 
    }
    
    virtual const Ray& ray() const {
        debugAssertM(false, "Not a ray");
        static Ray r;
        return r; 
    }

    virtual Cylinder& cylinder() { 
        debugAssertM(false, "Not a cylinder");
        static Cylinder c;
        return c; 
    }

    virtual const Cylinder& cylinder() const { 
        debugAssertM(false, "Not a cylinder");
        static Cylinder c;
        return c; 
    }

    virtual Sphere& sphere() { 
        debugAssertM(false, "Not a sphere");
        static Sphere s;
        return s; 
    }

    virtual const Sphere& sphere() const { 
        debugAssertM(false, "Not a sphere");
        static Sphere s;
        return s; 
    }

    virtual Capsule& capsule() { 
        debugAssertM(false, "Not a capsule");
        static Capsule c;
        return c; 
    }

    virtual const Capsule& capsule() const { 
        debugAssertM(false, "Not a capsule");
        static Capsule c;
        return c; 
    }

    virtual Plane& plane() { 
        debugAssertM(false, "Not a plane");
        static Plane p;
        return p;
    }

    virtual const Plane& plane() const { 
        debugAssertM(false, "Not a plane");
        static Plane p;
        return p;
    }

    virtual Vector3& point() {
        debugAssertM(false, "Not a point");
        static Vector3 v;
        return v;
    }

    virtual const Vector3& point() const {
        debugAssertM(false, "Not a point");
        static Vector3 v;
        return v;
    }

    virtual const Array<int>& indexArray() const {
        debugAssertM(false, "Not a mesh");
        static Array<int> x;
        return x;
    }

    virtual const Array<Vector3>& vertexArray() const {
        debugAssertM(false, "Not a mesh");
        static Array<Vector3> x;
        return x;
    }

    /** Surface area of the outside of this object. */
    virtual float area() const = 0;

    /** Volume of the interior of this object. */
    virtual float volume() const = 0;

    /** Center of mass for this object. */
    virtual Vector3 center() const = 0;

    /** Bounding sphere of this object. */
    virtual Sphere boundingSphere() const = 0;

    /** Bounding axis aligned box of this object. */
    virtual AABox boundingAABox() const = 0;

    /** A point selected uniformly at random with respect to the
        surface area of this object.  Not available on the Plane or
        Ray, which have infinite extent.  The normal has unit length
        and points out of the surface. */
    virtual void getRandomSurfacePoint(Vector3& P, 
                                       Vector3& N = Vector3::dummy) const = 0;

    /** A point selected uniformly at random with respect to the
        volume of this object.  Not available on objects with infinite
        extent.*/
    virtual Vector3 randomInteriorPoint() const = 0;

    virtual ~Shape() {}
};

/** 
 Mesh shape is intended for debugging and for collision detection.  It is not a general purpose
 mesh. 

 @sa G3D::PosedModel, G3D::IFSModel, G3D::MD2Model, G3D::MeshAlg, contrib/ArticulatedModel/ArticulatedModel.h
 */
class MeshShape : public Shape {

    Array<Vector3>      _vertexArray;
    Array<int>          _indexArray;

    /** If true, the bspTree and _area, _boundingSphere, _boundingAABox have been initialized. */
    bool                _hasTree;
    AABSPTree<Triangle> _bspTree;
    double              _area;
    Sphere              _boundingSphere;
    AABox               _boundingAABox;

    /** Creates _bspTree */
    void buildBSP();

public:

    /** Copies the geometry from the arrays.
        The index array must describe a triangle list; you can
        convert other primitives using the MeshAlg methods.*/
    MeshShape(const Array<Vector3>& vertex, const Array<int>& index);

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() const {
        return MESH;
    }

    const Array<Vector3>& vertexArray() const {  
        return _vertexArray;
    }

    /** Tri-list index array into vertexArray(). */
    const Array<int>& indexArray() const {
        return _indexArray;
    }

    /** Not computed until the first call to bspTree, area, or getRandomSurfacePoint. */
    virtual const AABSPTree<Triangle>& bspTree() const {
        if (! _hasTree) {
            const_cast<MeshShape*>(this)->buildBSP();
        }
        return _bspTree;
    }

    /** Slow the first time it is called because the BSP tree must be computed. */
    virtual float area() const;

    /** No volume; Mesh is treated as a 2D surface */
    virtual float volume() const;

    virtual Vector3 center() const;

    virtual Sphere boundingSphere() const;

    virtual AABox boundingAABox() const;

    virtual void getRandomSurfacePoint(Vector3& P, 
                                       Vector3& N = Vector3::dummy) const;

    /** Returns a point on the surface */
    virtual Vector3 randomInteriorPoint() const;
}; // Mesh


class BoxShape : public Shape {
    
    G3D::Box          geometry;

public:

    explicit inline BoxShape(const G3D::Box& b) : geometry(b) {}

    /** Creates a box of the given extents centered at the origin. */
    explicit inline BoxShape(const Vector3& extent) : geometry(-extent/2, extent) {}

    /** Creates a box of the given extents centered at the origin. */
    inline BoxShape(float x, float y, float z) : geometry(Vector3(-x/2, -y/2, -z/2), Vector3(x/2, y/2, z/2)) {}

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() const {
        return BOX;
    }

    virtual Box& box() { 
        return geometry;
    }

    virtual const Box& box() const { 
        return geometry;
    }

    virtual float area() const {
        return geometry.area();
    }

    virtual float volume() const {
        return geometry.volume();
    }

    virtual Vector3 center() const {
        return geometry.center();
    }

    virtual Sphere boundingSphere() const {
        Sphere s(geometry.center(), geometry.extent().magnitude() / 2);
        return s;
    }

    virtual AABox boundingAABox() const {
        AABox aab;
        geometry.getBounds(aab);
        return aab;
    }

    virtual void getRandomSurfacePoint(Vector3& P, 
                                       Vector3& N = Vector3::dummy) const {
        geometry.getRandomSurfacePoint(P, N);
    }

    virtual Vector3 randomInteriorPoint() const {
        return geometry.randomInteriorPoint();
    }
};


class RayShape : public Shape {
    
    G3D::Ray          geometry;

public:

    explicit inline RayShape(const G3D::Ray& r) : geometry(r) {}

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() const {
        return RAY;
    }

    virtual Ray& ray() { 
        return geometry;
    }

    virtual const Ray& ray() const { 
        return geometry;
    }

    virtual float area() const {
        return 0.0f;
    }

    virtual float volume() const {
        return 0.0f;
    }

    /** Set to origin of ray. */
    virtual Vector3 center() const {
        return geometry.origin;
    }

    /** Bounds the graphic representation of the ray */
    virtual Sphere BoundingSphere() const {
        return Sphere(geometry.origin + geometry.direction / 2, geometry.direction.magnitude() / 2);
    }

    /** Bounds the graphic representation of the ray */
    virtual AABox BoundingAABox() const {
        debugAssertM(false, "No bounding axis aligned box for ray.");
        static AABox aab;
        return aab;
    }

    /** Returns a point along the ray.  The normal is NaN */
    virtual void getRandomSurfacePoint(Vector3& P, Vector3& N = Vector3::dummy) const {
        P = randomInteriorPoint();
        N = Vector3::nan();
    }

    /** Returns a random point along the ray */
    virtual Vector3 randomInteriorPoint() const {
        return geometry.origin + geometry.direction * uniformRandom(0, 1);
    }

};


class AxesShape : public Shape {
    
    G3D::CoordinateFrame  geometry;

public:

    explicit inline AxesShape(const G3D::CoordinateFrame& a) : geometry(a) {}

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() const {
        return AXES;
    }

    virtual CoordinateFrame& axes() { 
        return geometry;
    }

    virtual const CoordinateFrame& axes() const { 
        return geometry;
    }

    virtual float area() const {
        return 0.0f;
    }

    virtual float volume() const {
        return 0.0f;
    }

    virtual Vector3 center() const {
        return geometry.translation;
    }

    /** Returns a bound on the graphic representation of the axes */
    virtual Sphere BoundingSphere() const {
        return Sphere(geometry.translation, 1);
    }

    virtual AABox BoundingAABox() const {
        debugAssertM(false, "No bounding axis aligned box for axes.");
        static AABox aab;
        return aab;
    }

    /** Returns the origin, normal = NaN*/
    virtual void getRandomSurfacePoint(Vector3& P, Vector3& N = Vector3::dummy) const {
        P = geometry.translation;
        N = Vector3::nan();
    }

    virtual Vector3 randomInteriorPoint() const {
        return geometry.translation;
    }
};


class CylinderShape : public Shape {

    G3D::Cylinder       geometry;

public:

    explicit inline CylinderShape(const G3D::Cylinder& c) : geometry(c) {}

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() const {
        return CYLINDER;
    }

    virtual Cylinder& cylinder() { 
        return geometry;
    }

    virtual const Cylinder& cylinder() const {
        return geometry;
    }

    virtual float area() const {
        return geometry.area();
    }

    virtual float volume() const {
        return geometry.volume();
    }

    virtual Vector3 center() const {
        return geometry.center();
    }

    virtual Sphere boundingSphere() const {
        Vector3 v(0, geometry.height() / 2, 0);
        v += Vector3(geometry.radius(), 0, 0);
        Sphere s(geometry.center(), G3D::abs(v.magnitude()));
        return s;
    }

    virtual AABox boundingAABox() const {
        AABox aab;
        geometry.getBounds(aab);
        return aab;
    }

    virtual void getRandomSurfacePoint(Vector3& P, 
                                       Vector3& N = Vector3::dummy) const {
        geometry.getRandomSurfacePoint(P, N);
    }

    virtual Vector3 randomInteriorPoint() const {
        return geometry.randomInteriorPoint();
    }

};


class SphereShape : public Shape {

    G3D::Sphere         geometry;

public:

    explicit inline SphereShape(const G3D::Sphere& s) : geometry(s) {}

    inline SphereShape(const Vector3& center, float radius) : geometry(center, radius) {}

    /** Creates a sphere centered at the origin with the given radius. */
    inline SphereShape(float radius) : geometry(Vector3::zero(), radius) {}

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() const {
        return SPHERE;
    }

    virtual Sphere& sphere() { 
        return geometry;
    }

    virtual const Sphere& sphere() const { 
        return geometry;
    }

    virtual void getRandomSurfacePoint(Vector3& P, Vector3& N = Vector3::dummy) const {
        P = geometry.randomSurfacePoint();
        N = (P - geometry.center).direction();
    }

    virtual Vector3 randomInteriorPoint() const {
        return geometry.randomInteriorPoint();
    }

    virtual float area() const {
        return geometry.area();
    }

    virtual float volume() const {
        return geometry.volume();
    }

    virtual Vector3 center() const {
        return geometry.center;
    }

    virtual Sphere boundingSphere() const {
        return geometry;
    }

    virtual AABox boundingAABox() const {
        AABox aab;
        geometry.getBounds(aab);
        return aab;
    }
};


class CapsuleShape : public Shape {
public:

    G3D::Capsule         geometry;

    explicit inline CapsuleShape(const G3D::Capsule& c) : geometry(c) {}

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() const {
        return CAPSULE;
    }

    virtual Capsule& capsule() { 
        return geometry;
    }

    virtual const Capsule& capsule() const { 
        return geometry;
    }

    virtual float area() const {
        return geometry.area();
    }

    virtual float volume() const {
        return geometry.volume();
    }

    virtual Vector3 center() const {
        return geometry.center();
    }

    virtual Sphere boundingSphere() const {
        Sphere s(geometry.center(), G3D::abs((geometry.center() - geometry.point(0)).magnitude()) + geometry.radius());
        return s;
    }

    virtual AABox boundingAABox() const {
        AABox aab;
        geometry.getBounds(aab);
        return aab;
    }

    virtual void getRandomSurfacePoint(Vector3& P, Vector3& N = Vector3::dummy) const {
        geometry.getRandomSurfacePoint(P, N);
    }

    virtual Vector3 randomInteriorPoint() const {
        return geometry.randomInteriorPoint();
    }

};


class PlaneShape : public Shape {

    G3D::Plane         geometry;

public:

    explicit inline PlaneShape(const G3D::Plane& p) : geometry(p) {}

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() const {
        return PLANE;
    }

    virtual Plane& plane() { 
        return geometry;
    }

    virtual const Plane& plane() const { 
        return geometry;
    }

    virtual float area() const {
        return inf();
    }

    virtual float volume() const {
        return 0.0f;
    }

    virtual Vector3 center() const {
        return geometry.center();
    }

    virtual Sphere boundingSphere() const {
        return Sphere(geometry.center(), inf());
    }

    virtual AABox boundingAABox() const {
        return AABox(-Vector3::inf(), Vector3::inf());
    }

    virtual void getRandomSurfacePoint(Vector3& P, Vector3& N = Vector3::dummy) const {
        P = Vector3::nan();
        N = geometry.normal();
    }

    virtual Vector3 randomInteriorPoint() const {
        return Vector3::nan();
    }

};


class PointShape : public Shape {

    Vector3    geometry;

public:

    explicit inline PointShape(const Vector3& p) : geometry(p) {}

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() const {
        return POINT;
    }

    virtual Vector3& point() { 
        return geometry;
    }

    virtual const Vector3& point() const { 
        return geometry;
    }

    virtual float area() const {
        return 0.0f;
    }

    virtual float volume() const {
        return 0.0f;
    }

    virtual Vector3 center() const {
        return geometry.xyz();
    }

    virtual Sphere boundingSphere() const {
        return Sphere(geometry, 0.0f);
    }

    virtual AABox boundingAABox() const {
      return AABox(geometry, geometry);
    }

    virtual void getRandomSurfacePoint(Vector3& P, Vector3& N = Vector3::dummy) const {
        P = geometry;
        N = Vector3::unitY();
    }

    virtual Vector3 randomInteriorPoint() const {
        return geometry;
    }

};

}

#endif
