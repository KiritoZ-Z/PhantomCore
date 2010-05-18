/**
  @file GLG3D/ThirdPersonManipulator.h

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2006-06-09
  @edited  2006-10-20
*/

#include "GLG3D/ThirdPersonManipulator.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/Draw.h"
#include "GLG3D/GWindow.h"
#include "GLG3D/UserInput.h"
#include "G3D/Sphere.h"
#include "G3D/Box.h"

namespace G3D {

namespace _internal {

Vector3 UIGeom::segmentNormal(const LineSegment& seg, const Vector3& eye) {
    Vector3 E = eye - seg.point(0);
    Vector3 V = seg.point(1) - seg.point(0);
    Vector3 U = E.cross(V);
    Vector3 N = V.cross(U).direction();
    return N;
}


Vector3 UIGeom::computeEye(RenderDevice* rd) {
    Vector3 eye = rd->getObjectToWorldMatrix().pointToObjectSpace(rd->getCameraToWorldMatrix().translation);
    return eye;
}

bool UIGeom::contains(
    const Vector2&  p, 
    float&          nearestDepth, 
    Vector2&        tangent2D,
    float&          projectionW,
    float           lineRadius) const {
    
    bool result = false;

    // Find the nearest containing polygon
    for (int i = 0; i < poly2D.size(); ++i) {
        bool reverse = m_twoSidedPolys && polyBackfacing[i];
        if ((polyDepth[i] < nearestDepth) && 
            poly2D[i].contains(p, reverse)) {
            
            nearestDepth = polyDepth[i];
            result = true;
        }
    }

    // Find the nearest containing line
    for (int i = 0; i < line2D.size(); ++i) {
        int segmentIndex = 0;
        float d = line2D[i].distance(p, segmentIndex);
        if (d <= lineRadius) {
            // Find the average depth of this segment
            float depth = (lineDepth[i][segmentIndex] + lineDepth[i][segmentIndex + 1]) / 2;
            if (depth < nearestDepth) {
                tangent2D = (line2D[i].segment(segmentIndex).point(1) - line2D[i].segment(segmentIndex).point(0)) / 
                            line3D[i].segment(segmentIndex).length();
                projectionW = (lineW[i][segmentIndex] + lineW[i][segmentIndex]) / 2;
                nearestDepth = depth;
                result = true;
            }
        }
    }

    return result;
}


void UIGeom::computeProjection(RenderDevice* rd) {
    Array<Vector2> vertex;

    // Polyline segments
    line2D.resize(line3D.size());
    lineDepth.resize(line2D.size());
    lineW.resize(line2D.size());
    for (int i = 0; i < line3D.size(); ++i) {
        const PolyLine& line = line3D[i];
        Array<float>& depth = lineDepth[i];
        Array<float>& w = lineW[i];

        // Project all vertices
        vertex.resize(line.numVertices(), false);
        depth.resize(vertex.size());
        w.resize(vertex.size());
        for (int j = 0; j < line.numVertices(); ++j) {
            Vector4 v = rd->project(line.vertex(j));
            vertex[j] = v.xy();
            depth[j] = v.z;
            w[j] = v.w;
        }

        line2D[i] = PolyLine2D(vertex);
    }

    // Polygons
    poly2D.resize(poly3D.size());
    polyDepth.resize(poly2D.size());
    if (poly3D.size() > 0) {
        polyBackfacing.resize(poly3D.size());

        // Used for backface classification
        Vector3 objEye = rd->getModelViewMatrix().inverse().translation;
        
        float z = 0;
        for (int i = 0; i < poly3D.size(); ++i) {
            const ConvexPolygon& poly = poly3D[i];
            vertex.resize(poly3D[i].numVertices(), false);
            for (int j = 0; j < poly.numVertices(); ++j) {
                Vector4 v = rd->project(poly.vertex(j));
                vertex[j] = v.xy();
                z += v.z;
            }

            polyDepth[i] = z / poly.numVertices();
            poly2D[i] = ConvexPolygon2D(vertex);
            Vector3 normal = poly.normal();
            Vector3 toEye = objEye - poly.vertex(0);
            polyBackfacing[i] = normal.dot(toEye) < 0;
        }
    }
}


void UIGeom::render(RenderDevice* rd, float lineScale) {
    if (! visible) {
        return;
    }

    if (m_twoSidedPolys) {
        rd->setCullFace(RenderDevice::CULL_NONE);
        rd->enableTwoSidedLighting();
    }

    for (int i = 0; i < poly3D.size(); ++i) {
        const ConvexPolygon& poly = poly3D[i];
        rd->beginPrimitive(RenderDevice::TRIANGLE_FAN);
            rd->setNormal(poly.normal());
            for (int v = 0; v < poly.numVertices(); ++v) {
                rd->sendVertex(poly.vertex(v));
            }
        rd->endPrimitive();
    }

    // Per segment normals
    Array<Vector3> normal;
    const Vector3 eye = computeEye(rd);
    for (int i = 0; i < line3D.size(); ++i) {
        const PolyLine& line = line3D[i];

        // Compute one normal for each segement
        normal.resize(line.numSegments(), false);
        for (int s = 0; s < line.numSegments(); ++s) {
            normal[s] = segmentNormal(line.segment(s), eye);
        }

        bool closed = line.closed();

        // Compute line width
        {
            Vector4 v = rd->project(line.vertex(0));
            float L = 2;
            if (v.w > 0) {
                L = min(15.0f * v.w, 10.0f);
            } else {
                 L = 10.0f;
            }
            rd->setLineWidth(L * lineScale);
        }

        bool colorWrite = rd->colorWrite();
        bool depthWrite = rd->depthWrite();

        // Draw lines twice.  The first time we draw color information
        // but do not set the depth buffer.  The second time we write
        // to the depth buffer; this prevents line strips from
        // corrupting their own antialiasing with depth conflicts at
        // the line joint points.
        if (colorWrite) {
            rd->setDepthWrite(false);
            rd->beginPrimitive(RenderDevice::LINE_STRIP);
                for (int v = 0; v < line.numVertices(); ++v) {
                    // Compute the smooth normal.  If we have a non-closed
                    // polyline, the ends are special cases.
                    Vector3 N;
                    if (! closed && (v == 0)) {
                        N = normal[0];
                    } else if (! closed && (v == line.numVertices())) {
                        N = normal.last();
                    } else {
                        // Average two adjacent normals
                        N = normal[v % normal.size()] + normal[(v - 1) % 
                                                               normal.size()];
                    }

                    if (N.squaredLength() < 0.05) {
                        // Too small to normalize; revert to the
                        // nearest segment normal
                        rd->setNormal(normal[iMin(v, normal.size() - 1)]);
                    } else {
                        rd->setNormal(N.direction());
                    }

                    rd->sendVertex(line.vertex(v));
                }                
            rd->endPrimitive();
        }

        if (depthWrite) {
            rd->setColorWrite(false);
            rd->setDepthWrite(true);
            rd->beginPrimitive(RenderDevice::LINE_STRIP);
                for (int v = 0; v < line.numVertices(); ++v) {
                    rd->sendVertex(line.vertex(v));
                }
            rd->endPrimitive();
        }

        rd->setColorWrite(colorWrite);
        rd->setDepthWrite(depthWrite);
    }
}    
} // _internal


ThirdPersonManipulatorRef ThirdPersonManipulator::create() {
    return new ThirdPersonManipulator();
}

void ThirdPersonManipulator::render(RenderDevice* rd) {
    rd->pushState();
    // Highlight the appropriate axis

    // X, Y, Z, XY, YZ, ZX, RX, RY, RZ
    Color3 color[] = 
        {Color3(0.9f, 0, 0), 
         Color3(0, 0.9f, 0.1f), 
         Color3(0.0f, 0.4f, 1.0f), 
         
         Color3(0.6f, 0.7f, 0.7f), 
         Color3(0.6f, 0.7f, 0.7f), 
         Color3(0.6f, 0.7f, 0.7f),
    
         Color3(0.9f, 0, 0), 
         Color3(0, 0.9f, 0.1f), 
         Color3(0.0f, 0.4f, 1.0f)}; 
         
    static const Color3 highlight[] = 
       {Color3(1.0f, 0.5f, 0.5f), 
        Color3(0.6f, 1.0f, 0.7f), 
        Color3(0.5f, 0.7f, 1.0f), 
        
        Color3::white(), 
        Color3::white(), 
        Color3::white(),
    
        Color3(1.0f, 0.5f, 0.5f), 
        Color3(0.6f, 1.0f, 0.7f), 
        Color3(0.5f, 0.7f, 1.0f)};

    static const Color3 usingColor = Color3::yellow();

    // Highlight whatever we're over
    if (m_overAxis != NO_AXIS) {
        color[m_overAxis] = highlight[m_overAxis];
    }

    // Show the selected axes
    for (int a = 0; a < NUM_GEOMS; ++a) {
        if (m_usingAxis[a]) {
            color[a] = usingColor;
        }
    }

    // The Draw::axes command automatically doubles whatever scale we give it
    Draw::axes(m_controlFrame, rd, 
               color[0], color[1], color[2], m_axisScale * 0.5f);

    rd->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, 
                     RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
    rd->setShadeMode(RenderDevice::SHADE_SMOOTH);
    rd->setObjectToWorldMatrix(m_controlFrame);

    if (m_translationEnabled) {
        for (int g = FIRST_TRANSLATION; g <= LAST_TRANSLATION; ++g) {
            rd->setColor(color[g]);
            m_geomArray[g].render(rd);
        }
    }

    if (m_rotationEnabled) {
        for (int g = FIRST_ROTATION; g <= LAST_ROTATION; ++g) {
            rd->setColor(color[g]);
            m_geomArray[g].render(rd, 0.8f);
        }
    }

    computeProjection(rd);
    rd->popState();
}


class TPMPosedModel : public PosedModel {
    friend class ThirdPersonManipulator;

