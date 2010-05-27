#ifndef G3D_CAMERACONTROLWINDOW_H
#define G3D_CAMERACONTROLWINDOW_H

/**
  @file CameraControlWindow.h

  @maintainer Morgan McGuire, morgan@cs.williams.edu

  @created 2002-07-28
  @edited  2006-08-10
*/
#include "G3D/platform.h"
#include "GLG3D/Widget.h"
#include "GLG3D/UprightSplineManipulator.h"
#include "GLG3D/GuiWindow.h"
#include "GLG3D/FirstPersonManipulator.h"

namespace G3D {

/**
 Gui used by DeveloperWindow default for recording camera position and making splines.
 @sa G3D::DeveloperWindow, G3D::GApp
 */
//
// If you are looking for an example of how to create a straightforward
// GUI in G3D do not look at this class!  CameraControlWindow uses a number
// of unusual tricks to provide a fancy compact interface that you do not
// need in a normal program.  The GUI code in this class is more complex
// than what you would have to write for a less dynamic UI.
class CameraControlWindow : public GuiWindow {
public:

    typedef ReferenceCountedPointer<class CameraControlWindow> Ref;

protected:

    static const Vector2        smallSize;
    static const Vector2        bigSize;

    std::string cameraLocation() const;
    void setCameraLocation(const std::string& s);

    /** Array of all .trk files in the current directory */
    Array<std::string>          trackFileArray;

    /** Index into trackFileArray */
    int                         trackFileIndex;

    /** Label for trackList.  Separate in order to allow condensed spacing */
    GuiLabel*                   trackLabel;
    GuiDropDownList*            trackList;

    /** Allows the user to override the current camera position */
    GuiTextBox*                 cameraLocationTextBox;

    GuiRadioButton*             playButton;
    GuiRadioButton*             stopButton;
    GuiRadioButton*             recordButton;

    /** The manipulator from which the camera is copying its frame */
    Pointer<Manipulator::Ref>   cameraManipulator;

    FirstPersonManipulatorRef   manualManipulator;
    UprightSplineManipulatorRef trackManipulator;

    GuiCheckBox*                visibleCheckBox;
    GuiCheckBox*                cyclicCheckBox;

    /** Button to expand and contract additional manual controls. */
    GuiButton*                  drawerButton;

    /** The button must be in its own pane so that it can float over
        the expanded pane. */
    GuiPane*                    drawerButtonPane;
    GuiCaption                  drawerExpandCaption;
    GuiCaption                  drawerCollapseCaption;

    GuiButton*                  saveButton;

    GuiLabel*                   helpLabel;

    GuiCaption                  manualHelpCaption;
    GuiCaption                  autoHelpCaption;
    GuiCaption                  recordHelpCaption;
    GuiCaption                  playHelpCaption;

    /** If true, the window is big enough to show all controls */
    bool                        m_expanded;

    /** True when the user has chosen to override program control of
        the camera. */
    bool                        manualOperation;

    CameraControlWindow(
        const FirstPersonManipulatorRef&    manualManipulator, 
        const UprightSplineManipulatorRef&  trackManipulator, 
        const Pointer<Manipulator::Ref>&    cameraManipulator,
        const GuiSkinRef&                   skin);

    /** Sets the controller for the cameraManipulator. */
    //void setSource(Source s);

    /** Control source that the Gui thinks should be in use */
    //Source desiredSource() const;

    void sync();

    void loadSpline(const std::string& filename);

    /** Updates the trackFileArray from the list of track files */
    void updateTrackFiles();

public:

    /**
     @param cameraManipulator The manipulator that should drive the camera.  This will be assigned to
     as the program runs.
     */
    static Ref create(
        const FirstPersonManipulatorRef&   manualManipulator,
        const UprightSplineManipulatorRef& trackManipulator,
        const Pointer<Manipulator::Ref>&   cameraManipulator,
        const GuiSkinRef&                  skin);

    virtual bool onEvent(const GEvent& event);
    virtual void onUserInput(UserInput*);
    virtual void setRect(const Rect2D& r);
};

}

#endif
