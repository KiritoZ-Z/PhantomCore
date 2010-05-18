/**
 @file GLG3D/GuiPane.h

 @created 2006-05-01
 @edited  2007-06-15

 G3D Library http://g3d-cpp.sf.net
 Copyright 2001-2007, Morgan McGuire morgan@users.sf.net
 All rights reserved.
*/
#ifndef G3D_GUIPANE_H
#define G3D_GUIPANE_H

#include <string>
#include "G3D/Pointer.h"
#include "G3D/Rect2D.h"
#include "GLG3D/GFont.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/Widget.h"
#include "GLG3D/GuiSkin.h"
#include "GLG3D/GuiControl.h"
#include "GLG3D/GuiCheckBox.h"
#include "GLG3D/GuiRadioButton.h"
#include "GLG3D/GuiSlider.h"
#include "GLG3D/GuiLabel.h"
#include "GLG3D/GuiTextBox.h"
#include "GLG3D/GuiButton.h"
#include "GLG3D/GuiDropDownList.h"

namespace G3D {

class GuiButton;

/**
 Sub-rectangle of a window.  Created by GuiWindow::addPane().
 If a pane is invisible, everything inside of it is also invisible.

 All coordinates of objects inside a pane are relative to the pane's
 clientRect().  See GuiWindow for an example of creating a user interface.
 */
class GuiPane : public GuiControl {
    friend class GuiWindow;
    friend class GuiControl;
    friend class GuiButton;
    friend class GuiRadioButton;
    friend class _GuiSliderBase;

private:

    enum {CONTROL_HEIGHT = 25};
    enum {CONTROL_WIDTH = 180};
    enum {BUTTON_WIDTH = 80};
    enum {TOOL_BUTTON_WIDTH = 50};

public:

    /** Controls the appearance of the pane's borders and background.
     */
    // These constants must match the GuiSkin::PaneStyle constants
    enum Style {SIMPLE_FRAME_STYLE, ORNATE_FRAME_STYLE, NO_FRAME_STYLE};
    
protected:

    /** If this is a mouse event, make it relative to the client rectangle */
    static void makeRelative(GEvent& e, const Rect2D& clientRect);

    class Morph {
    public:
        bool            active;
        Rect2D          start;
        RealTime        startTime;
        RealTime        duration;
        Rect2D          end;
        Morph();
        void morphTo(const Rect2D& s, const Rect2D& e);
        /** Morph the object using setRect */
        template<class T>
        void update(T* object) {
            RealTime now = System::time();
            float alpha = (now - startTime) / duration;
            if (alpha > 1.0f) {
                object->setRect(end);
                active = false;
                // The setRect will terminate the morph
            } else {
                object->setRect(start.lerp(end, alpha));
                // setRect turns off morphing, so we have to turn it back
                // on explicitly
                active = true;
            }
        }
    };

    Morph               m_morph;

    Style               m_style;

    Array<GuiControl*>  controlArray;
    /** Sub panes */
    Array<GuiPane*>     paneArray;
    Array<GuiLabel*>    labelArray;

    Rect2D              m_clientRect;

    GuiPane(GuiWindow* gui, GuiPane* parent, const GuiCaption& text, const Rect2D& rect, Style style);

    virtual void render(RenderDevice* rd, const GuiSkinRef& skin) const;
    
    /**
    Finds the visible, enabled control underneath the mouse
    @param control (Output) pointer to the control that the mouse is over
    @param mouse Relative to the parent of this pane.
    */
    void findControlUnderMouse(Vector2 mouse, GuiControl*& control) const;

    virtual bool onEvent(const GEvent& event) { return false; }

    /** Updates this pane to ensure that its client rect is least as wide and
        high as the specified extent, then recursively calls
        increaseBounds on its parent.
     */
    void increaseBounds(const Vector2& extent);

    /** Finds the next vertical position for a control relative to the client rect. */
    Vector2 nextControlPos() const;

    template<class T>
    T* addControl(T* c) {
        Vector2 p = nextControlPos();
        c->setRect(Rect2D::xywh(p, Vector2(max(m_clientRect.width() - p.x, (float)CONTROL_WIDTH), CONTROL_HEIGHT)));

        increaseBounds(c->rect().x1y1());

        controlArray.append(c);
        return c;
    }

    Vector2 contentsExtent() const;

public:

    /** Client rect bounds, relative to the parent pane (or window if
        there is no parent). */
    const Rect2D& clientRect() const {
        return m_clientRect;
    }

    /** Set relative to the parent pane (or window) */
    virtual void setRect(const Rect2D& rect);

    /**
       Causes the window to change shape and/or position to meet the
       specified location.  The window will not respond to drag events
       while it is morphing.
     */
    virtual void morphTo(const Rect2D& r);

    /** Returns true while a morph is in progress. */
    bool morphing() const {
        return m_morph.active;
    }

    ~GuiPane();

    /** 
        @param height Client size of the pane (size of the <i>inside</i>, not counting the border).  
            This will automatically grow as controls are added, so it can safely be left as zero.
     */
    GuiPane* addPane(const GuiCaption& text = "", float height = 0, GuiPane::Style style = GuiPane::SIMPLE_FRAME_STYLE);