    ThirdPersonManipulator* m_manipulator;

    TPMPosedModel(ThirdPersonManipulator* m) : m_manipulator(m) {}

public:

    virtual void render(RenderDevice* rd) const {
        m_manipulator->render(rd);
    }

    
    virtual std::string name() const {
        return "ThirdPersonManipulator";
    }

    virtual void getCoordinateFrame(CoordinateFrame& c) const {
        m_manipulator->getControlFrame(c);
    }

    virtual const MeshAlg::Geometry& objectSpaceGeometry() const {
        static MeshAlg::Geometry x;
        return x;
    }

    virtual const Array<Vector3>& objectSpaceFaceNormals
    (bool normalize = true) const {
        (void)normalize;
        static Array<Vector3> x;
        return x;
    }

    virtual const Array<MeshAlg::Face>& faces() const {
        static Array<MeshAlg::Face> x;
        return x;
    }

    virtual const Array<MeshAlg::Edge>& edges() const {
        static Array<MeshAlg::Edge> x;
        return x;
    }

    virtual const Array<MeshAlg::Vertex>& vertices() const {
        static Array<MeshAlg::Vertex> x;
        return x;
    }

    virtual const Array<MeshAlg::Face>& weldedFaces() const {
        static Array<MeshAlg::Face> x;
        return x;
    }

