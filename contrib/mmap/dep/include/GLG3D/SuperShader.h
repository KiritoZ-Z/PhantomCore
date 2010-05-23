/** 
  @file SuperShader.h

  A one-size-fits-all shader that combines most common illumination effects
  efficiently.

  @author Morgan McGuire, morgan@cs.brown.edu
 */

#ifndef SUPERSHADER_H
#define SUPERSHADER_H

#include "G3D/ReferenceCount.h"
#include "GLG3D/SkyParameters.h"
#include "GLG3D/Shader.h"

namespace G3D {

typedef ReferenceCountedPointer<class SuperShader> SuperShaderRef;

/**
 A one-size-fits-all shader that combines most common illumination effects
 efficiently.

 To use SuperShader you will need the four files in the current directory
 at runtime:
 <ul>
 <li>ShadowMappedLightPass.vrt
 <li>ShadowMappedLightPass.pix
 <li>NonShadowedPass.vrt
 <li>NonShadowedPass.pix
 </ol>

 these are located in the data/SuperShader directory of the G3D distribution.

 Example of using with a PosedModel:

 <pre>
    <i> members:</i>
    SuperShader::Material material;
    ShaderRef nonshadowShader;
    ShaderRef shadowShader;

    <i> in constructor:</i>
    ... set fields of material ...
    SuperShader::createShaders(material, nonshadowShader, shadowShader);

    <i> in onGraphics:</i>

    rd->pushState();
        SuperShader::configureShaderArgs(localLighting, material, shader->args);
        rd->setShader(shader);
        rd->setObjectToWorldMatrix(posed->coordinateFrame());
        rd->setShadeMode(RenderDevice::SHADE_SMOOTH);
        posed->sendGeometry(rd);
    rd->popState();
 </pre>

 @cite McGuire, The %SuperShader. Chapter 8.1, 485--498, in <i>ShaderX<sup>4</sup></i>, W. Engel ed., 2005.
 */
class SuperShader {
public:

    /** Material property coefficients are specified as 
        a constant color times a texture map.  If the color
        is white the texture map entirely controls the result.
        If the color is black the term is disabled.  On graphics
        cards with few texture units the map will be ignored.*/
    class Component {
    public:
        /** Color that is constant over the entire surface. */
        Color3              constant;

        /** Color that varies over position.  NULL means white.*/
        Texture::Ref        map;

        inline Component() : constant(0, 0, 0), map(NULL) {}
        inline Component(const Color3& c) : constant(c), map(NULL) {}
        inline Component(double c) : constant(c, c, c), map(NULL) {}
        inline Component(Texture::Ref t) : constant(1, 1, 1), map(t) {}

        /** True if this material is definitely (0,0,0) everywhere */
        inline bool isBlack() const {
            // To be black, it doesn't matter what your texture is--it will be multiplied by zero!
            return (constant.r == 0) && (constant.g == 0) && (constant.b == 0);
        }

        /** True if this material is definitely (1,1,1) everywhere */
        inline bool isWhite() const {
            // To be white, you have to have no texture map and all white constants.
            return (constant.r == 1) && (constant.g == 1) && (constant.b == 1) && map.isNull();
        }

        inline bool operator==(const Component& other) const {
            return (constant == other.constant) && (map == other.map);
        }

        /** Returns true if both components will produce similar non-zero terms in a
            lighting equation.  Black and white are only similar to themselves. */
        bool similarTo(const Component& other) const;
    };

    /** Describes the properties of a material to be used with SuperShader. Each of the Component fields
        can be set to a scalar constant, a color, a texture map, or the product of a color and a texture map.
        Unused components are optimized away by the SuperShader.
    
        Beta API; subject to change in future releases.
        Illumination Equation:

        dst1 = underlying value in frame buffer
        evt = environment map
        ambUp, ambDn = ambient map up and down values (ideally, environment map convolved with a hemisphere)

        dst2 = dst1 * transmission + 
               evt[n] * reflection +
               lerp(ambDn, ambUp, n.y/2 + 0.5) * diffuse +
               emissive +
               SUM OVER LIGHTS {
                 light * (diffuse * NdotL +
                          specular * NdotH^specularExponent)}

        When choosing material properties, the transmission, diffuse, and specular terms
        should sum to less than 1.  The reflection and specular terms are technically the
        same value and should be equal; the difference is that specular only applies to
        lights and reflection to the environment (less lights), a concession to artistic
        control.

        Note that most translucent materials should be two-sided and have comparatively
        low diffuse terms.  They should also be applied to convex objects (subdivide 
        non-convex objects) to prevent rendering surfaces out of order.

        */
    class Material {
	public:
        friend class SuperShader;

