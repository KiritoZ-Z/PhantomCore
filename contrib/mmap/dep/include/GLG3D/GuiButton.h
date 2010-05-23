/**
 @file GLG3D/GuiButton.h

 @created 2006-05-01
 @edited  2007-06-01

 G3D Library http://g3d-cpp.sf.net
 Copyright 2001-2007, Morgan McGuire morgan@users.sf.net
 All rights reserved.
*/
#ifndef G3D_GUIBUTTON_H
#define G3D_GUIBUTTON_H

#include "GLG3D/GuiControl.h"

namespace G3D {

class GuiWindow;
class GuiPane;

/** Push button that can be temporarily pressed.  When the button has been pressed and released,
    a G3D::GuiEvent of type G3D::GEventType::GUI_ACTION is fired on the containing window.

    See also GuiRadioButton and GuiCheckBox for creating buttons that stay down
    when pressed.
*/
class GuiButton : public GuiControl {
public:
    /** Controls the appearance of the button.  Tool buttons are square and less 3D.  They 
        are also able to shrink to smaller sizes.  The NO_STYLE creates buttons 
        with a caption but no visible borders.
     */
    // These constants must match the GuiSkin::PaneStyle constants
    enum Style {NORMAL_STYLE, TOOL_STYLE, NO_STYLE};

    friend class GuiWindow;
    friend class GuiPane;
protected:
    /** Is the mouse currently down over this control? */
    bool m_down;

    Style m_style;

    /** Called by GuiWindow */
    GuiButton(GuiWindow* gui, GuiPane*, const GuiCaption& text, Style style);

    /** Called by GuiWindow */
    virtual void render(RenderDevice* rd, const GuiSkinRef& skin) const;

    virtual bool onEvent(const GEvent& event);
    
public:
};

} // G3D

#endif
