/**
 @file Draw.cpp
  
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2003-10-29
 @edited  2006-02-18
 */

#include "G3D/platform.h"
#include "G3D/Rect2D.h"
#include "G3D/AABox.h"
#include "G3D/Box.h"
#include "G3D/Ray.h"
#include "G3D/Sphere.h"
#include "G3D/Line.h"
#include "G3D/LineSegment.h"
#include "G3D/Capsule.h"
#include "G3D/Cylinder.h"
#include "GLG3D/Draw.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/GLCaps.h"
#include "G3D/MeshAlg.h"

namespace G3D {

const int Draw::WIRE_SPHERE_SECTIONS = 26;
const int Draw::SPHERE_SECTIONS = 40;

const int Draw::SPHERE_PITCH_SECTIONS = 20;
const int Draw::SPHERE_YAW_SECTIONS = 40;

void Draw::poly2DOutline(const Array<Vector2>& polygon, RenderDevice* renderDevice, const Color4& color) {
    if (polygon.length() == 0) {
        return;
    }
    
    renderDevice->beginPrimitive(RenderDevice::LINE_STRIP);
        renderDevice->setColor(color);
        for (int i = 0; i < polygon.length(); ++i) {
            renderDevice->sendVertex(polygon[i]);
        }
        renderDevice->sendVertex(polygon[0]);
    renderDevice->endPrimitive();
}


void Draw::poly2D(const Array<Vector2>& polygon, RenderDevice* renderDevice, const Color4& color) {
    if (polygon.length() == 0) {
        return;
    }
    
    renderDevice->beginPrimitive(RenderDevice::TRIANGLE_FAN);
        renderDevice->setColor(color);
        for (int i = 0; i < polygon.length(); ++i) {
            renderDevice->sendVertex(polygon[i]);
        }
    renderDevice->endPrimitive();
}


void Draw::axes(
    RenderDevice*       renderDevice,
    const Color4&       xColor,
    const Color4&       yColor,
    const Color4&       zColor,
    float               scale) {

    axes(CoordinateFrame(), renderDevice, xColor, yColor, zColor, scale);
}


void Draw::arrow(
    const Vector3&      start,
    const Vector3&      direction,
    RenderDevice*       renderDevice,
    const Color4&       color,
    float               scale) {
    ray(Ray::fromOriginAndDirection(start, direction), renderDevice, color, scale);
}


void Draw::axes(
    const CoordinateFrame& cframe,
    RenderDevice*       renderDevice,
    const Color4&       xColor,
    const Color4&       yColor,
    const Color4&       zColor,
    float               scale) {

    Vector3 c = cframe.translation;
    Vector3 x = cframe.rotation.getColumn(0).direction() * 2 * scale;
    Vector3 y = cframe.rotation.getColumn(1).direction() * 2 * scale;
    Vector3 z = cframe.rotation.getColumn(2).direction() * 2 * scale;

    Draw::arrow(c, x, renderDevice, xColor, scale);
    Draw::arrow(c, y, renderDevice, yColor, scale);
    Draw::arrow(c, z, renderDevice, zColor, scale);
  
    // Text label scale
    const float xx = -3;
    const float yy = xx * 1.4f;

    // Project the 3D locations of the labels
    Vector4 xc2D = renderDevice->project(c + x * 1.1f);
    Vector4 yc2D = renderDevice->project(c + y * 1.1f);
    Vector4 zc2D = renderDevice->project(c + z * 1.1f);

    // If coordinates are behind the viewer, transform off screen
    Vector2 x2D = (xc2D.w > 0) ? xc2D.xy() : Vector2(-2000, -2000);
    Vector2 y2D = (yc2D.w > 0) ? yc2D.xy() : Vector2(-2000, -2000);
    Vector2 z2D = (zc2D.w > 0) ? zc2D.xy() : Vector2(-2000, -2000);

    // Compute the size of the labels
    float xS = (xc2D.w > 0) ? clamp(10 * xc2D.w * scale, .1f, 5) : 0;
    float yS = (yc2D.w > 0) ? clamp(10 * yc2D.w * scale, .1f, 5) : 0;
    float zS = (zc2D.w > 0) ? clamp(10 * zc2D.w * scale, .1f, 5) : 0;

    renderDevice->push2D();
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        renderDevice->setLineWidth(2);

        renderDevice->beginPrimitive(RenderDevice::LINES);
            // X
            renderDevice->setColor(xColor * 0.8f);
            renderDevice->sendVertex(Vector2(-xx,  yy) * xS + x2D);
            renderDevice->sendVertex(Vector2( xx, -yy) * xS + x2D);
            renderDevice->sendVertex(Vector2( xx,  yy) * xS + x2D);
            renderDevice->sendVertex(Vector2(-xx, -yy) * xS + x2D);

            // Y
            renderDevice->setColor(yColor * 0.8f);
            renderDevice->sendVertex(Vector2(-xx,  yy) * yS + y2D);
            renderDevice->sendVertex(Vector2(  0,  0)  * yS + y2D);
            renderDevice->sendVertex(Vector2(  0,  0)  * yS + y2D);
            renderDevice->sendVertex(Vector2(  0, -yy) * yS + y2D);
            renderDevice->sendVertex(Vector2( xx,  yy) * yS + y2D);
            renderDevice->sendVertex(Vector2(  0,  0)  * yS + y2D);
        renderDevice->endPrimitive();

        renderDevice->beginPrimitive(RenderDevice::LINE_STRIP);
            // Z
            renderDevice->setColor(zColor * 0.8f);    
            renderDevice->sendVertex(Vector2( xx,  yy) * zS + z2D);
            renderDevice->sendVertex(Vector2(-xx,  yy) * zS + z2D);
            renderDevice->sendVertex(Vector2( xx, -yy) * zS + z2D);
            renderDevice->sendVertex(Vector2(-xx, -yy) * zS + z2D);
        renderDevice->endPrimitive();
    renderDevice->pop2D();
}


void Draw::ray(
    const Ray&          ray,
    RenderDevice*       renderDevice,
    const Color4&       color,
    float               scale) {

    Vector3 tip = ray.origin + ray.direction;
    // Create a coordinate frame at the tip
    Vector3 u = ray.direction.direction();
    Vector3 v;
    if (abs(u.x) < abs(u.y)) {
        v = Vector3::unitX();
    } else {
        v = Vector3::unitY();
    }
    Vector3 w = u.cross(v).direction();
    v = w.cross(u).direction();
    Vector3 back = tip - u * 0.3f * scale;

    RenderDevice::ShadeMode oldShadeMode = renderDevice->shadeMode();
    Color4 oldColor = renderDevice->color();

    renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);
    renderDevice->setColor(color);

