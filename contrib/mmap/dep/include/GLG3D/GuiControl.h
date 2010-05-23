/**
 @file GLG3D/GuiControl.h

 @created 2006-05-01
 @edited  2007-06-01

 G3D Library http://g3d-cpp.sf.net
 Copyright 2001-2007, Morgan McGuire morgan@users.sf.net
 All rights reserved.
*/
#ifndef GUICONTROL_H
#define GUICONTROL_H

#include <string>
#include "GLG3D/GuiSkin.h"
#include "GLG3D/Widget.h"

namespace G3D {

class GuiWindow;
class GuiPane;

/** Base class for all controls. */
class GuiControl {
    friend class GuiWindow;
    friend class GuiPane;
protected:

    bool              m_enabled;

    /** The window that ultimately contains this control */
    GuiWindow*        m_gui;

    /** Parent pane */
    GuiPane*          m_parent;

    /** Rect bounds used for rendering and layout.
        Relative to the enclosing pane's clientRect. */
    Rect2D            m_rect;

    /** Rect bounds used for mouse actions.  Updated by setRect.*/
    Rect2D            m_clickRect;

    GuiCaption        m_caption;
    bool              m_visible;

    GuiControl(GuiWindow* gui, GuiPane* parent);
    GuiControl(GuiWindow* gui, GuiPane* parent, const GuiCaption& text);

    /** Fires an action event */
    void fireActionEvent();

public:

    virtual ~GuiControl() {}
    bool enabled() const;
    bool mouseOver() const;
    bool visible() const;
    void setVisible(bool b);
    bool focused() const;

    /** Grab or release keyboard focus */
    void setFocused(bool b);
    void setEnabled(bool e);
    const GuiCaption& caption() const;
    void setCaption(const GuiCaption& text);
    const Rect2D& rect() const;

    /** If you explicitly change the rectangle of a control, the
        containing pane may clip its borders.  Call pack() on the
        containing pane (or window) to resize that container
        appropriately.*/
    virtual void setRect(const Rect2D& rect);
    void setSize(const Vector2& v);
    void setSize(float x, float y);
    void setPosition(const Vector2& v);
    void setPosition(float x, float y);
    void setWidth(float w);
    void setHeight(float h);

    /** If these two controls have the same parent, move this one immediately to the right of the argument*/
    void moveRightOf(const GuiControl* control);
    void moveBy(const Vector2& delta);
    void moveBy(float dx, float dy);    

    GuiSkinRef skin() const;

protected:

    virtual void render(RenderDevice* rd, const GuiSkinRef& skin) const = 0;

    /** Events are only delivered to a control when the control that
        control has the key focus (which is transferred during a mouse
        down)
    */
    virtual bool onEvent(const GEvent& event) { return false; }
};

}

#endif
