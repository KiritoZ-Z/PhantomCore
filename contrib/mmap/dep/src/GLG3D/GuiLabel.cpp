/**
 @file GuiLabel.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com

 @created 2007-06-02
 @edited  2007-06-10
 */
#include "G3D/platform.h"
#include "GLG3D/GuiLabel.h"
#include "GLG3D/GuiWindow.h"
#include "GLG3D/GuiPane.h"

namespace G3D {

GuiLabel::GuiLabel(GuiWindow* gui, GuiPane* parent, const GuiCaption& text, GFont::XAlign x, GFont::YAlign y) 
    : GuiControl(gui, parent, text), m_xalign(x), m_yalign(y) {}


void GuiLabel::render(RenderDevice* rd, const GuiSkinRef& skin) const {
    if (m_visible) {
        skin->renderLabel(m_rect, m_caption, m_xalign, m_yalign, m_enabled);
    }
}


}