    float r = scale * 0.1f;
    // Arrow head.  Need this beginprimitive call to sync up G3D and OpenGL
    renderDevice->beginPrimitive(RenderDevice::TRIANGLES);
        for (int a = 0; a < SPHERE_SECTIONS; ++a) {
            float angle0 = a * (float)twoPi() / SPHERE_SECTIONS;
            float angle1 = (a + 1) * (float)twoPi() / SPHERE_SECTIONS;
            Vector3 dir0(cos(angle0) * v + sin(angle0) * w);
            Vector3 dir1(cos(angle1) * v + sin(angle1) * w);
            glNormal3fv(dir0);
            glVertex3fv(tip);

            glVertex3fv(back + dir0 * r);

            glNormal3fv(dir1);
            glVertex3fv(back + dir1 * r);                
        }
    renderDevice->endPrimitive();
    renderDevice->minGLStateChange(SPHERE_SECTIONS * 5);

    // Back of arrow head
    renderDevice->beforePrimitive();
    glBegin(GL_TRIANGLE_FAN);
        glNormal3fv(-u);
        for (int a = 0; a < SPHERE_SECTIONS; ++a) {
            float angle = a * (float)twoPi() / SPHERE_SECTIONS;
            Vector3 dir = sin(angle) * v + cos(angle) * w;
            glVertex3fv(back + dir * r);
        }
    glEnd();
    renderDevice->afterPrimitive();
    renderDevice->minGLStateChange(SPHERE_SECTIONS);

    renderDevice->setColor(oldColor);
    renderDevice->setShadeMode(oldShadeMode);
    lineSegment(LineSegment::fromTwoPoints(ray.origin, back), renderDevice, color, scale);
}


