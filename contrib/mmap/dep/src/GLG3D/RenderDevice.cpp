/**
 @file RenderDevice.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com
 
 @created 2001-07-08
 @edited  2007-07-30
 */

#include "G3D/platform.h"
#include "G3D/Log.h"
#include "G3D/GCamera.h"
#include "GLG3D/glcalls.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/Texture.h"
#include "GLG3D/getOpenGLState.h"
#include "GLG3D/VARArea.h"
#include "GLG3D/VAR.h"
#include "GLG3D/Framebuffer.h"
#include "G3D/fileutils.h"
#include "GLG3D/Lighting.h"
#include <time.h>
#ifdef G3D_WIN32
    #include "GLG3D/Win32Window.h"
#elif defined(G3D_OSX)
    #include "GLG3D/CarbonWindow.h"
#else
    #include "GLG3D/SDLWindow.h"    
#endif
#include "GLG3D/GLCaps.h"

namespace G3D {

const GWindow* GWindow::m_current = NULL;


RenderDevice* RenderDevice::lastRenderDeviceCreated = NULL;


GLenum BufferToGL[MAX_BUFFER_SIZE] = {GL_NONE,
                                      GL_FRONT_LEFT,
                                      GL_FRONT_RIGHT,
                                      GL_BACK_LEFT,
                                      GL_BACK_RIGHT,
                                      GL_FRONT,
                                      GL_BACK,
                                      GL_LEFT,
                                      GL_RIGHT,
                                      GL_FRONT_AND_BACK,
                                      0xFFFF,                   //BUFFER_CURRENT
                                      GL_COLOR_ATTACHMENT0_EXT,
                                      GL_COLOR_ATTACHMENT1_EXT,
                                      GL_COLOR_ATTACHMENT2_EXT,
                                      GL_COLOR_ATTACHMENT3_EXT,
                                      GL_COLOR_ATTACHMENT4_EXT,
                                      GL_COLOR_ATTACHMENT5_EXT,
                                      GL_COLOR_ATTACHMENT6_EXT,
                                      GL_COLOR_ATTACHMENT7_EXT,
                                      GL_COLOR_ATTACHMENT8_EXT,
                                      GL_COLOR_ATTACHMENT9_EXT,
                                      GL_COLOR_ATTACHMENT10_EXT,
                                      GL_COLOR_ATTACHMENT11_EXT,
                                      GL_COLOR_ATTACHMENT12_EXT,
                                      GL_COLOR_ATTACHMENT13_EXT,
                                      GL_COLOR_ATTACHMENT14_EXT,
                                      GL_COLOR_ATTACHMENT15_EXT};

static GLint toGLBlendFunc(RenderDevice::BlendFunc b) {
    switch (b) {
    case RenderDevice::BLEND_SRC_ALPHA:
        return GL_SRC_ALPHA;
        
    case RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA:
        return GL_ONE_MINUS_SRC_ALPHA;

    case RenderDevice::BLEND_ONE:
        return GL_ONE;
        
    case RenderDevice::BLEND_ZERO:
        return GL_ZERO;

    case RenderDevice::BLEND_SRC_COLOR:
        return GL_SRC_COLOR;

    case RenderDevice::BLEND_DST_COLOR:
        return GL_DST_COLOR;

    case RenderDevice::BLEND_ONE_MINUS_SRC_COLOR:
        return GL_ONE_MINUS_SRC_COLOR;

    default:
        debugAssertM(false, "Fell through switch");
        return GL_ZERO;
    }
}

static void _glViewport(double a, double b, double c, double d) {
    glViewport(iRound(a), iRound(b), 
	       iRound(a + c) - iRound(a), iRound(b + d) - iRound(b));
}

static GLenum primitiveToGLenum(RenderDevice::Primitive primitive) {
	switch (primitive) {
    case RenderDevice::LINES:
		return GL_LINES;

    case RenderDevice::LINE_STRIP:
		return GL_LINE_STRIP;

    case RenderDevice::TRIANGLES:
		return GL_TRIANGLES;

    case RenderDevice::TRIANGLE_STRIP:
		return GL_TRIANGLE_STRIP;

    case RenderDevice::TRIANGLE_FAN:
		return GL_TRIANGLE_FAN;

    case RenderDevice::QUADS:
		return GL_QUADS;

    case RenderDevice::QUAD_STRIP:
		return GL_QUAD_STRIP;

    case RenderDevice::POINTS:
        return GL_POINTS;

    default:
        debugAssertM(false, "Fell through switch");
        return 0;
    }
}


std::string RenderDevice::getCardDescription() const {
    return cardDescription;
}


void RenderDevice::getFixedFunctionLighting(const LightingRef& lighting) const {
    // Reset state
    lighting->lightArray.fastClear();
    lighting->shadowedLightArray.fastClear();
    lighting->ambientBottom = lighting->ambientTop = Color3::black();

    // Load the lights
    if (state.lights.lightEnabled) {

        lighting->ambientBottom = lighting->ambientTop = state.lights.ambient.rgb();

        for (int i = 0; i < MAX_LIGHTS; ++i) {
            if (state.lights.lightEnabled[i]) {
                lighting->lightArray.append(state.lights.light[i]);
            }
        }
    }
}

RenderDevice::RenderDevice() : _window(NULL), deleteWindow(false) {
    emwaTriangleRate  = 0;
    emwaTriangleCount = 0;

    _initialized = false;
    cleanedup = false;
    inPrimitive = false;
    inShader = false;
    _numTextureUnits = 0;
    _numTextures = 0;
    _numTextureCoords = 0;
    emwaFrameRate = 0;
    lastTime = System::time();

    for (int i = 0; i < GLCaps::G3D_MAX_TEXTURE_UNITS; ++i) {
        currentlyBoundTexture[i] = 0;
    }

    lastRenderDeviceCreated = this;
}


void RenderDevice::setVARAreaMilestone() {
    currentVARArea->renderDevice = this;

    if (VARArea::mode == VARArea::VBO_MEMORY) {
        // We don't need milestones when using VBO; the spec guarantees
        // correct synchronization.
        return;
    }

    MilestoneRef milestone = createMilestone("VAR Milestone");
    setMilestone(milestone);

    debugAssert(currentVARArea.notNull());

    // Overwrite any preexisting milestone
    currentVARArea->milestone = milestone;
}


RenderDevice::~RenderDevice() {
    debugAssertM(cleanedup, "You deleted a RenderDevice without calling RenderDevice::cleanup()");
}

/**
 Used by RenderDevice::init.
 */
static const char* isOk(bool x) {
    return x ? "ok" : "UNSUPPORTED";
}

/**
 Used by RenderDevice::init.
 */
static const char* isOk(void* x) {
    // GCC incorrectly claims this function is not called.
    return isOk(x != NULL);
}


bool RenderDevice::supportsOpenGLExtension(
    const std::string& extension) const {

    return GLCaps::supports(extension);
}


void RenderDevice::init(
    const GWindow::Settings&      _settings,
    Log*                        log) {

    deleteWindow = true;
    #ifdef G3D_WIN32
        init(Win32Window::create(_settings), log);
    #elif defined(G3D_OSX)
        init(CarbonWindow::create(_settings), log);
    #else
        init(new SDLWindow(_settings), log);
    #endif
}


GWindow* RenderDevice::window() const {
    return _window;
}


void RenderDevice::init(GWindow* window, Log* log) {
    debugAssert(! initialized());
    debugAssert(window);

    _swapBuffersAutomatically = true;
    swapGLBuffersPending = false;
    _window = window;

    GWindow::Settings settings;
    window->getSettings(settings);
    
    // Load the OpenGL extensions if they have not already been loaded.
    GLCaps::init();

    debugLog = log;

    beginEndFrame = 0;
    if (debugLog) {debugLog->section("Initialization");}

    // Under Windows, reset the last error so that our debug box
    // gives the correct results
    #ifdef G3D_WIN32
        SetLastError(0);
    #endif

    const int minimumDepthBits    = iMin(16, settings.depthBits);
    const int desiredDepthBits    = settings.depthBits;

    const int minimumStencilBits  = settings.stencilBits;
    const int desiredStencilBits  = settings.stencilBits;

    // Don't use more texture units than allowed at compile time.
    if (GLCaps::supports_GL_ARB_multitexture()) {
        _numTextureUnits = iMin(GLCaps::G3D_MAX_TEXTURE_UNITS, 
                                glGetInteger(GL_MAX_TEXTURE_UNITS_ARB));
    } else {
        _numTextureUnits = 1;
    }

    // NVIDIA cards with GL_NV_fragment_program have different 
    // numbers of texture coords, units, and textures
    if (GLCaps::supports("GL_NV_fragment_program")) {
        glGetIntegerv(GL_MAX_TEXTURE_COORDS_NV, &_numTextureCoords);
        _numTextureCoords = iClamp(_numTextureCoords,
                                   _numTextureUnits,
                                   GLCaps::G3D_MAX_TEXTURE_UNITS);

        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_NV, &_numTextures);
        _numTextures = iClamp(_numTextures,
                              _numTextureUnits, 
                              GLCaps::G3D_MAX_TEXTURE_UNITS);
    } else {
        _numTextureCoords = _numTextureUnits;
        _numTextures      = _numTextureUnits;
    }

    if (! GLCaps::supports_GL_ARB_multitexture()) {
        // No multitexture
        if (debugLog) {
            debugLog->println("No GL_ARB_multitexture support: "
                              "forcing number of texture units "
                              "to no more than 1");
        }
        _numTextureCoords = iMax(1, _numTextureCoords);
        _numTextures      = iMax(1, _numTextures);
        _numTextureUnits  = iMax(1, _numTextureUnits);
    }
    debugAssertGLOk();

    if (debugLog) {
        int t = 0;
       
        int t0 = 0;
        if (GLCaps::supports_GL_ARB_multitexture()) {
            t = t0 = glGetInteger(GL_MAX_TEXTURE_UNITS_ARB);
        }

        if (GLCaps::supports_GL_ARB_fragment_program()) {
            t = glGetInteger(GL_MAX_TEXTURE_IMAGE_UNITS_ARB);
        }
        
        debugLog->printf("numTextureCoords                      = %d\n"
                         "numTextures                           = %d\n"
                         "numTextureUnits                       = %d\n"
                         "glGet(GL_MAX_TEXTURE_UNITS_ARB)       = %d\n"
                         "glGet(GL_MAX_TEXTURE_IMAGE_UNITS_ARB) = %d\n",
                         _numTextureCoords, _numTextures, _numTextureUnits,
                         t0,
                         t);
    }

    if (debugLog) {debugLog->println("Setting video mode");}

    setVideoMode();

    if (!strcmp((char*)glGetString(GL_RENDERER), "GDI Generic") && debugLog) {
        debugLog->printf(
         "\n*********************************************************\n"
           "* WARNING: This computer does not have correctly        *\n"
           "*          installed graphics drivers and is using      *\n"
           "*          the default Microsoft OpenGL implementation. *\n"
           "*          Most graphics capabilities are disabled.  To *\n"
           "*          correct this problem, download and install   *\n"
           "*          the latest drivers for the graphics card.    *\n"
           "*********************************************************\n\n");
    }

    glViewport(0, 0, width(), height());

    int depthBits, stencilBits, redBits, greenBits, blueBits, alphaBits;
    depthBits       = glGetInteger(GL_DEPTH_BITS);
    stencilBits     = glGetInteger(GL_STENCIL_BITS);
    redBits         = glGetInteger(GL_RED_BITS);
    greenBits       = glGetInteger(GL_GREEN_BITS);
    blueBits        = glGetInteger(GL_BLUE_BITS);
    alphaBits       = glGetInteger(GL_ALPHA_BITS);
    debugAssertGLOk();

    bool depthOk   = depthBits >= minimumDepthBits;
    bool stencilOk = stencilBits >= minimumStencilBits;

