/**
   @file GApp.h
 
   @maintainer Morgan McGuire, morgan@graphics3d.com

   @created 2003-11-03
   @edited  2007-07-10
*/

#ifndef G3D_GAPP_H
#define G3D_GAPP_H

#include "G3D/Stopwatch.h"
#include "GLG3D/GFont.h"
#include "G3D/GCamera.h"
#include "GLG3D/FirstPersonManipulator.h"
#include "GLG3D/RenderDevice.h"
#include "G3D/NetworkDevice.h"
#include "GLG3D/GWindow.h"
#include "GLG3D/Widget.h"
#include "GLG3D/GConsole.h"
#include "GLG3D/ToneMap.h"
#include "GLG3D/DeveloperWindow.h"
#include "G3D/GThread.h"

namespace G3D {

class RenderDevice;
class UserInput;

//  See @link guideapp @endlink for a discussion of GApp and GApplet. 
/**

 For each frame, the GApp has several tasks that can be implemented by overriding
 base class methods.  The use of cooperative, round-robbin scheduling avoids the need
 for threads in most applications.  These tasks are:
 
 <ul>
 <li> Graphics
 <li> User Input
 <li> AI / Game Logic
 <li> Network receive (network send occurs wherever needed)
 <li> Physical simulation
 <li> Wait (sleep to maintain constant frame rate)
 </ul>
 
 Other event handlers include onInit/onCleanup, onEvent for fine-grain
 event handling, and onConsoleCommand.
 
 The onConsoleCommand handler allows you to add an in-game command console
 to your program.  By default it is activated when '~' is pressed; you can also
 set the GApp::escapeAction to open the console on ESC.  The console is a Widget,
 so you can completely disable it (e.g., in a release build of the program) by
 executing <code>removeWidget(console)</code>.
 
 To invoke a GApp and let it control the main loop, call
 run(). 
*/
class GApp {
public:
    friend class GWindow;

    class Settings {
    public:
        GWindow::Settings       window;

        /**
           If "<AUTO>", G3D will search for the standard
           data files.  It is recommended that you override this
           default and set dataDir to a directory relative
           to your executable (e.g. "./data/")
           so your programs can be distributed to users who
           do not have full the G3D data directory.
        */
        std::string             dataDir;
    	
        /**
           Can be relative to the G3D data directory (e.g. "font/dominant.fnt")
           or relative to the current directory.
           Default is "console-small.fnt"
        */
        std::string             debugFontName;
        
        std::string             logFilename;

        /** If true, the G3D::DeveleloperWindow and G3D::CameraControlWindow will be enabled and
            accessible by pushing F12.
            These require osx.skn, arial.fnt, greek.fnt, and icon.fnt to be in locations where
            System::findDataFile can locate them (the program working directory is one such location).
        */  
        bool                    useDeveloperTools;
    	
        /** 
            When true, GAapp ensures that g3d-license.txt exists in the current
            directory.  That file is written from the return value of G3D::license() */
        bool                    writeLicenseFile;
        
        Settings() : dataDir("<AUTO>"), debugFontName("console-small.fnt"), 
                     logFilename("log.txt"),useDeveloperTools(true), writeLicenseFile(true) {
        }
    };

private:

    /** Called from init. */
    void loadFont(const std::string& fontName);

    GWindow*                _window;
    bool                    _hasUserCreatedWindow;

protected:

    Stopwatch               m_graphicsWatch;
    Stopwatch               m_logicWatch;
    Stopwatch               m_networkWatch;
    Stopwatch               m_userInputWatch;
    Stopwatch               m_simulationWatch;
    Stopwatch               m_waitWatch;

    WidgetManager::Ref      m_widgetManager;

    bool                    m_endProgram;
    int                     m_exitCode;

    /**
       Used to find the frame for defaultCamera.
    */
    Manipulator::Ref        m_cameraManipulator;

    GMutex                  m_debugTextMutex;

    /**
       Strings that have been printed with screenPrintf.
       Protected by m_debugTextMutex.
    */
    Array<std::string>      debugText;

    /**
       Processes all pending events on the GWindow queue into the userInput.
    */
    virtual void processGEventQueue();

    static void staticConsoleCallback(const std::string& command, void* me);

public:

