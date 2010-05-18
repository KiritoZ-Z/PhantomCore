/**
  @file GLight.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-11-12
  @edited  2007-07-10
*/

#include "G3D/GLight.h"

namespace G3D {

GLight::GLight() {
    position        = Vector4(0, 0, 0, 0);
    color           = Color3::white();
    spotDirection   = Vector3(0, 0, -1);
    spotCutoff      = 180;
    enabled         = false;
    attenuation[0]  = 1.0;
    attenuation[1]  = 0.0;
    attenuation[2]  = 0.0;
    specular        = true;
    diffuse         = true;
}


GLight GLight::directional(const Vector3& toLight, const Color3& color, bool s, bool d) {
    GLight L;
    L.position = Vector4(toLight.direction(), 0);
    L.color    = color;
    L.specular = s;
    L.diffuse  = d;
    return L;
}


GLight GLight::point(const Vector3& pos, const Color3& color, float constAtt, float linAtt, float quadAtt, bool s, bool d) {
    GLight L;
    L.position = Vector4(pos, 1);
    L.color    = color;
    L.attenuation[0] = constAtt;
    L.attenuation[1] = linAtt;
    L.attenuation[2] = quadAtt;
    L.specular       = s;
    L.diffuse        = d;
    return L;
}


GLight GLight::spot(const Vector3& pos, const Vector3& pointDirection, float cutOffAngleDegrees, const Color3& color, float constAtt, float linAtt, float quadAtt, bool s, bool d) {
    GLight L;
    L.position = Vector4(pos, 1);
    L.spotDirection = pointDirection;
    L.spotCutoff = cutOffAngleDegrees;
    L.color    = color;
    L.attenuation[0] = constAtt;
    L.attenuation[1] = linAtt;
    L.attenuation[2] = quadAtt;
    L.specular       = s;
    L.diffuse        = d;
    return L;
}


bool GLight::operator==(const GLight& other) const {
    return (position == other.position) && 
        (spotDirection == other.spotDirection) &&
        (spotCutoff == other.spotCutoff) &&
        (attenuation[0] == other.attenuation[0]) &&
        (attenuation[1] == other.attenuation[1]) &&
        (attenuation[2] == other.attenuation[2]) &&
        (color == other.color) &&
        (enabled == other.enabled) &&
        (specular == other.specular) &&
        (diffuse == other.diffuse);
}

bool GLight::operator!=(const GLight& other) const {
    return !(*this == other);
}

}
