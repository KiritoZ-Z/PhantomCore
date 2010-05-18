/**
  @file DeveloperWindow.h

  @maintainer Morgan McGuire, morgan@cs.williams.edu

  @created 2007-06-10
  @edited  2007-06-28
*/
#ifndef G3D_DEVELOPERWINDOW_H
#define G3D_DEVELOPERWINDOW_H

#include "G3D/platform.h"
#include "G3D/Pointer.h"
#include "GLG3D/Widget.h"
#include "GLG3D/UprightSplineManipulator.h"
#include "GLG3D/FirstPersonManipulator.h"
#include "GLG3D/GuiWindow.h"
#include "GLG3D/GConsole.h"
#include "GLG3D/GuiSkin.h"

namespace G3D {
/**
 Develoepr controls instantiated by GApp for debugging.
 @sa G3D::GApp, G3D::CameraControlWindow, G3D::GConsole
 */
class DeveloperWindow : public GuiWindow {
protected:

    DeveloperWindow
    (const FirstPersonManipulatorRef&   manualManipulator,
     const UprightSplineManipulatorRef& trackManipulator,
     const Pointer<Manipulator::Ref>&   cameraManipulator,
     const GuiSkinRef&                  skin,
     GConsoleRef                        console,
     const Pointer<bool>&               debugVisible,
     bool*                              showStats,
     bool*                              showText);

public:

    typedef ReferenceCountedPointer<class DeveloperWindow> Ref;

    GuiWindow::Ref                cameraControlWindow;
    GConsoleRef                   consoleWindow;

    static Ref create
    (const FirstPersonManipulatorRef&   manualManipulator,
     const UprightSplineManipulatorRef& trackManipulator,
     const Pointer<Manipulator::Ref>&   cameraManipulator,
     const GuiSkinRef&                  skin,
     GConsoleRef                        console,
     const Pointer<bool>&               debugVisible,
     bool*                              showStats,
     bool*                              showText);
    
    virtual void setManager(WidgetManager* manager);

    virtual bool onEvent(const GEvent& event);
};

}

#endif