    if (debugLog) {
        debugLog->printf("Operating System: %s\n",
                         System::operatingSystem().c_str());

        debugLog->printf("Processor Architecture: %s\n\n", 
                         System::cpuArchitecture().c_str());

        debugLog->printf("GL Vendor:      %s\n", GLCaps::vendor().c_str());

        debugLog->printf("GL Renderer:    %s\n", GLCaps::renderer().c_str());

        debugLog->printf("GL Version:     %s\n", GLCaps::glVersion().c_str());

        debugLog->printf("Driver version: %s\n\n", GLCaps::driverVersion().c_str());

		std::string extStringCopy = (char*)glGetString(GL_EXTENSIONS);

        debugLog->printf(
            "GL extensions: \"%s\"\n\n",
            extStringCopy.c_str());
    }
 
    cardDescription = GLCaps::renderer() + " " + GLCaps::driverVersion();

    if (debugLog) {
    debugLog->section("Video Status");

    GWindow::Settings actualSettings;
    window->getSettings(actualSettings);

    // This call is here to make GCC realize that isOk is used.
    (void)isOk(false);
    (void)isOk((void*)NULL);

    debugLog->printf(
             "Capability    Minimum   Desired   Received  Ok?\n"
             "-------------------------------------------------\n"
             "* RENDER DEVICE \n"
             "Depth       %4d bits %4d bits %4d bits   %s\n"
             "Stencil     %4d bits %4d bits %4d bits   %s\n"
             "Alpha                           %4d bits   %s\n"
             "Red                             %4d bits   %s\n"
             "Green                           %4d bits   %s\n"
             "Blue                            %4d bits   %s\n"
             "FSAA                      %2d    %2d    %s\n"

             "Width             %8d pixels           %s\n"
             "Height            %8d pixels           %s\n"
             "Mode                 %10s             %s\n\n",

             minimumDepthBits, desiredDepthBits, depthBits, isOk(depthOk),
             minimumStencilBits, desiredStencilBits, stencilBits, isOk(stencilOk),

             alphaBits, "ok",
             redBits, "ok", 
             greenBits, "ok", 
             blueBits, "ok", 

             settings.fsaaSamples, actualSettings.fsaaSamples,
             isOk(settings.fsaaSamples == actualSettings.fsaaSamples),

             settings.width, "ok",
             settings.height, "ok",
             (settings.fullScreen ? "Fullscreen" : "Windowed"), "ok"
             );
    }

    inPrimitive        = false;
    inIndexedPrimitive = false;

    if (debugLog) debugLog->println("Done initializing RenderDevice.\n");

    _initialized = true;

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    _window->m_renderDevice = this;
}


bool RenderDevice::initialized() const {
    return _initialized;
}


#ifdef G3D_WIN32

HDC RenderDevice::getWindowHDC() const {
    return wglGetCurrentDC();
}

#endif

void RenderDevice::setGamma(
    double              brightness,
    double              gamma) {
    
    Array<uint16> gammaRamp(256);

    for (int i = 0; i < 256; ++i) {
        gammaRamp[i] =
            (uint16)min(65535, 
                      max(0, 
                      (int)(pow((brightness * (i + 1)) / 256.0, gamma) * 65535 + 0.5)));
	}
    
    _window->setGammaRamp(gammaRamp);
}


void RenderDevice::notifyResize(int w, int h) {
    swapBuffers();
    _window->notifyResize(w, h);
}


void RenderDevice::setVideoMode() {

    debugAssertM(stateStack.size() == 0, 
                 "Cannot call setVideoMode between pushState and popState");
    debugAssertM(beginEndFrame == 0, 
                 "Cannot call setVideoMode between beginFrame and endFrame");

    // Reset all state

    GWindow::Settings settings;
    _window->getSettings(settings);

    // Set the refresh rate
    #ifdef G3D_WIN32
        if (wglSwapIntervalEXT != NULL) {
            if (debugLog) {
                if (settings.asychronous) {
                    debugLog->printf("wglSwapIntervalEXT(0);\n");
                } else {
                    debugLog->printf("wglSwapIntervalEXT(1);\n");
                }
            }
            wglSwapIntervalEXT(settings.asychronous ? 0 : 1);
        }
    #endif

    // Enable proper specular lighting
    if (GLCaps::supports("GL_EXT_separate_specular_color")) {
        if (debugLog) {
            debugLog->println("Enabling separate specular lighting.\n");
        }
        glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL_EXT, 
                      GL_SEPARATE_SPECULAR_COLOR_EXT);
        debugAssertGLOk();
    } else if (debugLog) {
        debugLog->println("Cannot enable separate specular lighting, extension not supported.\n");
    }


    // Make sure we use good interpolation.
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // Smoothing by default on NVIDIA cards.  On ATI cards
    // there is a bug that causes shaders to generate incorrect
    // results and run at 0 fps, so we can't enable this.
    if (! beginsWith(GLCaps::vendor(), "ATI")) {
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POINT_SMOOTH);
    }
    

    // glHint(GL_GENERATE_MIPMAP_HINT_EXT, GL_NICEST);
    if (GLCaps::supports("GL_ARB_multisample")) {
        glEnable(GL_MULTISAMPLE_ARB);
    }

    debugAssertGLOk();
    if (GLCaps::supports("GL_NV_multisample_filter_hint")) {
        glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
    }

    resetState();

    if (debugLog) debugLog->printf("Done setting initial state.\n");
}


int RenderDevice::width() const {
    if (state.framebuffer.isNull()) {
        return _window->width();
    } else {
        return state.framebuffer->width();
    }
}


int RenderDevice::height() const {
    if (state.framebuffer.isNull()) {
        return _window->height();
    } else {
        return state.framebuffer->height();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////


Vector4 RenderDevice::project(const Vector3& v) const {
    return project(Vector4(v, 1));
}


Vector4 RenderDevice::project(const Vector4& v) const {
    return glToScreen(v);
}


void RenderDevice::cleanup() {
    debugAssert(initialized());

    if (debugLog) {debugLog->println("Shutting down RenderDevice.");}

    if (debugLog) {debugLog->println("Restoring gamma.");}
    setGamma(1, 1);

    if (debugLog) {debugLog->println("Freeing all VAR memory");}

    if (deleteWindow) {
        if (debugLog) {debugLog->println("Deleting window.");}
        VARArea::cleanupAllVARAreas();
        delete _window;
    }

    cleanedup = true;
}


void RenderDevice::push2D() {
    push2D(viewport());
}

void RenderDevice::push2D(const Rect2D& viewport) {
    push2D(state.framebuffer, viewport);
}

void RenderDevice::push2D(const FramebufferRef& fb) {
    const Rect2D& viewport = fb.notNull() ? fb->rect2DBounds() : Rect2D::xywh(0, 0, _window->width(), _window->height());
    push2D(fb, viewport);
}

void RenderDevice::push2D(const FramebufferRef& fb, const Rect2D& viewport) {
    pushState();

    setFramebuffer(fb);
    setDepthTest(DEPTH_ALWAYS_PASS);
    disableLighting();
    setCullFace(CULL_NONE);
    setDepthWrite(false);
    setViewport(viewport);
    setObjectToWorldMatrix(CoordinateFrame());

    // 0.375 is a float-to-int adjustment.  See: http://msdn.microsoft.com/library/default.asp?url=/library/en-us/opengl/apptips_7wqb.asp
    //setCameraToWorldMatrix(CoordinateFrame(Matrix3::identity(), Vector3(-0.375, -0.375, 0.0)));

    setCameraToWorldMatrix(CoordinateFrame());

    setProjectionMatrix(Matrix4::orthogonalProjection(viewport.x0(), viewport.x0() + viewport.width(), viewport.y0() + viewport.height(), viewport.y0(), -1, 1));
}


void RenderDevice::pop2D() {
    popState();
}

////////////////////////////////////////////////////////////////////////////////////////////////
RenderDevice::RenderState::RenderState(int width, int height, int htutc) :

    // WARNING: this must be kept in sync with the initialization code
    // in init();
    viewport(Rect2D::xywh(0, 0, width, height)),

    useClip2D(false),

    depthWrite(true),
    colorWrite(true),
    alphaWrite(true),

    depthTest(DEPTH_LEQUAL),
    alphaTest(ALPHA_ALWAYS_PASS),
    alphaReference(0.0) {

    framebuffer = NULL;

    lights.twoSidedLighting =    false;


    srcBlendFunc                = BLEND_ONE;
    dstBlendFunc                = BLEND_ZERO;
    blendEq                     = BLENDEQ_ADD;

    drawBuffer                  = BUFFER_BACK;

    stencil.stencilTest         = STENCIL_ALWAYS_PASS;
    stencil.stencilReference    = 0;
    stencil.frontStencilFail    = STENCIL_KEEP;
    stencil.frontStencilZFail   = STENCIL_KEEP;
    stencil.frontStencilZPass   = STENCIL_KEEP;
    stencil.backStencilFail     = STENCIL_KEEP;
    stencil.backStencilZFail    = STENCIL_KEEP;
    stencil.backStencilZPass    = STENCIL_KEEP;

    polygonOffset               = 0;
    lineWidth                   = 1;
    pointSize                   = 1;

    renderMode                  = RenderDevice::RENDER_SOLID;

    shininess                   = 15;
    specular                    = Color3::white() * 0.8f;

    lights.ambient              = Color4(0.25f, 0.25f, 0.25f, 1.0f);

    lights.lighting             = false;
    color                       = Color4(1,1,1,1);
    normal                      = Vector3(0,0,0);

    // Note: texture units and lights initialize themselves

    matrices.objectToWorldMatrix         = CoordinateFrame();
    matrices.cameraToWorldMatrix         = CoordinateFrame();
    matrices.cameraToWorldMatrixInverse  = CoordinateFrame();

    stencil.stencilClear        = 0;
    depthClear                  = 1;
    colorClear                  = Color4(0,0,0,1);

    shadeMode                   = SHADE_FLAT;

    vertexAndPixelShader        = NULL;
    shader                      = NULL;
    vertexProgram               = NULL;
    pixelProgram                = NULL;

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        lights.lightEnabled[i] = false;
    }

    // Set projection matrix
    double aspect;
    aspect = (double)viewport.width() / viewport.height();

    matrices.projectionMatrix = Matrix4::perspectiveProjection(-aspect, aspect, -1, 1, 0.1f, 100.0f);

    cullFace                    = CULL_BACK;

    lowDepthRange               = 0;
    highDepthRange              = 1;


    highestTextureUnitThatChanged = htutc;
}


RenderDevice::RenderState::TextureUnit::TextureUnit() : texture(NULL), LODBias(0) {
    texCoord        = Vector4(0,0,0,1);
    combineMode     = TEX_MODULATE;

    // Identity texture matrix
    memset(textureMatrix, 0, sizeof(float) * 16);
    for (int i = 0; i < 4; ++i) {
        textureMatrix[i + i * 4] = (float)1.0;
    }
}


void RenderDevice::resetState() {
    state = RenderState(width(), height());

    glClearDepth(1.0);

    glEnable(GL_NORMALIZE);

    debugAssertGLOk();
    if (GLCaps::supports_GL_EXT_stencil_two_side()) {
        glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);
    }

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    // Compute specular term correctly
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    debugAssertGLOk();

    if (debugLog) debugLog->println("Setting initial rendering state.\n");
    glDisable(GL_LIGHT0);
    debugAssertGLOk();
    {
        // WARNING: this must be kept in sync with the 
        // RenderState constructor
        state = RenderState(width(), height(), iMax(_numTextures, _numTextureCoords) - 1);

        _glViewport(state.viewport.x0(), state.viewport.y0(), state.viewport.width(), state.viewport.height());
        glDepthMask(GL_TRUE);
        glColorMask(1,1,1,0);

        if (GLCaps::supports_GL_EXT_stencil_two_side()) {
            glActiveStencilFaceEXT(GL_BACK);
        }
        for (int i = 0; i < 2; ++i) {
            glStencilMask((GLuint)~0);
            glDisable(GL_STENCIL_TEST);
            glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
            glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);
            glDisable(GL_ALPHA_TEST);
            if (GLCaps::supports_GL_EXT_stencil_two_side()) {
                glActiveStencilFaceEXT(GL_FRONT);
            }
        }

        glDepthFunc(GL_LEQUAL);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_SCISSOR_TEST);

        glDisable(GL_BLEND);
        glDisable(GL_POLYGON_OFFSET_FILL);
        glLineWidth(1);
        glPointSize(1);

        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, state.lights.ambient);
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, state.lights.twoSidedLighting);

        glDisable(GL_LIGHTING);

        glDrawBuffer(GL_BACK);

        for (int i = 0; i < MAX_LIGHTS; ++i) {
            setLight(i, NULL, true);
        }
        glColor4d(1, 1, 1, 1);
        glNormal3d(0, 0, 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        setShininess(state.shininess);
        setSpecularCoefficient(state.specular);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glShadeModel(GL_FLAT);

        glClearStencil(0);
        glClearDepth(1);
        glClearColor(0,0,0,1);
        glMatrixMode(GL_PROJECTION);
        glLoadMatrix(state.matrices.projectionMatrix);
        glMatrixMode(GL_MODELVIEW);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glDepthRange(0, 1);

        // Set up the texture units.
        if (glMultiTexCoord4fvARB != NULL) {
            for (int t = _numTextureCoords - 1; t >= 0; --t) {
                float f[] = {0,0,0,1};
                glMultiTexCoord4fvARB(GL_TEXTURE0_ARB + t, f);
            }
        } else if (_numTextureCoords > 0) {
            glTexCoord(Vector4(0,0,0,1));
        }

        if (glActiveTextureARB != NULL) {
            glActiveTextureARB(GL_TEXTURE0_ARB);
        }
    }
    debugAssertGLOk();

}