    virtual const Array<MeshAlg::Edge>& weldedEdges() const {
        static Array<MeshAlg::Edge> x;
        return x;
    }

    virtual const Array<MeshAlg::Vertex>& weldedVertices() const {
        static Array<MeshAlg::Vertex> x;
        return x;
    }

    virtual const Array<int>& triangleIndices() const {
        static Array<int> x;
        return x;
    }

    virtual void getObjectSpaceBoundingSphere(Sphere& s) const {
        s.radius = 2;
        s.center = Vector3::zero();
    }

    virtual void getObjectSpaceBoundingBox(Box& b) const {
        b = Box(Vector3(-2,-2,-2), Vector3(2,2,2));
    }

    virtual int numBoundaryEdges() const {
        return 0;
    }

    virtual int numWeldedBoundaryEdges() const {
        return 0;
    }
};


void ThirdPersonManipulator::onDragBegin(const Vector2& start) {
    (void)start;
    // Intentionally empty.
}


void ThirdPersonManipulator::onDragEnd(const Vector2& stop) {
    (void)stop;
    // Intentionally empty.
    for (int i = 0; i < NUM_GEOMS; ++i) {
        m_usingAxis[i] = false;
    }
}


Vector3 ThirdPersonManipulator::singleAxisTranslationDrag(int a, const Vector2& delta) {
    // Project the mouse delta onto the drag axis to determine how far to drag
    const LineSegment2D& axis = m_geomArray[a].line2D[0].segment(0);

    Vector2 vec = axis.point(1) - axis.point(0);
    float length2 = max(0.5f, vec.squaredLength());

    // Divide by squared length since we not only normalize but need to take into
    // account the angular foreshortening.
    float distance = vec.dot(delta) / length2;
    return m_controlFrame.rotation.getColumn(a) * distance;
}


Vector3 ThirdPersonManipulator::doubleAxisTranslationDrag(int a0, int a1, const Vector2& delta) {

    Vector2 axis[2];
    int   a[2] = {a0, a1};

    // The two dot products represent points on a non-orthogonal set of axes.
    float dot[2];

    for (int i = 0; i < 2; ++i) {
        // Project the mouse delta onto the drag axis to determine how far to drag
        const LineSegment2D& seg = m_geomArray[a[i]].line2D[0].segment(0);

        axis[i] = seg.point(1) - seg.point(0);
        float length2 = max(0.5f, axis[i].squaredLength());
        axis[i] /= length2;

        dot[i] = axis[i].dot(delta);
    }

    // Distance along both
    float common = axis[0].dot(axis[1]) * dot[0] * dot[1];
    float distance0 = dot[0] - common;
    float distance1 = dot[1] - common;

    return m_controlFrame.rotation.getColumn(a0) * distance0 + 
           m_controlFrame.rotation.getColumn(a1) * distance1;
}


void ThirdPersonManipulator::onDrag(const Vector2& delta) {
    if ((m_dragAxis >= XY) && (m_dragAxis <= ZX)) {

        // Translation, multiple axes
        m_controlFrame.translation += doubleAxisTranslationDrag(m_dragAxis - 3, (m_dragAxis - 2) % 3, delta);

    } else if ((m_dragAxis >= X) && (m_dragAxis <= Z)) {

        // Translation, single axis
        m_controlFrame.translation += singleAxisTranslationDrag(m_dragAxis, delta);

    } else {
        // Rotation
    
        // Drag distance.  We divide by the W coordinate so that
        // rotation distance is independent of the size of the widget
        // on screen.
        float angle = delta.dot(m_dragTangent) * 0.00004f / m_dragW;

        // Axis about which to rotate
        Vector3 axis = Vector3::zero();
        axis[m_dragAxis - RX] = 1.0f;

        Matrix3 R = Matrix3::fromAxisAngle(axis, angle);

        m_controlFrame.rotation = m_controlFrame.rotation * R;

    }
}


void ThirdPersonManipulator::computeProjection(RenderDevice* rd) {
    // TODO: clip to the near plane; modify LineSegment to go through a projection

    for (int g = 0; g < NUM_GEOMS; ++g) {
        m_geomArray[g].computeProjection(rd);
    }
}


bool ThirdPersonManipulator::enabled() const {
    return m_enabled;
}


void ThirdPersonManipulator::setEnabled(bool e) {
    if (! e && m_dragging) {
            m_dragging = false;
    }
    m_enabled = e;
}


ThirdPersonManipulator::ThirdPersonManipulator() : 
    m_axisScale(1), 
    m_dragging(false), 
    m_dragKey(GKey::LEFT_MOUSE),
    m_doubleAxisDrag(false),
    m_overAxis(NO_AXIS),
    m_maxAxisDistance2D(15),
    m_maxRotationDistance2D(10),
    m_rotationEnabled(true),
    m_translationEnabled(true),
    m_enabled(true) {

    for (int i = 0; i < NUM_GEOMS; ++i) {
        m_usingAxis[i] = false;
    }

    // Size of the 2-axis control widget
    const float hi = 0.80f, lo = 0.60f;

    // Single axis (cut a hole out of the center for grabbing all)
    const float cut = 0.20f;

    Array<Vector3> vertex;

    // Create the translation axes.  They will be rendered separately.
    vertex.clear();
    vertex.append(Vector3(cut, 0, 0), Vector3(1.1f, 0, 0));
    m_geomArray[X].line3D.append(_internal::PolyLine(vertex));
    m_geomArray[X].visible = false;

    vertex.clear();
    vertex.append(Vector3(0, cut, 0), Vector3(0, 1.1f, 0));
    m_geomArray[Y].line3D.append(_internal::PolyLine(vertex));
    m_geomArray[Y].visible = false;

    vertex.clear();
    vertex.append(Vector3(0, 0, cut), Vector3(0, 0, 1.1f));
    m_geomArray[Z].line3D.append(_internal::PolyLine(vertex));
    m_geomArray[Z].visible = false;

    // Create the translation squares that lie inbetween the axes
    if (m_doubleAxisDrag) {
        vertex.clear();
        vertex.append(Vector3(lo, hi, 0), Vector3(lo, lo, 0), Vector3(hi, lo, 0), Vector3(hi, hi, 0));
        m_geomArray[XY].poly3D.append(ConvexPolygon(vertex));

        vertex.clear();
        vertex.append(Vector3(0, hi, hi), Vector3(0, lo, hi), Vector3(0, lo, lo), Vector3(0, hi, lo));
        m_geomArray[YZ].poly3D.append(ConvexPolygon(vertex));

        vertex.clear();
        vertex.append(Vector3(lo, 0, lo), Vector3(lo, 0, hi), Vector3(hi, 0, hi), Vector3(hi, 0, lo));
        m_geomArray[ZX].poly3D.append(ConvexPolygon(vertex));
    }

    // Create the rotation circles
    const int ROT_SEGMENTS = 40;
    const float ROT_RADIUS = 0.65f;
    vertex.resize(ROT_SEGMENTS + 1);
    for (int v = 0; v <= ROT_SEGMENTS; ++v) {
        float a = twoPi() * (float)v / ROT_SEGMENTS;
        vertex[v].x = 0;
        vertex[v].y = cos(a) * ROT_RADIUS;
        vertex[v].z = sin(a) * ROT_RADIUS;
    }
    m_geomArray[RX].line3D.append(_internal::PolyLine(vertex));

    for (int v = 0; v <= ROT_SEGMENTS; ++v) {
        vertex[v] = vertex[v].zxy();
    }
    m_geomArray[RY].line3D.append(_internal::PolyLine(vertex));

    for (int v = 0; v <= ROT_SEGMENTS; ++v) {
        vertex[v] = vertex[v].zxy();
    }
    m_geomArray[RZ].line3D.append(_internal::PolyLine(vertex));

    m_posedModel = new TPMPosedModel(this);
}


void ThirdPersonManipulator::setRotationEnabled(bool r) {
    m_rotationEnabled = r;
}


bool ThirdPersonManipulator::rotationEnabled() const {
    return m_rotationEnabled;
}


void ThirdPersonManipulator::setTranslationEnabled(bool r) {
    m_translationEnabled = r;
}


bool ThirdPersonManipulator::translationEnabled() const {
    return m_translationEnabled;
}


CoordinateFrame ThirdPersonManipulator::computeOffsetFrame(
    const CoordinateFrame& controlFrame, 
    const CoordinateFrame& objectFrame) {
    return objectFrame * controlFrame.inverse();
}


CoordinateFrame ThirdPersonManipulator::frame() const {
    CoordinateFrame c;
    getFrame(c);
    return c;
}


void ThirdPersonManipulator::onPose(
    Array<PosedModel::Ref>& posedArray, 
    Array<PosedModel2DRef>& posed2DArray) {

    if (m_enabled) {
        (void)posed2DArray;
        posedArray.append(m_posedModel);
    }
}


void ThirdPersonManipulator::setControlFrame(const CoordinateFrame& c) {
    // Compute the offset for the new control frame, and then
    // change the control frame.
    m_offsetFrame = c.inverse() * frame();
    m_controlFrame = c;
}


void ThirdPersonManipulator::getControlFrame(CoordinateFrame& c) const {
    c = m_controlFrame;
}


void ThirdPersonManipulator::setFrame(const CoordinateFrame& c) {
    m_controlFrame = m_offsetFrame.inverse() * c;
}


/** Returns the object's new frame */
void ThirdPersonManipulator::getFrame(CoordinateFrame& c) const {
    c = m_controlFrame * m_offsetFrame;
}


void ThirdPersonManipulator::onSimulation(RealTime rdt, SimTime sdt, SimTime idt) {
    (void)rdt;
    (void)idt;
    (void)sdt;
}


bool ThirdPersonManipulator::onEvent(const GEvent& event) {
    (void)event;
    return false;
}


void ThirdPersonManipulator::onUserInput(UserInput* ui) {
    if (! m_enabled) {
        return;
    }

    if (m_dragging) {
        if (ui->keyDown(m_dragKey)) {
            // Continue dragging
            onDrag(ui->mouseDXY());
        } else {
            // Stop dragging
            m_dragging = false;
            onDragEnd(ui->mouseXY());
        }
    } else {
        // Highlight the axis closest to the mouse
        m_overAxis = NO_AXIS;
        float nearestDepth = (float)inf();

        if (m_translationEnabled) {
            for (int g = FIRST_TRANSLATION; g <= LAST_TRANSLATION; ++g) {
                const _internal::UIGeom& geom = m_geomArray[g];

                if (geom.contains(ui->mouseXY(), nearestDepth, m_dragTangent, m_dragW, m_maxAxisDistance2D)) {
                    m_overAxis = g;
                }
            }
        }

        if (m_rotationEnabled) {
            for (int g = FIRST_ROTATION; g <= LAST_ROTATION; ++g) {
                const _internal::UIGeom& geom = m_geomArray[g];

                if (geom.contains(ui->mouseXY(), nearestDepth, m_dragTangent, m_dragW, m_maxRotationDistance2D)) {
                    m_overAxis = g;
                }
            }
        }

        // Maybe start a drag
        if ((m_overAxis != NO_AXIS) &&  
            ui->keyPressed(m_dragKey) && 
            ui->keyDown(m_dragKey)) {
            
            // The user clicked on an axis
            m_dragAxis = m_overAxis;
            m_dragging = true;
            m_usingAxis[m_overAxis] = true;

            // Translation along two axes
            if ((m_overAxis >= XY) && (m_overAxis <= ZX)) {
                // Slect the two adjacent axes as well as the
                // box that was clicked on
                m_usingAxis[m_overAxis - 3] = true;
                m_usingAxis[(m_overAxis - 3 + 1) % 3] = true;
            }

            onDragBegin(ui->mouseXY());
        } // if drag
    } // if already dragging
} // userinput


void ThirdPersonManipulator::onNetwork() {
}


void ThirdPersonManipulator::onLogic() {
}

}
