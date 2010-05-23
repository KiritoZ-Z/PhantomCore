/**
 @file GLG3D/Shape.cpp

 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2005-08-30
 @edited  2005-08-30
 */

#include "GLG3D/Shape.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/Draw.h"

namespace G3D {

std::string Shape::typeToString(Type t) {
    const static std::string name[] = 
    {"NONE", "MESH", "BOX", "CYLINDER", "SPHERE", "RAY", "CAPSULE", "PLANE", "AXES", "POINT"};

    if (t <= POINT) {
        return name[t];
    } else {
        return "";
    }
}

////////////////////////////////////////////////////////////////////////

MeshShape::MeshShape(
    const Array<Vector3>& vertex, 
    const Array<int>& index) : 
    _vertexArray(vertex), 
    _indexArray(index),
    _hasTree(false) {
    
    debugAssert(index.size() % 3 == 0);
    debugAssert(index.size() >= 0);
}


void MeshShape::buildBSP() {
    debugAssert(! _hasTree);

    _area = 0;
    for (int i = 0; i < _indexArray.size(); i += 3) {
        const int v0 = _indexArray[i];
        const int v1 = _indexArray[i + 1];
        const int v2 = _indexArray[i + 2];

        Triangle tri(_vertexArray[v0], _vertexArray[v1], _vertexArray[v2]);
        _area += tri.area();
        _bspTree.insert(tri);
    }
    _bspTree.balance();

    Box box;
    MeshAlg::computeBounds(_vertexArray, box, _boundingSphere);
    _boundingSphere.getBounds(_boundingAABox);

    _hasTree = true;
}


float MeshShape::area() const {
    if (! _hasTree) {
        const_cast<MeshShape*>(this)->buildBSP();
    }
    return _area;
}


float MeshShape::volume() const {
    return 0;
}

Vector3 MeshShape::center() const {
    if (! _hasTree) {
        const_cast<MeshShape*>(this)->buildBSP();
    }

    return _boundingAABox.center();
}

Sphere MeshShape::boundingSphere() const {
    if (! _hasTree) {
        const_cast<MeshShape*>(this)->buildBSP();
    }
    return _boundingSphere;
}

AABox MeshShape::boundingAABox() const {
    if (! _hasTree) {
        const_cast<MeshShape*>(this)->buildBSP();
    }
    return _boundingAABox;
}

void MeshShape::getRandomSurfacePoint(
    Vector3& P,
    Vector3& N) const {
    
    if (! _hasTree) {
        const_cast<MeshShape*>(this)->buildBSP();
    }

    // Choose uniformly at random based on surface area
    float sum = uniformRandom(0, _area);
    typedef AABSPTree<Triangle>::Iterator IT;

    IT it  = _bspTree.begin();
    IT end = _bspTree.end();

    while (it != end) {
        const Triangle& tri = *it;
        sum -= tri.area();
        if (sum <= 0) {
            // We want a point from this triangle.
            N = tri.normal();
            P = tri.randomPoint();
            return;
        }
        ++it;
    }
    
    // Should never get here, but we might due to roundoff.  Choose a point 
    // on the first triangle.
    const Triangle& tri = *_bspTree.begin();
    N = tri.normal();
    P = tri.randomPoint();
}


Vector3 MeshShape::randomInteriorPoint() const {
    Vector3 P;
    getRandomSurfacePoint(P);
    return P;
}


void MeshShape::render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor, Color4 wireColor) {
    CoordinateFrame cframe0 = rd->getObjectToWorldMatrix();

    rd->pushState();
        rd->setObjectToWorldMatrix(cframe0 * cframe);
        if (solidColor.a < 1.0) {
            rd->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        }
        rd->setColor(solidColor);
        rd->beginPrimitive(RenderDevice::TRIANGLES);
            for (int i = 0; i < _indexArray.size(); i += 3) {
                const Vector3& v0 = _vertexArray[_indexArray[i + 0]];
                const Vector3& v1 = _vertexArray[_indexArray[i + 1]];
                const Vector3& v2 = _vertexArray[_indexArray[i + 2]];
                rd->sendVertex(v0);
                rd->sendVertex(v1);
                rd->sendVertex(v2);
            }
        rd->endPrimitive();

        rd->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        rd->setLineWidth(2);
        rd->setColor(wireColor);
        rd->beginPrimitive(RenderDevice::LINES);
            for (int i = 0; i < _indexArray.size(); i += 3) {
                const Vector3& v0 = _vertexArray[_indexArray[i + 0]];
                const Vector3& v1 = _vertexArray[_indexArray[i + 1]];
                const Vector3& v2 = _vertexArray[_indexArray[i + 2]];
                rd->sendVertex(v0);
                rd->sendVertex(v1);
                rd->sendVertex(v1);
                rd->sendVertex(v2);
                rd->sendVertex(v2);
                rd->sendVertex(v0);
            }
        rd->endPrimitive();
    rd->popState();
}

////////////////////////////////////////////////////////////////////////

void BoxShape::render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor, Color4 wireColor) {
    CoordinateFrame cframe0 = rd->getObjectToWorldMatrix();
    rd->setObjectToWorldMatrix(cframe0 * cframe);
    Draw::box(geometry, rd, solidColor, wireColor);
    rd->setObjectToWorldMatrix(cframe0);
}


void SphereShape::render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor, Color4 wireColor) {
    CoordinateFrame cframe0 = rd->getObjectToWorldMatrix();
    rd->setObjectToWorldMatrix(cframe0 * cframe);
    Draw::sphere(geometry, rd, solidColor, wireColor);
    rd->setObjectToWorldMatrix(cframe0);
}


void CylinderShape::render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor, Color4 wireColor) {
    CoordinateFrame cframe0 = rd->getObjectToWorldMatrix();
    rd->setObjectToWorldMatrix(cframe0 * cframe);
    Draw::cylinder(geometry, rd, solidColor, wireColor);
    rd->setObjectToWorldMatrix(cframe0);
}


void CapsuleShape::render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor, Color4 wireColor) {
    CoordinateFrame cframe0 = rd->getObjectToWorldMatrix();
    rd->setObjectToWorldMatrix(cframe0 * cframe);
    Draw::capsule(geometry, rd, solidColor, wireColor);
    rd->setObjectToWorldMatrix(cframe0);
}


void RayShape::render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor, Color4 wireColor) {
    (void)wireColor;
    CoordinateFrame cframe0 = rd->getObjectToWorldMatrix();
    rd->setObjectToWorldMatrix(cframe0 * cframe);
    Draw::ray(geometry, rd, solidColor);
    rd->setObjectToWorldMatrix(cframe0);
}


void AxesShape::render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor, Color4 wireColor) {
    (void)wireColor;
    (void)solidColor;
    CoordinateFrame cframe0 = rd->getObjectToWorldMatrix();
    rd->setObjectToWorldMatrix(cframe0 * cframe);
    Draw::axes(geometry, rd);
    rd->setObjectToWorldMatrix(cframe0);
}

void PointShape::render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor, Color4 wireColor) {
    (void)wireColor;
    Draw::sphere(Sphere(geometry, 0.1f), rd, solidColor, Color4::clear());
}

void PlaneShape::render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor, Color4 wireColor) {
    CoordinateFrame cframe0 = rd->getObjectToWorldMatrix();

    rd->setObjectToWorldMatrix(cframe0 * cframe);

    Draw::plane(geometry, rd, solidColor, wireColor);
    
    rd->setObjectToWorldMatrix(cframe0);
}

}