bool RenderDevice::RenderState::Lights::operator==(const Lights& other) const {
    for (int L = 0; L < MAX_LIGHTS; ++L) {
        if ((lightEnabled[L] != other.lightEnabled[L]) ||
            (lightEnabled[L] && (light[L] != other.light[L]))) {
            return false;
        }
    }

    debugAssertM(changed == false,
        "Should never enter lighting comparison "
        "when lighting has not changed.");

    return 
        (lighting == other.lighting) && 
        (ambient == other.ambient) &&
        (twoSidedLighting == other.twoSidedLighting);
}


bool RenderDevice::RenderState::Stencil::operator==(const Stencil& other) const {
    return 
        (stencilTest == other.stencilTest) &&
        (stencilReference == other.stencilReference) &&
        (stencilClear == other.stencilClear) &&
        (frontStencilFail == other.frontStencilFail) &&
        (frontStencilZFail == other.frontStencilZFail) &&
        (frontStencilZPass == other.frontStencilZPass) &&
        (backStencilFail == other.backStencilFail) &&
        (backStencilZFail == other.backStencilZFail) &&
        (backStencilZPass == other.backStencilZPass);
}


bool RenderDevice::RenderState::Matrices::operator==(const Matrices& other) const {
    return
        (objectToWorldMatrix == other.objectToWorldMatrix) &&
        (cameraToWorldMatrix == other.cameraToWorldMatrix) &&
        (projectionMatrix == other.projectionMatrix);
}


void RenderDevice::setState(
    const RenderState&          newState) {

    // The state change checks inside the individual
    // methods will (for the most part) minimize
    // the state changes so we can set all of the
    // new state explicitly.
    
	// Set framebuffer first, since it can affect the viewport
	if (state.framebuffer != newState.framebuffer) {
		setFramebuffer(newState.framebuffer);
        debugAssertGLOk();

		// Intentionally corrupt the viewport, forcing renderdevice to reset it
		state.viewport = Rect2D::xywh(-1,-1,-1,-1);
	}

    debugAssertGLOk();
    setViewport(newState.viewport);
    debugAssertGLOk();

    if (newState.useClip2D) {
        enableClip2D(newState.clip2D);
    } else {
        disableClip2D();
    }
    
    setDepthWrite(newState.depthWrite);
    setColorWrite(newState.colorWrite);
    setAlphaWrite(newState.alphaWrite);
    debugAssertGLOk();

    setDrawBuffer(newState.drawBuffer);

    setShadeMode(newState.shadeMode);
    setDepthTest(newState.depthTest);
    debugAssertGLOk();

    if (newState.stencil != state.stencil) {
        setStencilConstant(newState.stencil.stencilReference);

        debugAssertGLOk();
        setStencilTest(newState.stencil.stencilTest);

        setStencilOp(
            newState.stencil.frontStencilFail, newState.stencil.frontStencilZFail, newState.stencil.frontStencilZPass,
            newState.stencil.backStencilFail, newState.stencil.backStencilZFail, newState.stencil.backStencilZPass);

        setStencilClearValue(newState.stencil.stencilClear);
    }

    setDepthClearValue(newState.depthClear);

    setColorClearValue(newState.colorClear);

    setAlphaTest(newState.alphaTest, newState.alphaReference);

    setBlendFunc(newState.srcBlendFunc, newState.dstBlendFunc, newState.blendEq);

    setRenderMode(newState.renderMode);
    setPolygonOffset(newState.polygonOffset);
    setLineWidth(newState.lineWidth);
    setPointSize(newState.pointSize);

    setSpecularCoefficient(newState.specular);
    setShininess(newState.shininess);

    if (state.lights.changed) {
        if (newState.lights.lighting) {
            enableLighting();
        } else {
            disableLighting();
        }

        if (newState.lights.twoSidedLighting) {
            enableTwoSidedLighting();
        } else {
            disableTwoSidedLighting();
        }

        setAmbientLightColor(newState.lights.ambient);

        for (int i = 0; i < MAX_LIGHTS; ++i) {
            if (newState.lights.lightEnabled[i]) {
                setLight(i, newState.lights.light[i]);
            } else {
                setLight(i, NULL);
            }
        }
    }

    setColor(newState.color);
    setNormal(newState.normal);

    for (int u = state.highestTextureUnitThatChanged; u >= 0; --u) {
        if (newState.textureUnit[u] != state.textureUnit[u]) {

            if (u < (int)numTextures()) {
                setTexture(u, newState.textureUnit[u].texture);

                if (u < (int)numTextureUnits()) {
                    setTextureCombineMode(u, newState.textureUnit[u].combineMode);
                    setTextureMatrix(u, newState.textureUnit[u].textureMatrix);

                    setTextureLODBias(u, newState.textureUnit[u].LODBias);
                }
            }
            setTexCoord(u, newState.textureUnit[u].texCoord);            
        }
    }

    setCullFace(newState.cullFace);

    setDepthRange(newState.lowDepthRange, newState.highDepthRange);

    if (state.matrices.changed) { //(newState.matrices != state.matrices) { 
        if (newState.matrices.cameraToWorldMatrix != state.matrices.cameraToWorldMatrix) {
            setCameraToWorldMatrix(newState.matrices.cameraToWorldMatrix);
        }

        if (newState.matrices.objectToWorldMatrix != state.matrices.objectToWorldMatrix) {
            setObjectToWorldMatrix(newState.matrices.objectToWorldMatrix);
        }

        setProjectionMatrix(newState.matrices.projectionMatrix);
    }

    setVertexAndPixelShader(newState.vertexAndPixelShader);
    setShader(newState.shader);

    if (supportsVertexProgram()) {
        setVertexProgram(newState.vertexProgram);
    }

    if (supportsPixelProgram()) {
        setPixelProgram(newState.pixelProgram);
    }
    
    // Adopt the popped state's deltas relative the state that it replaced.
    state.highestTextureUnitThatChanged = newState.highestTextureUnitThatChanged;
    state.matrices.changed = newState.matrices.changed;
    state.lights.changed = newState.lights.changed;
}


void RenderDevice::enableTwoSidedLighting() {
    minStateChange();
    if (! state.lights.twoSidedLighting) {
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
        state.lights.twoSidedLighting = true;
        state.lights.changed = true;
        minGLStateChange();
    }
}


void RenderDevice::disableTwoSidedLighting() {
    minStateChange();
    if (state.lights.twoSidedLighting) {
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
        state.lights.twoSidedLighting = false;
        state.lights.changed = true;
        minGLStateChange();
    }
}


void RenderDevice::beforePrimitive() {
	if (! state.shader.isNull()) {
        debugAssert(! inShader);
        inShader = true;
		state.shader->beforePrimitive(this);
        inShader = false;
	}

    // If a Shader was bound, it will force this.  Otherwise we need to do so.
    forceVertexAndPixelShaderBind();
}


void RenderDevice::afterPrimitive() {
	if (! state.shader.isNull()) {
        debugAssert(! inShader);
		inShader = true;
        state.shader->afterPrimitive(this);
        inShader = false;
	}
}


void RenderDevice::setSpecularCoefficient(const Color3& c) {
    minStateChange();
    if (state.specular != c) {
        state.specular = c;

        static float spec[4];
        spec[0] = c[0];
        spec[1] = c[1];
        spec[2] = c[2];
        spec[3] = 1.0f;

        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
        minGLStateChange();
    }
}


void RenderDevice::setSpecularCoefficient(float s) {
    setSpecularCoefficient(s * Color3::white());
}


void RenderDevice::setShininess(float s) {
    minStateChange();
    if (state.shininess != s) {
        state.shininess = s;
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, s);
        minGLStateChange();
    }
}


void RenderDevice::setRenderMode(RenderMode m) {
    minStateChange();

    if (m == RENDER_CURRENT) {
        return;
    }

    if (state.renderMode != m) {
        minGLStateChange();
        state.renderMode = m;
        switch (m) {
        case RENDER_SOLID:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;

        case RENDER_WIREFRAME:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;

        case RENDER_POINTS:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;

        case RENDER_CURRENT:
            return;
            break;
        }
    }
}


RenderDevice::RenderMode RenderDevice::renderMode() const {
    return state.renderMode;
}


void RenderDevice::setDrawBuffer(Buffer b) {
    minStateChange();

    if (b == BUFFER_CURRENT) return;

    if (state.framebuffer.isNull()) {
        alwaysAssertM((b >= BUFFER_NONE) && (b <= BUFFER_CURRENT), 
                  "Drawing to a color buffer is only supported by application-created framebuffers!");
    } else {
        alwaysAssertM((b >= BUFFER_CURRENT) && (b <= BUFFER_COLOR15), 
                  "Visible buffers cannot be specified for application-created framebuffers!");
    }

    if (b != state.drawBuffer) {
        minGLStateChange();
        state.drawBuffer = b;
        glDrawBuffer (BufferToGL[state.drawBuffer]);
    }
}


void RenderDevice::setCullFace(CullFace f) {
    minStateChange();
    if (f != state.cullFace) {
        minGLStateChange();
        switch (f) {
        case CULL_FRONT:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            break;

        case CULL_BACK:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            break;

        case CULL_NONE:
            glDisable(GL_CULL_FACE);
            break;

        default:
            debugAssertM(false, "Fell through switch");
        }

        state.cullFace = f;
    }
}



void RenderDevice::pushState() {
    debugAssert(! inPrimitive);

    stateStack.push(state);

    // Record that that the lights and matrices are unchanged since the previous state.
    // This allows popState to restore the lighting environment efficiently.

    state.lights.changed = false;
    state.matrices.changed = false;
    state.highestTextureUnitThatChanged = -1;

    mDebugPushStateCalls += 1;
}


