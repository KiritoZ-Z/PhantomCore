/**
 @file GLG3D/GuiDropDownList.h

 @created 2007-06-15
 @edited  2007-06-19

 G3D Library http://g3d-cpp.sf.net
 Copyright 2001-2007, Morgan McGuire morgan@users.sf.net
 All rights reserved.
*/
#ifndef G3D_GUIDROPDOWNLIST_H
#define G3D_GUIDROPDOWNLIST_H

#include "GLG3D/GuiControl.h"
#include "G3D/Pointer.h"
#include "G3D/Array.h"

namespace G3D {

class GuiWindow;
class GuiPane;

/** List box for viewing strings.  Fires
    a G3D::GuiEvent of type G3D::GEventType::GUI_ACTION on the containing window when
    the user selects a new value.
*/
class GuiDropDownList : public GuiControl {
    friend class GuiWindow;
    friend class GuiPane;

protected:

    /** The index of the currently selected item. */
    Pointer<int>                    m_indexValue;

    Array<std::string>*             m_stringListValue;
    Array<GuiCaption>*              m_captionListValue;

    /** True when the menu is open */
    bool                            m_selecting;

    /** Which of the two list values to use */
    bool                            m_useStringList;

    /** Called by GuiPane */
    GuiDropDownList
       (GuiWindow*                  gui, 
        GuiPane*                    parent, 
        const GuiCaption&           caption, 
        const Pointer<int>&         indexValue, 
        Array<std::string>*         listValue);

    GuiDropDownList
       (GuiWindow*                  gui, 
        GuiPane*                    parent, 
        const GuiCaption&           caption, 
        const Pointer<int>&         indexValue, 
        Array<GuiCaption>*          listValue);

    /** Called by GuiPane */
    virtual void render(RenderDevice* rd, const GuiSkinRef& skin) const;

    virtual bool onEvent(const GEvent& event);

public:

    virtual void setRect(const Rect2D&);
    
};

} // G3D

#endif
