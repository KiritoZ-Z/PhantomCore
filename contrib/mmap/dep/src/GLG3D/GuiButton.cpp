/**
 @file GuiButton.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com

 @created 2007-06-02
 @edited  2007-06-10
 */
#include "G3D/platform.h"
#include "GLG3D/GuiButton.h"
#include "GLG3D/GuiWindow.h"
#include "GLG3D/GuiPane.h"

namespace G3D {

GuiButton::GuiButton(GuiWindow* gui, GuiPane* parent, const GuiCaption& text, Style style) : GuiControl(gui, parent, text), m_down(false), m_style(style) {}

void GuiButton::render(RenderDevice* rd, const GuiSkinRef& skin) const {
    if (m_visible) {
        skin->renderButton(m_rect, m_enabled, focused() || mouseOver(), m_down && mouseOver(), m_caption, (GuiSkin::ButtonStyle)m_style);
    }
}

bool GuiButton::onEvent(const GEvent& event) {
    switch (event.type) {
    case GEventType::MOUSE_BUTTON_DOWN:
        m_down = true;
        {
            GEvent response;
            response.gui.type = GEventType::GUI_DOWN;
            response.gui.control = this;
            m_gui->fireEvent(response);
            return true;
        }
    
    case GEventType::MOUSE_BUTTON_UP:
        {
            GEvent response;
            response.gui.type = GEventType::GUI_UP;
            response.gui.control = this;
            m_gui->fireEvent(response);
        }

        // Only trigger an action if the mouse was still over the control
        if (m_down && m_rect.contains(Vector2(event.button.x, event.button.y))) {
            fireActionEvent();
        }

        m_down = false;
        return true;
    }

    return false;
}

}