void Draw::plane(
    const Plane&         plane, 
    RenderDevice*        renderDevice,
    const Color4&        solidColor,
    const Color4&        wireColor) {

    renderDevice->pushState();
    CoordinateFrame cframe0 = renderDevice->getObjectToWorldMatrix();

    Vector3 N, P;
    
    {
        double d;
        plane.getEquation(N, d);
        P = N * (float)d;
    }

    CoordinateFrame cframe1(P);
    cframe1.lookAt(P + N);

    renderDevice->setObjectToWorldMatrix(cframe0 * cframe1);

    renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);
    renderDevice->enableTwoSidedLighting();

    if (solidColor.a > 0.0f) {
        // Draw concentric circles around the origin.  We break them up to get good depth
        // interpolation and reasonable shading.

        renderDevice->setPolygonOffset(0.7);

        if (solidColor.a < 1.0f) {
            renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        }

        renderDevice->setNormal(Vector3::unitZ());
        renderDevice->setColor(solidColor);

        renderDevice->setCullFace(RenderDevice::CULL_NONE);
        // Infinite strip
        const int N = 12;
        float r1 = 100;
        renderDevice->beginPrimitive(RenderDevice::QUAD_STRIP);
            for (int i = 0; i <= N; ++i) {
                float a = i * (float)twoPi() / N;
                float c = cosf(a);
                float s = sinf(a);

                renderDevice->sendVertex(Vector3(c * r1, s * r1, 0));
                renderDevice->sendVertex(Vector4(c, s, 0, 0));
            }
        renderDevice->endPrimitive();

        // Finite strips.  
        float r2 = 0;
        const int M = 4;
        for (int j = 0; j < M; ++j) {
            r2 = r1;
            r1 = r1 / 3;
            if (j == M - 1) {
                // last pass
                r1 = 0;
            }

            renderDevice->beginPrimitive(RenderDevice::QUAD_STRIP);
                for (int i = 0; i <= N; ++i) {
                    float a = i * (float)twoPi() / N;
                    float c = cosf(a);
                    float s = sinf(a);

                    renderDevice->sendVertex(Vector3(c * r1, s * r1, 0));
                    renderDevice->sendVertex(Vector3(c * r2, s * r2, 0));
                }
            renderDevice->endPrimitive();
        }


    }

    if (wireColor.a > 0) {
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

        renderDevice->setLineWidth(1.5);

        renderDevice->beginPrimitive(RenderDevice::LINES);
            renderDevice->setColor(wireColor);
            renderDevice->setNormal(Vector3::unitZ());

            // Lines to infinity
            renderDevice->sendVertex(Vector4( 1, 0, 0, 0));
            renderDevice->sendVertex(Vector4( 0, 0, 0, 1));

            renderDevice->sendVertex(Vector4(-1, 0, 0, 0));
            renderDevice->sendVertex(Vector4( 0, 0, 0, 1));

            renderDevice->sendVertex(Vector4(0, -1, 0, 0));
            renderDevice->sendVertex(Vector4(0,  0, 0, 1));

            renderDevice->sendVertex(Vector4(0,  1, 0, 0));
            renderDevice->sendVertex(Vector4(0,  0, 0, 1));
        renderDevice->endPrimitive();

        renderDevice->setLineWidth(0.5);

        renderDevice->beginPrimitive(RenderDevice::LINES);

            // Horizontal and vertical lines
            const int N = 10;
            const float space = 1;
            const float Ns = N * space;
            for (int x = -N; x <= N; ++x) {
                float sx = x * space;
                renderDevice->sendVertex(Vector3( Ns, sx, 0));
                renderDevice->sendVertex(Vector3(-Ns, sx, 0));

                renderDevice->sendVertex(Vector3(sx,  Ns, 0));
                renderDevice->sendVertex(Vector3(sx, -Ns, 0));
            }

        renderDevice->endPrimitive();
    }

    renderDevice->popState();
}
 


void Draw::capsule(
    const Capsule&       capsule, 
    RenderDevice*        renderDevice,
    const Color4&        solidColor,
    const Color4&        wireColor) {

    CoordinateFrame cframe(capsule.point(0));

    Vector3 Y = (capsule.point(1) - capsule.point(1)).direction();
    Vector3 X = (abs(Y.dot(Vector3::unitX())) > 0.9) ? Vector3::unitY() : Vector3::unitX();
    Vector3 Z = X.cross(Y).direction();
    X = Y.cross(Z);        
    cframe.rotation.setColumn(0, X);
    cframe.rotation.setColumn(1, Y);
    cframe.rotation.setColumn(2, Z);

    float radius = capsule.radius();
    float height = (capsule.point(1) - capsule.point(0)).magnitude();

    // Always render upright in object space
    Sphere sphere1(Vector3::zero(), radius);
    Sphere sphere2(Vector3(0, height, 0), radius);

    Vector3 top(0, height, 0);

    renderDevice->pushState();

        renderDevice->setObjectToWorldMatrix(renderDevice->getObjectToWorldMatrix() * cframe);
        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);

        if (solidColor.a > 0) {
            int numPasses = 1;

            if (solidColor.a < 1) {
                // Multiple rendering passes to get front/back blending correct
                renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
                numPasses = 2;
                renderDevice->setCullFace(RenderDevice::CULL_FRONT);
                renderDevice->setDepthWrite(false);
            }

            renderDevice->setColor(solidColor);
            for (int k = 0; k < numPasses; ++k) {
                sphereSection(sphere1, renderDevice, solidColor, false, true);
                sphereSection(sphere2, renderDevice, solidColor, true, false);

                // Cylinder faces
                renderDevice->beginPrimitive(RenderDevice::QUAD_STRIP);
                    for (int y = 0; y <= SPHERE_SECTIONS; ++y) {
                        const float yaw0 = y * (float)twoPi() / SPHERE_SECTIONS;
                        Vector3 v0 = Vector3(cosf(yaw0), 0, sinf(yaw0));

                        renderDevice->setNormal(v0);
                        renderDevice->sendVertex(v0 * radius);
                        renderDevice->sendVertex(v0 * radius + top);
                    }
                renderDevice->endPrimitive();

                renderDevice->setCullFace(RenderDevice::CULL_BACK);
            }

        }

        if (wireColor.a > 0) {
            renderDevice->setDepthWrite(true);
            renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

            wireSphereSection(sphere1, renderDevice, wireColor, false, true);
            wireSphereSection(sphere2, renderDevice, wireColor, true, false);

            // Line around center
            renderDevice->setColor(wireColor);
            Vector3 center(0, height / 2, 0);
            renderDevice->setLineWidth(2);
            renderDevice->beginPrimitive(RenderDevice::LINES);
                for (int y = 0; y < WIRE_SPHERE_SECTIONS; ++y) {
                    const float yaw0 = y * (float)twoPi() / WIRE_SPHERE_SECTIONS;
                    const float yaw1 = (y + 1) * (float)twoPi() / WIRE_SPHERE_SECTIONS;

                    Vector3 v0(cosf(yaw0), 0, sinf(yaw0));
                    Vector3 v1(cosf(yaw1), 0, sinf(yaw1));

                    renderDevice->setNormal(v0);
                    renderDevice->sendVertex(v0 * radius + center);
                    renderDevice->setNormal(v1);
                    renderDevice->sendVertex(v1 * radius + center);
                }

                // Edge lines
                for (int y = 0; y < 8; ++y) {
                    const float yaw = y * (float)pi() / 4;
                    const Vector3 x(cosf(yaw), 0, sinf(yaw));
        
                    renderDevice->setNormal(x);
                    renderDevice->sendVertex(x * radius);
                    renderDevice->sendVertex(x * radius + top);
                }
            renderDevice->endPrimitive();
        }

    renderDevice->popState();
}


