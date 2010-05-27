/**
 @file GLG3D/GuiWindow.h

 @created 2006-05-01
 @edited  2007-06-21

 G3D Library http://g3d-cpp.sf.net
 Copyright 2001-2007, Morgan McGuire morgan@users.sf.net
 All rights reserved.
*/
#ifndef G3D_GUIWINDOW_H
#define G3D_GUIWINDOW_H

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
#include "GLG3D/GuiPane.h"

namespace G3D {

class GuiPane;


class GuiDrawer {
private:
    bool m_open;
public:
    enum Side {TOP_SIDE, LEFT_SIDE, RIGHT_SIDE, BOTTOM_SIDE};
    
    /** Returns true if this drawer has been pulled out */
    bool open() const {
        return m_open;
    }

    void setOpen(bool b) {
        // TODO
    }
};
    
typedef ReferenceCountedPointer<class GuiWindow> GuiWindowRef;
/**
   Retained-mode graphical user interface window. 

   %G3D Guis (Graphical User Interfaces) are "skinnable", meaning that the appearance is controled by data files.
   %G3D provides already made skins in the data/gui directory of the installation.  See GuiSkin for information
   on how to draw your own.

   The Gui API connects existing variables and methods directly to controls.  Except for GuiButton, you don't have
   to write event handlers like in other APIs. Just pass a pointer to the variable that you want to receive the
   value of the control when the control is created.  An example of creating a dialog is shown below:

   <pre>
        GuiSkinRef skin = GuiSkin::fromFile(dataDir + "gui/osx.skn", app->debugFont);
        GuiWindow::Ref window = GuiWindow::create("Person", skin);

        GuiPane* pane = window->pane();
        pane->addCheckBox("Likes cats", &player.likesCats);
        pane->addCheckBox("Is my friend", &player, &Person::getIsMyFriend, &Person::setIsMyFriend);
        pane->addRadioButton("Male", Person::MALE, &player.gender);
        pane->addRadioButton("Female", Person::FEMALE, &player.gender);
        player.height = 1.5;
        pane->addSlider("Height", &player.height, 1.0f, 2.2f);
        GuiButton* invite = pane->addButton("Invite");

        addWidget(window);
   </pre>

   Note that in the example, one check-box is connected to a field of "player" and another to methods to get and set
   a value.  To process the button click, extend the GApp (or another Widget's) GApp::onEvent method as follows:

   <pre>
   bool App::onEvent(const GEvent& e) {
       if (e.type == GEventType::GUI_ACTION) {
           if (e.gui.control == invite) {
               ... handle the invite action here ...
               return true;
           }
       }
       return false;
   }
   </pre>

   It is not necessary to subclass GuiWindow to create a user
   interface.  Just instantiate GuiWindow and add controls to its
   pane.  If you do choose to subclass GuiWindow, be sure to call
   the superclass methods for those that you override.
 */
class GuiWindow : public Widget {

    friend class GuiControl;
    friend class GuiButton;
    friend class GuiRadioButton;
    friend class _GuiSliderBase;
    friend class GuiPane;
    friend class GuiTextBox;
    friend class GuiDropDownList;

private:
    enum {CONTROL_WIDTH = 180};
public:
    typedef ReferenceCountedPointer<GuiWindow> Ref;

    /** Controls the appearance of the window's borders and background.
        NORMAL_FRAME_STYLE - regular border and title
        TOOL_FRAME_STYLE   - small title, thin border
        DIALOG_FRAME_STYLE - thicker border
        NO_FRAME_STYLE     - do not render any background at all
     */
    // These constants are chosen to match the GuiSkin constants
    enum Style {NORMAL_FRAME_STYLE, TOOL_FRAME_STYLE, DIALOG_FRAME_STYLE, NO_FRAME_STYLE};

    /**
      Controls the behavior when the close button is pressed (if there
      is one).  

      NO_CLOSE - Do not show the close button
      IGNORE_CLOSE - Fire G3D::GEvent::GUI_CLOSE event but take no further action
      HIDE_ON_CLOSE - Set the window visibility to false and fire G3D::GEvent::GUI_CLOSE
      REMOVE_ON_CLOSE - Remove this GuiWindow from its containing WidgetManager and fire 
         G3D::GEvent::GUI_CLOSE with a NULL window argument (since the window may be garbage collected before the event is received).
     */
    enum CloseAction {NO_CLOSE, IGNORE_CLOSE, HIDE_ON_CLOSE, REMOVE_ON_CLOSE};
    
private:

    /** Used for rendering the UI */
    class Posed : public PosedModel2D {
    public:
        GuiWindow* gui;
        Posed(GuiWindow* gui);
        virtual Rect2D bounds () const;
        virtual float depth () const;
        virtual void render (RenderDevice *rd) const;
    };

    class ControlButton { 
    public:
        bool           down;
        bool           mouseOver;
        ControlButton() : down(false), mouseOver(false) {};
    };

    
    /** State for managing modal dialogs */
    class Modal {
    public:
        UserInput*          userInput;
        WidgetManager::Ref  manager;
        GWindow*            osWindow;
        RenderDevice*       renderDevice;
        
