/**
 @file G3DGameUnits.cpp

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2002-10-05
 @edited  2003-11-25
 */

#include "G3D/G3DGameUnits.h"

namespace G3D {

SimTime toSeconds(int hour, int minute, AMPM ap) {
    return toSeconds(hour, minute, 0, ap);
}

SimTime toSeconds(int hour, int minute, double seconds, AMPM ap) {
    double t = ((hour % 12) * 60 + minute) * 60 + seconds;

    if (ap == PM) {
        t += 12 * 60 * 60;
    }

    return t;
}

}

