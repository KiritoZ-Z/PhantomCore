/**
  @file DeveloperWindow.cpp

  @maintainer Morgan McGuire, morgan@cs.williams.edu

  @created 2007-06-01
  @edited  2007-06-28
*/
#include "G3D/platform.h"
#include "GLG3D/DeveloperWindow.h"
#include "GLG3D/CameraControlWindow.h"

namespace G3D {

DeveloperWindow::Ref DeveloperWindow::create(
     const FirstPersonManipulatorRef&   manualManipulator,
     const UprightSplineManipulatorRef& trackManipulator,
     const Pointer<Manipulator::Ref>&   cameraManipulator,
     const GuiSkinRef&                  skin,
     GConsoleRef                        console,
     const Pointer<bool>&               debugVisible,
     bool*                              showStats,
     bool*                              showText) {

    return new DeveloperWindow(manualManipulator, trackManipulator, cameraManipulator, skin, console, debugVisible, showStats, showText);
    
}

DeveloperWindow::DeveloperWindow(
     const FirstPersonManipulatorRef&   manualManipulator,
     const UprightSplineManipulatorRef& trackManipulator,
     const Pointer<Manipulator::Ref>&   cameraManipulator,
     const GuiSkinRef&                  skin,
     GConsoleRef                        console,
     const Pointer<bool>&               debugVisible,
     bool*                              showStats,
     bool*                              showText) : GuiWindow("Developer (F11)", skin, Rect2D::xywh(600,80,0,0), TOOL_FRAME_STYLE, HIDE_ON_CLOSE), consoleWindow(console) {

    cameraControlWindow = CameraControlWindow::create(manualManipulator, trackManipulator, cameraManipulator, skin);

    GuiPane* root = pane();
    GFontRef iconFont = GFont::fromFile(System::findDataFile("icon.fnt"));
    
    const float iconSize = 32;
    Vector2 buttonSize(32, 26);

    GuiCaption cameraIcon(std::string() + char(185), iconFont, iconSize);
    GuiCaption consoleIcon(std::string() + char(190), iconFont, iconSize * 0.9);
    GuiCaption statsIcon(std::string() + char(143), iconFont, iconSize);
    GuiCaption debugIcon("@", iconFont, iconSize * 0.8);
    GuiCaption printIcon(std::string() + char(157), iconFont, iconSize * 0.8); //105 = information

    GuiControl* cameraButton = root->addCheckBox(cameraIcon, Pointer<bool>(cameraControlWindow, &GuiWindow::visible, &GuiWindow::setVisible), GuiCheckBox::TOOL_STYLE);
    cameraButton->setSize(buttonSize);
    cameraButton->setPosition(0, 0);

    GuiControl* consoleButton = root->addCheckBox(consoleIcon, Pointer<bool>(consoleWindow, &GConsole::active, &GConsole::setActive), GuiCheckBox::TOOL_STYLE);
    consoleButton->setSize(buttonSize);
    consoleButton->moveRightOf(cameraButton);

    GuiControl* debugButton = root->addCheckBox(debugIcon, debugVisible, GuiCheckBox::TOOL_STYLE);
    debugButton->setSize(buttonSize);
    debugButton->moveRightOf(consoleButton);

    GuiControl* statsButton = root->addCheckBox(statsIcon, showStats, GuiCheckBox::TOOL_STYLE);
    statsButton->setSize(buttonSize);
    statsButton->moveRightOf(debugButton);

    GuiControl* printButton = root->addCheckBox(printIcon, showText, GuiCheckBox::TOOL_STYLE);
    printButton->setSize(buttonSize);
    printButton->moveRightOf(statsButton);

    cameraControlWindow->setVisible(false);
    pack();
    setRect(Rect2D::xywh(0, 0, 162, 38));
}


void DeveloperWindow::setManager(WidgetManager* manager) {
    if (m_manager) {
        // Remove from old
        m_manager->remove(cameraControlWindow);
    }

    if (manager) {
        // Add to new
        manager->add(cameraControlWindow);
    }

    GuiWindow::setManager(manager);

    // Move to the lower left
    Vector2 osWindowSize = manager->window()->dimensions().wh();
    setRect(Rect2D::xywh(osWindowSize - rect().wh(), rect().wh()));
}


bool DeveloperWindow::onEvent(const GEvent& event) {
    if (GuiWindow::onEvent(event)) {
        return true;
    }

    if ((event.type == GEventType::KEY_DOWN) && (event.key.keysym.sym == GKey::F11)) {
        // Toggle visibility
        setVisible(! visible());
        return true;
    }

    return false;
}

}
