/**
  @file GEvent.h

  @maintainer Morgan McGuire, morgan@graphics3d.com
  @created 2006-10-20
  @edited  2007-01-27
*/
#ifndef G3D_GEVENT_H
#define G3D_GEVENT_H

#include "G3D/platform.h"
#include "G3D/g3dmath.h"
#include "GLG3D/GKey.h"

namespace G3D {


/* General keyboard/mouse state definitions */
enum { SDL_PRESSED = 0x01, SDL_RELEASED = 0x00 };

//////////////////////////////////////////////////////////

// SDL_joystick.h

/*
 * Get the current state of a POV hat on a joystick
 * The return value is one of the following positions:
 */
#define SDL_HAT_CENTERED	0x00
#define SDL_HAT_UP          0x01
#define SDL_HAT_RIGHT		0x02
#define SDL_HAT_DOWN		0x04
#define SDL_HAT_LEFT		0x08
#define SDL_HAT_RIGHTUP		(SDL_HAT_RIGHT|SDL_HAT_UP)
#define SDL_HAT_RIGHTDOWN	(SDL_HAT_RIGHT|SDL_HAT_DOWN)
#define SDL_HAT_LEFTUP		(SDL_HAT_LEFT|SDL_HAT_UP)
#define SDL_HAT_LEFTDOWN	(SDL_HAT_LEFT|SDL_HAT_DOWN)

/* The joystick structure used to identify an SDL joystick */
struct _SDL_Joystick;
typedef struct _SDL_Joystick SDL_Joystick;


/* Enumeration of valid key mods (possibly OR'd together) */
typedef enum {
	GKEYMOD_NONE  = 0x0000,
	GKEYMOD_LSHIFT= 0x0001,
	GKEYMOD_RSHIFT= 0x0002,
	GKEYMOD_LCTRL = 0x0040,
	GKEYMOD_RCTRL = 0x0080,
	GKEYMOD_LALT  = 0x0100,
	GKEYMOD_RALT  = 0x0200,
	GKEYMOD_LMETA = 0x0400,
	GKEYMOD_RMETA = 0x0800,
	GKEYMOD_NUM   = 0x1000,
	GKEYMOD_CAPS  = 0x2000,
	GKEYMOD_MODE  = 0x4000,
	GKEYMOD_RESERVED = 0x8000
} GKeyMod;

#define GKEYMOD_CTRL	(GKEYMOD_LCTRL  | GKEYMOD_RCTRL)
#define GKEYMOD_SHIFT	(GKEYMOD_LSHIFT | GKEYMOD_RSHIFT)
#define GKEYMOD_ALT	    (GKEYMOD_LALT   | GKEYMOD_RALT)
#define GKEYMOD_META	(GKEYMOD_LMETA  | GKEYMOD_RMETA)


/* Keysym structure
   - The scancode is hardware dependent, and should not be used by general
     applications.  If no hardware scancode is available, it will be 0.

   - The 'unicode' translated character is only available when character
     translation is enabled by the SDL_EnableUNICODE() API.  If non-zero,
     this is a UNICODE character corresponding to the keypress.  If the
     high 9 bits of the character are 0, then this maps to the equivalent
     ASCII character:
	char ch;
	if ( (keysym.unicode & 0xFF80) == 0 ) {
		ch = keysym.unicode & 0x7F;
	} else {
		An international character..
	}
 */
class SDL_keysym {
public:
    /** Hardware specific scancode */
    uint8           scancode;			

    /** G3D virtual raw key code */
    GKey::Value     sym;		

    /** Current key modifiers */
    GKeyMod         mod;	