void Draw::cylinder(
    const Cylinder&      cylinder, 
    RenderDevice*        renderDevice,
    const Color4&        solidColor,
    const Color4&        wireColor) {

    CoordinateFrame cframe;
    
    cylinder.getReferenceFrame(cframe);

    float radius = cylinder.radius();
    float height = cylinder.height();

    // Always render upright in object space
    Vector3 bot(0, -height / 2, 0);
    Vector3 top(0, height / 2, 0);

    renderDevice->pushState();

        renderDevice->setObjectToWorldMatrix(renderDevice->getObjectToWorldMatrix() * cframe);
        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);

        if (solidColor.a > 0) {
            int numPasses = 1;

            if (solidColor.a < 1) {
                // Multiple rendering passes to get front/back blending correct
                renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
                numPasses = 2;
                renderDevice->setCullFace(RenderDevice::CULL_FRONT);
                renderDevice->setDepthWrite(false);
            }

            renderDevice->setColor(solidColor);
            for (int k = 0; k < numPasses; ++k) {

                // Top
                renderDevice->beginPrimitive(RenderDevice::TRIANGLE_FAN);
                    renderDevice->setNormal(Vector3::unitY());
                    renderDevice->sendVertex(top);
                    for (int y = 0; y <= SPHERE_SECTIONS; ++y) {
                        const float yaw0 = -y * (float)twoPi() / SPHERE_SECTIONS;
                        Vector3 v0 = Vector3(cosf(yaw0), 0, sinf(yaw0));

                        renderDevice->sendVertex(v0 * radius + top);
                    }
                renderDevice->endPrimitive();

                // Bottom
                renderDevice->beginPrimitive(RenderDevice::TRIANGLE_FAN);
                    renderDevice->setNormal(-Vector3::unitY());
                    renderDevice->sendVertex(bot);
                    for (int y = 0; y <= SPHERE_SECTIONS; ++y) {
                        const float yaw0 = y * (float)twoPi() / SPHERE_SECTIONS;
                        Vector3 v0 = Vector3(cosf(yaw0), 0, sinf(yaw0));

                        renderDevice->sendVertex(v0 * radius + bot);
                    }
                renderDevice->endPrimitive();

                // Cylinder faces
                renderDevice->beginPrimitive(RenderDevice::QUAD_STRIP);
                    for (int y = 0; y <= SPHERE_SECTIONS; ++y) {
                        const float yaw0 = y * (float)twoPi() / SPHERE_SECTIONS;
                        Vector3 v0 = Vector3(cosf(yaw0), 0, sinf(yaw0));

                        renderDevice->setNormal(v0);
                        renderDevice->sendVertex(v0 * radius + bot);
                        renderDevice->sendVertex(v0 * radius + top);
                    }
                renderDevice->endPrimitive();

                renderDevice->setCullFace(RenderDevice::CULL_BACK);
            }

        }

        if (wireColor.a > 0) {
            renderDevice->setDepthWrite(true);
            renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

            // Line around center
            renderDevice->setColor(wireColor);
            renderDevice->setLineWidth(2);
            renderDevice->beginPrimitive(RenderDevice::LINES);
                for (int z = 0; z < 3; ++z) {
                    Vector3 center(0, 0.0, 0);
                    for (int y = 0; y < WIRE_SPHERE_SECTIONS; ++y) {
                        const float yaw0 = y * (float)twoPi() / WIRE_SPHERE_SECTIONS;
                        const float yaw1 = (y + 1) * (float)twoPi() / WIRE_SPHERE_SECTIONS;

                        Vector3 v0(cos(yaw0), 0, sin(yaw0));
                        Vector3 v1(cos(yaw1), 0, sin(yaw1));

                        renderDevice->setNormal(v0);
                        renderDevice->sendVertex(v0 * radius + center);
                        renderDevice->setNormal(v1);
                        renderDevice->sendVertex(v1 * radius + center);
                    }
                }

                // Edge lines
                for (int y = 0; y < 8; ++y) {
                    const float yaw = y * (float)pi() / 4;
                    const Vector3 x(cos(yaw), 0, sin(yaw));
                    const Vector3 xr = x * radius;
    
                    // Side
                    renderDevice->setNormal(x);
                    renderDevice->sendVertex(xr + bot);
                    renderDevice->sendVertex(xr + top);

                    // Top
                    renderDevice->setNormal(Vector3::unitY());
                    renderDevice->sendVertex(top);
                    renderDevice->sendVertex(xr + top);

                    // Bottom
                    renderDevice->setNormal(Vector3::unitY());
                    renderDevice->sendVertex(bot);
                    renderDevice->sendVertex(xr + bot);
                }
            renderDevice->endPrimitive();
        }

    renderDevice->popState();
}

    
void Draw::vertexNormals(
    const MeshAlg::Geometry&    geometry,
    RenderDevice*               renderDevice,
    const Color4&               color,
    float                       scale) {

    renderDevice->pushState();
        renderDevice->setColor(color);
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

        const Array<Vector3>& vertexArray = geometry.vertexArray;
        const Array<Vector3>& normalArray = geometry.normalArray;

        const float D = clamp(5.0f / ::powf((float)vertexArray.size(), 0.25f), 0.1f, .8f) * scale;
        
        renderDevice->setLineWidth(1);
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (int v = 0; v < vertexArray.size(); ++v) {
                renderDevice->sendVertex(vertexArray[v] + normalArray[v] * D);
                renderDevice->sendVertex(vertexArray[v]);
            }
        renderDevice->endPrimitive();
        
        renderDevice->setLineWidth(2);
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (int v = 0; v < vertexArray.size(); ++v) {
                renderDevice->sendVertex(vertexArray[v] + normalArray[v] * D * .96f);
                renderDevice->sendVertex(vertexArray[v] + normalArray[v] * D * .84f);
            }
        renderDevice->endPrimitive();

        renderDevice->setLineWidth(3);
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (int v = 0; v < vertexArray.size(); ++v) {
                renderDevice->sendVertex(vertexArray[v] + normalArray[v] * D * .92f);
                renderDevice->sendVertex(vertexArray[v] + normalArray[v] * D * .84f);
            }
        renderDevice->endPrimitive();
    renderDevice->popState();
}


