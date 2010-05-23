/**
  @file ShadowMap.h

  @author Morgan McGuire, morgan@cs.williams.edu
 */
#ifndef G3D_SHADOWMAP_H
#define G3D_SHADOWMAP_H

#include "G3D/Matrix4.h"
#include "G3D/GLight.h"
#include "GLG3D/Texture.h"
#include "GLG3D/Framebuffer.h"
#include "GLG3D/PosedModel.h"

namespace G3D {

class ShadowMap {    
private:
    TextureRef          m_depthTexture;

    /** If NULL, use the backbuffer */
    FramebufferRef      m_framebuffer;

    Matrix4             m_lightMVP;

    float               m_polygonOffset;

public:

    /** Increase to hide self-shadowing artifacts, decrease to avoid gap between shadow and object.  Default = 0.5 */
    void setPolygonOffset(float s) {
        m_polygonOffset = s;
    }

    float polygonOffset() const {
        return m_polygonOffset;
    }

    ShadowMap();

    /** Call with desiredSize = 0 to turn off shadow maps */
    void setSize(int desiredSize = 1024);

    bool enabled() const;

    void updateDepth(
        class RenderDevice* renderDevice, 
        const GLight& light, 
        float lightProjX,
        float lightProjY,
        float lightProjNear,
        float lightProjFar,
        const Array<PosedModel::Ref>& shadowCaster);

    const Matrix4& lightMVP() const {
        return m_lightMVP;
    }

    TextureRef depthTexture() const {
        return m_depthTexture;
    }
};

}

#endif // G3D_SHADOWMAP_H