    /** Translated character */
    uint16          unicode;
};

/* This is the mask which refers to all hotkey bindings */
#define SDL_ALL_HOTKEYS		0xFFFFFFFF

///////////////////////////////////////////////////////////

/** Event code enumerations */
class GEventType {
public:
    enum Value { 
       NONE = 0,	        /* Unused (do not remove) */
       ACTIVE,	        	/* Application loses/gains visibility */
       KEY_DOWN,	        /* Keys pressed */
       KEY_UP,	            /* Keys released */
       MOUSE_MOTION,		/* Mouse moved */
       MOUSE_BUTTON_DOWN,	/* Mouse button pressed */
       MOUSE_BUTTON_UP,		/* Mouse button released */
       JOY_AXIS_MOTION,		/* Joystick axis motion */
       JOY_BALL_MOTION,		/* Joystick trackball motion */
       JOY_HAT_MOTION,		/* Joystick hat position change */
       JOY_BUTTON_DOWN,		/* Joystick button pressed */
       JOY_BUTTON_UP,		/* Joystick button released */
       QUIT,		        /* User-requested quit */
       SYSWMEVENT,		    /* System specific event */
       EVENT_RESERVEDA,		/* Reserved for future use.. */
       EVENT_RESERVEDB,		/* Reserved for future use.. */
       VIDEO_RESIZE,		/* User resized video mode */
       VIDEO_EXPOSE,		/* Screen needs to be redrawn */
       EVENT_RESERVED2,		/* Reserved for future use.. */
       EVENT_RESERVED3,		/* Reserved for future use.. */
       EVENT_RESERVED4,		/* Reserved for future use.. */
       EVENT_RESERVED5,		/* Reserved for future use.. */
       EVENT_RESERVED6,		/* Reserved for future use.. */
       EVENT_RESERVED7,		/* Reserved for future use.. */
       GUI_DOWN,            /* GuiControl button, etc. pressed. */
       GUI_UP,              /* GuiControl button, etc. released. */
       GUI_ACTION,          /* Button fire, enter pressed in a text box, slider released, menu selecion. */
       GUI_CHANGE,          /* Text box value changed, slider dragged. */
       GUI_CANCEL,          /* esc pressed in a text box or menu */
       GUI_CLOSE,           /* GuiWindow close button pressed. */
       FILE_DROP,           /* Signifies that files have been dropped onto the program. Call 
                               GWindow.getDroppedFilenames to receive the actual data.*/

       /* This last event is only for bounding internal arrays
  	     It is the number of bits in the event mask datatype -- uint32
        */
       NUMEVENTS
    };
private:

    Value value;

public:

    G3D_DECLARE_ENUM_CLASS_METHODS(GEventType);
};

}

G3D_DECLARE_ENUM_CLASS_HASHCODE(G3D::GEventType);

namespace G3D {

/** Application visibility event structure */
class ActiveEvent {
public:
    /** GEventType::ACTIVE */
    uint8 type;

    /** Whether given states were gained or lost (1/0) */
    uint8 gain;

    /** A mask of the focus states */
    uint8 state;
};

/** Keyboard event structure */
class KeyboardEvent {
public:
    /** GEventType::KEY_DOWN or GEventType::KEY_UP */
    uint8 type;

    /** The keyboard device index */
    uint8 which;

    /** SDL_PRESSED or SDL_RELEASED */
    uint8 state;

    SDL_keysym keysym;
};


/** Mouse motion event structure */
class MouseMotionEvent {
public:
    /** SDL_MOUSEMOTION */
    uint8 type;	

    /** The mouse device index */
    uint8 which;

    /** The current button state */
    uint8 state;

    /** The X/Y coordinates of the mouse relative to the window. */
    uint16 x, y;

    /** The relative motion in the X direction.  Not supported on all platforms. */
    int16 xrel;

    /** The relative motion in the Y direction.  Not supported on all platforms. */
    int16 yrel;
};

/* Mouse button event structure */
class MouseButtonEvent {
public:
    /** MOUSEBUTTONDOWN or MOUSEBUTTONUP */
    uint8 type;

    /** The mouse device index */
    uint8 which;	
    
    /** The mouse button index */
    uint8 button;	

    /* SDL_PRESSED or SDL_RELEASED */
    uint8 state;

    /** The X/Y coordinates of the mouse at press time */
    uint16 x, y;
    
    // TODO: add     /** Current key modifiers */    GKeyMod         mod;	
};


/** Joystick axis motion event structure */
class JoyAxisEvent {
public:
    /* JOYAXISMOTION */
    uint8 type;

    /** The joystick device index */
    uint8 which;	

    /** The joystick axis index */
    uint8 axis;

    /** The axis value (range: -32768 to 32767) */
    int16 value;
};

/** Joystick trackball motion event structure */
class JoyBallEvent {
public:
    /** JOYBALLMOTION */
    uint8 type;

    /** The joystick device index */
    uint8 which;

    /** The joystick trackball index */
    uint8 ball;

    /** The relative motion in the X direction */
    int16 xrel;

