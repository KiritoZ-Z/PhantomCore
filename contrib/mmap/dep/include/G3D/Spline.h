/**
 @file Spline.h

 @author Morgan McGuire, morgan@cs.williams.edu
 */

#ifndef G3D_SPLINE_H
#define G3D_SPLINE_H

#include "G3D/platform.h"
#include "G3D/Array.h"
#include "G3D/g3dmath.h"
#include "G3D/Matrix4.h"
#include "G3D/Vector4.h"

namespace G3D {

/**
 Piecewise 3rd-order Catmull-Rom spline curve for uniformly spaced control points in time.
 See Real Time Rendering, 2nd edition, ch 12 for discussion of splines.

 The template parameter must support operator+(Control) and operator*(float).

 To provide shortest-path interpolation, override ensureShortestPath().

 See also G3D::UprightSpline, G3D::QuatSpline.
 */
template<typename Control>
class Spline {
public:

    /** Control points. */
    Array<Control>          control;
    
    /** If cyclic, then the control points are assumed to wrap 
        around.  If not cyclic, then the derivatives at the ends 
        of the spline point to the final control points.*/
    bool                    cyclic;
    
    Spline() : cyclic(true) {
    }

    virtual ~Spline() {}
    
    void append(const Control& c) {
        control.append(c);
    }

    void clear() {
        control.clear();
    }

    /** Number of control points */
    int size() const {
        return control.size();
    }

protected:

    /** Returns the requested control point, wrapping at the ends.*/
    inline const Control& getControl(int i) const {
        return control[iWrap(i, control.size())];
    }

    /** The indices may be assumed to be treated cyclically. */
    void getControls(int i, Control* A, int N) const {
        for (int j = 0; j < N; ++j) {
            A[j] = getControl(i + j);
        }
        ensureShortestPath(A, N);
    }


    /**
       Mutates the array of N control points. It is useful to override this
       method by one that wraps the values if they are angles or quaternions
       for which "shortest path" interpolation is significant.
     */
    virtual void ensureShortestPath(Control* A, int N) const {
        // Default does nothing
    }

    /** Normalize or otherwise adjust this interpolated Control before returning it */
    virtual void correct(Control& A) const {}

private:

    /**
     Handles the case for non-cyclic splines where s <= 1 or s >= control.size() - 2
     to simplify the implementation of evaluate().
     */
    Control evaluateBorderCase(float s) const {
        const int last = control.size() - 1;

        if (s <= 0) {
            // Clamp to the first point
            return control[0];
        } else if (s >= last) {

            // Clamp to the last point
            return control[last];

        } else {
            // Multiply by -1 instead of subtracting to avoid requiring a "-" operator
            // on the Control type.
            Control tangent;
            Control start;
            Control end;
            float t;
            
            if (s <= 1) {
                Control p[3];
                getControls(0, p, 3);
                
                // At s = 1, the tangent is given by the two adjacent control points and
                // the position is given by control[1].
                //
                // At s = 0 the tangent is undefined and the position is control[0].
                //
                // Move away from s = 1 in a line along the tangent and interpolate towards 
                // control 0.

                tangent = (p[0] + p[2] * -1.0f) * 0.5f;
                start = p[1];
                end = p[0];
                t = s;
            } else {
                Control p[3];
                getControls(last - 2, p, 3);
                
                tangent = (p[2] + p[0] * -1.0f) * 0.5f;
                start = p[1];
                end = p[2];
                t = last - s;
            }

            // At t = 0, located at end.

            float a = t;
            return end * (1.0f - a) + (start + tangent * (1.0f - t)) * a;
        }
    }

public:

    /**
       Return the position at parameter s.

       For a non-cyclic spline, the curve is clamped at the beginning and ending points
       and between 0 and 1
     */
    Control evaluate(float s) const {

        if (control.size() < 4) {
            switch (control.size()) {
            case 0:
                {
                    Control c;
                    // Hide the fact from C++ that we are using an uninitialized
                    // variable here
                    return *(&c) * 0;
                }

            case 1:
                // Single point
                return control[0];

            case 2:
                // Linear interpolation
                {
                    float a;
                    if (cyclic) {
                        a = wrap(s, 2.0f);
                        if (a > 1.0f) {
                            // Reflect around end points
                            a = 2.0f - a;
                        }
                    } else {
                        a = clamp(s, 0.0f, 1.0f);
                    }
                    return control[0] * (1.0f - a) + control[1] * a;
                }
   
            case 3:
                // Should be quadratic, but we just use piecewise
                // linear in this implementation
                {
                    // interpolation param
                    float a;
                    // control point index
                    int i = 0;
                    if (cyclic) {
                        a = wrap(s, 4.0f);
                        if (a > 2.0f) {
                            // Reflect around end points
                            a = 4.0f - a;
                        }
                    } else {
                        a = clamp(s, 0.0f, 2.0f);
                    }
                    if (a > 1.0f) {
                        i = 1;
                        a -= 1.0f;
                    }
                    return control[i] * (1.0f - a) + control[i + 1] * a;
                }
                
            }
        }

        debugAssertM(control.size() >= 4, "Requires at least four control points.");

        // Catmull-Rom basis, such that 
        //     q(s) = [t^0 t^1 t^2 t^3] * basis *[p_-1 p_0 p_1 p_2]^T for 0<t<1
        //    
        static const Matrix4 basis = Matrix4
            ( 0,   2,   0,   0,
             -1,   0,   1,   0,
              2,  -5,   4,  -1,
             -1,   3,  -3,   1) * 0.5f;

        if (! cyclic && ((s <= 1) || (s >= control.size() - 2))) {
            // Special case for the ends.
            Control result = evaluateBorderCase(s);
            correct(result);
            return result;
        }

        // Index of the first control point
        int i = 0;

        // Integer control point
        i = iFloor(s);

        // Fractional part
        float t = s - i;

        // Powers of t
        Vector4 tvec(1.0f, t, t*t, t*t*t);

        // Compute the weights on each of the control points.
        Vector4 weights = tvec * basis;

        Control p[4];
        getControls(i - 1, p, 4);

        // Compute the weighted sum of the neighboring control points.
        Control sum = p[0] * weights[0];
        for (int j = 1; j < 4; ++j) {
            sum = sum + p[j] * weights[j];
        }

        correct(sum);
        return sum;
    }

};

}

#endif
