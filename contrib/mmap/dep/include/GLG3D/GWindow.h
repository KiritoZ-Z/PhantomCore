/**
  @file GWindow.h

  @maintainer Morgan McGuire, morgan@graphics3d.com
  @created 2005-02-10
  @edited  2006-03-04
*/

#ifndef G3D_GWINDOW_H
#define G3D_GWINDOW_H

#include "G3D/platform.h"
#include "G3D/GImage.h"
#include "G3D/Array.h"
#include "G3D/Queue.h"
#include "GLG3D/GEvent.h"

namespace G3D {

class Rect2D;

/**
 Interface to window APIs for window management, event processing,
 and OpenGL context management.  A GWindow may be a user-level window, 
 with a minimize button and frame, or simply a rectangular area within 
 a larger window.  In the latter case, several of the methods (e.g.,
 setCaption) are likely to be ignored by the implementation.  See
 G3D::SDLWindow and the wxWindows, Qt, and Glut implementations in
 the contrib directory.
   
 Many parts of G3D assume that there is only one
 OS-level window, with one OpenGL context.  (Although you <B>can</B> have
 multiple non-GL windows if using a native windowing API and you can even have
 multiple OpenGL contexts if you manage switching between them yourself).

 Subclass this interface to support your preferred window system
 (e.g. ActiveX window, OS/X Carbon, MFC window, glut) or use the 
 generic SDLWindow subclass that supports all platforms.

 All dimensions are of the client area (inside the frame, if the
 window has a frame).

 After instantiation, a GWindow guarantees that the OpenGL context for this
 window is bound.  It may be unbound by later code--use GWindow::makeCurrent
 if you have multiple windows in your application.

 <B>Subclassing</B>

 It is common to accept a GWindowSettings as an argument to the 
 constructor.

 Subclasses are required to call GLCaps::init
        from the end of their constructor/create function to
        finish initializing OpenGL. 

 <B>Input</B>
 This class will be extended with UI events and constants 
 in a future revision.  For now it is limited to rendering
 and window management.  Consider writing your own UserInput
 replacement in the mean time.

 <B>Implementations</B>
 The following GWindow subclasses already exist: 
 G3D::SDLWindow, 
 G3D::Win32Window, 
 <A HREF="../contrib/wxGWindow">wxGWindow</A> (wxWidgets),
 <A HREF = "../contrib/GlutWindow">GlutWindow</a>,
 and <A HREF="../contrib/CoreyGWindow/QGWindow.h">QGWindow</a> (Qt).

 One typically chooses among these based on the GUI API used 
 to manage the main window.
 */
class GWindow {
public:

    class Settings {
    public:

        /** Width of the client area in pixels. */
        int     width;
          
	    int     height;

	    /** Initial x position of the client area in screen space(can
	        be overridden by fullScreen and center) */
	    int     x;
    	
	    /** Initial y position of the client area in screen space(can
	        be overridden by fullScreen and center) */
	    int     y;
    	
	    /** If true, overrides the x and y settings */
	    bool    center;

	    /** The number of bits in <B>each</B> color channel of the
	        frame buffer.  5, <B>8</B>.*/
	    int     rgbBits;

	    /* The number of bits in the alpha channel of the frame
	       buffer. <B>0</B>, 1, 8 Used in Win32Window to help
	       determine resolution color depth (Added to rgbBits). */
	    int     alphaBits;

	    /** <B>0 (autodetect the best for this card)</B>, 16,
	        <B>24</B>, 32 */
	    int     depthBits;

	    /** <B>8</B> */
	    int     stencilBits;
    	
	    /** Number of samples per pixel for anti-aliasing purposes.
	        <B>1</B> (none), 4, 8 */
	    int     fsaaSamples;
    	
	    /** Will you accept a software rendering pipeline?  Used to
	      determine if a hardware only pixel format is
	      required. */
	    bool    hardware;

	    /** Applications that fill the screen may also want to set
	        framed = false. */
	    bool    fullScreen;

	    /** Should buffer flips be un-hitched from refresh rate?
			    <B>true</B>, false.  True generally gives
			    higher frame rates.*/
	    bool    asychronous;

	    /** Allocate a stereo display context. true, <B>false</B> */
	    bool    stereo;
    	
	    /** In cycles/sec */
	    int     refreshRate;
    	
	    /**
	       If true, you should set up your event loop as described in the 
	       docs for RenderDevice::resize.
	    */
	    bool    resizable;

