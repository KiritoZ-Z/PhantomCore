/**
  @file UserInput.cpp
 
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2002-09-29
  @edited  2004-09-05
 */

#include "GLG3D/UserInput.h"
#include "GLG3D/RenderDevice.h"

namespace G3D {


UserInput::UserInput(
    GWindow*                          window) {
    init(window, NULL);
}


void UserInput::init(
    GWindow*                    window,
    Table<GKey, UIFunction>* keyMapping) {

    _pureDeltaMouse = false;
    deltaMouse = Vector2(0, 0);

    alwaysAssertM(window != NULL, "Window must not be NULL");

    _window = window;
    windowCenter = Vector2((float)window->width() / 2.0f, (float)window->height() / 2.0f);

	up = left = down = right = false;
	jx = jy = 0.0;

    inEventProcessing = false;

    bool tempMapping = (keyMapping == NULL);

    if (tempMapping) {
        keyMapping = new Table<GKey, UIFunction>();
        keyMapping->set(GKey::RIGHT, RIGHT);
        keyMapping->set(GKey::LEFT, LEFT);
        keyMapping->set(GKey::UP, UP);
        keyMapping->set(GKey::DOWN, DOWN);
        keyMapping->set('d', RIGHT);
        keyMapping->set('a', LEFT);
        keyMapping->set('w', UP);
        keyMapping->set('s', DOWN);
    }

    // Will be initialized by setKeyMapping don't need to memset
    keyState.resize(GKey::LAST);
    
    keyFunction.resize(keyState.size());
    setKeyMapping(keyMapping);

    if (tempMapping) {
        delete keyMapping;
        keyMapping = NULL;
    }

    useJoystick = _window->numJoysticks() > 0;
    _window->getRelativeMouseState(mouse, mouseButtons);
    guiMouse = mouse;

    appHadFocus = _window->hasFocus();
}


GWindow* UserInput::window() const {
    return _window;
}


void UserInput::setKeyMapping(
    Table<GKey, UIFunction>* keyMapping) {

    for (GKey i = (GKey)(keyState.size() - 1); (int)i >= 0; i = (GKey)(i - 1)) {
        keyState[(int)i]    = false;
        if (keyMapping->containsKey(i)) {
            keyFunction[(int)i] = keyMapping->get(i);
        } else {
            keyFunction[(int)i] = NONE;
        }
    }
}


UserInput::~UserInput() {
}


void UserInput::processEvent(const GEvent& event) {
    
    debugAssert(inEventProcessing);
    // Translate everything into a key code then call processKey

    switch(event.type) {
    case GEventType::KEY_UP:
        processKey(event.key.keysym.sym, GEventType::KEY_UP);
        break;
        
    case GEventType::KEY_DOWN:
        processKey(event.key.keysym.sym, GEventType::KEY_DOWN);
        break;
        
    case GEventType::MOUSE_BUTTON_DOWN:
        processKey((GKey)(GKey::LEFT_MOUSE + event.button.button), GEventType::KEY_DOWN);
        break;
        
    case GEventType::MOUSE_BUTTON_UP:
        processKey(GKey(GKey::LEFT_MOUSE + event.button.button), GEventType::KEY_UP);
        break;
    }
}


void UserInput::beginEvents() {
    debugAssert(! inEventProcessing);
    inEventProcessing = true;
    justPressed.resize(0, DONT_SHRINK_UNDERLYING_ARRAY);
    justReleased.resize(0, DONT_SHRINK_UNDERLYING_ARRAY);
}


void UserInput::endEvents() {
    debugAssert(inEventProcessing);

    inEventProcessing = false;
    if (useJoystick) {
        static Array<float> axis;
        static Array<bool> button;
        _window->getJoystickState(0, axis, button);

        if (axis.size() >= 2) {
            jx = axis[0];
            jy = -axis[1];
        }
    }

    windowCenter =
        Vector2((float)window()->width(), (float)window()->height()) / 2.0f;

    Vector2 oldMouse = mouse;
    _window->getRelativeMouseState(mouse, mouseButtons);

    if ((mouse.x < 0) || (mouse.x > 10000)) {
        // Sometimes we get bad values on the first frame;
        // ignore them.
        mouse = oldMouse;
    }

    deltaMouse = mouse - oldMouse;

    bool focus = _window->hasFocus();

    if (_pureDeltaMouse) {

        // Reset the mouse periodically.  We can't do this every
        // frame or the mouse will not be able to move substantially
        // at high frame rates.
        if ((mouse.x < windowCenter.x * 0.5) || (mouse.x > windowCenter.x * 1.5) ||
            (mouse.y < windowCenter.y * 0.5) || (mouse.y > windowCenter.y * 1.5)) {
        
            mouse = windowCenter;
            if (focus) {
                setMouseXY(mouse);
            }
        }

        // Handle focus-in and focus-out gracefully
        if (focus && ! appHadFocus) {
            // We just gained focus.
            grabMouse();
        } else if (! focus && appHadFocus) {
            // Just lost focus
            releaseMouse();
        }
    } else {
        guiMouse = mouse;
    }

    appHadFocus = focus;
}


Vector2 UserInput::getXY() const {
    return Vector2(getX(), getY());
}


float UserInput::getX() const {

	if (left && !right) {
		return -1.0f;
	} else if (right && !left) {
		return 1.0f;
	}

	if (useJoystick && (fabs(jx) > 0.1)) {
		return (float)jx;
	}

	return 0.0f;
}


float UserInput::getY() const {
	if (down && !up) {
		return -1.0f;
	} else if (up && !down) {
		return 1.0f;
	}

	if (useJoystick && (fabs(jy) > 0.1)) {
		return (float)jy;
	}

	return 0.0f;
}


void UserInput::processKey(GKey code, int event) {
	bool state = (event == GEventType::KEY_DOWN);

    if (code < GKey(keyFunction.size())) {
        switch (keyFunction[(int)code]) {
        case RIGHT:
            right = state;
            break;

        case LEFT:
            left = state;
            break;

        case UP:
            up = state;
            break;

        case DOWN:
            down = state;
            break;

        case NONE:
            break;
	    }

        keyState[(int)code] = state;

        if (state) {
            justPressed.append(code);
        } else {
            justReleased.append(code);
        }
    }
}


float UserInput::mouseDX() const {
    return deltaMouse.x;
}


float UserInput::mouseDY() const {
    return deltaMouse.y;
}


Vector2 UserInput::mouseDXY() const {
    return deltaMouse;
}


void UserInput::setMouseXY(float x, float y) {
    mouse.x = x;
    mouse.y = y;
    _window->setRelativeMousePosition(mouse);
}


int UserInput::getNumJoysticks() const {
    return _window->numJoysticks();
}


bool UserInput::keyDown(GKey code) const {
    if (code > GKey::LAST) {
        return false;
    } else {
        return keyState[(int)code];
    }
}


bool UserInput::keyPressed(GKey code) const {
    for (int i = justPressed.size() - 1; i >= 0; --i) {
        if (code == justPressed[i]) {
            return true;
        }
    }

    return false;
}


bool UserInput::keyReleased(GKey code) const {
    for (int i = justReleased.size() - 1; i >= 0; --i) {
        if (code == justReleased[i]) {
            return true;
        }
    }

    return false;
}


void UserInput::pressedKeys(Array<GKey>& code) const {
    code.resize(justPressed.size());
    memcpy(code.getCArray(), justPressed.getCArray(), sizeof(GKey) * justPressed.size());
}


void UserInput::releasedKeys(Array<GKey>& code) const {
    code.resize(justReleased.size());
    memcpy(code.getCArray(), justReleased.getCArray(), sizeof(GKey) * justReleased.size());
}


bool UserInput::anyKeyPressed() const {
    return (justPressed.size() > 0);
}


bool UserInput::pureDeltaMouse() const {
    return _pureDeltaMouse;
}


void UserInput::grabMouse() {
    uint8 dummy;
    // Save the old mouse position for when we deactivate
    _window->getRelativeMouseState(guiMouse, dummy);
    
    mouse = windowCenter;
    _window->setRelativeMousePosition(mouse);
    deltaMouse = Vector2(0, 0);
 
    window()->incMouseHideCount();

    #ifndef G3D_DEBUG
        // In debug mode, don't grab the cursor because
        // it is annoying when you hit a breakpoint and
        // can't move the mouse.
        window()->incInputCaptureCount();
    #endif
}


void UserInput::releaseMouse() {
    #ifndef G3D_DEBUG
        // In debug mode, don't grab the cursor because
        // it is annoying when you hit a breakpoint and
        // cannot move the mouse.
        window()->decInputCaptureCount();
    #endif

    // Restore the old mouse position
    setMouseXY(guiMouse);

    window()->decMouseHideCount();
}


void UserInput::setPureDeltaMouse(bool m) {
    if (_pureDeltaMouse != m) {
        _pureDeltaMouse = m;

        if (_pureDeltaMouse) {
            grabMouse();
        } else {
            releaseMouse();
        }
    }
}


}
