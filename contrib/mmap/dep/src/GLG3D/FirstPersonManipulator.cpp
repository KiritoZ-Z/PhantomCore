/**
  @file FirstPersonManipulator.cpp

  @maintainer Morgan McGuire, morgan@cs.brown.edu

  @created 2002-07-28
  @edited  2006-08-10
*/

#include "G3D/platform.h"

#include "G3D/Rect2D.h"
#include "GLG3D/FirstPersonManipulator.h"
#include "GLG3D/GWindow.h"
#include "GLG3D/UserInput.h"

namespace G3D {

FirstPersonManipulatorRef FirstPersonManipulator::create() {
    return new FirstPersonManipulator();
}

FirstPersonManipulator::FirstPersonManipulator() : 
    maxMoveRate(10),
    maxTurnRate(20),
    m_yaw(0),
    m_pitch(0),
    _active(false),
    m_mouseMode(MOUSE_DIRECT) {
}


void FirstPersonManipulator::getFrame(CoordinateFrame& c) const {
	c.translation = translation;
    c.rotation = Matrix3::fromEulerAnglesZYX(0, -m_yaw, -m_pitch);
    debugAssert(isFinite(c.rotation[0][0]));
}


CoordinateFrame FirstPersonManipulator::frame() const {
	CoordinateFrame c;
	getFrame(c);
	return c;
}


FirstPersonManipulator::~FirstPersonManipulator() {
}


FirstPersonManipulator::MouseMode FirstPersonManipulator::mouseMode() const {
    return m_mouseMode;
}


void FirstPersonManipulator::setMouseMode(FirstPersonManipulator::MouseMode m) {
    if (m_mouseMode != m) {
        bool wasActive = active();

        if (wasActive) {
            // Toggle activity to let the cursor and 
            // state variables reset.
            setActive(false);

        }

        m_mouseMode = m;

        if (wasActive) {
            setActive(true);
        }
    }
}


bool FirstPersonManipulator::active() const {
    return _active;
}


void FirstPersonManipulator::reset() {
    _active      = false;
    m_yaw        = -halfPi();
    m_pitch      = 0;
    translation  = Vector3::zero();
    setMoveRate(10);

#   ifdef G3D_OSX
        // OS X has a really slow mouse by default
        setTurnRate(pi() * 12);
#   else
        setTurnRate(pi() * 5);
#   endif
}

bool FirstPersonManipulator::rightDown(UserInput* ui) const {
#   ifdef G3D_OSX
    // Treat trackpad wheel click as right mouse button
       return 
           userInput->keyDown(GKey::RIGHT_MOUSE) || 
           (userInput->keyDown(GKey::LEFT_MOUSE) && 
            (
             userInput->keyDown(GKey::LSHIFT) ||
             userInput->keyDown(GKey::RSHIFT) ||
             userInput->keyDown(GKey::LCTRL) ||
             userInput->keyDown(GKey::RCTRL)));
#   else
       return userInput->keyDown(GKey::RIGHT_MOUSE);
#   endif
}

void FirstPersonManipulator::setActive(bool a) {
    if (_active == a) {
        return;
    }
    _active = a;

    switch (m_mouseMode) {
    case MOUSE_DIRECT:
        userInput->setPureDeltaMouse(_active);
        break;

    case MOUSE_DIRECT_RIGHT_BUTTON:
        // Only turn on when activeand the right mouse button is down
        userInput->setPureDeltaMouse(_active && rightDown(userInput));
        break;

    case MOUSE_SCROLL_AT_EDGE:
    case MOUSE_PUSH_AT_EDGE:        
        userInput->setPureDeltaMouse(false);
        if (_active) {
            userInput->window()->incInputCaptureCount();
        } else {
            userInput->window()->decInputCaptureCount();
        }
        break;

    default:
        debugAssert(false);
    }
}


void FirstPersonManipulator::setMoveRate(double metersPerSecond) {
    maxMoveRate = metersPerSecond;
}


void FirstPersonManipulator::setTurnRate(double radiansPerSecond) {
    maxTurnRate = radiansPerSecond;
}


void FirstPersonManipulator::lookAt(
    const Vector3&      position) {

    const Vector3 look = (position - translation);

    m_yaw   = aTan2(look.x, -look.z);
    m_pitch = -aTan2(look.y, distance(look.x, look.z));
}


void FirstPersonManipulator::setFrame(const CoordinateFrame& c) {
    Vector3 look = c.lookVector();

    setPosition(c.translation);

    // this is work towards a patch for bug #1022341
    /*
    if (fuzzyEq(abs(look.dot(Vector3::unitY())), 1.0)) {
        // Looking straight up or down; lookAt won't work
        float dummy;
        float y, p;
        c.rotation.toEulerAnglesZYX(dummy, y, p);
        yaw = -y;
        pitch = -p;

    } else {
    */
        lookAt(c.translation + look);
//    }
}


void FirstPersonManipulator::onPose(Array<PosedModel::Ref>& p3d, Array<PosedModel2DRef>& p2d) {
}


void FirstPersonManipulator::onNetwork() {
}


void FirstPersonManipulator::onLogic() {
}


void FirstPersonManipulator::onSimulation(RealTime rdt, SimTime sdt, SimTime idt) {
    if (! _active) {
        return;
    }

    if (userInput == NULL) {
        return;
    }

    RealTime elapsedTime = sdt;

    {
        // Translation direction
        Vector2 direction(userInput->getX(), userInput->getY());
        direction.unitize();

        // Translate forward
        translation += (lookVector() * direction.y + 
            frame().rightVector() * direction.x) * elapsedTime * maxMoveRate;
    }
    
    // Desired change in yaw and pitch
    Vector2 delta;
    float maxTurn = maxTurnRate * elapsedTime;

    switch (m_mouseMode) {
    case MOUSE_DIRECT_RIGHT_BUTTON:
        {
            bool mouseDown = rightDown(userInput);
            userInput->setPureDeltaMouse(mouseDown);
            if (! mouseDown) {
                // Skip bottom case
                break;
            }
        }
        // Intentionally fall through to MOUSE_DIRECT

    case MOUSE_DIRECT:
        delta = userInput->mouseDXY() / 100.0f;
        break;


    case MOUSE_SCROLL_AT_EDGE:
        {
            Rect2D viewport = Rect2D::xywh(0, 0, userInput->window()->width(), userInput->window()->height());
            Vector2 mouse = userInput->mouseXY();

            Vector2 hotExtent(max(50.0f, viewport.width() / 8), 
                              max(50.0f, viewport.height() / 6));

            // The hot region is outside this rect
            Rect2D hotRegion = Rect2D::xyxy(
                viewport.x0() + hotExtent.x, viewport.y0() + hotExtent.y,
                viewport.x1() - hotExtent.y, viewport.y1() - hotExtent.y);

            // See if the mouse is near an edge
            if (mouse.x <= hotRegion.x0()) {
                delta.x = -square(1.0 - (mouse.x - viewport.x0()) / hotExtent.x);
                // - Yaw
            } else if (mouse.x >= hotRegion.x1()) {
                delta.x = square(1.0 - (viewport.x1() - mouse.x) / hotExtent.x);
                // + Yaw
            }

            if (mouse.y <= hotRegion.y0()) {
                delta.y = -square(1.0 - (mouse.y - viewport.y0()) / hotExtent.y) * 0.6;
                // - pitch
            } else if (mouse.y >= hotRegion.y1()) {
                delta.y = square(1.0 - (viewport.y1() - mouse.y) / hotExtent.y) * 0.6;
                // + pitch
            }

            delta *= maxTurn / 5;
        }
        break;

//    case MOUSE_PUSH_AT_EDGE: 
    default:
        debugAssert(false);
    }


    // Turn rate limiter
    if (G3D::abs(delta.x) > maxTurn) {
        delta.x = maxTurn * G3D::sign(delta.x);
    }

    if (G3D::abs(delta.y) > maxTurn) {
        delta.y = maxTurn * G3D::sign(delta.y);
    }

    m_yaw   += delta.x;
    m_pitch += delta.y;

    // As a patch for a setCoordinateFrame bug, we prevent 
    // the camera from looking exactly along the y-axis.
    m_pitch = clamp(m_pitch, (float)-halfPi() + 0.001f, (float)halfPi() - 0.001f);

    debugAssert(isFinite(m_yaw));
    debugAssert(isFinite(m_pitch));
}


void FirstPersonManipulator::onUserInput(UserInput* ui) {
    userInput = ui;
}


bool FirstPersonManipulator::onEvent(const GEvent& event) {
    return false;
}


}

