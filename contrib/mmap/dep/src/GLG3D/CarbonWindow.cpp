/**
  @file CarbonWindow.h
  
  @maintainer Casey O'Donnell, caseyodonnell@gmail.com
  @created 2006-08-24
  @edited  2007-04-03
*/

#include "G3D/platform.h"
#include "GLG3D/GLCaps.h"

// This file is ignored on other platforms
#ifdef G3D_OSX

#include "G3D/Log.h"
#include "GLG3D/CarbonWindow.h"
#include "GLG3D/glcalls.h"
#include "GLG3D/UserInput.h"

#define OSX_MENU_WINDOW_TITLE_HEIGHT 45

namespace G3D {

#pragma mark Private:

std::auto_ptr<CarbonWindow> CarbonWindow::_shareWindow(NULL);

namespace _internal {
pascal OSStatus OnWindowSized(EventHandlerCallRef handlerRef, EventRef event, void *userData) {
    CarbonWindow* pWindow = (CarbonWindow*)userData;
    
    if(pWindow) {
        WindowRef win = NULL;
        if(GetEventParameter(event,kEventParamDirectObject,typeWindowRef,NULL,sizeof(WindowRef),NULL,&win)==noErr) {
            Rect rect;
            if(GetWindowBounds(win, kWindowContentRgn, &rect)==noErr) {
                pWindow->injectSizeEvent(rect.right-rect.left, rect.bottom-rect.top);
                std::cout << "Resize Event (" << rect.right-rect.left << "," << rect.bottom-rect.top << ")\n";
            }
        }
    }
	
    return eventNotHandledErr;
}

pascal OSStatus OnWindowClosed(EventHandlerCallRef handlerRef, EventRef event, void *userData) {
    CarbonWindow* pWindow = (CarbonWindow*)userData;
    
    if(pWindow) {
        pWindow->_receivedCloseEvent = true;
        std::cout << "Close Event\n";
    }
    
    return eventNotHandledErr;
}

pascal OSStatus OnAppQuit(EventHandlerCallRef handlerRef, EventRef event, void *userData) {
    CarbonWindow* pWindow = (CarbonWindow*)userData;
    
    if (pWindow) {
        pWindow->_receivedCloseEvent = true;
    }
    
    return eventNotHandledErr;
}

pascal OSStatus OnActivation(EventHandlerCallRef handlerRef, EventRef event, void *userData) {
    CarbonWindow* pWindow = (CarbonWindow*)userData;
	
    if(pWindow) {
        GEvent e;
        pWindow->_windowActive = true;
        e.active.type = GEventType::ACTIVE;
        e.active.gain = 1;
        //e.active.state = SDL_APPMOUSEFOCUS|SDL_APPINPUTFOCUS|SDL_APPACTIVE;
        pWindow->fireEvent(e);
    }
    
    return eventNotHandledErr;
}

pascal OSStatus OnDeactivation(EventHandlerCallRef handlerRef, EventRef event, void *userData) {
    CarbonWindow* pWindow = (CarbonWindow*)userData;
    
    if(pWindow) {
        GEvent e;
        pWindow->_windowActive = false;
        e.active.type = GEventType::ACTIVE;
        e.active.gain = 0;
        //e.active.state = SDL_APPMOUSEFOCUS|SDL_APPINPUTFOCUS|SDL_APPACTIVE;
        pWindow->fireEvent(e);
    }

    return eventNotHandledErr;
}
} // namespace _internal

// Static Variables for Standard Event Handler Installation
bool CarbonWindow::_runApplicationEventLoopCalled = false;
EventLoopTimerRef CarbonWindow::_timer = NULL;
EventLoopTimerUPP CarbonWindow::_timerUPP = NULL;

// Static Event Type Specs
EventTypeSpec CarbonWindow::_closeSpec = {kEventClassWindow, kEventWindowClose};
EventTypeSpec CarbonWindow::_resizeSpec = {kEventClassWindow, kEventWindowResizeCompleted};
EventTypeSpec CarbonWindow::_appQuitSpec[] = {{kEventClassApplication, kEventAppTerminated},{kEventClassApplication, kEventAppQuit},{kEventClassCommand,kHICommandQuit}};
EventTypeSpec CarbonWindow::_activateSpec[] = {{kEventClassWindow, kEventWindowActivated},{kEventClassApplication, kEventAppActivated},{kEventClassWindow, kEventWindowFocusAcquired},{kEventClassApplication, kEventAppShown}};
EventTypeSpec CarbonWindow::_deactivateSpec[] = {{kEventClassWindow, kEventWindowDeactivated},{kEventClassApplication, kEventAppDeactivated},{kEventClassApplication, kEventAppHidden}};

// Static Helper Functions Prototypes
static unsigned char makeKeyEvent(EventRef,GEvent&);

pascal void CarbonWindow::quitApplicationEventLoop(EventLoopTimerRef inTimer, void* inUserData) {
	QuitApplicationEventLoop();
}

void CarbonWindow::init(WindowRef window, bool creatingShareWindow /*= false*/) {
}

void CarbonWindow::createShareWindow(GWindow::Settings s) {
	static bool hasInited = false;
	
	if (hasInited) {
		return;
	}
	
	hasInited = true;

	// We want a small (low memory), invisible window
	s.visible = false;
	s.width = 16;
	s.height = 16;
	s.framed = false;

	_shareWindow.reset(new CarbonWindow(s, true));
}

CarbonWindow::CarbonWindow(const GWindow::Settings& s, bool creatingShareWindow /*= false*/) : _createdWindow(true) {
	// If we haven't yet initialized our standard event hanlders
	// by having RunApplicationEventLoopCalled, we have to do that
	// now.
	if(!_runApplicationEventLoopCalled) {
		EventLoopRef mainLoop;
		
		mainLoop = GetMainEventLoop();
		
		_timerUPP = NewEventLoopTimerUPP(quitApplicationEventLoop);
		InstallEventLoopTimer(mainLoop,kEventDurationMicrosecond,kEventDurationForever,_timerUPP,NULL,&_timer);
		
		RunApplicationEventLoop();
		
		// Hack to get our window/process to the front...
		ProcessSerialNumber psn = { 0, kCurrentProcess};    
		TransformProcessType(&psn, kProcessTransformToForegroundApplication);
		SetFrontProcess (&psn);

		RemoveEventLoopTimer(_timer);
		_timer=NULL;
		DisposeEventLoopTimerUPP(_timerUPP);
		_timerUPP = NULL;
		
		_runApplicationEventLoopCalled = true;
	}
	
	OSStatus osErr = noErr;

	_inputCapture = false;
	_mouseVisible = true;
	_receivedCloseEvent = false;
	_windowActive = true;
	_settings = s;	
	
	GLint attribs[100];
	int i = 0;
	
	attribs[i++] = AGL_RED_SIZE;			attribs[i++] = _settings.rgbBits;
	attribs[i++] = AGL_GREEN_SIZE;			attribs[i++] = _settings.rgbBits;
	attribs[i++] = AGL_BLUE_SIZE;			attribs[i++] = _settings.rgbBits;
	attribs[i++] = AGL_ALPHA_SIZE;			attribs[i++] = _settings.alphaBits;

	attribs[i++] = AGL_DEPTH_SIZE;			attribs[i++] = _settings.depthBits;
	attribs[i++] = AGL_STENCIL_SIZE;		attribs[i++] = _settings.stencilBits;

	if (_settings.stereo)					attribs[i++] = AGL_STEREO;

	// use component colors, not indexed colors
	attribs[i++] = AGL_RGBA;				attribs[i++] = GL_TRUE;
	attribs[i++] = AGL_WINDOW;				attribs[i++] = GL_TRUE;
	attribs[i++] = AGL_DOUBLEBUFFER;		attribs[i++] = GL_TRUE;
	attribs[i++] = AGL_PBUFFER;				attribs[i++] = GL_TRUE;
	attribs[i++] = AGL_NO_RECOVERY;			attribs[i++] = GL_TRUE;
	attribs[i++] = AGL_NONE;
	attribs[i++] = NULL;
	
	AGLPixelFormat format;
	
	// If the user is creating a windowed application that is above the menu
	// bar, it will be confusing. We'll move it down for them so it makes more
	// sense.
	if(! _settings.fullScreen) {
		if(_settings.y <= OSX_MENU_WINDOW_TITLE_HEIGHT)
			_settings.y = OSX_MENU_WINDOW_TITLE_HEIGHT;
	}

	Rect rWin = {_settings.y, _settings.x, _settings.height+_settings.y, _settings.width+_settings.x};
	
	_title = _settings.caption;

	CFStringRef strRef = CFStringCreateWithCString(kCFAllocatorDefault, _title.c_str(), kCFStringEncodingUnicode);
	
	_window = NewCWindow(NULL, &rWin, CFStringGetPascalStringPtr(strRef, kCFStringEncodingUnicode), _settings.visible, zoomDocProc, (WindowPtr) -1L, true, 0);

	osErr = InstallStandardEventHandler(GetWindowEventTarget(_window));
	osErr = InstallWindowEventHandler(_window, NewEventHandlerUPP(_internal::OnWindowSized), GetEventTypeCount(_resizeSpec), &_resizeSpec, this, NULL);
	osErr = InstallWindowEventHandler(_window, NewEventHandlerUPP(_internal::OnWindowClosed), GetEventTypeCount(_closeSpec), &_closeSpec, this, NULL);
    osErr = InstallWindowEventHandler(_window, NewEventHandlerUPP(_internal::OnAppQuit), GetEventTypeCount(_appQuitSpec), _appQuitSpec, this, NULL);
    osErr = InstallWindowEventHandler(_window, NewEventHandlerUPP(_internal::OnActivation), GetEventTypeCount(_activateSpec), _activateSpec, this, NULL);
    osErr = InstallWindowEventHandler(_window, NewEventHandlerUPP(_internal::OnDeactivation), GetEventTypeCount(_deactivateSpec), _deactivateSpec, this, NULL);
	CFRelease(strRef);

	_glDrawable = (AGLDrawable) GetWindowPort(_window);
	
	format = aglChoosePixelFormat(NULL,0,attribs);
	
	_glContext = aglCreateContext(format,NULL/*TODO: Share context*/);
	
	aglSetDrawable(_glContext, _glDrawable);
	aglSetCurrentContext(_glContext);
}

CarbonWindow::CarbonWindow(const GWindow::Settings& s, WindowRef window) : _createdWindow(false) {
}

#pragma mark Public:

CarbonWindow* CarbonWindow::create(const GWindow::Settings& s /*= GWindow::Settings()*/) {
	return new CarbonWindow(s);
}

CarbonWindow* CarbonWindow::create(const GWindow::Settings& s, WindowRef window) {
	return new CarbonWindow(s, window);
}

CarbonWindow::~CarbonWindow() {
	aglSetCurrentContext(NULL);
	aglDestroyContext(_glContext);
	
	if(_createdWindow)
		DisposeWindow(_window);
}

std::string CarbonWindow::getAPIVersion() const {
	return "0.1";
}

std::string CarbonWindow::getAPIName() const {
	return "Carbon";
}

void CarbonWindow::getSettings(GWindow::Settings& s) const {
	s = _settings;
}

int CarbonWindow::width() const {
	return _settings.width;
}

int CarbonWindow::height() const {
	return _settings.height;
}

Rect2D CarbonWindow::dimensions() const {
	return Rect2D::xyxy(_settings.x,_settings.y,_settings.x+_settings.width,_settings.y+_settings.height);
}

void CarbonWindow::setDimensions(const Rect2D &dims) {
}

void CarbonWindow::getDroppedFilenames(Array<std::string>& files) {
}

bool CarbonWindow::hasFocus() const {
	return true;
}

void CarbonWindow::setGammaRamp(const Array<uint16>& gammaRamp) {
}

void CarbonWindow::setCaption(const std::string& title) {
	_title = title;
	CFStringRef strRef = CFStringCreateWithCString(kCFAllocatorDefault, _title.c_str(), kCFStringEncodingUnicode);
	
	SetWindowTitleWithCFString(_window,strRef);

	CFRelease(strRef);
}

std::string CarbonWindow::caption() {
	return _title;
}

int CarbonWindow::numJoysticks() const {
	return 0;
}

std::string CarbonWindow::joystickName(unsigned int stickNum) {
	return "";
}

void CarbonWindow::notifyResize(int w, int h) {
	_settings.width = w;
	_settings.height = h;
}

void CarbonWindow::setRelativeMousePosition(double x, double y) {
    CGPoint point;

    point.x = x + _settings.x;
    point.y = y + _settings.y;
    
    CGSetLocalEventsSuppressionInterval(0.0);
    CGWarpMouseCursorPosition(point);
}


void CarbonWindow::setRelativeMousePosition(const Vector2 &p) {
    setRelativeMousePosition(p.x, p.y);
}


void CarbonWindow::getRelativeMouseState(Vector2& position, uint8 &mouseButtons) const {
    int x, y;
    getRelativeMouseState(x, y, mouseButtons);
    position.x = x;
    position.y = y;
}


void CarbonWindow::getRelativeMouseState(int &x, int &y, uint8 &mouseButtons) const {
    Point point;
    GetGlobalMouse(&point);
    
    x = point.h - _settings.x;
    y = point.v - _settings.y;
    
    mouseButtons = buttonsToUint8(_mouseButtons);
}


void CarbonWindow::getRelativeMouseState(double &x, double &y, uint8 &mouseButtons) const {
    int ix, iy;
    getRelativeMouseState(ix, iy, mouseButtons);
    x = ix;
    y = iy;
}
void CarbonWindow::getJoystickState(unsigned int stickNum, Array<float> &axis, Array<bool> &buttons) {
}

void CarbonWindow::setInputCapture(bool c) {
	_inputCapture = c;
}

bool CarbonWindow::inputCapture() const {
	return _inputCapture;
}

void CarbonWindow::setMouseVisible(bool b) {
    if(_mouseVisible == b)
        return;
    
    _mouseVisible = b;
    
    if(_mouseVisible)
        CGDisplayShowCursor(kCGDirectMainDisplay);
    else
        CGDisplayHideCursor(kCGDirectMainDisplay);
}

bool CarbonWindow::mouseVisible() const {
	return _mouseVisible;
}

void CarbonWindow::swapGLBuffers() {
	if(_glContext) {
		aglSetCurrentContext(_glContext);
		aglSwapBuffers(_glContext);
	}
}

#pragma mark Private - CarbonWindow - Mouse Event Generation:

bool CarbonWindow::makeMouseEvent(EventRef theEvent, GEvent& e) {
    UInt32 eventKind = GetEventKind(theEvent);
    HIPoint point;
    EventMouseButton button;
    Rect rect, rectGrow;
    
    GetEventParameter(theEvent, kEventParamMouseLocation, typeHIPoint, NULL, sizeof(HIPoint), NULL, &point);
    
    if(GetWindowBounds(_window, kWindowContentRgn, &rect)==noErr) {
        // If we've captured the mouse, we want to force the mouse to stay in our window region.
        if(_inputCapture) {
            bool reposition = false;
            CGPoint newPoint = point;
            
            if(point.x < rect.left) {
                newPoint.x = rect.left;
                reposition = true;
            }
            if(point.x > rect.right) {
                newPoint.x = rect.right;
                reposition = true;
            }
            if(point.y < rect.top) {
                newPoint.y = rect.top;
                reposition = true;
            }
            if(point.y > rect.bottom) {
                newPoint.y = rect.bottom;
                reposition = true;
            }
            
            if(reposition) {
                CGWarpMouseCursorPosition(newPoint);
                point = newPoint;
            }
        }
	
        // If the mouse event didn't happen in our content region, then
        // we want to punt it to other event handlers. (Return FALSE)
        if((point.x >= rect.left) && (point.y >= rect.top) && (point.x <= rect.right) && (point.y <= rect.bottom)) {
            // If the user wants to resize, we should allow them to.
            GetWindowBounds(_window, kWindowGrowRgn, &rectGrow);
            if(!_settings.fullScreen && _settings.resizable && ((point.x >= rectGrow.left) && (point.y >= rectGrow.top)))
                return false;
            
            GetEventParameter(theEvent, kEventParamMouseButton, typeMouseButton, NULL, sizeof(button), NULL, &button);
            
            switch (eventKind) {
            case kEventMouseDown:
            case kEventMouseUp:
                e.type = ((eventKind == kEventMouseDown) || (eventKind == kEventMouseDragged)) ? GEventType::MOUSE_BUTTON_DOWN : GEventType::MOUSE_BUTTON_UP;
                e.button.x = point.x-rect.left;
                e.button.y = point.y-rect.top;
                
                // Mouse button index
                e.button.which = 0;		// TODO: Which Mouse is Being Used?
                e.button.state = ((eventKind == kEventMouseDown) || (eventKind == kEventMouseDragged)) ? SDL_PRESSED : SDL_RELEASED;
                
                if(kEventMouseButtonPrimary == button) {
                    e.button.button = 0;
                    _mouseButtons[0] = (eventKind == kEventMouseDown);
                } else if(kEventMouseButtonTertiary == button) {
                    e.button.button = 1;
                    _mouseButtons[2] = (eventKind == kEventMouseDown);
                } else if (kEventMouseButtonSecondary == button) {
                    e.button.button = 2;
                    _mouseButtons[1] = (eventKind == kEventMouseDown);
                }
                return true;
            case kEventMouseDragged:
            case kEventMouseMoved:
                e.motion.type = GEventType::MOUSE_MOTION;
                e.motion.which = 0;		// TODO: Which Mouse is Being Used?
                e.motion.state = buttonsToUint8(_mouseButtons);
                e.motion.x = point.x-rect.left;
                e.motion.y = point.y-rect.top;
                e.motion.xrel = 0;
                e.motion.yrel = 0;
                return true;
            default:
                break;
            }
        } else if(!_settings.fullScreen && eventKind == kEventMouseDown) {
            // We want to indentify and handle menu events too
            // because we don't have the standard event handlers.
            Point thePoint;
            WindowRef theWindow;
            thePoint.v = point.y;
            thePoint.h = point.x;
            WindowPartCode partCode = FindWindow(thePoint, &theWindow);
            if(partCode == inMenuBar) {
                long iVal = MenuSelect(thePoint);
                short iID = HiWord(iVal);
                short iItem = LoWord(iVal);
                HiliteMenu(iID);
                
                if(iItem == 9)
                    _receivedCloseEvent = true;
            }
        }
    }

    return false;
}

#pragma mark Protected:

bool CarbonWindow::pollOSEvent(GEvent &e) {
	EventRef		theEvent;
	EventTargetRef	theTarget;
	OSStatus osErr = noErr;
	
	osErr = ReceiveNextEvent(0, NULL,kEventDurationNanosecond,true, &theEvent);
	
	// If we've gotten no event, we should just return false so that
	// a render pass can occur.
	if(osErr == eventLoopTimedOutErr)
		return false;

	UInt32 eventClass = GetEventClass(theEvent);
	UInt32 eventKind = GetEventKind(theEvent);
	
	switch(eventClass) { 
		case kEventClassMouse:	
            if (_windowActive) {
                if (makeMouseEvent(theEvent, e)) {
                    return true;
                }
            }

//			OSStatus result = CallNextEventHandler(handlerRef, event);	
//			if (eventNotHandledErr != result) return result;
			/*else*/
			switch (eventKind) {
				case kEventMouseDown:
				case kEventMouseUp:
				case kEventMouseDragged:
				case kEventMouseMoved:
				case kEventMouseWheelMoved:
				default:
					break;
			} break;
		case kEventClassKeyboard:
			switch (eventKind) {
				case kEventRawKeyDown:
				case kEventRawKeyRepeat:
					
					e.key.type = GEventType::KEY_DOWN;
					e.key.state = SDL_PRESSED;
					
					_keyboardButtons[makeKeyEvent(theEvent, e)] = true;
					
					return true;
					
				case kEventRawKeyUp:
				
					e.key.type = GEventType::KEY_UP;
					e.key.state = SDL_RELEASED;
					
					_keyboardButtons[makeKeyEvent(theEvent, e)] = false;
					
					return true;
				
				case kEventRawKeyModifiersChanged:
				case kEventHotKeyPressed:
				case kEventHotKeyReleased:
				default:
					break;
			} break;
		case kEventClassWindow:
			switch (eventKind) {
				case kEventWindowCollapsing:
				case kEventWindowActivated:
				case kEventWindowDrawContent:
				case kEventWindowClose:
				case kEventWindowShown:
				case kEventWindowBoundsChanged:
				case kEventWindowZoomed:
				default:
					break;
			}  break;
		case kEventClassCommand:
			switch (eventKind) {
				case kEventCommandProcess:
				case kEventCommandUpdateStatus:
				default:
					break;
			} break;
		case kEventClassTablet:
		case kEventClassApplication:
		case kEventClassMenu:
		case kEventClassTextInput:
		case kEventClassAppleEvent:
		case kEventClassControl:
		case kEventClassVolume:
		case kEventClassAppearance:
		case kEventClassService:
		case kEventClassToolbar:
		case kEventClassToolbarItem:
		case kEventClassToolbarItemView:
		case kEventClassAccessibility:
		case kEventClassSystem:
		case kEventClassInk:
		case kEventClassTSMDocumentAccess:
		default:
			break;
	}
	
	if(_receivedCloseEvent)
	{
		_receivedCloseEvent = false;
		e.type = GEventType::QUIT;
		std::cout << "Close Event Sent\n";
		return true;
	}
	
	Rect rect;
	if(GetWindowBounds(_window, kWindowStructureRgn, &rect)==noErr) {
		_settings.x = rect.left;
		_settings.y = rect.top;
	}
	
	if(GetWindowBounds(_window, kWindowContentRgn, &rect)==noErr) {
		_settings.width = rect.right-rect.left;
		_settings.height = rect.bottom-rect.top;
	}
	
    if (_sizeEventInjects.size() > 0) {
        e = _sizeEventInjects.last();
        _sizeEventInjects.clear();
		aglSetCurrentContext(_glContext);
		aglUpdateContext(_glContext);
		std::cout << "Resize Event Sent (" << e.resize.w << "," << e.resize.h << ")\n";
        return true;
    }

	if(osErr == noErr) {
		theTarget = GetEventDispatcherTarget();	
		SendEventToEventTarget (theEvent, theTarget);
		ReleaseEvent(theEvent);
	}
	
	return false;
}

/** Fills out @e and returns the index of the key for use with _keyboardButtons.*/
unsigned char CarbonWindow::makeKeyEvent(EventRef theEvent, GEvent& e) {
    UniChar uc;
    unsigned char c;
    UInt32 key;
    UInt32 modifiers;
    
    KeyMap keyMap;
    unsigned char * keyBytes;
    enum {
        /* modifier keys (TODO: need to determine right command key value) */
        kVirtualLShiftKey = 0x038,
        kVirtualLControlKey = 0x03B,
        kVirtualLOptionKey = 0x03A,
        kVirtualRShiftKey = 0x03C,
        kVirtualRControlKey = 0x03E,
        kVirtualROptionKey = 0x03D,
        kVirtualCommandKey = 0x037
    };
    
    GetEventParameter(theEvent, kEventParamKeyUnicodes, typeUnicodeText, NULL, sizeof(uc), NULL, &uc);
    GetEventParameter(theEvent, kEventParamKeyMacCharCodes, typeChar, NULL, sizeof(c), NULL, &c);
    GetEventParameter(theEvent, kEventParamKeyCode, typeUInt32, NULL, sizeof(key), NULL, &key);
    GetEventParameter(theEvent, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(modifiers), NULL, &modifiers);

    GetKeys(keyMap);
    keyBytes = (unsigned char *)keyMap;
    
    e.key.keysym.scancode = key;
    e.key.keysym.unicode = uc;
    e.key.keysym.mod = GKEYMOD_NONE;

    if (modifiers & shiftKey) {
        if (keyBytes[kVirtualLShiftKey >> 3] & (1 << (kVirtualLShiftKey & 7))) {
            e.key.keysym.mod = (GKeyMod)(e.key.keysym.mod | GKEYMOD_LSHIFT);
            //debugPrintf(" LSHIFT ");
        }

        if (keyBytes[kVirtualRShiftKey >> 3] & (1 << (kVirtualRShiftKey & 7))) {
            e.key.keysym.mod = (GKeyMod)(e.key.keysym.mod | GKEYMOD_RSHIFT);
            //debugPrintf(" RSHIFT ");
        }
    }
	
    if (modifiers & controlKey) {
        if (keyBytes[kVirtualLControlKey >> 3] & (1 << (kVirtualLControlKey & 7))) {
            e.key.keysym.mod = (GKeyMod)(e.key.keysym.mod | GKEYMOD_LCTRL);
            //debugPrintf(" LCTRL ");
        }
        
        if (keyBytes[kVirtualRControlKey >> 3] & (1 << (kVirtualRControlKey & 7))) {
            e.key.keysym.mod = (GKeyMod)(e.key.keysym.mod | GKEYMOD_RCTRL);
            //debugPrintf(" RCTRL ");
        }
    }
    
    if (modifiers & optionKey) {
        if (keyBytes[kVirtualLOptionKey >> 3] & (1 << (kVirtualLOptionKey & 7))) {
            e.key.keysym.mod = (GKeyMod)(e.key.keysym.mod | GKEYMOD_LALT);
            //debugPrintf(" LALT ");
        }

        if(keyBytes[kVirtualROptionKey >> 3] & (1 << (kVirtualROptionKey & 7))) {
            e.key.keysym.mod = (GKeyMod)(e.key.keysym.mod | GKEYMOD_RALT);
            //            debugPrintf(" RALT ");
	}
    }

    if (modifiers & cmdKey) {
        e.key.keysym.mod = (GKeyMod)(e.key.keysym.mod | GKEYMOD_LMETA);
        //debugPrintf(" LMETA ");
    }

    /*
      The FN key is weird on MacBook Pro--it claims to be pressed down
      whenever a function key is pressed.  Trapping this is undesirable because
      it doesn't match other OSes, so we ignore it.

    if (modifiers & kEventKeyModifierFnMask) {
        e.key.keysym.mod = (GKeyMod::Value)(e.key.keysym.mod | GKeyMod::MODE);
    }
    */
    
    if (modifiers & alphaLock) {
        e.key.keysym.mod = (GKeyMod)(e.key.keysym.mod | GKEYMOD_CAPS);
        //debugPrintf(" CAPS ");
    }

    if (modifiers & kEventKeyModifierNumLockMask) {
        e.key.keysym.mod = (GKeyMod)(e.key.keysym.mod | GKEYMOD_NUM);
        //debugPrintf(" NUM ");
    }
    
    // If c is 0, then we've actually recieved a modifier key event,
    // which takes a little more logic to figure out. Especially since
    // under Carbon there isn't a distinction between right/left hand
    // versions of keys.  In all other cases, we have a "normal" key.

    if (c != 0) {
        //debugPrintf("\nRaw code = %d\n", key);
        // Non-modifier key
        switch(key) {
        case 122: e.key.keysym.sym = GKey::F1;    break;
        case 120: e.key.keysym.sym = GKey::F2;    break;
        case 99:  e.key.keysym.sym = GKey::F3;    break;
        case 118: e.key.keysym.sym = GKey::F4;    break;
        case 96:  e.key.keysym.sym = GKey::F5;    break;
        case 97:  e.key.keysym.sym = GKey::F6;    break;
        case 98:  e.key.keysym.sym = GKey::F7;    break;
        case 100: e.key.keysym.sym = GKey::F8;    break;
        case 101: e.key.keysym.sym = GKey::F9;    break;
        case 109: e.key.keysym.sym = GKey::F10;   break;
        case 103: e.key.keysym.sym = GKey::F11;   break;
        case 111: e.key.keysym.sym = GKey::F12;   break;

        case 119: e.key.keysym.sym = GKey::END;   break;
        case 115: e.key.keysym.sym = GKey::HOME;  break;
        case 116: e.key.keysym.sym = GKey::PAGEUP; break;
        case 121: e.key.keysym.sym = GKey::PAGEDOWN; break;
        case 123: e.key.keysym.sym = GKey::LEFT;   break;
        case 124: e.key.keysym.sym = GKey::RIGHT;  break;
        case 126: e.key.keysym.sym = GKey::UP;     break;
        case 125: e.key.keysym.sym = GKey::DOWN;   break;

        default:
            if ((c >= 'A') && (c <= 'Z')) {
                // Capital letter; make canonically lower case
                e.key.keysym.sym = (GKey::Value)(c - 'A' + 'a');
            } else {
                e.key.keysym.sym = (GKey::Value)c;
            }
        }
        //debugPrintf("Sym code = %d\n", e.key.keysym.sym);

    } else {
        // Modifier key

        // We must now determine what changed since last time and see
        // if we've got a key-up or key-down. The assumption is a
        // key down, so we must only set e.key.type = GEventType::KEY_UP
        // if we've lost a modifier.
/* temporary disabled 
        if (lastMod > e.key.keysym.mod) {
            // Lost a modifier key (bit)
            e.key.type = GEventType::KEY_UP;
            e.key.state = SDL_RELEASED;
        } else {
            e.key.type = GEventType::KEY_DOWN;
            e.key.state = SDL_PRESSED;
        }
        
        //printf(" lastMod = 0x%x, mod = 0x%x, xor = 0x%x ", lastMod, 
        //       e.key.keysym.mod, e.key.keysym.mod ^ lastMod);

        // Find out which bit flipped
        switch (e.key.keysym.mod ^ lastMod) {
        case GKeyMod::LSHIFT:
            e.key.keysym.sym = GKey::LSHIFT;
            break;
        case GKeyMod::RSHIFT:
            e.key.keysym.sym = GKey::RSHIFT;
            break;
        case GKeyMod::LCTRL:
            e.key.keysym.sym = GKey::LCTRL;
            break;
        case GKeyMod::RCTRL:
            e.key.keysym.sym = GKey::RCTRL;
            break;
        case GKeyMod::LALT:
            e.key.keysym.sym = GKey::LALT;
            break;
        case GKeyMod::RALT:
            e.key.keysym.sym = GKey::RALT;
            break;
        }*/
    }
    // temporary dasabled : lastMod = e.key.keysym.mod;
    
    return e.key.keysym.sym;
}


uint8 buttonsToUint8(const bool* buttons) {
    uint8 mouseButtons = 0;
    // Clear mouseButtons and set each button bit.
    mouseButtons |= (buttons[0] ? 1 : 0) << 0;
    mouseButtons |= (buttons[1] ? 1 : 0) << 1;
    mouseButtons |= (buttons[2] ? 1 : 0) << 2;
    mouseButtons |= (buttons[3] ? 1 : 0) << 4;
    mouseButtons |= (buttons[4] ? 1 : 0) << 8;
    return mouseButtons;
}

} // namespace G3D

#endif // G3D_OSX
