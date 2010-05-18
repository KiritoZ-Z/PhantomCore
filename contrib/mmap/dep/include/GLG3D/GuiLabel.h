/**
 @file GLG3D/GuiLabel.h

 @created 2006-05-01
 @edited  2007-06-01

 G3D Library http://g3d-cpp.sf.net
 Copyright 2001-2007, Morgan McGuire morgan@users.sf.net
 All rights reserved.
*/
#ifndef G3D_GUILABEL_H
#define G3D_GUILABEL_H

#include "GLG3D/GuiControl.h"

namespace G3D {

class GuiWindow;
class GuiPane;

/** A text label.  Will never be the focused control and will never
    have the mouse over it. */
class GuiLabel : public GuiControl {
private:
    friend class GuiWindow;
    friend class GuiPane;

    GFont::XAlign m_xalign;
    GFont::YAlign m_yalign;
    
    GuiLabel(GuiWindow*, GuiPane*,const GuiCaption&, GFont::XAlign, GFont::YAlign);

    void render(RenderDevice* rd, const GuiSkinRef&) const;

public:
};

} // G3D

#endif