    /** Add your own debugging controls to this window.*/
    GuiWindow::Ref          debugWindow;
    /** debugWindow->pane() */
    GuiPane*                debugPane;

    const Stopwatch& graphicsWatch() const {
        return m_graphicsWatch;
    }

    const Stopwatch& waitWatch() const {
        return m_waitWatch;
    }

    const Stopwatch& logicWatch() const {
        return m_logicWatch;
    }

    const Stopwatch& networkWatch() const {
        return m_networkWatch;
    }

    const Stopwatch& userInputWatch() const {
        return m_userInputWatch;
    }

    const Stopwatch& simulationWatch() const {
        return m_simulationWatch;
    }

    /** Initialized to GApp::Settings::dataDir, or if that is "<AUTO>", 
        to System::demoFindData(). To make your program
        distributable, override the default 
        and copy all data files you need to a local directory.
        Recommended setting is "data/" or "./", depending on where
        you put your data relative to the executable.

        Your data directory must contain the default debugging font, 
        "console-small.fnt", unless you change it.
    */
    std::string             dataDir;
    Log*                    debugLog;
    RenderDevice*           renderDevice;

    /** Command console. */
    GConsoleRef             console;

    ToneMapRef              toneMap;

    /** The window that displays buttons for debugging.  If GApp::Settings::useDeveloperTools is true
        this will be created and added as a Widget on the GApp.  Otherwise this will be NULL.
    */
    DeveloperWindow::Ref    developerWindow;

    /**
       NULL if not loaded
    */
    GFontRef                debugFont;
    UserInput*              userInput;

    /** Invoke to true to end the program at the end of the next event loop. */
    virtual void setExitCode(int code = 0);

    /**
       A default camera that is driven by the defaultController.
    */
    GCamera		            defaultCamera;

    /**
       Allows first person (Quake game-style) control
       using the arrow keys or W,A,S,D and the mouse.
    */
    FirstPersonManipulatorRef       defaultController;

    /**
       The manipulator that positions the defaultCamera every frame.
       By default, this is set to the defaultController.  This may be
       set to NULL to disable explicit camera positioning.
    */
    void setCameraManipulator(const Manipulator::Ref& man) {
        m_cameraManipulator = man;
    }

    Manipulator::Ref cameraManipulator() const {
        return m_cameraManipulator;
    }
    
    inline GWindow* window() const {
        return _window;
    }

    /**
       When true, debugPrintf prints to the screen.
       (default is true)
    */
    bool                    showDebugText;

    enum Action {
        ACTION_NONE,
        ACTION_QUIT,
        ACTION_SHOW_CONSOLE
    };

    /**
       When true an GKey::ESCAPE keydown event
       quits the program.
       (default is true)
    */
    Action                  escapeKeyAction;

    /**
       When true GKey::F2 keydown deactivates
       the camera and restores the mouse cursor.
       (default is true). This works even if GApp::Settings::useDeveloperTools is false.
    */
    bool                    fastSwitchCamera;

    /**
       When true,   renderDebugInfo prints the frame rate and
       other data to the screen.
    */
    bool                    showRenderingStats;

    /**
       When true and the window is resizable, automatically
       responds to SDL_RESIZE events by notifying the
       RenderDevice that the window has been resized and
       resetting the viewport to full screen.
       (default is true)
    */
    bool                    autoResize;

    /**
       When true, the G3D::UserInput->beginEvents/endEvents processing is handled 
       for you by calling processGEventQueue() before G3D::GApp::onUserInput is called.  If you turn
       this off, you must call processGEventQueue() or provide your own event to userInput processing in onUserInput.
       (default is true)
    */
    bool                    manageUserInput;

    /**
       When true, there is an assertion failure if an exception is 
       thrown.

       Default is true.
    */
    bool                    catchCommonExceptions;

    /**
       If debugShowText is true, prints to an on-screen buffer that
       is cleared every frame.

       Called by G3D::screenPrintf

       @sa G3D::logPrintf, G3D::screenPrintf, G3D::consolePrintf
    */
    void screenPrintf(const char* fmt ...) G3D_CHECK_PRINTF_METHOD_ARGS;;