void RenderDevice::popState() {
    debugAssert(! inPrimitive);
    debugAssertM(stateStack.size() > 0, "More calls to RenderDevice::pushState() than RenderDevice::popState().");
    setState(stateStack.last());
    stateStack.popDiscard();
}


void RenderDevice::clear(bool clearColor, bool clearDepth, bool clearStencil) {
    debugAssert(! inPrimitive);
#   ifdef G3D_DEBUG
    {
        std::string why;
        debugAssertM(currentFramebufferComplete(why), why);
    }
#   endif
    majStateChange();
    majGLStateChange();

    GLint mask = 0;

    // TODO: do we need to enable write to clear these buffers?
    bool oldColorWrite = colorWrite();
    if (clearColor) {
        mask |= GL_COLOR_BUFFER_BIT;
        setColorWrite(true);
    }

    bool oldDepthWrite = depthWrite();
    if (clearDepth) {
        mask |= GL_DEPTH_BUFFER_BIT;
        setDepthWrite(true);
    }

    int oldMask;
    glGetIntegerv(GL_STENCIL_WRITEMASK, &oldMask);

    if (clearStencil) {
        mask |= GL_STENCIL_BUFFER_BIT;
        glStencilMask((GLuint)~0);
        minGLStateChange();
        minStateChange();
    }

    glClear(mask);
    glStencilMask(oldMask);
    minGLStateChange();
    minStateChange();
    setColorWrite(oldColorWrite);
    setDepthWrite(oldDepthWrite);
}


uint RenderDevice::numTextureUnits() const {
    return _numTextureUnits;
}


uint RenderDevice::numTextures() const {
    return _numTextures;
}


uint RenderDevice::numTextureCoords() const {
    return _numTextureCoords;
}


void RenderDevice::beginFrame() {
    if (swapGLBuffersPending) {
        swapBuffers();
    }

    mDebugNumMajorOpenGLStateChanges = 0;
    mDebugNumMinorOpenGLStateChanges = 0;
    mDebugNumMajorStateChanges = 0;
    mDebugNumMinorStateChanges = 0;
    mDebugPushStateCalls = 0;

    ++beginEndFrame;
    m_triangleCount = 0;
    debugAssertM(beginEndFrame == 1, "Mismatched calls to beginFrame/endFrame");
}


void RenderDevice::swapBuffers() {
    // Process the pending swap buffers call
    m_swapTimer.tick();
    _window->swapGLBuffers();
    m_swapTimer.tock();
    swapGLBuffersPending = false;
}


void RenderDevice::setSwapBuffersAutomatically(bool b) {
    if (b == _swapBuffersAutomatically) {
        // Setting to current state; nothing to do.
        return;
    }

    if (swapGLBuffersPending) {
        swapBuffers();
    }

    _swapBuffersAutomatically = b;
}


void RenderDevice::endFrame() {
    --beginEndFrame;
    debugAssertM(beginEndFrame == 0, "Mismatched calls to beginFrame/endFrame");

    // Schedule a swap buffer iff we are handling them automatically.
    swapGLBuffersPending = _swapBuffersAutomatically;

    debugAssertM(stateStack.size() == 0, "Missing RenderDevice::popState or RenderDevice::pop2D.");

    double now = System::time();
    double dt = now - lastTime;
    if (dt == 0) {
        dt = 0.0001;
    }

    {
        // high frame rate: A (interpolation parameter) is small
        // low frame rate: A is big
        const double A = clamp(dt, .07, 1);
    
        emwaFrameRate     = lerp(emwaFrameRate, 1 / dt, A);
        emwaTriangleRate  = lerp(emwaTriangleRate, m_triangleCount / dt, A);
        emwaTriangleCount = lerp(emwaTriangleCount, m_triangleCount, A);
    }

    if ((emwaFrameRate == inf()) || (isNaN(emwaFrameRate))) {
        emwaFrameRate = 1000000;
    } else if (emwaFrameRate < 0) {
        emwaFrameRate = 0;
    }

    if ((emwaTriangleRate == inf()) || isNaN(emwaTriangleRate)) {
        emwaTriangleRate = 1e20;
    } else if (emwaTriangleRate < 0) {
        emwaTriangleRate = 0;
    }

    if ((emwaTriangleCount == inf()) || isNaN(emwaTriangleCount)) {
        emwaTriangleRate = 1e20;
    } else if (emwaTriangleCount < 0) {
        emwaTriangleCount = 0;
    }

    lastTime = now;
}


bool RenderDevice::alphaWrite() const {
    return state.alphaWrite;
}


bool RenderDevice::depthWrite() const {
    return state.depthWrite;
}


bool RenderDevice::colorWrite() const {
    return state.colorWrite;
}


void RenderDevice::setStencilClearValue(int s) {
    debugAssert(! inPrimitive);
    minStateChange();
    if (state.stencil.stencilClear != s) {
        minGLStateChange();
        glClearStencil(s);
        state.stencil.stencilClear = s;
    }
}


void RenderDevice::setDepthClearValue(double d) {
    minStateChange();
    debugAssert(! inPrimitive);
    if (state.depthClear != d) {
        minGLStateChange();
        glClearDepth(d);
        state.depthClear = d;
    }
}


void RenderDevice::setColorClearValue(const Color4& c) {
    debugAssert(! inPrimitive);
    minStateChange();
    if (state.colorClear != c) {
        minGLStateChange();
        glClearColor(c.r, c.g, c.b, c.a);
        state.colorClear = c;
    }
}


void RenderDevice::setViewport(const Rect2D& v) {
    minStateChange();
    if (state.viewport != v) {
        // Flip to OpenGL y-axis
		float h = height();
        _glViewport(v.x0(), h - v.y1(), v.width(), v.height());
        state.viewport = v;
        minGLStateChange();
    }
}


void RenderDevice::enableClip2D(const Rect2D& clip) {
    minStateChange();
    minGLStateChange();
    state.clip2D = clip;

    int clipX0 = iFloor(clip.x0());
    int clipY0 = iFloor(clip.y0());
    int clipX1 = iCeil(clip.x1());
    int clipY1 = iCeil(clip.y1());

    glScissor(clipX0, height() - clipY1, clipX1 - clipX0, clipY1 - clipY0);

    if (clip.area() == 0) {
        // On some graphics cards a clip region that is zero without being (0,0,0,0) 
        // fails to actually clip everything.
        glScissor(0,0,0,0);
        glEnable(GL_SCISSOR_TEST);
    }

    if (! state.useClip2D) {
        glEnable(GL_SCISSOR_TEST);
        minStateChange();
        minGLStateChange();
        state.useClip2D = true;
    }
}


void RenderDevice::disableClip2D() {
    minStateChange();
    if (state.useClip2D) {
        minGLStateChange();
        glDisable(GL_SCISSOR_TEST);
        state.useClip2D = false;
    }
}


void RenderDevice::setProjectionAndCameraMatrix(const GCamera& camera) {
    Matrix4 P;
    camera.getProjectUnitMatrix(viewport(), P);
    setProjectionMatrix(P);
	setCameraToWorldMatrix(camera.coordinateFrame());
}


Rect2D RenderDevice::viewport() const {
    return state.viewport;
}


void RenderDevice::pushState(const FramebufferRef& fb) {
    pushState();

    if (fb.notNull()) {
        setFramebuffer(fb);
        setViewport(fb->rect2DBounds());
    }
}

void RenderDevice::setFramebuffer(const FramebufferRef &fbo) {
    // Check for extension
    majStateChange();
    if (fbo != state.framebuffer) {
        majGLStateChange();
        // Set Framebuffer
        if (fbo.isNull()) {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
            state.drawBuffer = BUFFER_BACK;
        } else {
            debugAssertM(GLCaps::supports_GL_EXT_framebuffer_object(), 
                "Framebuffer Object not supported!");
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo->openGLID());
            state.drawBuffer = BUFFER_COLOR0;
        }
        state.framebuffer = fbo;
    }
}


void RenderDevice::setDepthTest(DepthTest test) {
    debugAssert(! inPrimitive);

    minStateChange();

	if (test == DEPTH_CURRENT) {
		return;
	}

    if (state.depthTest != test) {
        minGLStateChange();
        if (test == DEPTH_ALWAYS_PASS) {
            glDisable(GL_DEPTH_TEST);
        } else {
            minStateChange();
            minGLStateChange();
            glEnable(GL_DEPTH_TEST);
            switch (test) {
            case DEPTH_LESS:
                glDepthFunc(GL_LESS);
                break;

            case DEPTH_LEQUAL:
                glDepthFunc(GL_LEQUAL);
                break;

            case DEPTH_GREATER:
                glDepthFunc(GL_GREATER);
                break;

            case DEPTH_GEQUAL:
                glDepthFunc(GL_GEQUAL);
                break;

            case DEPTH_EQUAL:
                glDepthFunc(GL_EQUAL);
                break;

            case DEPTH_NOTEQUAL:
                glDepthFunc(GL_NOTEQUAL);
                break;

            case DEPTH_NEVER_PASS:
                glDepthFunc(GL_NEVER);
                break;

            default:
                debugAssertM(false, "Fell through switch");
            }
        }

        state.depthTest = test;

    }
}

static GLenum toGLEnum(RenderDevice::StencilTest t) {

    switch (t) {
    case RenderDevice::STENCIL_CURRENT:
        return 0;

    case RenderDevice::STENCIL_ALWAYS_PASS:
        return GL_ALWAYS;

    case RenderDevice::STENCIL_LESS:
        return GL_LESS;

    case RenderDevice::STENCIL_LEQUAL:
        return GL_LEQUAL;

    case RenderDevice::STENCIL_GREATER:
        return GL_GREATER;

    case RenderDevice::STENCIL_GEQUAL:
        return GL_GEQUAL;

    case RenderDevice::STENCIL_EQUAL:
        return GL_EQUAL;

    case RenderDevice::STENCIL_NOTEQUAL:
        return GL_NOTEQUAL;

    case RenderDevice::STENCIL_NEVER_PASS:
        return GL_NEVER;

    default:
        debugAssertM(false, "Fell through switch");
        return GL_NONE;
    }
}


void RenderDevice::_setStencilTest(RenderDevice::StencilTest test, int reference) {

	if (test == RenderDevice::STENCIL_CURRENT) {
		return;
	}

    const GLenum t = toGLEnum(test);

    if (GLCaps::supports_GL_EXT_stencil_two_side()) {
        // NVIDIA/EXT
        glActiveStencilFaceEXT(GL_BACK);
        glStencilFunc(t, reference, 0xFFFFFFFF);
        glActiveStencilFaceEXT(GL_FRONT);
        glStencilFunc(t, reference, 0xFFFFFFFF);
        minGLStateChange(4);
    } else if (GLCaps::supports_GL_ATI_separate_stencil()) {
        // ATI
        glStencilFuncSeparateATI(t, t, reference, 0xFFFFFFFF);
        minGLStateChange(1);
    } else {
        // Default OpenGL
        glStencilFunc(t, reference, 0xFFFFFFFF);
        minGLStateChange(1);
    }
}


void RenderDevice::setStencilConstant(int reference) {
    minStateChange();

    debugAssert(! inPrimitive);
    if (state.stencil.stencilReference != reference) {

        state.stencil.stencilReference = reference;
        _setStencilTest(state.stencil.stencilTest, reference);
        minStateChange();

    }
}


