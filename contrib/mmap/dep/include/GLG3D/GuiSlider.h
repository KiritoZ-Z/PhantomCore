/**
 @file GLG3D/GuiSlider.h

 @created 2006-05-01
 @edited  2007-06-01

 G3D Library http://g3d-cpp.sf.net
 Copyright 2001-2007, Morgan McGuire morgan@users.sf.net
 All rights reserved.
*/
#ifndef G3D_GUISLIDER_H
#define G3D_GUISLIDER_H

#include "GLG3D/GuiControl.h"
#include "G3D/Pointer.h"

namespace G3D {

class GuiWindow;
class GuiPane;

class _GuiSliderBase : public GuiControl {
    friend class GuiPane;

protected:

    bool              m_horizontal;
    bool              m_inDrag;
    float             m_dragStartValue;

    _GuiSliderBase(GuiWindow* gui, GuiPane* parent, const GuiCaption& text, bool horizontal);

    /** Position from which the mouse drag started, relative to
        m_gui.m_clientRect.  When dragging the thumb, the cursor may not be
        centered on the thumb the way it is when the mouse clicks
        on the track.
    */
    Vector2           m_dragStart;
    
    /** Get value on the range 0 - 1 */
    virtual float floatValue() const = 0;

    /** Set value on the range 0 - 1 */
    virtual void setFloatValue(float f) = 0;
    
    virtual void render(RenderDevice* rd, const GuiSkinRef& skin) const;

    virtual bool onEvent(const GEvent& event);

};


/** Slider.  See GuiWindow for an example of creating a slider. */
template<class Value>
class GuiSlider : public _GuiSliderBase {
    friend class GuiPane;
    friend class GuiWindow;
protected:

    Pointer<Value>    m_value;
    Value             m_minValue;
    Value             m_maxValue;

    float floatValue() const {
        return (float)(*m_value - m_minValue) / (float)(m_maxValue - m_minValue);
    }

    void setFloatValue(float f) {
        *m_value = (Value)(f * (m_maxValue - m_minValue) + m_minValue);
    }

    GuiSlider(GuiWindow* gui, GuiPane* parent, const GuiCaption& text, 
              const Pointer<Value>& value, Value minValue, Value maxValue, bool horizontal) :
        _GuiSliderBase(gui, parent, text, horizontal), m_value(value), 
        m_minValue(minValue), m_maxValue(maxValue) {}

public:

    Value minValue() const {
        return m_minValue;
    }

    Value maxValue() const {
        return m_maxValue;
    }
};

} // G3D

#endif