    void vscreenPrintf
    (
     const char*                 fmt,
     va_list                     argPtr) G3D_CHECK_VPRINTF_METHOD_ARGS;;

    /**
       If debugShowText is true, prints to an on-screen buffer that
       is cleared every frame.
       @deprecated
       @sa G3D::screenPrintf
    */
    virtual void debugPrintf(const char* fmt ...) G3D_CHECK_PRINTF_METHOD_ARGS;
    
    /**
       Called from GApplet::run immediately after doGraphics to render
       the debugging text.  Does nothing if debugMode is false.  It
       is not usually necessary to override this method.
    */
    virtual void renderDebugInfo();

    /**
       @param window If null, a SDLWindow will be created for you. This
       argument is useful for substituting a different window
       system (e.g. GlutWindow)
    */
    GApp(const Settings& options = Settings(), GWindow* window = NULL);

    virtual ~GApp();

    /**
       Call this to run the app.
    */
    int run();

private:
    /** Used by doSimulation for elapsed time. */
    RealTime            now, lastTime;

    /** Used by doWait for elapsed time. */
    RealTime            lastWaitTime;

    float               m_desiredFrameRate;
    double              m_simTimeRate;
    RealTime            m_realTime;
    SimTime             m_simTime;
    SimTime             m_idealSimTime;

    Array<PosedModelRef> m_posed3D;
    Array<PosedModel2DRef> m_posed2D;

private:

    /** Helper for run() that actually starts the program loop. Called from run(). */
    void onRun();

    /**
       Initializes state at the beginning of onRun, including calling onCleanup.
    */
    void beginRun();

    /**
       Cleans up at the end of onRun, including calling onCleanup.
    */
    void endRun();

    /** 
        A single frame of rendering, simulation, AI, events, networking,
        etc.  Invokes the onXXX methods. 
    */
    void oneFrame();

public:

    /**
       Installs a module.  Actual insertion may be delayed until the next frame.
    */
    virtual void addWidget(const Widget::Ref& module);


    /**
       The actual removal of the module may be delayed until the next frame.
    */
    virtual void removeWidget(const Widget::Ref& module);

    /** Amount of time that passes in simTime for every second of realTime.
        e.g., 1.0 == real-time, 2.0 == fast, 0.5 == slow, 0.0 = stop time.
        Default is 1.0.
    */
    inline double simTimeRate() const {
        return m_simTimeRate;
    }

    virtual void setSimTimeRate(double s) {
        m_simTimeRate = s;
    }

    /** Accumulated wall-clock time since init was called on this applet. 
        Since this time is accumulated, it may drift from the true
        wall-clock obtained by System::time().*/
    inline RealTime realTime() const {
        return m_realTime;
    }

    virtual void setRealTime(RealTime r) {
        m_realTime = r;
    }

    /** In-simulation time since init was called on this applet.  
        Takes into account simTimeSpeed.  Automatically incremented
        after doSimulation.
    */
    inline SimTime simTime() const {
        return m_simTime;
    }

    virtual void setIdealSimTime(SimTime s) {
        m_idealSimTime = s;
    }

    /**
       Simulation time that is always advanced by precisely the
       desiredFrameDuration * simTimeRate, regardless of the 
       actual frame duration.
    */
    inline SimTime idealSimTime() const {
        return m_idealSimTime;
    }

    virtual void setSimTime(SimTime s) {
        m_simTime = s;
    }

    /** Change to invoke frame limiting via doWait.
        Defaults to inf() */
    virtual void setDesiredFrameRate(float fps) {
        debugAssert(fps > 0);
        m_desiredFrameRate = fps;
    }

    float desiredFrameRate() const {
        return m_desiredFrameRate;
    }

    RealTime desiredFrameDuration() const {
        return 1.0 / m_desiredFrameRate;
    }
    
protected:

    /**
       Load your data here.  Unlike the constructor, this catches common exceptions.
       It is called before the first frame is processed.
    */
    virtual void onInit() {}

    /**
       Unload/deallocate your data here.  Unlike the constructor, this catches common exceptions.
       It is called after the last frame is processed.
    */
    virtual void onCleanup() {}