    /**
       <pre>
       bool enabled;
       gui->addCheckBox("Enabled", &enabled);

       Foo* foo = new Foo();
       gui->addCheckBox("Enabled", Pointer<bool>(foo, &Foo::enabled, &Foo::setEnabled));

       BarRef foo = Bar::create();
       gui->addCheckBox("Enabled", Pointer<bool>(bar, &Bar::enabled, &Bar::setEnabled));
       </pre>
    */
    GuiCheckBox* addCheckBox
    (const GuiCaption& text,
     const Pointer<bool>& pointer,
     GuiCheckBox::Style style = GuiCheckBox::BOX_STYLE
     ) {
        GuiCheckBox* c = addControl(new GuiCheckBox(m_gui, this, text, pointer, style));
        if (style == GuiCheckBox::TOOL_STYLE) {
            c->setSize(Vector2(TOOL_BUTTON_WIDTH, CONTROL_HEIGHT));
        } else if (style == GuiCheckBox::BUTTON_STYLE) {
            c->setSize(Vector2(BUTTON_WIDTH, CONTROL_HEIGHT));
        }
        return c;
    }

    GuiCheckBox* addCheckBox
    (const GuiCaption& text,
     bool* pointer,
     GuiCheckBox::Style style = GuiCheckBox::BOX_STYLE
     ) {
        return addCheckBox(text, Pointer<bool>(pointer), style);
    }

    GuiTextBox* addTextBox
    (const GuiCaption& caption,
     const Pointer<std::string>& stringPointer,
     GuiTextBox::Update update = GuiTextBox::DELAYED_UPDATE
     ) {        
        return addControl(new GuiTextBox(m_gui, this, caption, stringPointer, update));
    }
/*
   GuiTextBox* addTextBox
    (const GuiCaption& caption,
     std::string* stringPointer,
     GuiTextBox::Update update = GuiTextBox::DELAYED_UPDATE
     ) {        
         return addTextBox(caption, Pointer<std::string>(stringPointer), update);
    }
*/
    GuiDropDownList* addDropDownList(const GuiCaption& caption, const Pointer<int>& indexPointer, Array<std::string>* list);
    GuiDropDownList* addDropDownList(const GuiCaption& caption, const Pointer<int>& indexPointer, Array<GuiCaption>* list);
    
    template<typename EnumOrInt, class T>
    GuiRadioButton* addRadioButton(const GuiCaption& text, int myID,  
        T* object,
        EnumOrInt (T::*get)() const,
        void (T::*set)(EnumOrInt), 
        GuiRadioButton::Style style) {
        
        // Turn enums into ints to allow this to always act as a pointer to an int
        GuiRadioButton* c = addControl(new GuiRadioButton
                          (m_gui, this, text, myID, 
                           Pointer<int>(object, 
                                        reinterpret_cast<int (T::*)() const>(get), 
                                        reinterpret_cast<void (T::*)(int)>(set)), 
                           style));
        if (style == GuiRadioButton::TOOL_STYLE) {
            c->setSize(Vector2(TOOL_BUTTON_WIDTH, CONTROL_HEIGHT));
        } else if (style == GuiRadioButton::BUTTON_STYLE) {
            c->setSize(Vector2(BUTTON_WIDTH, CONTROL_HEIGHT));
        }
        return c;
    }

    template<typename Value>
    GuiSlider<Value>* addSlider(const GuiCaption& text, const Pointer<Value>& value, Value min, Value max, bool horizontal = true) {
        return addControl(new GuiSlider<Value>(m_gui, this, text, value, min,  max, horizontal));
    }

    
    template<typename Value>
    GuiSlider<Value>* addSlider(const GuiCaption& text, Value* value, Value min, Value max, bool horizontal = true) {
        return addSlider(text, Pointer<Value>(value), min,  max, horizontal);
    }

    /**
       Example:
       <pre>
       enum Day {SUN, MON, TUE, WED, THU, FRI, SAT};

       Day day;
       
       gui->addRadioButton("Sun", SUN, &day);
       gui->addRadioButton("Mon", MON, &day);
       gui->addRadioButton("Tue", TUE, &day);
       ...
       </pre>

       @param selection Must be a pointer to an int or enum.  The
       current selection value for a group of radio buttons.
     */
    GuiRadioButton* addRadioButton(const GuiCaption& text, int myID, void* selection, GuiRadioButton::Style style = GuiRadioButton::RADIO_STYLE);

    GuiButton* addButton(const GuiCaption& text, GuiButton::Style style = GuiButton::NORMAL_STYLE);

    GuiLabel* addLabel(const GuiCaption& text, GFont::XAlign xalign = GFont::XALIGN_LEFT, GFont::YAlign = GFont::YALIGN_CENTER);

    /**
     Removes this control from the GuiPane.
     */
    void remove(GuiControl* gui);

    /** 
        Resize this pane so that all of its controls are visible and so that there is
        no wasted space.

        @sa G3D::GuiWindow::pack
     */
    void pack();
};

}

#endif
