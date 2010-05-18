/**
 @file ToneMap.h

 @created 2005-04-06
 @edited  2006-10-20

 Copyright 2005-2007, Morgan McGuire.
 All rights reserved.
*/

#ifndef G3D_TONEMAP_H
#define G3D_TONEMAP_H

#include <GLG3D/RenderDevice.h>
#include <GLG3D/Texture.h>
#include <GLG3D/Shader.h>
#include <GLG3D/SkyParameters.h>

namespace G3D {

typedef ReferenceCountedPointer<class ToneMap> ToneMapRef;

/** 
  Applies gamma correction and bloom. 

  <b>GApp subclasses have a built-in ToneMap named GApp::toneMap.</b>

  In the real world, the shadow under a car is about 1,000,000 times darker than 
  the specular highlight off its hood.  Most computer displays can only represent
  a dynamic range of 255 values (eight bits) per color channel, so bright areas
  are blown out and all detail is lost in dark areas.  This is the dynamic range problem
  of displays.
  
  Furthermore, displays were 
  designed for 2D rendering where rgb=128 should <i>appear</i> half as bright as rgb=255.
  In contrast, 3D rendering simulates light, so rgb=128 should have half as much <i>energy</i>
  as rgb=255, which does not correspond to the perception of half as bright.  This is the
  gamma ramp problem.

  There are many sophisticated algorithms for altering a rendered image to correct both of these.
  This ToneMap class uses an algorithm that is not the most sophisticated, but is extremely
  fast and produces attractive results.  It alters the lights in a scene to conserve
  dynamic range and then post-processes an image to make overexposed areas "glow" and 
  correct the gamma ramp for all other areas.  The resulting images have more detail in dark areas
  and appear more realistic than unprocessed ones.

  Use Example:

  <PRE>
  void onGraphics(RenderDevice* rd) {
    LightingRef        lighting      = toneMap.prepareLighting(app->lighting);
    SkyParameters      skyParameters = toneMap.prepareSkyParameters(app->skyParameters);

    toneMap.beginFrame(rd);
        <i>rendering code ...</i>
    toneMap.endFrame(rd);
  }
  </PRE>

*/
class ToneMap : public ReferenceCountedObject {
public:

    class Settings {
    public:
        
    };

private:

    /** PS14ATI shaders */
    static unsigned int         gammaShaderPS14ATI; 

    /** Three pass algorithm. */
    static ShaderRef            bloomShader[3];

    Texture::Ref                  screenImage;

    bool                        stereo;

    /** When in stereo mode, BloomMap 0 is the left eye, bloom map 1 is the right eye. */
    Texture::Ref                  stereoBloomMap[2];

    /** Intermediate result used when computing the bloom map.  This
        is 1/4 the horizontal resolution of the screen.*/
    Texture::Ref                  bloomMapIntermediate;

    bool                        mEnabled;
    
    Settings                    mSettings;

    /** Inverse gamma ramps. */
    // For programmable we don't use B
    static Texture::Ref           RG, B;

    static void makeGammaCorrectionTextures();

    static void makeShadersPS14ATI();

    static void makeShadersPS20();

    /** Resizes screenImage and bloomMap if needed to match the screen size.*/
    void resizeImages(RenderDevice* rd);

    enum Profile {NO_TONE, UNINITIALIZED, PS14ATI, PS14NVIDIA, PS20};

    static Profile profile;

    void applyPS20(RenderDevice* rd);
    void applyPS14ATI(RenderDevice* rd);
    void applyPS14NVIDIA(RenderDevice* rd);

    /** Called from resizeImages and to clear the old bloom map on occasion. */
    void resizeBloomMap(int w, int h);

    /** Returns the appropriate bloom map for the current draw buffer (i.e., resolves stereo issues)*/
    Texture::Ref getBloomMap(RenderDevice* rd) const;

    ToneMap();

public:

    inline static ToneMapRef create() {
        return new ToneMap();
    }

    void setEnabled(bool e);

    inline bool enabled() const {
        return mEnabled;
    }


    /** Call before rendering the scene to create a tone-mapping compatible lighting environment.
        Guaranteed to return a new lighting environment that is safe to further mutate. 
    
        If you created the lighting from SkyParameters that was itself prepared, do not call
        this method or the lights will be too dark.
    */
    LightingRef prepareLighting(const LightingRef& L) const;

    /** Call before rendering the scene to create a tone-mapping compatible lighting environment. */
    SkyParameters prepareSkyParameters(const SkyParameters& L) const;

    /** Call before rendering anything (including clearing the screen. */
    void beginFrame(RenderDevice* rd);

    /** Call after rendering the rest of the scene to apply tone mapping. */
    void endFrame(RenderDevice* rd);

};

}

#endif