void RenderDevice::setStencilTest(StencilTest test) {
    minStateChange();

	if (test == STENCIL_CURRENT) {
		return;
	}

    debugAssert(! inPrimitive);

    if (state.stencil.stencilTest != test) {
        glEnable(GL_STENCIL_TEST);

        if (test == STENCIL_ALWAYS_PASS) {

            // Can't actually disable if the stencil op is using the test as well
            // due to an OpenGL limitation.
            if ((state.stencil.frontStencilFail   == STENCIL_KEEP) &&
                (state.stencil.frontStencilZFail  == STENCIL_KEEP) &&
                (state.stencil.frontStencilZPass  == STENCIL_KEEP) &&
                (! GLCaps::supports_GL_EXT_stencil_two_side() ||
                 ((state.stencil.backStencilFail  == STENCIL_KEEP) &&
                  (state.stencil.backStencilZFail == STENCIL_KEEP) &&
                  (state.stencil.backStencilZPass == STENCIL_KEEP)))) {
                minGLStateChange();
                glDisable(GL_STENCIL_TEST);
            }

        } else {

            _setStencilTest(test, state.stencil.stencilReference);
        }

        state.stencil.stencilTest = test;

    }
}


RenderDevice::AlphaTest RenderDevice::alphaTest() const {
    return state.alphaTest;
}


double RenderDevice::alphaTestReference() const {
    return state.alphaReference;
}


void RenderDevice::setAlphaTest(AlphaTest test, double reference) {
    debugAssert(! inPrimitive);

    minStateChange();

	if (test == ALPHA_CURRENT) {
		return;
	}

    if ((state.alphaTest != test) || (state.alphaReference != reference)) {
        minGLStateChange();
        if (test == ALPHA_ALWAYS_PASS) {
            
            glDisable(GL_ALPHA_TEST);

        } else {

            glEnable(GL_ALPHA_TEST);
            switch (test) {
            case ALPHA_LESS:
                glAlphaFunc(GL_LESS, reference);
                break;

            case ALPHA_LEQUAL:
                glAlphaFunc(GL_LEQUAL, reference);
                break;

            case ALPHA_GREATER:
                glAlphaFunc(GL_GREATER, reference);
                break;

            case ALPHA_GEQUAL:
                glAlphaFunc(GL_GEQUAL, reference);
                break;

            case ALPHA_EQUAL:
                glAlphaFunc(GL_EQUAL, reference);
                break;

            case ALPHA_NOTEQUAL:
                glAlphaFunc(GL_NOTEQUAL, reference);
                break;

            case ALPHA_NEVER_PASS:
                glAlphaFunc(GL_NEVER, reference);
                break;

            default:
                debugAssertM(false, "Fell through switch");
            }
        }

        state.alphaTest = test;
        state.alphaReference = reference;
    }
}




GLint RenderDevice::toGLStencilOp(RenderDevice::StencilOp op) const {

    switch (op) {
    case RenderDevice::STENCIL_KEEP:
        return GL_KEEP;

    case RenderDevice::STENCIL_ZERO:
        return GL_ZERO;

    case RenderDevice::STENCIL_REPLACE:
        return GL_REPLACE;

    case RenderDevice::STENCIL_INVERT:
        return GL_INVERT;

    case RenderDevice::STENCIL_INCR_WRAP:
        if (GLCaps::supports_GL_EXT_stencil_wrap()) {
            return GL_INCR_WRAP_EXT;
        }
        // Intentionally fall through

    case RenderDevice::STENCIL_INCR:
        return GL_INCR;


    case RenderDevice::STENCIL_DECR_WRAP:
        if (GLCaps::supports_GL_EXT_stencil_wrap()) {
            return GL_DECR_WRAP_EXT;
        }
        // Intentionally fall through

    case RenderDevice::STENCIL_DECR:
        return GL_DECR;

    default:
        debugAssertM(false, "Fell through switch");
        return GL_KEEP;
    }
}


void RenderDevice::setShader(const ShaderRef& s) {
    majStateChange();
    
    if (s != state.shader) {
      debugAssertM(! inShader, "Cannot set the Shader from within a Shader!");
      state.shader = s;
    }
}


void RenderDevice::forceVertexAndPixelShaderBind() {
    // Only change the vertex shader if it does not match the one used
    // last time.
    if (lastVertexAndPixelShader != state.vertexAndPixelShader) {

        majGLStateChange();
        if (state.vertexAndPixelShader.isNull()) {
            // Disables the programmable pipeline
            glUseProgramObjectARB(0);
        } else {
            glUseProgramObjectARB(state.vertexAndPixelShader->glProgramObject());
        }
        debugAssertGLOk();

        lastVertexAndPixelShader = state.vertexAndPixelShader;
    }
}


void RenderDevice::setVertexAndPixelShader(const VertexAndPixelShaderRef& s) {
    majStateChange();

    if (s != state.vertexAndPixelShader) {

        state.vertexAndPixelShader = s;

        if (s.notNull()) {
            alwaysAssertM(s->ok(), s->messages());
        }

        // The actual shader will change in beforePrimitive or when the arg list is bound.
    }
}


void RenderDevice::setVertexAndPixelShader(
    const VertexAndPixelShaderRef&          s,
    const VertexAndPixelShader::ArgList&    args) {

    setVertexAndPixelShader(s);

    if (s.notNull()) {
        s->bindArgList(this, args);
    }
}


void RenderDevice::setVertexProgram(const VertexProgramRef& vp) {
    majStateChange();
    if (vp != state.vertexProgram) {
        if (state.vertexProgram != (VertexProgramRef)NULL) {
            state.vertexProgram->disable();
        }

        majGLStateChange();
        if (vp != (VertexProgramRef)NULL) {
            debugAssert(supportsVertexProgram());
            vp->bind();
        }

        state.vertexProgram = vp;
    }
}


void RenderDevice::setVertexProgram(
    const VertexProgramRef& vp,
    const GPUProgram::ArgList& args) {

    setVertexProgram(vp);
    
    vp->setArgs(this, args);
}


void RenderDevice::setPixelProgram(const PixelProgramRef& pp) {
    majStateChange();
    if (pp != state.pixelProgram) {
        if (state.pixelProgram != (PixelProgramRef)NULL) {
            state.pixelProgram->disable();
        }
        if (pp != (PixelProgramRef)NULL) {
            debugAssert(supportsPixelProgram());
            pp->bind();
        }
        majGLStateChange();
        state.pixelProgram = pp;
    }
}


void RenderDevice::setPixelProgram(
    const PixelProgramRef& pp,
    const GPUProgram::ArgList& args) {

    setPixelProgram(pp);
    
    pp->setArgs(this, args);
}


void RenderDevice::setStencilOp(
    StencilOp                       frontStencilFail,
    StencilOp                       frontZFail,
    StencilOp                       frontZPass,
    StencilOp                       backStencilFail,
    StencilOp                       backZFail,
    StencilOp                       backZPass) {

    minStateChange();

	if (frontStencilFail == STENCILOP_CURRENT) {
		frontStencilFail = state.stencil.frontStencilFail;
	}
	
	if (frontZFail == STENCILOP_CURRENT) {
		frontZFail = state.stencil.frontStencilZFail;
	}
	
	if (frontZPass == STENCILOP_CURRENT) {
		frontZPass = state.stencil.frontStencilZPass;
	}

	if (backStencilFail == STENCILOP_CURRENT) {
		backStencilFail = state.stencil.backStencilFail;
	}
	
	if (backZFail == STENCILOP_CURRENT) {
		backZFail = state.stencil.backStencilZFail;
	}
	
	if (backZPass == STENCILOP_CURRENT) {
		backZPass = state.stencil.backStencilZPass;
	}
    
	if ((frontStencilFail  != state.stencil.frontStencilFail) ||
        (frontZFail        != state.stencil.frontStencilZFail) ||
        (frontZPass        != state.stencil.frontStencilZPass) || 
        (GLCaps::supports_two_sided_stencil() && 
        ((backStencilFail  != state.stencil.backStencilFail) ||
         (backZFail        != state.stencil.backStencilZFail) ||
         (backZPass        != state.stencil.backStencilZPass)))) { 

        if (GLCaps::supports_GL_EXT_stencil_two_side()) {
            // NVIDIA

            // Set back face operation
            glActiveStencilFaceEXT(GL_BACK);
            glStencilOp(
                toGLStencilOp(backStencilFail),
                toGLStencilOp(backZFail),
                toGLStencilOp(backZPass));
            
            // Set front face operation
            glActiveStencilFaceEXT(GL_FRONT);
            glStencilOp(
                toGLStencilOp(frontStencilFail),
                toGLStencilOp(frontZFail),
                toGLStencilOp(frontZPass));

            minGLStateChange(4);

        } else if (GLCaps::supports_GL_ATI_separate_stencil()) {
            // ATI
            minGLStateChange(2);
            glStencilOpSeparateATI(GL_FRONT, 
                toGLStencilOp(frontStencilFail),
                toGLStencilOp(frontZFail),
                toGLStencilOp(frontZPass));

            glStencilOpSeparateATI(GL_BACK, 
                toGLStencilOp(backStencilFail),
                toGLStencilOp(backZFail),
                toGLStencilOp(backZPass));

        } else {
            // Generic OpenGL

            // Set front face operation
            glStencilOp(
                toGLStencilOp(frontStencilFail),
                toGLStencilOp(frontZFail),
                toGLStencilOp(frontZPass));

            minGLStateChange(1);
        }


        // Need to manage the mask as well

        if ((frontStencilFail  == STENCIL_KEEP) &&
            (frontZPass        == STENCIL_KEEP) && 
            (frontZFail        == STENCIL_KEEP) &&
            (! GLCaps::supports_two_sided_stencil() ||
            ((backStencilFail  == STENCIL_KEEP) &&
             (backZPass        == STENCIL_KEEP) &&
             (backZFail        == STENCIL_KEEP)))) {

            // Turn off writing.  May need to turn off the stencil test.
            if (state.stencil.stencilTest == STENCIL_ALWAYS_PASS) {
                // Test doesn't need to be on
                glDisable(GL_STENCIL_TEST);
            }

        } else {

            debugAssertM(glGetInteger(GL_STENCIL_BITS) > 0,
                "Allocate stencil bits from RenderDevice::init before using the stencil buffer.");

            // Turn on writing.  We also need to turn on the
            // stencil test in this case.

            if (state.stencil.stencilTest == STENCIL_ALWAYS_PASS) {
                // Test is not already on
                glEnable(GL_STENCIL_TEST);
                _setStencilTest(state.stencil.stencilTest, state.stencil.stencilReference);
            }
        }

        state.stencil.frontStencilFail  = frontStencilFail;
        state.stencil.frontStencilZFail = frontZFail;
        state.stencil.frontStencilZPass = frontZPass;
        state.stencil.backStencilFail   = backStencilFail;
        state.stencil.backStencilZFail  = backZFail;
        state.stencil.backStencilZPass  = backZPass;
    }
}


void RenderDevice::setStencilOp(
    StencilOp           fail,
    StencilOp           zfail,
    StencilOp           zpass) {
    debugAssert(! inPrimitive);

    setStencilOp(fail, zfail, zpass, fail, zfail, zpass);
}


static GLenum toGLBlendEq(RenderDevice::BlendEq e) {
    switch (e) {
    case RenderDevice::BLENDEQ_MIN:
        debugAssert(GLCaps::supports("GL_EXT_blend_minmax"));
        return GL_MIN;

    case RenderDevice::BLENDEQ_MAX:
        debugAssert(GLCaps::supports("GL_EXT_blend_minmax"));
        return GL_MAX;

    case RenderDevice::BLENDEQ_ADD:
        return GL_FUNC_ADD;

    case RenderDevice::BLENDEQ_SUBTRACT:
        debugAssert(GLCaps::supports("GL_EXT_blend_subtract"));
        return GL_FUNC_SUBTRACT;

    case RenderDevice::BLENDEQ_REVERSE_SUBTRACT:
        debugAssert(GLCaps::supports("GL_EXT_blend_subtract"));
        return GL_FUNC_REVERSE_SUBTRACT;

    default:
        debugAssertM(false, "Fell through switch");
        return GL_ZERO;
    }
}