void Draw::vertexVectors(
    const Array<Vector3>&       vertexArray,
    const Array<Vector3>&       directionArray,
    RenderDevice*               renderDevice,
    const Color4&               color,
    float                       scale) {

    renderDevice->pushState();
        renderDevice->setColor(color);
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

        const float D = clamp(5.0f / ::powf((float)vertexArray.size(), 0.25f), 0.1f, 0.8f) * scale;
        
        renderDevice->setLineWidth(1);
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (int v = 0; v < vertexArray.size(); ++v) {
                renderDevice->sendVertex(vertexArray[v] + directionArray[v] * D);
                renderDevice->sendVertex(vertexArray[v]);
            }
        renderDevice->endPrimitive();
        
        renderDevice->setLineWidth(2);
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (int v = 0; v < vertexArray.size(); ++v) {
                renderDevice->sendVertex(vertexArray[v] + directionArray[v] * D * .96f);
                renderDevice->sendVertex(vertexArray[v] + directionArray[v] * D * .84f);
            }
        renderDevice->endPrimitive();

        renderDevice->setLineWidth(3);
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (int v = 0; v < vertexArray.size(); ++v) {
                renderDevice->sendVertex(vertexArray[v] + directionArray[v] * D * .92f);
                renderDevice->sendVertex(vertexArray[v] + directionArray[v] * D * .84f);
            }
        renderDevice->endPrimitive();
    renderDevice->popState();
}


void Draw::line(
    const Line&         line,
    RenderDevice*       renderDevice,
    const Color4&       color) {

    renderDevice->pushState();
        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);
        renderDevice->setColor(color);
        renderDevice->setLineWidth(2);
        renderDevice->setDepthTest(RenderDevice::DEPTH_LEQUAL);
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

        Vector3 v0 = line.point();
        Vector3 d  = line.direction();
        renderDevice->beginPrimitive(RenderDevice::LINE_STRIP);
            // Off to infinity
            renderDevice->sendVertex(Vector4(-d, 0));

            for (int i = -10; i <= 10; i += 2) {
                renderDevice->sendVertex(v0 + d * (float)i * 100.0f);
            }

            // Off to infinity
            renderDevice->sendVertex(Vector4(d, 0));
        renderDevice->endPrimitive();
    renderDevice->popState();
}


void Draw::lineSegment(
    const LineSegment&  lineSegment,
    RenderDevice*       renderDevice,
    const Color4&       color,
    float               scale) {

    renderDevice->pushState();

        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);
        renderDevice->setColor(color);
        
        // Compute perspective line width
        Vector3 v0 = lineSegment.point(0);
        Vector3 v1 = lineSegment.point(1);

        Vector4 s0 = renderDevice->project(v0);
        Vector4 s1 = renderDevice->project(v1);

        float L = 2 * scale;
        if ((s0.w > 0) && (s1.w > 0)) {
            L = 15.0f * (s0.w + s1.w) / 2.0f;
        } else if (s0.w > 0) {
            L = max(15 * s0.w, 10.0f);
        } else if (s1.w > 0) {
            L = max(15.0f * s1.w, 10.0f);
        }

        renderDevice->setLineWidth(L);

        // Find the object space vector perpendicular to the line
        // that points closest to the eye.
        Vector3 eye = renderDevice->getObjectToWorldMatrix().pointToObjectSpace(renderDevice->getCameraToWorldMatrix().translation);
        Vector3 E = eye - v0;
        Vector3 V = v1 - v0;
        Vector3 U = E.cross(V);
        Vector3 N = V.cross(U).direction();

        renderDevice->setDepthTest(RenderDevice::DEPTH_LEQUAL);
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

        renderDevice->beginPrimitive(RenderDevice::LINES);        
            renderDevice->setNormal(N);
            renderDevice->sendVertex(v0);
            renderDevice->sendVertex(v1);
        renderDevice->endPrimitive();
    renderDevice->popState();
}


