/**
 @file GApp.cpp
  
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2003-11-03
 @edited  2007-07-22
 */

#include "G3D/platform.h"

#include "GLG3D/GApp.h"
#include "G3D/GCamera.h"
#include "G3D/fileutils.h"
#include "G3D/Log.h"
#include "G3D/NetworkDevice.h"
#include "GLG3D/FirstPersonManipulator.h"
#include "GLG3D/UserInput.h"
#include "GLG3D/GWindow.h"
#include "GLG3D/Shader.h"
#include "GLG3D/Draw.h"

namespace G3D {

static GApp* lastGApp = NULL;

void screenPrintf(const char* fmt ...) {
    va_list argList;
    va_start(argList, fmt);
    if (lastGApp) {
        lastGApp->vscreenPrintf(fmt, argList);
    }
    va_end(argList);
}

void GApp::screenPrintf(const char* fmt ...) {
    va_list argList;
    va_start(argList, fmt);
    vscreenPrintf(fmt, argList);
    va_end(argList);
}

void GApp::debugPrintf(const char* fmt ...) {
    va_list argList;
    va_start(argList, fmt);
    vscreenPrintf(fmt, argList);
    va_end(argList);
}

void GApp::vscreenPrintf
(
 const char*                 fmt,
 va_list                     argPtr) {
    if (showDebugText) {
        std::string s = G3D::vformat(fmt, argPtr);
        m_debugTextMutex.lock();
        debugText.append(s);
        m_debugTextMutex.unlock();
    }
}


/** Attempt to write license file */
static void writeLicense() {
    FILE* f = fopen("g3d-license.txt", "wt");
    if (f != NULL) {
        fprintf(f, "%s", license().c_str());
        fclose(f);
    }
}


GApp::GApp(const Settings& settings, GWindow* window) :
    lastWaitTime(System::time()),
    m_desiredFrameRate(2000),
    m_simTimeRate(1.0), 
    m_realTime(0), 
    m_simTime(0) {

    debugLog          = NULL;
    debugFont         = NULL;
    m_endProgram      = false;
    m_exitCode        = 0;

    debugLog = new Log(settings.logFilename);
    lastGApp = this;

    defaultController = FirstPersonManipulator::create();

    if (settings.dataDir == "<AUTO>") {
        dataDir = demoFindData(false);
    } else {
        dataDir = settings.dataDir;
    }

    if (settings.writeLicenseFile && ! fileExists("g3d-license.txt", false)) {
        writeLicense();
    }

    renderDevice = new RenderDevice();

    if (window != NULL) {
        _hasUserCreatedWindow = true;
        renderDevice->init(window, debugLog);
    } else {
        _hasUserCreatedWindow = false;    
        renderDevice->init(settings.window, debugLog);
    }

    debugAssertGLOk();

    _window = renderDevice->window();
    _window->makeCurrent();
    debugAssertGLOk();

    m_widgetManager = WidgetManager::create(_window);

    {
        TextOutput t;

        t.writeSymbols("System","{");
        t.pushIndent();
        t.writeNewline();
        System::describeSystem(t);
        if (renderDevice) {
            renderDevice->describeSystem(t);
        }

        NetworkDevice::instance()->describeSystem(t);
        t.writeNewline();
        t.writeSymbol("}");
        t.writeNewline();

        std::string s;
        t.commitString(s);
        debugLog->printf("%s\n", s.c_str());
    }

    defaultCamera  = GCamera();

    debugAssertGLOk();
    loadFont(settings.debugFontName);
    debugAssertGLOk();

    userInput = new UserInput(_window);

    defaultController->onUserInput(userInput);
    defaultController->setMoveRate(10);
    defaultController->setPosition(Vector3(0, 0, 4));
    defaultController->lookAt(Vector3::zero());
    defaultController->setActive(false);
    defaultCamera.setPosition(defaultController->position());
    defaultCamera.lookAt(Vector3::zero());
    addWidget(defaultController);
    setCameraManipulator(defaultController);
 
    autoResize                  = true;
    showDebugText               = true;
    escapeKeyAction             = ACTION_QUIT;
    showRenderingStats          = true;
    fastSwitchCamera            = true;
    catchCommonExceptions       = true;
    manageUserInput             = true;

    {
        GConsole::Settings settings;
        settings.backgroundColor = Color3::green() * 0.1f;
        console = GConsole::create(debugFont, settings, staticConsoleCallback, this);
        console->setActive(false);
        addWidget(console);
    }

    toneMap = ToneMap::create();

    defaultController->setMouseMode(FirstPersonManipulator::MOUSE_DIRECT_RIGHT_BUTTON);
    defaultController->setActive(true);

    if (settings.useDeveloperTools) {
        UprightSplineManipulator::Ref splineManipulator = UprightSplineManipulator::create(&defaultCamera);
        addWidget(splineManipulator);
        
        GFontRef arialFont = GFont::fromFile(System::findDataFile("icon.fnt"));
        GuiSkinRef skin = GuiSkin::fromFile(System::findDataFile("osx.skn"), arialFont);

        debugWindow = GuiWindow::create("Debug Controls", skin, Rect2D::xywh(0, 50, 150, 200), GuiWindow::TOOL_FRAME_STYLE, GuiWindow::HIDE_ON_CLOSE);
        debugWindow->setVisible(false);
        debugPane = debugWindow->pane();
        addWidget(debugWindow);

        developerWindow = DeveloperWindow::create
            (defaultController, 
             splineManipulator, 
             Pointer<Manipulator::Ref>(this, &GApp::cameraManipulator, &GApp::setCameraManipulator), 
             skin,
             console,
             Pointer<bool>(debugWindow, &GuiWindow::visible, &GuiWindow::setVisible),
             &showRenderingStats,
             &showDebugText);

        addWidget(developerWindow);
    } else {
        debugPane = NULL;
    }

    debugAssertGLOk();

    m_simTime     = 0;
    m_realTime    = 0;
    m_simTimeRate = 1.0;
    lastWaitTime  = System::time();
}


void GApp::setExitCode(int code) {
    m_endProgram = true;
    m_exitCode = code;
}


void GApp::loadFont(const std::string& fontName) {
    std::string filename = System::findDataFile(fontName);
    if (fileExists(filename)) {
        debugFont = GFont::fromFile(filename);
    } else {
        debugLog->printf(
            "Warning: G3D::GApp could not load font \"%s\".\n"
            "This may be because the G3D::GApp::Settings::dataDir was not\n"
            "properly set in main().\n",
            filename.c_str());

        debugFont = NULL;
    }
}


GApp::~GApp() {
    if (lastGApp == this) {
        lastGApp = NULL;
    }

    NetworkDevice::cleanup();

    debugFont = NULL;
    delete userInput;
    userInput = NULL;

    renderDevice->cleanup();
    delete renderDevice;
    renderDevice = NULL;

    if (_hasUserCreatedWindow) {
        delete _window;
        _window = NULL;
    }

    VARArea::cleanupAllVARAreas();

    delete debugLog;
    debugLog = NULL;
}


int GApp::run() {
    int ret = 0;
    if (catchCommonExceptions) {
        try {
            onRun();
            ret = m_exitCode;
        } catch (const char* e) {
            alwaysAssertM(false, e);
            ret = -1;
        } catch (const GImage::Error& e) {
            alwaysAssertM(false, e.reason + "\n" + e.filename);
            ret = -1;
        } catch (const std::string& s) {
            alwaysAssertM(false, s);
            ret = -1;
        } catch (const TextInput::WrongTokenType& t) {
            alwaysAssertM(false, t.message);
            ret = -1;
        } catch (const TextInput::WrongSymbol& t) {
            alwaysAssertM(false, t.message);
            ret = -1;
        } catch (const VertexAndPixelShader::ArgumentError& e) {
            alwaysAssertM(false, e.message);
            ret = -1;
        } catch (const LightweightConduit::PacketSizeException& e) {
            alwaysAssertM(false, e.message);
            ret = -1;
        }
    } else {
        onRun();
        ret = m_exitCode;
    }

    return ret;
}


void GApp::onRun() {
    if (window()->requiresMainLoop()) {
        
        // The window push/pop will take care of 
        // calling beginRun/oneFrame/endRun for us.
        window()->pushLoopBody(this);

    } else {
        beginRun();

        // Main loop
        do {
            oneFrame();   
        } while (! m_endProgram);

        endRun();
    }
}


void GApp::renderDebugInfo() {
    if (debugFont.notNull()) {
        // Capture these values before we render debug output
        int majGL  = renderDevice->debugNumMajorOpenGLStateChanges();
        int majAll = renderDevice->debugNumMajorStateChanges();
        int minGL  = renderDevice->debugNumMinorOpenGLStateChanges();
        int minAll = renderDevice->debugNumMinorStateChanges();
        int pushCalls = renderDevice->debugNumPushStateCalls();

        renderDevice->push2D();
            Color3 color = Color3::white();
            double size = 10;

            double x = 5;
            Vector2 pos(x, 5);

            if (showRenderingStats) {

                renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
                Draw::fastRect2D(Rect2D::xywh(2, 2, renderDevice->width() - 4, size * 5), renderDevice, Color4(0, 0, 0, 0.3f));

                Color3 statColor = Color3::yellow();
                debugFont->configureRenderDevice(renderDevice);

                debugFont->send2DQuads(renderDevice, renderDevice->getCardDescription() + "   " + System::version(), 
                    pos, size, color);
                pos.y += size * 1.5f;
                
                double fps = m_graphicsWatch.smoothFPS();
                std::string s = format(
                    "% 4dfps % 4.1gM tris % 4.1gM tris/s   GL Calls: %d/%d Maj; %d/%d Min; %d push", 
                    iRound(fps),
                    iRound(fps * renderDevice->trianglesPerFrame() / 1e5) * 0.1f,
                    iRound(fps * renderDevice->triangleRate() / 1e5) * 0.1f,
                    majGL, majAll, minGL, minAll, pushCalls);
                debugFont->send2DQuads(renderDevice, s, pos, size, statColor);

                pos.x = x;
                pos.y += size * 1.5;

                {
                float g = m_graphicsWatch.smoothElapsedTime();
                float n = m_networkWatch.smoothElapsedTime();
                float s = m_simulationWatch.smoothElapsedTime();
                float L = m_logicWatch.smoothElapsedTime();
                float u = m_userInputWatch.smoothElapsedTime();
                float w = m_waitWatch.smoothElapsedTime();

                float total = g + n + s + L + u + w;

                float norm = 100.0f / total;

                float swapTime = renderDevice->swapBufferTimer().smoothElapsedTime();
                g -= swapTime;

                // Normalize the numbers
                g *= norm;
                swapTime *= norm;
                n *= norm;
                s *= norm;
                L *= norm;
                u *= norm;
                w *= norm;

                std::string str = 
                    format("Time:%3.0f%% Gfx,%3.0f%% Sync,%3.0f%% Sim,%3.0f%% AI,%3.0f%% Net,%3.0f%% UI,%3.0f%% idle", 
                        g, swapTime, s, L, n, u, w);
                debugFont->send2DQuads(renderDevice, str, pos, size, statColor);
                }

                pos.x = x;
                pos.y += size * 3;
            } else if (debugText.length() > 0) {
                debugFont->configureRenderDevice(renderDevice);
            }

            m_debugTextMutex.lock();
            for (int i = 0; i < debugText.length(); ++i) {
                debugFont->send2DQuads(renderDevice, debugText[i], pos, size, color, Color3::black());
                pos.y += size * 1.5;
            }
            m_debugTextMutex.unlock();

        renderDevice->pop2D();
    }
}


bool GApp::onEvent(const GEvent& event) {
    return false;
}


void GApp::onGraphics(RenderDevice* rd, Array<PosedModel::Ref>& posedArray, Array<PosedModel2DRef>& posed2DArray) {
    Array<PosedModel::Ref>        opaque, transparent;

    SkyParameters lighting(G3D::toSeconds(11, 00, 00, AM));

    rd->setProjectionAndCameraMatrix(defaultCamera);
    rd->clear(true, true, true);

    rd->enableLighting();
        rd->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
        rd->setAmbientLightColor(lighting.ambient);

        // 3D
        if (posedArray.size() > 0) {
            Vector3 lookVector = renderDevice->getCameraToWorldMatrix().lookVector();
            PosedModel::sort(posedArray, lookVector, opaque, transparent);
            
            for (int i = 0; i < opaque.size(); ++i) {
                opaque[i]->render(renderDevice);
            }

            for (int i = 0; i < transparent.size(); ++i) {
                transparent[i]->render(renderDevice);
            }
        }
    rd->disableLighting();

    if (posed2DArray.size() > 0) {
        renderDevice->push2D();
            PosedModel2D::sort(posed2DArray);
            for (int i = 0; i < posed2DArray.size(); ++i) {
                posed2DArray[i]->render(renderDevice);
            }
        renderDevice->pop2D();
    }
}


void GApp::addWidget(const Widget::Ref& module) {
    m_widgetManager->add(module);
}


void GApp::removeWidget(const Widget::Ref& module) {
    m_widgetManager->remove(module);
}


void GApp::oneFrame() {
    lastTime = now;
    now = System::time();
    RealTime timeStep = now - lastTime;

    // User input
    m_userInputWatch.tick();
    if (manageUserInput) {
        processGEventQueue();
    }
    onUserInput(userInput);
    m_widgetManager->onUserInput(userInput);
    m_userInputWatch.tock();

    // Network
    m_networkWatch.tick();
    onNetwork();
    m_widgetManager->onNetwork();
    m_networkWatch.tock();

    // Simulation
    m_simulationWatch.tick();
    {
        double rate = simTimeRate();    
        RealTime rdt = timeStep;
        SimTime  sdt = timeStep * rate;
        SimTime  idt = desiredFrameDuration() * rate;

        onBeforeSimulation(rdt, sdt, idt);
        onSimulation(rdt, sdt, idt);
        m_widgetManager->onSimulation(rdt, sdt, idt);
        onAfterSimulation(rdt, sdt, idt);

        if (m_cameraManipulator.notNull()) {
            defaultCamera.setCoordinateFrame(m_cameraManipulator->frame());
        }

        setRealTime(realTime() + rdt);
        setSimTime(simTime() + sdt);
        setIdealSimTime(idealSimTime() + idt);
    }
    m_simulationWatch.tock();

    // Logic
    m_logicWatch.tick();
    {
        onLogic();
        m_widgetManager->onLogic();
    }
    m_logicWatch.tock();

    // Wait 
    // Note: we might end up spending all of our time inside of
    // RenderDevice::beginFrame.  Waiting here isn't double waiting,
    // though, because while we're sleeping the CPU the GPU is working
    // to catch up.

    m_waitWatch.tick();
    {
        RealTime now = System::time();
        // Compute accumulated time
        onWait(now - lastWaitTime, desiredFrameDuration());
        lastWaitTime = System::time();
    }
    m_waitWatch.tock();

    // Graphics
    m_graphicsWatch.tick();
    {
        m_posed3D.fastClear();
        m_posed2D.fastClear();
        m_widgetManager->onPose(m_posed3D, m_posed2D);
        onPose(m_posed3D, m_posed2D);

        renderDevice->beginFrame();
        {
            renderDevice->pushState();
            {
                onGraphics(renderDevice, m_posed3D, m_posed2D);
            }
            renderDevice->popState();
            renderDebugInfo();
        }
        renderDevice->endFrame();
        debugText.clear();
    }
    m_graphicsWatch.tock();

    if (m_endProgram && window()->requiresMainLoop()) {
        window()->popLoopBody();
    }
}


void GApp::onWait(RealTime t, RealTime desiredT) {
    System::sleep(max(0.0, desiredT - t));
}


void GApp::beginRun() {

    m_endProgram = false;
    m_exitCode = 0;

    onInit();

    // Move the controller to the camera's location
    defaultController->setFrame(defaultCamera.coordinateFrame());

    now = System::time() - 0.001;
}


void GApp::endRun() {
    onCleanup();

    Log::common()->section("Files Used");
    Set<std::string>::Iterator end = _internal::currentFilesUsed.end();
    Set<std::string>::Iterator f = _internal::currentFilesUsed.begin();
    
    while (f != end) {
        Log::common()->println(*f);
        ++f;
    }
    Log::common()->println("");


    if (window()->requiresMainLoop() && m_endProgram) {
        ::exit(m_exitCode);
    }
}


void GApp::staticConsoleCallback(const std::string& command, void* me) {
    ((GApp*)me)->onConsoleCommand(command);
}


void GApp::onConsoleCommand(const std::string& cmd) {
    if (trimWhitespace(cmd) == "exit") {
        setExitCode(0);
        return;
    }
}


void GApp::onUserInput(UserInput* userInput) {
}

void GApp::processGEventQueue() {
    userInput->beginEvents();

    // Event handling
    GEvent event;
    while (window()->pollEvent(event)) {

        if (onEvent(event)) {
            // Event was consumed
            continue;
        }

        if (WidgetManager::onEvent(event, m_widgetManager)) {
            continue;
        }

        switch(event.type) {
        case GEventType::QUIT:
            setExitCode(0);
            break;

        case GEventType::VIDEO_RESIZE:
            if (autoResize) {
                renderDevice->notifyResize
                    (event.resize.w, event.resize.h);
                Rect2D full = 
                    Rect2D::xywh(0, 0, 
                                 renderDevice->width(), 
                                 renderDevice->height());
                renderDevice->setViewport(full);
            }
            break;

        case GEventType::KEY_DOWN:

            if (! console->active()) {
                switch (event.key.keysym.sym) {
                case GKey::ESCAPE:
                    switch (escapeKeyAction) {
                    case ACTION_QUIT:
                        setExitCode(0);
                        break;
                    
                    case ACTION_SHOW_CONSOLE:
                        console->setActive(true);
                        continue;
                        break;

                    case ACTION_NONE:
                        break;
                    }
                    break;

                case GKey::F2:
                    if (fastSwitchCamera && developerWindow.isNull()) {
                        defaultController->setActive(! defaultController->active());
                        // Consume event
                        continue;
                    }
                    break;

                // Add other key handlers here
                default:;
                }
            }
        break;

        // Add other event handlers here

        default:;
        }

        userInput->processEvent(event);
    }

    userInput->endEvents();
}

}