        /** Diffuse reflection of lights. The alpha channel is used as a mask, e.g., to cut out the shape of
            a leaf or a billboard, but does NOT encode transparency.  Use the transmit member to specify
            (optionally colored) transparency.*/
        Component               diffuse;

        /** Glow without illuminating other objects. */
        Component               emit;

        /** Specular (glossy) reflection of lights. */
        Component               specular;

        /** Sharpness of light reflections.*/
        Component               specularExponent;

        /** Translucency.  Can be colored. */
        Component               transmit;

        /** Perfect specular (mirror) reflection of the environment. */
        Component               reflect;

        /** RGB*2-1 = tangent space normal, A = tangent space bump height.
          If NULL bump mapping is disabled. */
        Texture::Ref              normalBumpMap;
       
        /** Multiply normal map alpha values (originally on the range 0-1)
            by this constant to obtain the real-world bump height. Should
            already be factored in to the normal map normals.*/
        float                   bumpMapScale;

        /**
         If the diffuse texture is changed, set this to true.  Defaults to true.
         */
        bool                    changed;

        Material() : diffuse(1), emit(0), 
            specular(0.25), specularExponent(60), 
            transmit(0), reflect(0), changed(true) {
        }

        /** Returns true if this material uses similar terms as other
            (used by SuperShader), although the actual textures may differ. */
        bool similarTo(const Material& other) const;

        /** 
         To be identical, two materials must not only have the same images in their
         textures but must share pointers to the same underlying Texture objects.
         */
        inline bool operator==(const Material& other) const {
            return (diffuse == other.diffuse) &&
                   (specular == other.specular) &&
                   (specularExponent == other.specularExponent) &&
                   (transmit == other.transmit) &&
                   (reflect == other.reflect) &&
                   (normalBumpMap == other.normalBumpMap) &&
                   (bumpMapScale == other.bumpMapScale);
        }

        inline bool operator!=(const Material& other) const {
            return !(*this == other);
        }

        /** 
          If Material::changed is true, copies the diffuse texture's alpha channel to 
          all other maps. Call before rendering with this material.
         */
        void enforceDiffuseMask();
    };

    /** Configures the material arguments on a SuperShader NonShadowed shader for
        the opaque pass with 0, 1, or 2 lights. */
    static void configureShaderArgs(
        const LightingRef&              lighting,
        const Material&                 material,
        VertexAndPixelShader::ArgList&  args);

    /** Configures a SuperShader NonShadowed shader for an additive light pass
        using lights lightArray[lightIndex] and lightArray[lightIndex+1] (if both are in bounds.)*/
    static void configureShaderExtraLightArgs(
        const Array<GLight>&           lightArray,
        int                             lightIndex,
        VertexAndPixelShader::ArgList&  args);

    static void configureShadowShaderArgs(
        const GLight&                   light, 
        const Matrix4&                  lightMVP, 
        const Texture::Ref&             shadowMap,
        const Material&                 material,
        VertexAndPixelShader::ArgList&  args);

private:

    class Cache {
    public:
        struct Pair {
        public:
            ShaderRef       nonShadowedShader;
            ShaderRef       shadowMappedShader;
        };
    private:

        Array<Material>     materialArray;
        Array<Pair>         shaderArray;

    public:

        // TODO: mechansim for purging old shaders

        /** Adds a shader to the list of cached ones.  Only call when 
            getSimilar returned NULL.*/
        void add(const Material& mat, const Pair& pair);

        /** Returns the shader for a similar material or 
            NULL, NULL if one does not exist. */
        Pair getSimilar(const Material& mat) const;
    };

    static Cache cache;

    static Cache::Pair getShader(const Material& material);

    // Don't call
    SuperShader() {
        debugAssert(false);
    }

public:

    static void createShaders(
        const Material& material,
        ShaderRef&      nonShadowedShader,
        ShaderRef&      shadowMappedShader);

}; // SuperShader

}

#endif
