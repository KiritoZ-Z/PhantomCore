/**
 @file SkyParameters.h

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2002-10-05
 @edited  2005-06-01

 Copyright 2000-2005, Morgan McGuire.
 All rights reserved.
 */

#ifndef G3D_SKYPARAMETERS_H
#define G3D_SKYPARAMETERS_H

#include "G3D/platform.h"
#include "GLG3D/Texture.h"
#include "G3D/Color3.h"
#include "G3D/Vector3.h"
#include "G3D/CoordinateFrame.h"
#include "G3D/GLight.h"

namespace G3D {

#define BROWN_UNIVERSITY_LATITUDE 41.7333f
#define BROWN_UNIVERSITY_LONGITUDE 71.4333f

#define WILLIAMS_COLLEGE_LATITUDE 42.71f
#define WILLIAMS_COLLEGE_LONGITUDE -73.20f

/* Definition of a sidereal day */
#define SIDEREAL_DAY ((23*HOUR)+(56*MINUTE)+(4.071f*SECOND))

typedef ReferenceCountedPointer<class Lighting> LightingRef;
typedef ReferenceCountedPointer<class Sky> SkyRef;
/**
 Provides a reasonable (but not 100% physically correct) set of lighting 
 parameters based on the time of day.  See also G3D::Lighting, which describes
 a rich lighting environment.
 */
class SkyParameters {
public:
    /** Multiply this by all emissive values when rendering.  
        Some algorithms (e.g. contrib/ArticulatedModel/ToneMap) scale
        down light intensity to preserve dynamic range.*/
    Color3                  emissiveScale;

    /** Modulate sky box color */
    Color3                  skyAmbient;

    /**
     Use this for objects that do not receive directional lighting
     (e.g. billboards).
     */
    Color3                  diffuseAmbient;

    /**
     Directional light color.
     */
    Color3                  lightColor;
    Color3                  ambient;

    /** Only one light source, the sun or moon, is active at a given time. */
    Vector3                 lightDirection;
    enum {SUN, MOON}        source;

    /** Using physically correct parameters.  When false, the sun and moon
        travel in a perfectly east-west arc where +x = east and -x = west. */
    bool                    physicallyCorrect;

    /** The vector <B>to</B> the sun */
    Vector3		            trueSunPosition;
    Vector3                 sunPosition;

    /** The vector <B>to</B> the moon */
    Vector3		            trueMoonPosition;
    Vector3                 moonPosition;
	double			        moonPhase;	

    /** The coordinate frame and vector related to the starfield */
    CoordinateFrame	        starFrame;
	CoordinateFrame		    trueStarFrame;
    Vector3		            starVec;

    /** Geographic position */
    float                   geoLatitude;
	
    SkyParameters();

    /**
     Sets light parameters for the sun/moon based on the
     specified time since midnight, as well as geographic 
     latitude for starfield orientation (positive for north 
     of the equator and negative for south) and geographic 
     longitude for sun positioning (postive for east of 
     Greenwich, and negative for west). The latitude and 
     longitude is set by default to that of Williamstown, MA, 
     USA.
     */
     SkyParameters(
	     const GameTime     _time,
	     bool 	            _physicallyCorrect = true,
	     float              _latitude = WILLIAMS_COLLEGE_LATITUDE);

    void setTime(const GameTime _time);
	void setLatitude(float _latitude);

    /**
     Returns a directional light composed from the light direction
     and color.
     */
    GLight directionalLight() const;

};


// TODO: Remove
/** @deprecated */
typedef SkyParameters SkyParameters;

/** A rich environment lighting model that contains both global and local sources.
    See also SkyParameters, a class that describes a sun and moon lighting 
    model. */
class Lighting : public ReferenceCountedObject {
private:

    Lighting() : emissiveScale(Color3::white()) {}

public:

    LightingRef static fromSky(const SkyRef& sky, const SkyParameters&, const Color3& groundColor);

    /** Multiply this by all emissive values when rendering.  
        Some algorithms (e.g., G3D::ToneMap) scale
        down light intensity to preserve dynamic range.*/
    Color3              emissiveScale;

    /** Light reflected from the sky (usually slightly blue) */
    Color3              ambientTop;

    /** Light reflected from the ground (usually neutral, brown, or green).  May be identical to ambientTop. */
    Color3              ambientBottom;

    /** Cube map or sphere map of the surrounding environment (often just the skybox, although 
         it may be rendered per-object). */
    Texture::Ref          environmentMap;

    /** Color to modulate environment map by */
    Color3              environmentMapColor;

    /** Local illumination sources that do not cast shadows. */
    Array<GLight>       lightArray;

    /** Local illumination sources that cast shadows. */
    Array<GLight>       shadowedLightArray;

    /** Creates a (dark) environment. */
    static ReferenceCountedPointer<Lighting> create() {
        return new Lighting();
    }

    inline Color3 ambientAverage() const {
        return (ambientBottom + ambientTop) / 2.0f;
    }

    /** Make a copy of this lighting environment (does not clone the environment map) */
    LightingRef clone() const;

    /**
     Removes the dimmest non-shadowed light and adds its contribution in to the ambient terms.
     (not implemented in this beta)
     */
    void reduceNonShadowedLights(int numLeft = 1);

    /**
     Removes the dimmest shadowed light and adds its contribution in to the ambient terms.
     (not implemented in this beta)
     */
    void reduceShadowedLights(int numLeft = 0);
};

}

#endif


