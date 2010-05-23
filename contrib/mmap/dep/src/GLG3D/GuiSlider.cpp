/**
 @file GuiSlider.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com

 @created 2007-06-02
 @edited  2007-06-10
 */
#include "G3D/platform.h"
#include "GLG3D/GuiSlider.h"
#include "GLG3D/GuiWindow.h"

namespace G3D {

_GuiSliderBase::_GuiSliderBase(GuiWindow* gui, GuiPane* parent, const GuiCaption& text, bool horizontal) :
    GuiControl(gui, parent, text), 
    m_horizontal(horizontal),
    m_inDrag(false) {
}


void _GuiSliderBase::render(RenderDevice* rd, const GuiSkinRef& skin) const {
    if (m_visible) {
        if (m_horizontal) {
            skin->renderHorizontalSlider(m_rect, floatValue(), m_enabled, focused() || mouseOver(), m_caption);
        }
    }
}


bool _GuiSliderBase::onEvent(const GEvent& event) {
    if (! m_visible) {
        return false;
    }

    if (event.type == GEventType::MOUSE_BUTTON_DOWN) {
        Vector2 mouse = Vector2(event.button.x, event.button.y);

        float v = floatValue();
        Rect2D thumbRect = skin()->horizontalSliderToThumbBounds(m_rect, v);
        Rect2D trackRect = skin()->horizontalSliderToTrackBounds(m_rect);
        
        if (thumbRect.contains(mouse)) {
            // Begin drag
            m_inDrag = true;
            m_dragStart = mouse;
            m_dragStartValue = v;
            return true;
        } else if (trackRect.contains(mouse)) {
            // Jump to this position
            float p = clamp((mouse.x - trackRect.x0()) / trackRect.width(), 0.0f, 1.0f);
            setFloatValue(p);
            m_inDrag = false;
            return true;
        }

    } else if (event.type == GEventType::MOUSE_BUTTON_UP) {
        if (m_inDrag) {
            // End the drag
            m_inDrag = false;
            return true;
        }

    } else if (m_inDrag && (event.type == GEventType::MOUSE_MOTION)) {
        // We'll only receive these events if we have the keyFocus, but we can't
        // help receiving the key focus if the user clicked on the control!

        Vector2 mouse = Vector2(event.button.x, event.button.y);
        Rect2D trackRect = skin()->horizontalSliderToTrackBounds(m_rect);

        float delta = (mouse.x - m_dragStart.x) / trackRect.width();
        float p = clamp(m_dragStartValue + delta, 0.0f, 1.0f);
        setFloatValue(p);

        return true;
    }
    return false;
}

}