        /** Image of the screen under the modal dialog.*/
        TextureRef          image;

        /** Size of the screen */
        Rect2D              viewport;
        
        /** The dialog that is running */
        GuiWindow*          dialog;

        Modal(GWindow* osWindow);
        /** Run an event loop until the window closes */
        void run(GuiWindow::Ref dialog);
        /** Callback for GWindow loop body */
        static void loopBody(void* me);
        /** Called from loop Body */
        void oneFrame();
        void processEventQueue();
        ~Modal();
    };

    Modal*                 modal;

    /** Window label */
    GuiCaption          m_text;

    /** Window border bounds. Actual rendering may be outside these bounds. */
    Rect2D              m_rect;

    /** Client rect bounds, absolute on the GWindow. */
    Rect2D              m_clientRect;
    
    /** Is this window visible? */
    bool                m_visible;

    Style               m_style;

    CloseAction         m_closeAction;
    ControlButton       m_closeButton;

    PosedModel2DRef     posed;

    GuiSkinRef          m_skin;

    /** True when the window is being dragged */
    bool                inDrag;

    /** Position at which the drag started */
    Vector2             dragStart;
    Rect2D              dragOriginalRect;

    GuiControl*         mouseOverGuiControl;
    GuiControl*         keyFocusGuiControl;
    
    bool                m_focused;
    bool                m_mouseVisible;
    
    GuiPane::Morph      m_morph;

    Array<GuiDrawer*>   m_drawerArray;
    GuiPane*            m_rootPane;

protected:

    GuiWindow(const GuiCaption& text, GuiSkinRef skin, const Rect2D& rect, Style style, CloseAction closeAction);

private:

    void render(RenderDevice* rd);

    /** Take the specified close action */
    void close();

    /**
      Called when tab is pressed.
     */
    void focusOnNextControl();

    void setFocusControl(GuiControl* c);

    /** Called by GuiPane::increaseBounds() */
    void increaseBounds(const Vector2& extent);

public:

    /** 
        Blocks until the dialog is closed (visible = false).  Do not call between
        RenderDevice::beginFrame and RenderDevice::endFrame.
     */
    void showModal(GWindow* osWindow);

    void showModal(GuiWindow::Ref parent);

    /** Is this window in focus on the WidgetManager? */
    inline bool focused() const {
        return m_focused;
    }

    /** Window bounds, including shadow and glow, absolute on the GWindow. */
    const Rect2D& rect() const {
        return m_rect;
    }

    /** Interior bounds of the window, absolute on the GWindow */
    const Rect2D& clientRect() const {
        return m_clientRect;
    }

    GuiSkinRef skin() const {
        return m_skin;
    }

    /**
     Set the border bounds relative to the GWindow. 
     The window may render outside the bounds because of drop shadows
     and glows.
      */
    virtual void setRect(const Rect2D& r);

    /** Move to the center of the screen */
    void moveToCenter();

    /**
       Causes the window to change shape and/or position to meet the
       specified location.  The window will not respond to drag events
       while it is morphing.
     */
    void morphTo(const Rect2D& r);

    /** Returns true while a morph is in progress. */
    bool morphing() const {
        return m_morph.active;
    }

    bool visible() const {
        return m_visible;
    }

    /** Hide this entire window.  The window cannot have
        focus if it is not visible. 

        Removing the GuiWindow from the WidgetManager is more efficient
        than making it invisible.
    */
    void setVisible(bool v) { 
        m_visible = v;
    }

    ~GuiWindow();

    GuiPane* pane() {
        return m_rootPane;
    }

    /** As controls are added, the window will automatically grow to contain them as needed */
    static Ref create(const GuiCaption& windowTitle, const GuiSkinRef& skin, 
                      const Rect2D& rect = Rect2D::xywh(100, 100, 100, 50), 
                      Style style = NORMAL_FRAME_STYLE, CloseAction = NO_CLOSE);

    /**
       Drawers are like windows that slide out of the side of another
       GuiWindow.  Drawers are initially sized based on the side of
       the window that they slide out of, but they can be explicitly
       sized.  Multiple drawers can be attached to the same side,
       however it is up to the caller to ensure that they do not overlap.

       @param side Side that the drawer sticks out of
     */
    virtual GuiDrawer* addDrawer(const GuiCaption& caption = "", GuiDrawer::Side side = GuiDrawer::RIGHT_SIDE) { return NULL; }

    virtual void onPose(Array<PosedModelRef>& posedArray, Array<PosedModel2DRef>& posed2DArray);

    virtual bool onEvent(const GEvent& event);

    virtual void onLogic() {}

    virtual void onNetwork() {}

    virtual void onSimulation(RealTime rdt, SimTime sdt, SimTime idt) {}

    virtual void onUserInput(UserInput *ui);

    /** 
        Resize the pane so that all of its controls are visible and so that there is
        no wasted space, then resize the window around the pane.

        @sa G3D::GuiPane::pack
     */
    void pack();
};

}

#endif
