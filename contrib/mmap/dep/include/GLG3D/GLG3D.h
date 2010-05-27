/**
 @file GLG3D.h

 This header includes all of the GLG3D libraries in 
 appropriate namespaces.

 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2002-08-07
 @edited  2007-06-01

 Copyright 2001-2007, Morgan McGuire.
 All rights reserved.
*/

#ifndef G3D_GLG3D_H
#define G3D_GLG3D_H

#include "G3D/G3D.h"

// Set up the linker on Windows
#ifdef G3D_WIN32

#   pragma comment(lib, "ole32")
#   pragma comment(lib, "opengl32")
#   pragma comment(lib, "glu32")
#   pragma comment(lib, "shell32") // for drag drop

#   ifdef _DEBUG
        // Don't link against G3D when building G3D itself.
#       ifndef G3D_BUILDING_LIBRARY_DLL
#           pragma comment(lib, "GLG3Dd")
#       endif
#   else
        // Don't link against G3D when building G3D itself.
#       ifndef G3D_BUILDING_LIBRARY_DLL
#           pragma comment(lib, "GLG3D")
#       endif
#   endif
#endif

#include "GLG3D/glheaders.h"
#include "GLG3D/glcalls.h"
#include "GLG3D/getOpenGLState.h"
#include "GLG3D/TextureFormat.h"
#include "GLG3D/Texture.h"
#include "GLG3D/glFormat.h"
#include "GLG3D/Milestone.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/VARArea.h"
#include "GLG3D/VAR.h"
#include "GLG3D/GFont.h"
#include "GLG3D/SkyParameters.h"
#include "GLG3D/Sky.h"
#include "GLG3D/UserInput.h"
#include "GLG3D/FirstPersonManipulator.h"
#include "GLG3D/VertexProgram.h"
#include "GLG3D/Draw.h"
#include "GLG3D/tesselate.h"
#include "GLG3D/GApp.h"
#include "GLG3D/PosedModel.h"
#include "GLG3D/IFSModel.h"
#include "GLG3D/MD2Model.h"
#include "GLG3D/shadowVolume.h"
#include "GLG3D/GWindow.h"
#include "GLG3D/SDLWindow.h"
#include "GLG3D/edgeFeatures.h"
#include "GLG3D/Shader.h"
#include "GLG3D/GLCaps.h"
#include "GLG3D/Shape.h"
#include "GLG3D/Renderbuffer.h"
#include "GLG3D/Framebuffer.h"
#include "GLG3D/Widget.h"
#include "GLG3D/ThirdPersonManipulator.h"
#include "GLG3D/ToneMap.h"
#include "GLG3D/GConsole.h"
#include "GLG3D/BSPMAP.h"
#include "GLG3D/GKey.h"
#include "GLG3D/ArticulatedModel.h"
#include "GLG3D/SuperShader.h"
#include "GLG3D/GaussianBlur.h"

#include "GLG3D/GuiSkin.h"
#include "GLG3D/GuiButton.h"
#include "GLG3D/GuiWindow.h"
#include "GLG3D/GuiCheckBox.h"
#include "GLG3D/GuiControl.h"
#include "GLG3D/GuiLabel.h"
#include "GLG3D/GuiPane.h"
#include "GLG3D/GuiRadioButton.h"
#include "GLG3D/GuiSlider.h"
#include "GLG3D/GuiTextBox.h"
#include "GLG3D/GuiDropDownList.h"

#include "GLG3D/UprightSplineManipulator.h"
#include "GLG3D/CameraControlWindow.h"
#include "GLG3D/DeveloperWindow.h"

#include "GLG3D/ShadowMap.h"

#ifdef G3D_OSX
#include "GLG3D/CarbonWindow.h"
#endif

#ifdef G3D_WIN32
#include "GLG3D/Win32Window.h"
#include "GLG3D/DXCaps.h"
#endif

#ifdef G3D_LINUX
#include "GLG3D/X11Window.h"
#endif

#ifdef G3D_OSX
#include "GLG3D/SDLWindow.h"
#endif

#endif