void RenderDevice::setBlendFunc(
    BlendFunc src,
    BlendFunc dst,
    BlendEq   eq) {
    debugAssert(! inPrimitive);

    minStateChange();
	if (src == BLEND_CURRENT) {
		src = state.srcBlendFunc;
	}

	if (dst == BLEND_CURRENT) {
		dst = state.dstBlendFunc;
	}

    if (eq == BLENDEQ_CURRENT) {
        eq = state.blendEq;
    }

    if ((state.dstBlendFunc != dst) ||
        (state.srcBlendFunc != src) ||
        (state.blendEq != eq)) {

        minGLStateChange();

        if ((dst == BLEND_ZERO) && (src == BLEND_ONE) && ((eq == BLENDEQ_ADD) || (eq == BLENDEQ_SUBTRACT))) {
            glDisable(GL_BLEND);
        } else {
            glEnable(GL_BLEND);
            glBlendFunc(toGLBlendFunc(src), toGLBlendFunc(dst));

            debugAssert(eq == BLENDEQ_ADD ||
                GLCaps::supports("GL_EXT_blend_minmax") ||
                GLCaps::supports("GL_EXT_blend_subtract"));

            static bool supportsBlendEq = GLCaps::supports("GL_EXT_blend_minmax");

            if (supportsBlendEq && (glBlendEquationEXT != 0)) {
                glBlendEquationEXT(toGLBlendEq(eq));
            }
        }
        state.dstBlendFunc = dst;
        state.srcBlendFunc = src;
        state.blendEq = eq;
    }
}


void RenderDevice::setLineWidth(
    double               width) {
    debugAssert(! inPrimitive);
    minStateChange();
    if (state.lineWidth != width) {
        glLineWidth(width);
        minGLStateChange();
        state.lineWidth = width;
    }
}


void RenderDevice::setPointSize(
    double               width) {
    debugAssert(! inPrimitive);
    minStateChange();
    if (state.pointSize != width) {
        glPointSize(width);
        minGLStateChange();
        state.pointSize = width;
    }
}


void RenderDevice::setAmbientLightColor(
    const Color4&        color) {
    debugAssert(! inPrimitive);

    minStateChange();
    if (color != state.lights.ambient) {
        state.lights.changed = true;
        minGLStateChange();
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, color);
        state.lights.ambient = color;
    }
}


void RenderDevice::setAmbientLightColor(
    const Color3&        color) {
    setAmbientLightColor(Color4(color, 1.0));
}


void RenderDevice::enableLighting() {
    debugAssert(! inPrimitive);
    minStateChange();
    if (! state.lights.lighting) {
        glEnable(GL_LIGHTING);
        minGLStateChange();
        state.lights.lighting = true;
        state.lights.changed = true;
    }
}


void RenderDevice::disableLighting() {
    debugAssert(! inPrimitive);
    minStateChange();
    if (state.lights.lighting) {
        glDisable(GL_LIGHTING);
        minGLStateChange();
        state.lights.lighting = false;
        state.lights.changed = true;
    }
}


void RenderDevice::setObjectToWorldMatrix(
    const CoordinateFrame& cFrame) {
    
    minStateChange();
    debugAssert(! inPrimitive);

    // No test to see if it is already equal; this is called frequently and is
    // usually different.
    state.matrices.changed = true;
    state.matrices.objectToWorldMatrix = cFrame;
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrix(state.matrices.cameraToWorldMatrixInverse * state.matrices.objectToWorldMatrix);
    minGLStateChange();
}


const CoordinateFrame& RenderDevice::getObjectToWorldMatrix() const {
    return state.matrices.objectToWorldMatrix;
}


void RenderDevice::setCameraToWorldMatrix(
    const CoordinateFrame& cFrame) {

    debugAssert(! inPrimitive);
    majStateChange();
    majGLStateChange();
    
    state.matrices.changed = true;
    state.matrices.cameraToWorldMatrix = cFrame;
    state.matrices.cameraToWorldMatrixInverse = cFrame.inverse();

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrix(state.matrices.cameraToWorldMatrixInverse * state.matrices.objectToWorldMatrix);

    // Reload lights since the camera matrix changed.  We must do this even for lights
    // that are not enabled since those lights will not be re-set at the GL level if they
    // are enabled without being otherwise changed.  Of course, we must follow up by disabling
    // those lights again.
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        bool wasEnabled = state.lights.lightEnabled[i];
        setLight(i, &state.lights.light[i], true);
        if (! wasEnabled) {
            setLight(i, NULL);
        }
    }
}


const CoordinateFrame& RenderDevice::getCameraToWorldMatrix() const {
    return state.matrices.cameraToWorldMatrix;
}


Matrix4 RenderDevice::getProjectionMatrix() const {
    return state.matrices.projectionMatrix;
}


CoordinateFrame RenderDevice::getModelViewMatrix() const {
    return state.matrices.cameraToWorldMatrixInverse * getObjectToWorldMatrix();
}


Matrix4 RenderDevice::getModelViewProjectionMatrix() const {
    return getProjectionMatrix() * getModelViewMatrix();
}


void RenderDevice::setProjectionMatrix(const Matrix4& P) {
    minStateChange();
    if (state.matrices.projectionMatrix != P) {
        state.matrices.projectionMatrix = P;
        state.matrices.changed = true;
        glMatrixMode(GL_PROJECTION);
        glLoadMatrix(P);
        glMatrixMode(GL_MODELVIEW);
        minGLStateChange();
    }
}


void RenderDevice::forceSetTextureMatrix(int unit, const double* m) {
    float f[16];
    for (int i = 0; i < 16; ++i) {
        f[i] = m[i];
    }

    forceSetTextureMatrix(unit, f);
}


void RenderDevice::forceSetTextureMatrix(int unit, const float* m) {
    minStateChange();
    minGLStateChange();

    state.touchedTextureUnit(unit);
    memcpy(state.textureUnit[unit].textureMatrix, m, sizeof(float)*16);
    if (GLCaps::supports_GL_ARB_multitexture()) {
        glActiveTextureARB(GL_TEXTURE0_ARB + unit);
    }

    // Transpose the texture matrix
    float tt[16];
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            tt[i + j * 4] = m[j + i * 4];
        }
    }
    glMatrixMode(GL_TEXTURE);
    glLoadMatrixf(tt);

    Texture::Ref texture = state.textureUnit[unit].texture;

    // invert y
    if ((texture != NULL) && texture->invertY) {

        float ymax = 1.0;
    
        if (texture->dimension() == Texture::DIM_2D_RECT) {
            ymax = texture->texelHeight();
        }

        float m[16] = 
        { 1,  0,  0,  0,
          0, -1,  0,  0,
          0,  0,  1,  0,
          0,  ymax,  0,  1};

        glMultMatrixf(m);
    }

}


Matrix4 RenderDevice::getTextureMatrix(uint unit) {
    debugAssertM((int)unit < _numTextureUnits,
        format("Attempted to access texture unit %d on a device with %d units.",
        unit, _numTextureUnits));

    const float* M = state.textureUnit[unit].textureMatrix;

    return Matrix4(M[0], M[4], M[8],  M[12],
                   M[1], M[5], M[9],  M[13],
                   M[2], M[6], M[10], M[14],
                   M[3], M[7], M[11], M[15]);
}



void RenderDevice::setTextureMatrix(
    uint                 unit,
    const Matrix4&	     m) {

    float f[16];
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            f[r * 4 + c] = m[r][c];
        }
    }
    
    setTextureMatrix(unit, f);
}


void RenderDevice::setTextureMatrix(
    uint                 unit,
    const double*        m) {

    debugAssert(! inPrimitive);
    debugAssertM((int)unit < _numTextureUnits,
        format("Attempted to access texture unit %d on a device with %d units.",
        unit, _numTextureUnits));

    forceSetTextureMatrix(unit, m);
}


void RenderDevice::setTextureMatrix(
    uint                 unit,
    const float*        m) {

    debugAssert(! inPrimitive);
    debugAssertM((int)unit < _numTextureUnits,
        format("Attempted to access texture unit %d on a device with %d units.",
        unit, _numTextureUnits));

    if (memcmp(m, state.textureUnit[unit].textureMatrix, sizeof(float)*16)) {
        forceSetTextureMatrix(unit, m);
    }
}


void RenderDevice::setTextureMatrix(
    uint                    unit,
    const CoordinateFrame&  c) {

    float m[16] = 
    {c.rotation[0][0], c.rotation[0][1], c.rotation[0][2], c.translation.x,
     c.rotation[1][0], c.rotation[1][1], c.rotation[1][2], c.translation.y,
     c.rotation[2][0], c.rotation[2][1], c.rotation[2][2], c.translation.z,
                    0,                0,                0,               1};

    setTextureMatrix(unit, m);
}


void RenderDevice::setTextureLODBias(
    uint                    unit,
    float                   bias) {

    minStateChange();
    if (state.textureUnit[unit].LODBias != bias) {
        state.touchedTextureUnit(unit);

        if (GLCaps::supports_GL_ARB_multitexture()) {
            glActiveTextureARB(GL_TEXTURE0_ARB + unit);
        }

        state.textureUnit[unit].LODBias = bias;

        minGLStateChange();
        glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, bias);
    }
}


void RenderDevice::setTextureCombineMode(
    uint                    unit,
    const CombineMode       mode) {

    minStateChange();
	if (mode == TEX_CURRENT) {
		return;
	}

    debugAssertM((int)unit < _numTextureUnits,
        format("Attempted to access texture unit %d on a device with %d units.",
        unit, _numTextureUnits));

    if ((state.textureUnit[unit].combineMode != mode)) {
        minGLStateChange();
        state.touchedTextureUnit(unit);

        state.textureUnit[unit].combineMode = mode;

        if (GLCaps::supports_GL_ARB_multitexture()) {
            glActiveTextureARB(GL_TEXTURE0_ARB + unit);
        }

        static const bool hasAdd = GLCaps::supports("GL_EXT_texture_env_add");
        static const bool hasCombine = GLCaps::supports("GL_ARB_texture_env_combine");
        static const bool hasDot3 = GLCaps::supports("GL_ARB_texture_env_dot3");

        switch (mode) {
        case TEX_REPLACE:
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            break;

        case TEX_BLEND:
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
            break;

        case TEX_MODULATE:
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            break;

        case TEX_INTERPOLATE:
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
            break;

        case TEX_ADD:
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, hasCombine ? GL_ADD : GL_BLEND);
            break;

        case TEX_SUBTRACT:
            // (add and subtract are in the same extension)
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, hasAdd ? GL_SUBTRACT_ARB : GL_BLEND);
            break;

        case TEX_ADD_SIGNED:
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, hasAdd ? GL_ADD_SIGNED_ARB : GL_BLEND);
            break;
            
        case TEX_DOT3_RGB:
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, hasDot3 ? GL_DOT3_RGB_ARB : GL_BLEND);
            break;
             
        case TEX_DOT3_RGBA:
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, hasDot3 ? GL_DOT3_RGBA_ARB : GL_BLEND);
            break;

        default:
            debugAssertM(false, "Unrecognized texture combine mode");
        }
    }
}


void RenderDevice::resetTextureUnit(
    uint                    unit) {
    debugAssertM((int)unit < _numTextureUnits,
        format("Attempted to access texture unit %d on a device with %d units.",
        unit, _numTextureUnits));

    RenderState newState(state);
    state.textureUnit[unit] = RenderState::TextureUnit();
    state.touchedTextureUnit(unit);
    setState(newState);
}


void RenderDevice::setPolygonOffset(
    double              offset) {
    debugAssert(! inPrimitive);

    minStateChange();

    if (state.polygonOffset != offset) {
        minGLStateChange();
        if (offset != 0) {
            glEnable(GL_POLYGON_OFFSET_FILL);
            glEnable(GL_POLYGON_OFFSET_LINE);
            glEnable(GL_POLYGON_OFFSET_POINT);
            glPolygonOffset(offset, sign(offset));
        } else {
            glDisable(GL_POLYGON_OFFSET_POINT);
            glDisable(GL_POLYGON_OFFSET_FILL);
            glDisable(GL_POLYGON_OFFSET_LINE);
        }
        state.polygonOffset = offset;
    }
}