        /**
            If a window's settings define it to be both resizable and allowed
            to maximize, it will have an activated maximize button (for Win32
            windows only)
        */
        bool    allowMaximize;
    	
	    /**
	       When true, a window frame and title bar are present.
	    */
	    bool    framed;
    	
	    /**
	       When true, the window begins visible.  <B>true</B>, false.
	    */
	    bool    visible;

	    /**
	       Default icon that the GWindow implementation tries to set initially.
	    */
	    std::string  defaultIconFilename;

            /**
             Window title
            */
            std::string  caption;

	    Settings() :
	      width(800),
	      height(600),
	      x(0),
	      y(0),
	      center(true),
	      rgbBits(8),
	      alphaBits(0),
	      depthBits(24),
	      stencilBits(8),
	      fsaaSamples(1),
	      hardware(true),
	      fullScreen(false),
	      asychronous(true),
	      stereo(false),
	      refreshRate(85),
	      resizable(false),
          allowMaximize(true),
	      framed(true),
	      visible(true),
	      defaultIconFilename("nodefault"),
	      caption("3D") {}
    };
protected:

    Queue<GEvent>               m_eventQueue;

    /** Extract one event from the underlying operating system */
    virtual bool pollOSEvent(GEvent& e);

private:

    /** */
    class LoopBody {
    public:
        union{
            void (*func)(void*);
            class GApp*         app;
        };

        void*                   arg;        
        
        bool                    isGApp;

        LoopBody() : func(NULL), arg(NULL), isGApp(false) {}
        LoopBody(GApp* a) : app(a), arg(NULL),  isGApp(true) {}
        LoopBody(void (*f)(void*), void* a) : func(f), arg(a), isGApp(false) {}
    };

    Array<LoopBody>             loopBodyStack;

protected:

    bool notDone() {
        return loopBodyStack.size() > 0;
    }

    /** Subclasses should call from their idle function. */
    void executeLoopBody();

    int                     m_inputCaptureCount;
    int                     m_mouseHideCount;

    
    /** 
      Capture the keyboard and mouse focus, locking the mouse to the client area of this window.
      Sets the inputCaptureCount to 1 if @a c is true and 0 if @a c is false
      @deprecated use setInputCaptureCount
     */
    virtual void setInputCapture(bool c) = 0;

    /** @deprecated Use setMouseVisibleCount */
    virtual void setMouseVisible(bool b) = 0;

public:

    /** 
        Inserts an event into the queue.
     */
    virtual void fireEvent(const GEvent& event);

    /** Return the <I>actual</I> properties of this window (as opposed to
        the desired settings from which it was initialized) */
    virtual void getSettings(GWindow::Settings& settings) const = 0;

    /**
     Measured in pixels, this is the client area of the window.
     Returns the same width as getSettings and dimensions().
     Convenience method for users.
     */
    virtual int width() const = 0;

    virtual int height() const = 0;

    /**
     Shape of the client area of the window in screen coordinates.
     */
    virtual Rect2D dimensions() const = 0;

    /**
     Fails silently if unable to change the dimensions.
     The value returned by getSettings will not change immediately-- 
     it waits for a notifyResize call.
     */
    virtual void setDimensions(const Rect2D& dims) = 0;

    /**
      Clears the @a files array and then sets it to a list of all 
      files that were dropped in the last GEventType::FILE_DROP event.
     */
    virtual void getDroppedFilenames(Array<std::string>& files) = 0;

    /**
     Fails silently if unable to change the position.
     Sets the window (not client) position.
     */
    virtual void setPosition(int x, int y) = 0;

    /** Returns true if this window currently has [keyboard] focus
        (i.e. is in the foreground, not minimized, recieves keystrokes.) */
    virtual bool hasFocus() const = 0;

    /** Description of the windowing API for logging purposes (e.g. "1.2.7" for SDL 1.2.7). */
    virtual std::string getAPIVersion() const = 0;

    /** Description of the windowing API for logging purposes (e.g. "SDL"). */
    virtual std::string getAPIName() const = 0;

    /** gammaRamp.length() = 256 */
    virtual void setGammaRamp(const Array<uint16>& gammaRamp) = 0;

    virtual void setCaption(const std::string& caption) = 0;

    /** Returns 0 if there are no joysticks available */
    virtual int numJoysticks() const = 0;

