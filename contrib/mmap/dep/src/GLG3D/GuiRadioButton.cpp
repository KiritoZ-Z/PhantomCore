#include "GLG3D/GuiRadioButton.h"
#include "GLG3D/GuiWindow.h"

namespace G3D {

GuiRadioButton::GuiRadioButton(GuiWindow* gui, GuiPane* parent, const GuiCaption& text, int myID, 
                               const Pointer<int>& value, Style style) 
    : GuiControl(gui, parent, text), m_value(value), m_myID(myID), m_style(style) {
}


void GuiRadioButton::setSelected() {
    if ((int)*m_value != m_myID) {
        *m_value = m_myID;
        fireActionEvent();
    }
}


void GuiRadioButton::render(RenderDevice* rd, const GuiSkinRef& skin) const {
    if (m_visible) {
        if (m_style == RADIO_STYLE) {
            skin->renderRadioButton(m_rect, m_enabled, focused() || mouseOver(), selected(), m_caption);
        } else {
            skin->renderButton(m_rect, m_enabled, focused() || mouseOver(), selected(), m_caption,
                               (m_style == BUTTON_STYLE) ? GuiSkin::NORMAL_BUTTON_STYLE : GuiSkin::TOOL_BUTTON_STYLE);
        }
    }
}


void GuiRadioButton::setRect(const Rect2D& rect) {
     if (m_style == RADIO_STYLE) {
         // TODO: use the actual font size etc. to compute bounds
         // Prevent the radio button from stealing clicks very far away
         m_rect = rect;
         m_clickRect = Rect2D::xywh(rect.x0y0(), Vector2(min(rect.width(), 30.0f), rect.height()));
     } else {
         GuiControl::setRect(rect);
     }
}


bool GuiRadioButton::selected() const {
    return *m_value == m_myID;
}


bool GuiRadioButton::onEvent(const GEvent& event) {
    if (m_visible) {
        if (event.type == GEventType::MOUSE_BUTTON_DOWN) {
            setSelected();
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

}