void RenderDevice::setNormal(const Vector3& normal) {
    state.normal = normal;
    glNormal3fv(normal);
    minStateChange();
    minGLStateChange();
}


void RenderDevice::setTexCoord(uint unit, const Vector4& texCoord) {
    debugAssertM((int)unit < _numTextureCoords,
        format("Attempted to access texture coordinate %d on a device with %d coordinates.",
        unit, _numTextureCoords));

    state.textureUnit[unit].texCoord = texCoord;
    if (GLCaps::supports_GL_ARB_multitexture()) {
        glMultiTexCoord(GL_TEXTURE0_ARB + unit, texCoord);
    } else {
        debugAssertM(unit == 0, "This machine has only one texture unit");
        glTexCoord(texCoord);
    }
    state.touchedTextureUnit(unit);
    minStateChange();
    minGLStateChange();
}


void RenderDevice::setTexCoord(uint unit, const Vector3& texCoord) {
    setTexCoord(unit, Vector4(texCoord, 1));
}


void RenderDevice::setTexCoord(uint unit, const Vector3int16& texCoord) {
    setTexCoord(unit, Vector4(texCoord.x, texCoord.y, texCoord.z, 1));
}


void RenderDevice::setTexCoord(uint unit, const Vector2& texCoord) {
    setTexCoord(unit, Vector4(texCoord, 0, 1));
}


void RenderDevice::setTexCoord(uint unit, const Vector2int16& texCoord) {
    setTexCoord(unit, Vector4(texCoord.x, texCoord.y, 0, 1));
}


void RenderDevice::setTexCoord(uint unit, double texCoord) {
    setTexCoord(unit, Vector4(texCoord, 0, 0, 1));
}


void RenderDevice::sendVertex(const Vector2& vertex) {
    debugAssertM(inPrimitive, "Can only be called inside beginPrimitive()...endPrimitive()");
    glVertex2fv(vertex);
    ++currentPrimitiveVertexCount;
}


void RenderDevice::sendVertex(const Vector3& vertex) {
    debugAssertM(inPrimitive, "Can only be called inside beginPrimitive()...endPrimitive()");
    glVertex3fv(vertex);
    ++currentPrimitiveVertexCount;
}


void RenderDevice::sendVertex(const Vector4& vertex) {
    debugAssertM(inPrimitive, "Can only be called inside beginPrimitive()...endPrimitive()");
    glVertex4fv(vertex);
    ++currentPrimitiveVertexCount;
}


void RenderDevice::beginPrimitive(Primitive p) {
    debugAssertM(! inPrimitive, "Already inside a primitive");
    debugAssertM( currentFramebufferComplete(), "Bound Framebuffer Incomplete!");

    beforePrimitive();
    
    inPrimitive = true;
    currentPrimitiveVertexCount = 0;
    currentPrimitive = p;

    debugAssertGLOk();

    glBegin(primitiveToGLenum(p));
}


void RenderDevice::endPrimitive() {
    debugAssertM(inPrimitive, "Call to endPrimitive() without matching beginPrimitive()");

    mDebugNumMinorStateChanges += currentPrimitiveVertexCount;
    mDebugNumMinorOpenGLStateChanges += currentPrimitiveVertexCount;
	countTriangles(currentPrimitive, currentPrimitiveVertexCount);

    glEnd();
    inPrimitive = false;

    afterPrimitive();
}


void RenderDevice::countTriangles(RenderDevice::Primitive primitive, int numVertices) {
	switch (primitive) {
    case LINES:
        m_triangleCount += (numVertices / 2);
        break;

    case LINE_STRIP:
        m_triangleCount += (numVertices - 1);
        break;

    case TRIANGLES:
        m_triangleCount += (numVertices / 3);
        break;

    case TRIANGLE_STRIP:
    case TRIANGLE_FAN:
        m_triangleCount += (numVertices - 2);
        break;

    case QUADS:
        m_triangleCount += ((numVertices / 4) * 2);
        break;

    case QUAD_STRIP:
        m_triangleCount += (((numVertices / 2) - 1) * 2);
        break;

    case POINTS:
        m_triangleCount += numVertices;
        break;
    }
}


void RenderDevice::setTexture(
    uint                unit,
    Texture::Ref          texture) {

    // NVIDIA cards have more textures than texture units.
    // "fixedFunction" textures have an associated unit 
    // and must be glEnabled.  Programmable units *cannot*
    // be enabled.
    bool fixedFunction = ((int)unit < _numTextureUnits);

    debugAssertM(! inPrimitive, 
                 "Can't change textures while rendering a primitive.");

    debugAssertM((int)unit < _numTextures,
        format("Attempted to access texture %d"
               " on a device with %d textures.",
               unit, _numTextures));

    Texture::Ref oldTexture = state.textureUnit[unit].texture;
    majStateChange();

    if (oldTexture == texture) {
        return;
    }

    majGLStateChange();

    state.textureUnit[unit].texture = texture;
    state.touchedTextureUnit(unit);

    if (GLCaps::supports_GL_ARB_multitexture()) {
        glActiveTextureARB(GL_TEXTURE0_ARB + unit);
    }

    // Turn off whatever was on previously if this is a fixed function unit
    if (fixedFunction) {
        glDisableAllTextures();
    }

    if (texture.notNull()) {
        GLint id = texture->openGLID();
        GLint u = texture->openGLTextureTarget();

        if ((GLint)currentlyBoundTexture[unit] != id) {
            glBindTexture(u, id);
            currentlyBoundTexture[unit] = id;
        }

        if (fixedFunction) {
            glEnable(u);
        }
    } else {
        // Disabled texture unit
        currentlyBoundTexture[unit] = 0;
    }

    // Force a reload of the texture matrix if invertY != old invertY.
    // This will take care of flipping the texture when necessary.
    if (oldTexture.isNull() ||
        texture.isNull() ||
        (oldTexture->invertY != texture->invertY)) {

        if (fixedFunction) {
            // We can only set the matrix for some units
            forceSetTextureMatrix(unit, state.textureUnit[unit].textureMatrix);
        }
    }
}


double RenderDevice::getDepthBufferValue(
    int                     x,
    int                     y) const {

    GLfloat depth = 0;
    debugAssertGLOk();

    if (state.framebuffer.notNull()) {
        debugAssertM(state.framebuffer->has(FrameBuffer::DEPTH_ATTACHMENT),
            "No depth attachment");
    }

    glReadPixels(x,
	         (height() - 1) - y,
                 1, 1,
                 GL_DEPTH_COMPONENT,
	         GL_FLOAT,
	         &depth);

    debugAssertM(glGetError() != GL_INVALID_OPERATION, 
        "getDepthBufferValue failed, probably because you did not allocate a depth buffer.");

    return depth;
}


/**
 Returns the current date as a string in the form YYYY-MM-DD
 */
static std::string currentDateString() {
    time_t t1;
    time(&t1);
    tm* t = localtime(&t1);
    return format("%d-%02d-%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday); 
}


void RenderDevice::screenshotPic(GImage& dest, bool useBackBuffer, bool getAlpha) const {
    if (useBackBuffer) {
        glReadBuffer(GL_BACK);
    } else {
        glReadBuffer(GL_FRONT);
    }
    
    int ch = getAlpha ? 4 : 3;

    if ((dest.channels != ch) ||
        (dest.width != width()) ||
        (dest.height != height())) {
        // Only resize if the current size is not correct
        dest.resize(width(), height(), ch);
    }

    glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width(), height(), 
        getAlpha ? GL_RGBA : GL_RGB,
        GL_UNSIGNED_BYTE, dest.byte());    

    glPopClientAttrib();

    // Flip right side up
    if (getAlpha) {
	GImage::flipRGBAVertical(dest.byte(), dest.byte(), width(), height());
    } else {
        GImage::flipRGBVertical(dest.byte(), dest.byte(), width(), height());
    }

    // Restore the read buffer to the back
    glReadBuffer(GL_BACK);

    int i;
    double s = 1.0;

    if (s != 1.0) {
        // Adjust the coloring for gamma correction
        // Lookup table for mapping v -> v * lightSaturation;
        uint8 L[255];
        uint8 *data = dest.byte();
        
        for (i = 255; i >= 0; --i) {
            L[i] = iMin(255, iRound((double)i * s));
        }

        // skip the alpha channel if there is one
        int sz = dest.width * dest.height * dest.channels;
        for (i = 0; i < sz; i += ch) {
            for (int c = 0; c < 3; ++c) {
                data[i + c] = L[data[i + c]];
            }
        }
    }

}


std::string RenderDevice::screenshot(const std::string& filepath) const {
    GImage screen;
    int i;

    screenshotPic(screen);

    // Save the file
    std::string basename = currentDateString();
    std::string filename;
    i = 0;
    do {
        filename = filepath + basename + "_" + format("%03d", i) + ".jpg";
        ++i;
    } while (fileExists(filename));   

    screen.save(filename);

    return filename;
}


double RenderDevice::frameRate() const {
    return emwaFrameRate;
}


double RenderDevice::triangleRate() const {
    return emwaTriangleRate;
}


double RenderDevice::trianglesPerFrame() const {
    return emwaTriangleCount;
}


void RenderDevice::beginIndexedPrimitives() {
	debugAssert(! inPrimitive);
	debugAssert(! inIndexedPrimitive);

    // TODO: can we avoid this push?
	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT); 

	inIndexedPrimitive = true;
}


void RenderDevice::endIndexedPrimitives() {
	debugAssert(! inPrimitive);
	debugAssert(inIndexedPrimitive);


  if (GLCaps::supports_GL_ARB_vertex_buffer_object()) {
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
  }


	glPopClientAttrib();
	inIndexedPrimitive = false;
    currentVARArea = NULL;
}


void RenderDevice::setVARAreaFromVAR(const class VAR& v) {
	debugAssert(inIndexedPrimitive);
	debugAssert(! inPrimitive);
    alwaysAssertM(currentVARArea.isNull() || (v.area == currentVARArea), 
        "All vertex arrays used within a single begin/endIndexedPrimitive"
        " block must share the same VARArea.");

    majStateChange();

    if (v.area != currentVARArea) {
        currentVARArea = const_cast<VAR&>(v).area;

        if (VARArea::mode == VARArea::VBO_MEMORY) {
            // Bind the buffer (for MAIN_MEMORY, we need do nothing)
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, currentVARArea->glbuffer);
            majGLStateChange();
        }
    }

}


void RenderDevice::setVertexArray(const class VAR& v) {
    setVARAreaFromVAR(v);
	v.vertexPointer();
}


void RenderDevice::setVertexAttribArray(unsigned int attribNum, const class VAR& v, bool normalize) {
    setVARAreaFromVAR(v);
	v.vertexAttribPointer(attribNum, normalize);
}


void RenderDevice::setNormalArray(const class VAR& v) {
    setVARAreaFromVAR(v);
	v.normalPointer();
}


void RenderDevice::setColorArray(const class VAR& v) {
    setVARAreaFromVAR(v);
	v.colorPointer();
}


void RenderDevice::setTexCoordArray(unsigned int unit, const class VAR& v) {
    setVARAreaFromVAR(v);
	v.texCoordPointer(unit);
}


MilestoneRef RenderDevice::createMilestone(const std::string& name) {
    return new Milestone(name);
}


void RenderDevice::setMilestone(const MilestoneRef& m) {
    minStateChange();
    minGLStateChange();
    m->set();
}


void RenderDevice::waitForMilestone(const MilestoneRef& m) {
    minStateChange();
    minGLStateChange();
    m->wait();
}