void Draw::box(
    const AABox&        _box,
    RenderDevice*       renderDevice,
    const Color4&       solidColor,
    const Color4&       wireColor) {

    box(Box(_box), renderDevice, solidColor, wireColor);
}


void Draw::box(
    const Box&          box,
    RenderDevice*       renderDevice,
    const Color4&       solidColor,
    const Color4&       wireColor) {

    renderDevice->pushState();
        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);

        if (solidColor.a > 0) {
            int numPasses = 1;

            if (solidColor.a < 1) {
                // Multiple rendering passes to get front/back blending correct
                renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
                numPasses = 2;
                renderDevice->setCullFace(RenderDevice::CULL_FRONT);
                renderDevice->setDepthWrite(false);
            } else {
                renderDevice->setCullFace(RenderDevice::CULL_BACK);
            }

            renderDevice->setColor(solidColor);
            for (int k = 0; k < numPasses; ++k) {
                renderDevice->beginPrimitive(RenderDevice::QUADS);
                    for (int i = 0; i < 6; ++i) {
                        Vector3 v0, v1, v2, v3;
                        box.getFaceCorners(i, v0, v1, v2, v3);

                        Vector3 n = (v1 - v0).cross(v3 - v0);
                        renderDevice->setNormal(n.direction());
                        renderDevice->sendVertex(v0);
                        renderDevice->sendVertex(v1);
                        renderDevice->sendVertex(v2);
                        renderDevice->sendVertex(v3);
                    }
                renderDevice->endPrimitive();
                renderDevice->setCullFace(RenderDevice::CULL_BACK);
            }
        }

        if (wireColor.a > 0) {
            renderDevice->setDepthWrite(true);
            renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
            renderDevice->setColor(wireColor);
            renderDevice->setLineWidth(2);

            Vector3 c = box.center();
            Vector3 v;

            // Wire frame
            renderDevice->setDepthTest(RenderDevice::DEPTH_LEQUAL);
            renderDevice->beginPrimitive(RenderDevice::LINES);

                // Front and back
                for (int i = 0; i < 8; i += 4) {
                    for (int j = 0; j < 4; ++j) {
                        v = box.corner(i + j);
                        renderDevice->setNormal((v - c).direction());
                        renderDevice->sendVertex(v);

                        v = box.corner(i + ((j + 1) % 4));
                        renderDevice->setNormal((v - c).direction());
                        renderDevice->sendVertex(v);
                    }
                }

                // Sides
                for (int i = 0; i < 4; ++i) {
                    v = box.corner(i);
                    renderDevice->setNormal((v - c).direction());
                    renderDevice->sendVertex(v);

                    v = box.corner(i + 4);
                    renderDevice->setNormal((v - c).direction());
                    renderDevice->sendVertex(v);
                }


            renderDevice->endPrimitive();
        }
    renderDevice->popState();
}


void Draw::wireSphereSection(
    const Sphere&       sphere,
    RenderDevice*       renderDevice,
    const Color4&       color,
    bool                top,
    bool                bottom) {
    
    int sections = WIRE_SPHERE_SECTIONS;
    int start = top ? 0 : (sections / 2);
    int stop = bottom ? sections : (sections / 2);

    renderDevice->pushState();
        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);
        renderDevice->setColor(color);
        renderDevice->setLineWidth(2);
        renderDevice->setDepthTest(RenderDevice::DEPTH_LEQUAL);
        renderDevice->setCullFace(RenderDevice::CULL_BACK);
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

        float radius = sphere.radius;
        const Vector3& center = sphere.center;

        // Wire frame
        for (int y = 0; y < 8; ++y) {
            const float yaw = y * (float)pi() / 4;
            const Vector3 x(cos(yaw) * radius, 0, sin(yaw) * radius);
            //const Vector3 z(-sin(yaw) * radius, 0, cos(yaw) * radius);

            renderDevice->beginPrimitive(RenderDevice::LINE_STRIP);
                for (int p = start; p <= stop; ++p) {
                    const float pitch0 = p * (float)pi() / (sections * 0.5f);

                    Vector3 v0 = cosf(pitch0) * x + Vector3::unitY() * radius * sinf(pitch0);
                    renderDevice->setNormal(v0.direction());
                    renderDevice->sendVertex(v0 + center);
                }
            renderDevice->endPrimitive();
        }


        int a = bottom ? -1 : 0;
        int b = top ? 1 : 0; 
        for (int p = a; p <= b; ++p) {
            const float pitch = p * (float)pi() / 6;

            renderDevice->beginPrimitive(RenderDevice::LINE_STRIP);
                for (int y = 0; y <= sections; ++y) {
                    const float yaw0 = y * (float)pi() / 13;
                    Vector3 v0 = Vector3(cos(yaw0) * cos(pitch), sin(pitch), sin(yaw0) * cos(pitch)) * radius;
                    renderDevice->setNormal(v0.direction());
                    renderDevice->sendVertex(v0 + center);
                }
            renderDevice->endPrimitive();
        }

    renderDevice->popState();
}


