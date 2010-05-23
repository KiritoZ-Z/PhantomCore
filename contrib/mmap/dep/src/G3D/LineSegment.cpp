/**
 @file LineSegment.cpp
  
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2003-02-08
 @edited  2006-04-20
 */

#include "G3D/platform.h"
#include "G3D/LineSegment.h"
#include "G3D/Sphere.h"
#include "G3D/debug.h"

namespace G3D {


Vector3 LineSegment::closestPoint(const Vector3& p) const {

    // The vector from the end of the capsule to the point in question.
    Vector3 v(p - _point);

    // Projection of v onto the line segment scaled by 
    // the length of direction.
    float t = direction.dot(v);

    // Avoid some square roots.  Derivation:
    //    t/direction.length() <= direction.length()
    //      t <= direction.squaredLength()

    if ((t >= 0) && (t <= direction.squaredMagnitude())) {
    
        // The point falls within the segment.  Normalize direction,
        // divide t by the length of direction.
        return _point + direction * t / direction.squaredMagnitude();
    
    } else {

        // The point does not fall within the segment; see which end is closer.

        // Distance from 0, squared
        float d0Squared = v.squaredMagnitude();

        // Distance from 1, squared
        float d1Squared = (v - direction).squaredMagnitude();

        if (d0Squared < d1Squared) {

            // Point 0 is closer
            return _point;

        } else {

            // Point 1 is closer
            return _point + direction;
        
        }
    }

}

Vector3 LineSegment::point(int i) const {
    switch (i) {
    case 0:
        return _point;

    case 1:
        return _point + direction;

    default:
        debugAssertM(i == 0 || i == 1, "Argument to point must be 0 or 1");
        return _point;
    }
}


bool LineSegment::intersectsSolidSphere(const class Sphere& s) const {
    return distanceSquared(s.center) <= square(s.radius);
}


LineSegment::LineSegment(class BinaryInput& b) {
	deserialize(b);
}


void LineSegment::serialize(class BinaryOutput& b) const {
	_point.serialize(b);
	direction.serialize(b);
}


void LineSegment::deserialize(class BinaryInput& b) {
	_point.deserialize(b);
	direction.deserialize(b);
}


Vector3 LineSegment::randomPoint() const {
    return _point + uniformRandom(0, 1) * direction;
}


/////////////////////////////////////////////////////////////////////////////////////

LineSegment2D LineSegment2D::fromTwoPoints(const Vector2& p0, const Vector2& p1) {
    LineSegment2D s;
    s.m_origin      = p0;
    s.m_direction   = p1 - p0;
    s.m_length      = s.m_direction.length();
    return s;
}


Vector2 LineSegment2D::point(int i) const {
    debugAssert(i == 0 || i == 1);
    if (i == 0) {
        return m_origin;
    } else {
        return m_direction + m_origin;
    }
}


Vector2 LineSegment2D::closestPoint(const Vector2& Q) const {
    // Two constants that appear in the result
    const Vector2 k1(m_origin - Q);
    const Vector2& k2 = m_direction;
    
    if (fuzzyEq(m_length, 0)) {
        // This line segment has no length
        return m_origin;
    }

    // Time [0, 1] at which we hit the closest point travelling from p0 to p1.
    // Derivation can be obtained by minimizing the expression
    //     ||P0 + (P1 - P0)t - Q||.
    const float t = -k1.dot(k2) / (m_length * m_length);

    if (t < 0) {
        // Clipped to low end point
        return m_origin;
    } else if (t > 1) {
        // Clipped to high end point
        return m_origin + m_direction;
    } else {
        // Subsitute into the line equation to find 
        // the point on the segment.
        return m_origin + k2 * t;
    }
}


float LineSegment2D::distance(const Vector2& p) const {
    Vector2 closest = closestPoint(p);
    return (closest - p).length();
}


float LineSegment2D::length() const {
    return m_length;
}

}