    /**
       Override this with your simulation code.
       Called from GApp::run.
        
       Default implementation does nothing.

       simTime(), idealSimTime() and realTime() are incremented after
       doSimulation is called, so at the beginning of call the current
       time is the end of the previous frame.

       @param rdt Elapsed real-world time since the last call to doSimulation.

       @param sdt Elapsed sim-world time since the last call to
       doSimulation, computed by multiplying the wall-clock time by the
       simulation time rate.

       @param idt Elapsed ideal sim-world time.  Use this for perfectly
       reproducible timing results.  Ideal time always advances by the
       desiredFrameDuration * simTimeRate, no matter how much wall-clock
       time has elapsed.
    */
    virtual void onSimulation(RealTime rdt, SimTime sdt, SimTime idt) {
        (void)idt;
        (void)rdt;
        (void)sdt;
    }

    /** Invoked before onSimulation is run on the installed GModules and GApp.
        This is not used by most programs; it is primarily a hook for those performing
        extensive physical simulation on the GModules that need a setup and cleanup step.
    */
    virtual void onBeforeSimulation(RealTime rdt, SimTime sdt, SimTime idt) {        
        (void)idt;
        (void)rdt;
        (void)sdt;
    }

    /**
       Invoked after onSimulation is run on the installed GModules and GApp.
       Not used by most programs.
    */
    virtual void onAfterSimulation(RealTime rdt, SimTime sdt, SimTime idt) {        
        (void)idt;
        (void)rdt;
        (void)sdt;
    }

    /**
       Rendering callback used to paint the screen.  Called automatically.

       Override and implement.  The debugCamera's projection and object to world
       matrices are set by default; you can set other cameras as desired. 
       RenderDevice::beginFrame and endFrame are called for you.
    */
    virtual void onGraphics(RenderDevice* rd, Array<PosedModelRef>& posed3D, Array<PosedModel2DRef>& posed2D);

    /** Called before onGraphics.  Append any models that you want
        rendered (you can also explicitly pose and render in your
        onGraphics method).  The provided arrays will already contain
        posed models from any installed Widgets. */
    virtual void onPose(Array<PosedModelRef>& posed3D, Array<PosedModel2DRef>& posed2D) {}

    /**
       For a networked app, override this to implement your network
       message polling.
    */
    virtual void onNetwork() {}

    /**
       Task to be used for frame rate limiting.  

       Overriding onWait is not recommended.

       Frame rate limiting is useful
       to avoid overloading a maching that is running background tasks and
       for situations where fixed time steps are needed for simulation and there
       is no reason to render faster.

       Default implementation System::sleep()s until cumulativeTime + time
       in wait is greater than or equal to @a frameDuration = 1 / desiredFrameRate.
    */
    virtual void onWait(RealTime cumulativeTime, RealTime frameDuration);


    /**
       Update any state you need to here.  This is a good place for
       AI code, for example.  Called after network and user input,
       before simulation.
    */
    virtual void onLogic() {}

    
    /**
       It is recommended to override onUserInput() instead of this method.

       Override if you need to explicitly handle events raw in the order
       they appear rather than once per frame by checking the current
       system state.
     
       Note that the userInput contains a record of all
       keys pressed/held, mouse, and joystick state, so 
       you do not have to override this method to handle
       basic input events.

       Return true if the event has been consumed (i.e., no-one else 
       including GApp should process it further).

       The default implementation does nothing.
    */
    virtual bool onEvent(const GEvent& event);

    /**
       Routine for processing user input from the previous frame.  Default implementation does nothing.
    */
    virtual void onUserInput(class UserInput* userInput);

    /**
       Invoked when a user presses enter in the in-game console.  The default implementation
       ends the program if the command is "exit".
    */
    virtual void onConsoleCommand(const std::string& cmd);
};


/**
   Displays output on the last G3D::GApp instantiated.  If there was no GApp instantiated,
   does nothing.  Threadsafe.
   
   This is primarily useful for code that prints (almost) the same
   values every frame (e.g., "current position = ...") because those
   values will then appear in the same position on screen.

   For one-off print statements (e.g., "network message received")
   see G3D::consolePrintf.
 */
void screenPrintf(const char* fmt ...) G3D_CHECK_PRINTF_ARGS;

}

#endif
