/**
 @file GLG3D/GuiRadioButton.h

 @created 2006-05-01
 @edited  2007-06-01

 G3D Library http://g3d-cpp.sf.net
 Copyright 2001-2007, Morgan McGuire morgan@users.sf.net
 All rights reserved.
*/
#ifndef G3D_GUIRADIOBUTTON_H
#define G3D_GUIRADIOBUTTON_H

#include "GLG3D/GuiControl.h"
#include "G3D/Pointer.h"

namespace G3D {

class GuiWindow;

/**
   Radio button or exclusive set of toggle butons.
 */
class GuiRadioButton : public GuiControl {
    friend class GuiWindow;
    friend class GuiPane;
public:

    enum Style {RADIO_STYLE, BUTTON_STYLE, TOOL_STYLE};

protected:
    
    Pointer<int>     m_value;
    int              m_myID;
    Style            m_style;

    /** 
        @param myID The ID of this button 
        
        @param groupSelection Pointer to the current selection.  This button is selected
        when myID == *groupSelection
     */
    GuiRadioButton(GuiWindow* gui, GuiPane* parent, const GuiCaption& text, int myID,
                   const Pointer<int>& groupSelection, Style style = RADIO_STYLE);

    bool selected() const;
    
    void setSelected();

    virtual void render(RenderDevice* rd, const GuiSkinRef& skin) const;

    virtual bool onEvent(const GEvent& event);

public:
    virtual void setRect(const Rect2D& rect);
};

} // G3D

#endif