void Draw::sphereSection(
    const Sphere&       sphere,
    RenderDevice*       renderDevice,
    const Color4&       color,
    bool                top,
    bool                bottom) {

    // The first time this is invoked we create a series of quad strips in a vertex array.
    // Future calls then render from the array. 

    CoordinateFrame cframe = renderDevice->getObjectToWorldMatrix();

    // Auto-normalization will take care of normal length
    cframe.translation += cframe.rotation * sphere.center;
    cframe.rotation = cframe.rotation * sphere.radius;


    // Track enable bits individually instead of using slow glPush
    bool resetNormalize = false;
    bool usedRescaleNormal = false;
    if (GLCaps::supports("GL_EXT_rescale_normal")) {
        // Switch to using GL_RESCALE_NORMAL, which should be slightly faster.
        resetNormalize = (glIsEnabled(GL_NORMALIZE) == GL_TRUE);
        usedRescaleNormal = true;
        glDisable(GL_NORMALIZE);
        glEnable(GL_RESCALE_NORMAL);
    }

    renderDevice->pushState();
        renderDevice->setObjectToWorldMatrix(cframe);

        renderDevice->setColor(color);
        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);

        static bool initialized = false;
        static VAR vbuffer;
        static Array< uint16 > stripIndexArray;

        if (! initialized) {
            // The normals are the same as the vertices for a sphere
            Array<Vector3> vertex;

            stripIndexArray.resize(iFloor((pi() / (float)SPHERE_PITCH_SECTIONS) + 2 * 
                                   (twoPi() / SPHERE_YAW_SECTIONS)));

            int i = 0;

            for (int p = 0; p < SPHERE_PITCH_SECTIONS; ++p) {
                const float pitch0 = p * (float)pi() / SPHERE_PITCH_SECTIONS;
                const float pitch1 = (p + 1) * (float)pi() / SPHERE_PITCH_SECTIONS;

                const float sp0 = sin(pitch0);
                const float sp1 = sin(pitch1);
                const float cp0 = cos(pitch0);
                const float cp1 = cos(pitch1);

                for (int y = 0; y <= SPHERE_YAW_SECTIONS; ++y) {
                    const float yaw = -y * (float)twoPi() / SPHERE_YAW_SECTIONS;

                    const float cy = cos(yaw);
                    const float sy = sin(yaw);

                    const Vector3 v0(cy * sp0, cp0, sy * sp0);
                    const Vector3 v1(cy * sp1, cp1, sy * sp1);

                    vertex.append(v0, v1);
                    stripIndexArray.append(i, i + 1);
                    i += 2;
                }

                const Vector3& degen = Vector3(1.0f * sp1, cp1, 0.0f * sp1);

                vertex.append(degen, degen);
                stripIndexArray.append(i, i + 1);
                i += 2;    
            }

            VARAreaRef area = VARArea::create(vertex.size() * sizeof(Vector3), VARArea::WRITE_ONCE);
            vbuffer = VAR(vertex, area);
            initialized = true;
        }

        renderDevice->beginIndexedPrimitives();
            renderDevice->setNormalArray(vbuffer);
            renderDevice->setVertexArray(vbuffer);
            if (top) {
                renderDevice->sendIndices(RenderDevice::QUAD_STRIP, (stripIndexArray.length() / 2),
                    stripIndexArray.getCArray());
            }
            if (bottom) {
                renderDevice->sendIndices(RenderDevice::QUAD_STRIP, (stripIndexArray.length() / 2), 
                    stripIndexArray.getCArray() + (stripIndexArray.length() / 2));
            }
        renderDevice->endIndexedPrimitives();

    renderDevice->popState();

    if (usedRescaleNormal) {
        glDisable(GL_RESCALE_NORMAL);
        if (resetNormalize) {
            glEnable(GL_NORMALIZE);
        }
    }
}


void Draw::sphere(
    const Sphere&       sphere,
    RenderDevice*       renderDevice,
    const Color4&       solidColor,
    const Color4&       wireColor) {

    if (solidColor.a > 0) {
        renderDevice->pushState();

            int numPasses = 1;

            if (solidColor.a < 1) {
                numPasses = 2;
                renderDevice->setCullFace(RenderDevice::CULL_FRONT);
                renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
                renderDevice->setDepthWrite(false);
            } else {
                renderDevice->setCullFace(RenderDevice::CULL_BACK);
            }

            if (wireColor.a > 0) {
                renderDevice->setPolygonOffset(3);
            }

            for (int k = 0; k < numPasses; ++k) {
                sphereSection(sphere, renderDevice, solidColor, true, true);
                renderDevice->setCullFace(RenderDevice::CULL_BACK);
            }
        renderDevice->popState();
    }

    if (wireColor.a > 0) {
        wireSphereSection(sphere, renderDevice, wireColor, true, true);
    }
}