    /** The relative motion in the Y direction */
    int16 yrel;
};


/** Joystick hat position change event structure */
class JoyHatEvent {
public:
    /* JOYHATMOTION */
    uint8 type;

    /** The joystick device index */
    uint8 which;

    /** The joystick hat index */
    uint8 hat;

    /** The hat position value:
        SDL_HAT_LEFTUP   SDL_HAT_UP       SDL_HAT_RIGHTUP
        SDL_HAT_LEFT     SDL_HAT_CENTERED SDL_HAT_RIGHT
        SDL_HAT_LEFTDOWN SDL_HAT_DOWN     SDL_HAT_RIGHTDOWN
        Note that zero means the POV is centered.
    */
    uint8 value;
};

/* Joystick button event structure */
class JoyButtonEvent {
public:
    /** JOYBUTTONDOWN or JOYBUTTONUP */
    uint8 type;

    /** The joystick device index */
    uint8 which;	

    /** The joystick button index */
    uint8 button;

    /** SDL_PRESSED or SDL_RELEASED */
    uint8 state;
};

/* The "window resized" event
   When you get this event, you are responsible for setting a new video
   mode with the new width and height.
 */
class ResizeEvent {
public:
    /** SDL_VIDEORESIZE */
    uint8 type;	

    /** New width */
    int w;
    
    /** New height */
    int h;
};

/**
 Triggered when one or more files are dropped onto a window.  The actual filenames dropped can be obtained by calling
 G3D::GWindow::getDroppedFilenames
*/
class FileDropEvent {
public:
    /** GEventType::FILE_DROP */
    uint8 type;
    /** Mouse position of drop */
    int   x, y;
};

/** The "screen redraw" event */
class ExposeEvent {
public:
    /** GEventType::VIDEOEXPOSE */
    uint8 type;
};


/** The "quit requested" event */
class QuitEvent {
public:
    /** GEventType::QUIT */
    uint8 type;	
};


/** A user-defined event type */
class UserEvent {
public:
    /** GEventType::USEREVENT through GEventType::NUMEVENTS-1 */
    uint8 type;

    /** User defined event code */
    int code;	

    /** User defined data pointer */
    void *data1;

    /** User defined data pointer */
    void *data2;
};


/** Events triggered by the G3D Gui system (see G3D::GuiWindow for discussion) */
class GuiEvent {
public:
    /** GEventType::GUI_UP, GEventType::GUI_DOWN, GEventType::GUI_ACTION. */
    uint8              type;

    /** The control that produced the event. */
    class GuiControl*  control;
};

/** Triggered by pressing the close button on a G3D::GuiWindow */
class GuiCloseEvent {
public:
    /** GUI_CLOSE */
    uint8              type;

    /** The GuiWindow that was closed */
    class GuiWindow*   window;
};

/* If you want to use this event, you should include SDL_syswm.h */
struct SDL_SysWMmsg;
typedef struct SDL_SysWMmsg SDL_SysWMmsg;
typedef struct SDL_SysWMEvent {
    uint8 type;
    SDL_SysWMmsg *msg;
} SDL_SysWMEvent;

/** 
  General low-level event structure.
  
  Most event processing code looks like:

  <pre>
  switch (event.type) {
  case GEventType::MOUSEBUTTONDOWN:
     ...
     break;
  ...
  }
  </pre>

  @sa G3D::GEventType, G3D::Widget::onEvent, GApp::onEvent, G3D::GWindow::pollEvent.

  @cite Based on libsdl's SDL_Event, which is based on X11 and Win32 events
 */
typedef union {
    /** This is a G3D::GEventType, but is given uint8 type so that it does not call the constructor because GEvent is a union. */
    uint8                   type;
    
    ActiveEvent             active;
    KeyboardEvent           key;
    MouseMotionEvent        motion;
    MouseButtonEvent        button;
    JoyAxisEvent            jaxis;
    JoyBallEvent            jball;
    JoyHatEvent             jhat;
    JoyButtonEvent          jbutton;
    ResizeEvent             resize;
    ExposeEvent             expose;
    QuitEvent               quit;
    UserEvent               user;
    SDL_SysWMEvent          syswm;
    GuiEvent                gui;
    GuiCloseEvent           guiClose;
    FileDropEvent           drop;
} GEvent;

}

#endif