    /** The name by which the OS refers to this joystick (e.g. "Gravis Gamepad") */
    virtual std::string joystickName(unsigned int sticknum) = 0;

    /** The window title */
    virtual std::string caption() = 0;

    /** Set the icon (if supported).  Fails silently if not supported
        or the window has no frame.  <I>May also fail if the GWindow implementation's
        underlying GUI library is too limited.</I>
        @param image May have any dimension supported by underlying OS.*/
    virtual void setIcon(const GImage& image) {
        (void)image;
    }

    virtual void setIcon(const std::string& imageFilename) {
        setIcon(GImage(imageFilename));
    }

    /** Swap the OpenGL front and back buffers.  Called by RenderDevice::endFrame. */
    virtual void swapGLBuffers() = 0;

    /** Notifies the window that it has been resized 
        (called by RenderDevice::notifyResize).  Some window systems
        (e.g. SDL) need explicit notification in this form when
        a resize event comes in.
        
        <B>Must not</B> destroy the underlying
        OpenGL context (i.e. textures, vertex buffers, and GPU programs
        must be preserved across calls).
     */
    virtual void notifyResize(int w, int h) = 0;

    virtual void setRelativeMousePosition(double x, double y) = 0;

    /** Relative to the window origin */
    virtual void setRelativeMousePosition(const Vector2& p) = 0;

    /** Checks to see if any events are waiting.  If there is an event,
        returns true and fills out the GEvent structure.  Otherwise
        returns false.  The caller is responsible for invoking GWindow::notifyResize
        with any resize events; the GWindow does not notify itself. */
    virtual bool pollEvent(GEvent& e);

    /** Returns the current mouse position and the state of the mouse buttons.
        It is essential to sample both simultaneously so that the mouse has
        not moved from the location where a click occurred.

        @param position In pixels, with y=up.  Relative to the
        upper left corner of the window.

        @param mouseButtons If button <I>i</I> is currently pressed then bit <I>i</I> is set.
    */
    virtual void getRelativeMouseState(Vector2& position, uint8& mouseButtons) const = 0;
    virtual void getRelativeMouseState(int& x, int& y, uint8& mouseButtons) const = 0;
    virtual void getRelativeMouseState(double& x, double& y, uint8& mouseButtons) const = 0;

    /**
     Returns the state of the given joystick.  Not all sticks support all buttons and
     axes and frequently the state of one stick overlaps the state of another.
     An asserition fails if stickNum > numJoysticks().
     Joystick axes are normalized to the range [-1, 1] (but might not be calibrated...).
     Joystick axes should be in the starting order: X, Y, Z, Slider1, Slider2, rX, rY, rZ
     */
    virtual void getJoystickState(unsigned int stickNum, Array<float>& axis, Array<bool>& button) = 0;

    void setInputCaptureCount(int c) {
        if ((m_inputCaptureCount > 0) && (c <= 0)) {
            // Release mouse
            setInputCapture(false);
        } else if ((m_inputCaptureCount <= 0) && (c > 0)) {
            // Capture mouse
            setInputCapture(true);
        }

        // Set this variable after the setInputCapture statements since
        // they corrupt its value.
        m_inputCaptureCount = c;
    }


    /**
      If the count is 1 or greater, then the keyboard and mouse focus is captured, 
      locking the mouse to the client area of this window.  The use of a count instead of 
      an explicit capture/release API allows multiple parts of a program to capture input
      without accidentally releasing and enclosing capture.
      */
    int inputCaptureCount() const {
        return m_inputCaptureCount;
    }

    virtual void incInputCaptureCount() {
        setInputCaptureCount(inputCaptureCount() + 1);
    }

    virtual void decInputCaptureCount() {
        setInputCaptureCount(inputCaptureCount() - 1);
    }


    void setMouseHideCount(int c) {
        if ((m_mouseHideCount > 0) && (c <= 0)) {
            // Release mouse
            setMouseVisible(true);
        } else if ((m_mouseHideCount <= 0) && (c > 0)) {
            // Capture mouse
            setMouseVisible(false);
        }

        // Set this variable after the setMouseVisible statements since
        // they corrupt its value.
        m_mouseHideCount = c;
    }


    /**
      If the count is 1 or greater, then the keyboard and mouse focus is captured, 
      locking the mouse to the client area of this window.  The use of a count instead of 
      an explicit capture/release API allows multiple parts of a program to capture input
      without accidentally releasing and enclosing capture.
      */
    int mouseHideCount() const {
        return m_mouseHideCount;
    }

