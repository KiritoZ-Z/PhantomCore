/**
 @file GuiControl.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com

 @created 2007-06-01
 @edited  2007-06-19
 */

#include "G3D/platform.h"
#include "GLG3D/GuiControl.h"
#include "GLG3D/GuiWindow.h"

namespace G3D {


void GuiControl::setFocused(bool b) {
    if (! b) {
        if (m_gui->keyFocusGuiControl == this) {
            m_gui->focusOnNextControl();
        }
    } else {
        m_gui->setFocusControl(this);
    }
}


GuiSkinRef GuiControl::skin() const {
    return m_gui->skin();
}

void GuiControl::setPosition(float x, float y) {
    setPosition(Vector2(x, y));
}

void GuiControl::setPosition(const Vector2& v) {
    setRect(Rect2D::xywh(v, rect().wh()));
}

void GuiControl::moveBy(float x, float y) {
    moveBy(Vector2(x, y));
}

void GuiControl::moveBy(const Vector2& v) {
    setRect(Rect2D::xywh(rect().x0y0() + v, rect().wh()));
}

void GuiControl::setSize(float x, float y) {
    setSize(Vector2(x, y));
}

void GuiControl::setSize(const Vector2& v) {
    setRect(Rect2D::xywh(rect().x0y0(), v));
}

void GuiControl::setHeight(float h) {
    setRect(Rect2D::xywh(rect().x0y0(), Vector2(rect().width(), h)));
}

void GuiControl::setWidth(float w) {
    setRect(Rect2D::xywh(rect().x0y0(), Vector2(w, rect().height())));
}

void GuiControl::moveRightOf(const GuiControl* control) {
    setRect(Rect2D::xywh(control->rect().x1y0(), rect().wh()));
}

bool GuiControl::mouseOver() const {
    return m_gui->mouseOverGuiControl == this;
}
        
bool GuiControl::focused() const {
    // In focus if this control has the key focus and this window is in focus
    return (m_gui->keyFocusGuiControl == this) && m_gui->focused();
}

bool GuiControl::visible() const {
    return m_visible;
}

void GuiControl::setVisible(bool v) {
    m_visible = v;
}

bool GuiControl::enabled() const {
    return m_enabled;
}

void GuiControl::setEnabled(bool e) {
    m_enabled = e;
}

const GuiCaption& GuiControl::caption() const {
    return m_caption;
}

void GuiControl::setCaption(const GuiCaption& text) {
    m_caption = text;
}

const Rect2D& GuiControl::rect() const {
    return m_rect;
}

void GuiControl::setRect(const Rect2D& rect) {
    m_clickRect = m_rect = rect;
}

GuiControl::GuiControl(GuiWindow* gui, GuiPane* parent) : m_enabled(true), m_gui(gui), m_parent(parent), m_visible(true) {}

GuiControl::GuiControl(GuiWindow* gui, GuiPane* parent, const GuiCaption& caption) : m_enabled(true), m_gui(gui), m_parent(parent), m_caption(caption), m_visible(true) {}


void GuiControl::fireActionEvent() {
    GEvent response;
    response.gui.type = GEventType::GUI_ACTION;
    response.gui.control = this;
    m_gui->fireEvent(response);
}

}