void RenderDevice::setLight(int i, const GLight& light) {
    
    setLight(i, &light, false);
}


void RenderDevice::setLight(int i, void* x) {
    debugAssert(x == NULL); (void)x;
    setLight(i, NULL, false);
}


void RenderDevice::setLight(int i, const GLight* _light, bool force) {
    debugAssert(i >= 0);
    debugAssert(i < MAX_LIGHTS);
    int gi = GL_LIGHT0 + i;

    const GLight& light = *_light;


    minStateChange();

    if (_light == NULL) {

        if (state.lights.lightEnabled[i] || force) {
            // Don't bother copying this light over
            state.lights.lightEnabled[i] = false;
            state.lights.changed = true;
            glDisable(gi);
        }

        minGLStateChange();
    
    } else {

        for (int j = 0; j < 3; ++j) {
            debugAssert(light.attenuation[j] >= 0);
        }

    
        if (! state.lights.lightEnabled[i] || force) {
            glEnable(gi);
            state.lights.lightEnabled[i] = true;
            state.lights.changed = true;
        }

    
        if ((state.lights.light[i] != light) || force) {
            state.lights.changed = true;
            state.lights.light[i] = light;

            minGLStateChange();

            Color4 zero(0, 0, 0, 1);
            Color4 brightness(light.color, 1);

            int mm = glGetInteger(GL_MATRIX_MODE);
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
                glLoadIdentity();
                glLoadMatrix(state.matrices.cameraToWorldMatrixInverse);
                glLightfv(gi, GL_POSITION,              light.position);
                glLightfv(gi, GL_SPOT_DIRECTION,        light.spotDirection);
                glLightf (gi, GL_SPOT_CUTOFF,           light.spotCutoff);
                glLightfv(gi, GL_AMBIENT,               zero);
                if (light.diffuse) {
                    glLightfv(gi, GL_DIFFUSE,               brightness);
                } else {
                    glLightfv(gi, GL_DIFFUSE,               zero);
                }
                if (light.specular) {
                    glLightfv(gi, GL_SPECULAR,              brightness);
                } else {
                    glLightfv(gi, GL_SPECULAR,              zero);
                }
                glLightf (gi, GL_CONSTANT_ATTENUATION,  light.attenuation[0]);
                glLightf (gi, GL_LINEAR_ATTENUATION,    light.attenuation[1]);
                glLightf (gi, GL_QUADRATIC_ATTENUATION, light.attenuation[2]);
            glPopMatrix();
            glMatrixMode(mm);
        }    
    }
    
}


void RenderDevice::configureShadowMap(
    uint                unit,
    const Matrix4&      lightMVP,
    const Texture::Ref&   shadowMap) {

    minStateChange();
    minGLStateChange();

    // http://www.nvidia.com/dev_content/nvopenglspecs/GL_ARB_shadow.txt

    debugAssertM(shadowMap->format()->openGLBaseFormat == GL_DEPTH_COMPONENT,
        "Can only configure shadow maps from depth textures");

    debugAssertM(shadowMap->settings().depthReadMode != Texture::DEPTH_NORMAL,
        "Shadow maps must be configured for either Texture::DEPTH_LEQUAL"
        " or Texture::DEPTH_GEQUAL comparisions.");

    debugAssertM(GLCaps::supports_GL_ARB_shadow(),
        "The device does not support shadow maps");


    // Texture coordinate generation will use the current MV matrix
    // to determine OpenGL "eye" space.  We want OpenGL "eye" space to
    // be our world space, so set the object to world matrix to be the
    // identity.
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrix(state.matrices.cameraToWorldMatrixInverse);

    setTexture(unit, shadowMap);
    
    if (GLCaps::supports_GL_ARB_multitexture()) {
        glActiveTextureARB(GL_TEXTURE0_ARB + unit);
    }

    static const Matrix4 bias(
        0.5f, 0.0f, 0.0f, 0.5f,
        0.0f, 0.5f, 0.0f, 0.5f,
        0.0f, 0.0f, 0.5f, 0.5f - .000001f,
        0.0f, 0.0f, 0.0f, 1.0f);
    
    Matrix4 textureMatrix = glGetMatrix(GL_TEXTURE_MATRIX);

	Matrix4 textureProjectionMatrix2D =
        textureMatrix * bias * lightMVP;

	// Set up tex coord generation - all 4 coordinates required
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_S, GL_EYE_PLANE, textureProjectionMatrix2D[0]);
	glEnable(GL_TEXTURE_GEN_S);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_T, GL_EYE_PLANE, textureProjectionMatrix2D[1]);
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_R, GL_EYE_PLANE, textureProjectionMatrix2D[2]);
	glEnable(GL_TEXTURE_GEN_R);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_Q, GL_EYE_PLANE, textureProjectionMatrix2D[3]);
	glEnable(GL_TEXTURE_GEN_Q);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}


void RenderDevice::configureReflectionMap(
    uint                textureUnit,
    Texture::Ref          reflectionTexture) {

    debugAssert(! GLCaps::hasBug_normalMapTexGen());
    debugAssert(reflectionTexture->dimension() == Texture::DIM_CUBE_MAP);

    // Texture coordinates will be generated in object space.
    // Set the texture matrix to transform them into camera space.
    CoordinateFrame cframe = getCameraToWorldMatrix();

    // The environment map assumes we are always in the center,
    // so zero the translation.
    cframe.translation = Vector3::zero();

    // The environment map is in world space.  The reflection vector
    // will automatically be multiplied by the object->camera space 
    // transformation by hardware (just like any other vector) so we 
    // take it back from camera space to world space for the correct
    // vector.
    setTexture(textureUnit, reflectionTexture);

    setTextureMatrix(textureUnit, cframe);

    minStateChange();
    minGLStateChange();
    glActiveTextureARB(GL_TEXTURE0_ARB + textureUnit);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);
}


void RenderDevice::sendSequentialIndices(RenderDevice::Primitive primitive, int numVertices) {

    beforePrimitive();

    glDrawArrays(primitiveToGLenum(primitive), 0, numVertices);
    // Mark all active arrays as busy.
    setVARAreaMilestone();

	countTriangles(primitive, numVertices);
    afterPrimitive();

    minStateChange();
    minGLStateChange();
}


void RenderDevice::internalSendIndices(
    RenderDevice::Primitive primitive,
    size_t                  indexSize, 
    int                     numIndices, 
    const void*             index) {

    beforePrimitive();

	GLenum i;

	switch (indexSize) {
	case sizeof(uint32):
		i = GL_UNSIGNED_INT;
		break;

	case sizeof(uint16):
		i = GL_UNSIGNED_SHORT;
		break;

	case sizeof(uint8):
		i = GL_UNSIGNED_BYTE;
		break;

	default:
		debugAssertM(false, "Indices must be either 8, 16, or 32-bytes each.");
        i = 0;
	}

    GLenum p = primitiveToGLenum(primitive);

	glDrawElements(p, numIndices, i, index);

    afterPrimitive();
}


bool RenderDevice::supportsTwoSidedStencil() const {
    return GLCaps::supports_GL_EXT_stencil_two_side();
}


bool RenderDevice::supportsTextureRectangle() const {
    return GLCaps::supports_GL_EXT_texture_rectangle();
}


bool RenderDevice::supportsVertexProgramNV2() const {
    return GLCaps::supports_GL_NV_vertex_program2();
}


bool RenderDevice::supportsVertexBufferObject() const { 
    return GLCaps::supports_GL_ARB_vertex_buffer_object();
}


uint32 RenderDevice::debugNumMajorOpenGLStateChanges() const {
    return mDebugNumMajorOpenGLStateChanges;
}


uint32 RenderDevice::debugNumPushStateCalls() const {
    return mDebugPushStateCalls;
}


uint32 RenderDevice::debugNumMinorOpenGLStateChanges() const {
    return mDebugNumMinorOpenGLStateChanges;
}


uint32 RenderDevice::debugNumMajorStateChanges() const {
    return mDebugNumMajorStateChanges;
}


uint32 RenderDevice::debugNumMinorStateChanges() const {
    return mDebugNumMinorStateChanges;
}

std::string RenderDevice::dummyString;
bool RenderDevice::checkFramebuffer(std::string& whyNot) const {
    GLenum status;
    status = (GLenum)glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

    switch(status) {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
        return true;

    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
        whyNot = "Framebuffer Incomplete: Incomplete Attachment.";
		break;

    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        whyNot = "Unsupported framebuffer format.";
		break;

    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
        whyNot = "Framebuffer Incomplete: Missing attachment.";
		break;

    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
        whyNot = "Framebuffer Incomplete: Attached images must have same dimensions.";
		break;

    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
        whyNot = "Framebuffer Incomplete: Attached images must have same format.";
		break;

    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
        whyNot = "Framebuffer Incomplete: Missing draw buffer.";
		break;

    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
        whyNot = "Framebuffer Incomplete: Missing read buffer.";
		break;

    default:
        whyNot = "Framebuffer Incomplete: Unknown error.";
    }

    return false;    
}


Rect2D RenderDevice::clip2D() const {
    if (state.useClip2D) {
        return state.clip2D;
    } else {
        return state.viewport;
    }
}

/////////////////////////////////////////////////////////////////////////////////////


static void var(TextOutput& t, const std::string& name, const std::string& val) {
    t.writeSymbols(name,"=");
    t.writeString(val);
    t.writeNewline();
}


static void var(TextOutput& t, const std::string& name, const bool val) {
    t.writeSymbols(name, "=", val ? "Yes" : "No");
    t.writeNewline();
}


static void var(TextOutput& t, const std::string& name, const int val) {
    t.writeSymbols(name,"=");
    t.writeNumber(val);
    t.writeNewline();
}


void RenderDevice::describeSystem(
    TextOutput& t) {

    t.writeSymbols("GPU", "{");
    t.writeNewline();
    t.pushIndent();
        var(t, "Chipset", GLCaps::renderer());
        var(t, "Vendor", GLCaps::vendor());
        var(t, "Driver", GLCaps::driverVersion());
        var(t, "OpenGL version", GLCaps::glVersion());
        var(t, "Textures", GLCaps::numTextures());
        var(t, "Texture coordinates", GLCaps::numTextureCoords());
        var(t, "Texture units", GLCaps::numTextureUnits());
    t.popIndent();
    t.writeSymbols("}");
    t.writeNewline();
    t.writeNewline();

    GWindow* w = window();
    GWindow::Settings settings;
    w->getSettings(settings);

    t.writeSymbols("Window", "{");
    t.writeNewline();
    t.pushIndent();
        var(t, "API", w->getAPIName());
        var(t, "Version", w->getAPIVersion());
        t.writeNewline();

        var(t, "In focus", w->hasFocus());
        var(t, "Centered", settings.center);
        var(t, "Framed", settings.framed);
        var(t, "Visible", settings.visible);
        var(t, "Resizable", settings.resizable);
        var(t, "Full screen", settings.fullScreen);
        var(t, "Top", settings.y);
        var(t, "Left", settings.x);
        var(t, "Width", settings.width);
        var(t, "Height", settings.height);
        var(t, "Refresh rate", settings.refreshRate);
        t.writeNewline();

        var(t, "Alpha bits", settings.alphaBits);
        var(t, "Red bits", settings.rgbBits);
        var(t, "Green bits", settings.rgbBits);
        var(t, "Blue bits", settings.rgbBits);
        var(t, "Depth bits", settings.depthBits);
        var(t, "Stencil bits", settings.stencilBits);
        var(t, "Asynchronous", settings.asychronous);
        var(t, "Stereo", settings.stereo);
        var(t, "FSAA samples", settings.fsaaSamples);

    t.popIndent();
    t.writeSymbols("}");
    t.writeNewline();
    t.writeNewline();
}

} // namespace