    virtual void incMouseHideCount() {
        setMouseHideCount(mouseHideCount() + 1);
    }

    virtual void decMouseHideCount() {
        setMouseHideCount(mouseHideCount() - 1);
    }

    /** Windows for which this is true require a program
        to hand control of the main loop to GWindow::startMainLoop.
        The program's functionality may then be implemented through
        the "loop body" function.
    
        That is, if requiresMainLoop returns true, you must use
        the following structure:
        <PRE>
           void doEvents() {
              GEvent e;
              while (window->pollEvent(e)) {
                  ... // Event handling
              } 
           }

           void doGraphics() {
              renderDevice->beginFrame();
                  renderDevice->clear(true, true, true);
                  ...  // draw stuff
              renderDevice->endFrame();
           }

           void loopBody(void*) {
              // all per-frame code; event-handling, physics, networking, AI, etc.
              doEvents();
              doLogic();
              doNetwork();
              doAI();
              doGraphics();

              // To end the program, invoke window->popLoopBody
           }

           window->pushLoopBody(callback, NULL);
           window->runMainLoop(); // doesn't return
        </PRE>

        When requiresMainLoop returns false, you may use either the
        above structure or the following one (which puts you in more
        control of when graphics vs. other code is executed):

        <PRE>
            while (true) {
              doEvents();
              doLogic();
              doNetwork();
              doAI();
              doGraphics();
            }       
        </PRE>

        This design is necessary because some underlying Window APIs
        (e.g. ActiveX, GLUT) enforce an event-driven structure.
    */
    virtual bool requiresMainLoop() const {
        return false;
    }

    /** Pushes a function onto the stack of functions called by runMainLoop */
    virtual void pushLoopBody(void (*body)(void*), void* arg) {
        loopBodyStack.push(LoopBody(body, arg));
    }

    /** Invokes GApplet::beginRun after the applet is on the stack. */
    virtual void pushLoopBody(GApp* app);


    /** Pops a loop body off the stack.  If the loop body was a GApplet,
        invokes GApplet::endRun on it.  If the loop body was a GApp2,
        invokes GApp2::endRun on it.*/
    virtual void popLoopBody();

    /**
     Executes an event loop, invoking callback repeatedly.  Put everything
     that you want to execute once per frame into the callback function.
     It is guaranteed safe to call pollEvents and all other GWindow methods
     from the callback function.
     
     The default implementation (for requiresMainLoop() == false GWindows)
     just calls the callback continuously.  Subclasses should use the
     notDone() and executeLoopBody() functions.
     */
    virtual void runMainLoop() {
        
        alwaysAssertM(requiresMainLoop() == false,
            "This GWindow implementation failed to overwrite runMainLoop "
            "even though it requires a main loop.");

        while (notDone()) {
            executeLoopBody();
        }
    }

private:
    /** Tracks the current GWindow.  If back-to-back calls are made to make
        the same GWindow current, they are ignored. */
    static const GWindow* m_current;

    friend class RenderDevice;

    class RenderDevice* m_renderDevice;

protected:

    GWindow() : m_inputCaptureCount(0), m_mouseHideCount(0), m_renderDevice(NULL) {}

    /** Override this with the glMakeCurrent call appropriate for your window.*/
    virtual void reallyMakeCurrent() const {
    }    

public:
    /** Closes the window and frees any resources associated with it.
        When subclassing, put any shutdown code (e.g. SDL_Quit()) in 
        your destructor.  Put initialization code (e.g. SDL_Init()) in
        the constructor. */
    virtual ~GWindow() {
        if (m_current == this) {
            // Once this window is destroyed, there will be no current context.
            m_current = NULL;
        }
    }

    /** 
      If a RenderDevice has been created and initialized for this window, 
      returns that renderDevice.  Otherwise returns NULL.
      */
    inline RenderDevice* renderDevice() const {
        return m_renderDevice;
    }

    inline static const GWindow* current() {
        return m_current;
    }
    /**
      Makes the OpenGL context of this window current.  If you have multiple
      windows, call this before rendering to one of them.
      <b>beta</b>
     */
    inline void makeCurrent() const {
        if (m_current != this) {
            reallyMakeCurrent();
            m_current = this;
        }
    }

};

}

#endif
