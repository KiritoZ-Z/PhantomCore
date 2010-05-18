/**
  @file ShadowMap.cpp

  @author Morgan McGuire, morgan@cs.williams.edu
 */
#include "GLG3D/ShadowMap.h"
#include "GLG3D/RenderDevice.h"
#include "G3D/AABox.h"
#include "G3D/Box.h"

namespace G3D {

ShadowMap::ShadowMap() : m_polygonOffset(0.5f) {}

void ShadowMap::setSize(int desiredSize) {
    if (desiredSize == 0) {
        m_depthTexture = NULL;
        m_framebuffer = NULL;
        return;
    }

    if (! GLCaps::supports_GL_ARB_shadow()) {
        return;
    }
    
    int SHADOW_MAP_SIZE = desiredSize;

    if (GLCaps::enumVendor() == GLCaps::ATI) {
        // ATI shadows are really slow and require
        // a small shadow map
        SHADOW_MAP_SIZE = 128;
    } else if (! GLCaps::supports_GL_EXT_framebuffer_object()) {
        // Restrict to screen size
        SHADOW_MAP_SIZE = 512;
    }

    m_depthTexture = Texture::createEmpty(
                                     "Shadow Map",
                                     SHADOW_MAP_SIZE, SHADOW_MAP_SIZE,
                                     TextureFormat::depth(),
                                     Texture::DIM_2D, 
                                     Texture::Settings::shadow());
    
    if (GLCaps::supports_GL_EXT_framebuffer_object()) {
        m_framebuffer = Framebuffer::create("Shadow Frame Buffer");
        m_framebuffer->set(Framebuffer::DEPTH_ATTACHMENT, m_depthTexture);
    }
}


bool ShadowMap::enabled() const {
    return m_depthTexture.notNull();
}


void ShadowMap::updateDepth(
    RenderDevice* renderDevice,
    const GLight& light, 
    float lightProjX,
    float lightProjY,
    float lightProjNear,
    float lightProjFar,
    const Array<PosedModel::Ref>& shadowCaster) {

    if (shadowCaster.size() == 0) {
        return;
    }

    debugAssert(GLCaps::supports_GL_ARB_shadow()); 

    if (m_framebuffer.isNull()) {
        // Ensure that the buffer fits on screen
        if ((m_depthTexture->width() > renderDevice->width()) ||
            (m_depthTexture->height() > renderDevice->height())) {

            int size = iMin(renderDevice->width(), renderDevice->height());
            if (! G3D::isPow2(size)) {
                // Round *down* to the nearest power of 2 (can't round up or
                // we might exceed the renderdevice size.
                size = G3D::ceilPow2(size) / 2;
            }
            setSize(size);
        }
    }

    Rect2D rect = m_depthTexture->rect2DBounds();
    
    renderDevice->pushState(m_framebuffer);
        if (m_framebuffer.notNull()) {
            glReadBuffer(GL_NONE);
            glDrawBuffer(GL_NONE);
        } else {
            debugAssert(rect.height() <= renderDevice->height());
            debugAssert(rect.width() <= renderDevice->width());
            renderDevice->setViewport(rect);
        }
        
        // Construct a projection and view matrix for the camera so we can 
        // render the scene from the light's point of view
        //
        // Since we're working with a directional light, 
        // we want to make the center of projection for the shadow map
        // be in the direction of the light but at a finite distance 
        // to preserve z precision.
        Matrix4 lightProjectionMatrix(Matrix4::orthogonalProjection(-lightProjX, lightProjX, -lightProjY, 
                                                                    lightProjY, lightProjNear, lightProjFar));

        // Find the scene bounds
        AABox sceneBounds;
        shadowCaster[0]->worldSpaceBoundingBox().getBounds(sceneBounds);

        for (int i = 1; i < shadowCaster.size(); ++i) {
            AABox bounds;
            shadowCaster[i]->worldSpaceBoundingBox().getBounds(bounds);
            sceneBounds.merge(bounds);
        }

        CoordinateFrame lightCFrame;
        Vector3 center = sceneBounds.center();
        lightCFrame.translation = (light.position.xyz() * 20 + center) * (1 - light.position.w) + light.position.w * light.position.xyz();

        Vector3 perp = Vector3::unitZ();
        // Avoid singularity when looking along own z-axis
        if (abs((lightCFrame.translation - center).direction().dot(perp)) > 0.8) {
            perp = Vector3::unitY();
        }
        lightCFrame.lookAt(center, perp);
        
        //renderDevice->setColorClearValue(Color3::white());
        bool debugShadows = false;
        renderDevice->setColorWrite(debugShadows);
        renderDevice->setDepthWrite(true);
        renderDevice->clear(debugShadows, true, false);

        // Draw from the light's point of view
        renderDevice->setCameraToWorldMatrix(lightCFrame);
        renderDevice->setProjectionMatrix(lightProjectionMatrix);

        // Flip the Y-axis to account for the upside down Y-axis on read back textures
        m_lightMVP = lightProjectionMatrix * lightCFrame.inverse();

        // Avoid z-fighting
        renderDevice->setPolygonOffset(m_polygonOffset);

        renderDevice->setAlphaTest(RenderDevice::ALPHA_GREATER, 0.5);
        for (int s = 0; s < shadowCaster.size(); ++s) {
            shadowCaster[s]->render(renderDevice);
        }
    renderDevice->popState();

    if (m_framebuffer.isNull()) {
        debugAssert(m_depthTexture.notNull());
        m_depthTexture->copyFromScreen(rect);
    } else {
        glDrawBuffer(GL_BACK);
    }
}

}