void Draw::fullScreenImage(const GImage& im, RenderDevice* renderDevice) {
    debugAssert( im.channels == 3 || im.channels == 4 );
    renderDevice->push2D();
        glPixelZoom((float)renderDevice->width() / (float)im.width, 
                   -(float)renderDevice->height() / (float)im.height);
        glRasterPos4d(0.0, 0.0, 0.0, 1.0);

        glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glDrawPixels(im.width, im.height, (im.channels == 3) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid*)im.byte());

        glPopClientAttrib();
    renderDevice->pop2D();
}


void Draw::rect2D(
    const Rect2D& rect,
    RenderDevice* rd,
    const Color4& color,
    const Vector2& texCoord0,
    const Vector2& texCoord1,
    const Vector2& texCoord2,
    const Vector2& texCoord3) {

    Draw::rect2D(rect, rd, color,
        Rect2D::xywh(0,0,texCoord0.x, texCoord0.y),
        Rect2D::xywh(0,0,texCoord1.x, texCoord1.y),
        Rect2D::xywh(0,0,texCoord2.x, texCoord2.y),
        Rect2D::xywh(0,0,texCoord3.x, texCoord3.y));
}


void Draw::rect2D(
    const Rect2D& rect,
    RenderDevice* rd,
    const Color4& color,
    const Rect2D& texCoord0,
    const Rect2D& texCoord1,
    const Rect2D& texCoord2,
    const Rect2D& texCoord3) {

    const Rect2D* tx[4];
    tx[0] = &texCoord0;
    tx[1] = &texCoord1;
    tx[2] = &texCoord2;
    tx[3] = &texCoord3;

    int N = iMin(4, GLCaps::numTextureCoords());

    rd->pushState();
    rd->setColor(color);
    rd->beginPrimitive(RenderDevice::QUADS);
        for (int i = 0; i < 4; ++i) {
            for (int t = 0; t < N; ++t) {
                rd->setTexCoord(t, tx[t]->corner(i));
            }
            rd->sendVertex(rect.corner(i));
        }
    rd->endPrimitive();
    rd->popState();
}


void Draw::fastRect2D(
    const Rect2D&       rect,
    RenderDevice*       rd,
    const Color4&       color) {

    rd->setColor(color);
    // Use begin primitive in case there are any 
    // lazy state changes pending.
    rd->beginPrimitive(RenderDevice::QUADS);
        
        glTexCoord2f(0, 0);
        glVertex2f(rect.x0(), rect.y0());

        glTexCoord2f(0, 1);
        glVertex2f(rect.x0(), rect.y1());

        glTexCoord2f(1, 1);
        glVertex2f(rect.x1(), rect.y1());

        glTexCoord2f(1, 0);
        glVertex2f(rect.x1(), rect.y0());

    rd->endPrimitive();

    // Record the cost of the raw OpenGL calls
    rd->minGLStateChange(8);
}



void Draw::rect2DBorder(
    const class Rect2D& rect,
    RenderDevice* rd,
    const Color4& color,
    float outerBorder,
    float innerBorder) {


    //
    //
    //   **************************************
    //   **                                  **
    //   * **                              ** *
    //   *   ******************************   *
    //   *   *                            *   *
    //
    //
    const Rect2D outer = rect.border(outerBorder);
    const Rect2D inner = rect.border(-innerBorder); 

    rd->pushState();
    rd->setColor(color);
    rd->beginPrimitive(RenderDevice::QUAD_STRIP);

    for (int i = 0; i < 5; ++i) {
        int j = i % 4;
        rd->sendVertex(outer.corner(j));
        rd->sendVertex(inner.corner(j));
    }

    rd->endPrimitive();
    rd->popState();
}


void Draw::frustum(
    const class GCamera::Frustum& frustum,
    RenderDevice* rd,
    const Color4& color,
    const Color4& wire) {

    // Draw edges
    rd->pushState();

    if (wire.a > 0) {
        rd->setColor(wire);
        for (int f = 0; f < frustum.faceArray.size(); ++f) {
            rd->beginPrimitive(RenderDevice::LINE_STRIP);
            for (int v = 0; v < 5; ++v) {
                rd->sendVertex(frustum.vertexPos[frustum.faceArray[f].vertexIndex[v % 4]]);
            }
            rd->endPrimitive();
        }
    }

    rd->setDepthWrite(false);
        rd->setCullFace(RenderDevice::CULL_NONE);
        rd->enableTwoSidedLighting();
        rd->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        rd->setColor(color);
        rd->beginPrimitive(RenderDevice::QUADS);
        for (int f = 0; f < frustum.faceArray.size(); ++f) {
            rd->setNormal(frustum.faceArray[f].plane.normal());
            for (int v = 0; v < 4; ++v) {
                rd->sendVertex(frustum.vertexPos[frustum.faceArray[f].vertexIndex[v]]);
            }
        }
        rd->endPrimitive();

    rd->popState();
    // TODO:face colors
}

}

