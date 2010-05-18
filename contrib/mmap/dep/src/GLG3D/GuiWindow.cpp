/**
 @file GuiWindow.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com

 @created 2007-06-02
 @edited  2007-06-10
 */
#include "G3D/platform.h"
#include "GLG3D/GuiWindow.h"
#include "GLG3D/GuiControl.h"
#include "GLG3D/GuiPane.h"
#include "GLG3D/UserInput.h"
#include "GLG3D/Draw.h"

namespace G3D {

GuiWindow::Ref GuiWindow::create
(const GuiCaption& label, 
 const GuiSkinRef& skin, 
 const Rect2D& rect, 
 Style style, 
 CloseAction close) {

    return new GuiWindow(label, skin, rect, style, close);
}


GuiWindow::GuiWindow(const GuiCaption& text, GuiSkinRef skin, const Rect2D& rect, Style style, CloseAction close) 
    : m_text(text), m_rect(rect), m_visible(true), 
      m_style(style), m_closeAction(close), m_skin(skin), 
      inDrag(false),
      mouseOverGuiControl(NULL), 
      keyFocusGuiControl(NULL),
      m_focused(false),
      m_mouseVisible(false) {

    setRect(rect);
    posed = new Posed(this);
    m_rootPane = new GuiPane(this, NULL, "", clientRect() - clientRect().x0y0(), GuiPane::NO_FRAME_STYLE);
}


GuiWindow::~GuiWindow() {
    delete m_rootPane;
}


void GuiWindow::setFocusControl(GuiControl* c) {
    if (c->enabled() && c->visible()) { 
        keyFocusGuiControl = c;
    }
}


void GuiWindow::increaseBounds(const Vector2& extent) {
    if ((m_clientRect.width() < extent.x) || (m_clientRect.height() < extent.y)) {
        // Create the new client rect
        Rect2D newRect = Rect2D::xywh(Vector2(0,0), extent.max(m_clientRect.wh()));

        // Transform the client rect into an absolute rect
        if (m_style != NO_FRAME_STYLE) {
            newRect = m_skin->clientToWindowBounds(newRect, GuiSkin::WindowStyle(m_style));
        }

        // The new window has the old position and the new width
        setRect(Rect2D::xywh(m_rect.x0y0(), newRect.wh()));
    }
}


void GuiWindow::morphTo(const Rect2D& r) {
    // Terminate any drag
    inDrag = false;

    m_morph.morphTo(rect(), r);
}


void GuiWindow::setRect(const Rect2D& r) {
    m_rect = r;
    m_morph.active = false;
    
    if (m_style == NO_FRAME_STYLE) {
        m_clientRect = m_rect;
    } else {
        m_clientRect = m_skin->windowToClientBounds(m_rect, GuiSkin::WindowStyle(m_style));
    }
}


void GuiWindow::focusOnNextControl() {
    // For now, just lose focus
    keyFocusGuiControl = NULL;

    // TODO
    /*
     Find the pane containing the current control.
          Find the index of the control that has focus
          Go to the next control in that pane.  
          If there are no more controls, go to the parent's next pane.  
          If there are no more panes, go to the parent's controls.

     If we run out on the root pane, go to its deepest pane child's first control.

     ...
    */
}

void GuiWindow::onUserInput(UserInput* ui) {
    // Not in focus if the mouse is invisible
    m_mouseVisible = (ui->window()->mouseHideCount() <= 0);

    m_focused =
        m_visible &&
        (m_manager->focusedWidget().pointer() == this) &&
        m_mouseVisible;

    if (! focused()) {
        return;
    }

    Vector2 mouse = ui->mouseXY();
    mouseOverGuiControl = NULL;

    if (inDrag) {
        setRect(dragOriginalRect + mouse - dragStart);
        return;
    }

    m_closeButton.mouseOver = false;
    if (m_rect.contains(mouse)) {
        // The mouse is over this window, update the mouseOver control
        
        if ((m_closeAction != NO_CLOSE) && (m_style != NO_FRAME_STYLE)) {
            m_closeButton.mouseOver = 
                m_skin->windowToCloseButtonBounds(m_rect, GuiSkin::WindowStyle(m_style)).contains(mouse);
        }

        mouse -= m_clientRect.x0y0();
        m_rootPane->findControlUnderMouse(mouse, mouseOverGuiControl);
    }
}


void GuiWindow::onPose(Array<PosedModelRef>& posedArray, Array<PosedModel2DRef>& posed2DArray) {
    if (m_visible) {
        posed2DArray.append(posed);
    }
}

static bool isMouseEvent(const GEvent& e) {
    return (e.type == GEventType::MOUSE_MOTION) ||
        (e.type == GEventType::MOUSE_BUTTON_DOWN) ||
        (e.type == GEventType::MOUSE_BUTTON_UP);
}

static GEvent makeRelative(const GEvent& e, const Vector2& clientOrigin) {
    GEvent out(e);

    switch (e.type) {
    case GEventType::MOUSE_MOTION:
        out.motion.x -= (uint16)clientOrigin.x;
        out.motion.y -= (uint16)clientOrigin.y;
        break;

    case GEventType::MOUSE_BUTTON_DOWN:
    case GEventType::MOUSE_BUTTON_UP:
        out.button.x -= (uint16)clientOrigin.x;
        out.button.y -= (uint16)clientOrigin.y;        
        break;
    }

    return out;
}


bool GuiWindow::onEvent(const GEvent &event) {
    if (! m_mouseVisible || ! m_visible) {
        // Can't be using the GuiWindow if the mouse isn't visible or the gui isn't visible
        return false;
    }

    bool consumed = false;

    if (event.type == GEventType::MOUSE_BUTTON_DOWN) {
        // Mouse down; change the focus
        Vector2 mouse(event.button.x, event.button.y);

        if (! m_rect.contains(mouse)) {
            // The click was not on this object.  Lost focus if we have it
            if (focused()) {
                m_manager->setFocusedWidget(this);
            }
            return false;
        }

        if (! focused()) {
            // Set focus
            m_manager->setFocusedWidget(this);
            m_focused = true;

            // Most windowing systems do not allow the original click
            // to reach a control if it was consumed on focusing the
            // window.  However, we deliver events because for most 3D
            // programs the multiple windows are probably acting like
            // tool windows and should not require multiple clicks for
            // selection.
        }

        Rect2D titleRect;
        Rect2D closeRect;
        if (m_style ==  NO_FRAME_STYLE) {
            titleRect = Rect2D::xywh(m_rect.x0y0(), Vector2(m_rect.width(), 0));
        } else {
            titleRect = m_skin->windowToTitleBounds(m_rect, GuiSkin::WindowStyle(m_style));
            closeRect = m_skin->windowToCloseButtonBounds(m_rect, GuiSkin::WindowStyle(m_style));
        }

        if ((m_closeAction != NO_CLOSE) && closeRect.contains(mouse)) {
            close();
            return true;
        }

        if (titleRect.contains(mouse)) {
            inDrag = true;
            dragStart = mouse;
            dragOriginalRect = m_rect;
            return true;

        } else {

            mouse -= m_clientRect.x0y0();

            keyFocusGuiControl = NULL;
            m_rootPane->findControlUnderMouse(mouse, keyFocusGuiControl);
        }

        if (m_style != NO_FRAME_STYLE) {
            // Consume the click, since it was somewhere on this window (it may still
            // be used by another one of the controls on this window).
            consumed = true;
        }
    } else if (event.type == GEventType::MOUSE_BUTTON_UP) {
        if (inDrag) {
            inDrag = false;
            return true;
        }
    }

    // If this window is not in focus, don't bother checking to see if
    // its controls will receive the event.
    if (! focused()) {
        return consumed;
    }
    
    if (keyFocusGuiControl != NULL) {
        // Deliver event to the control

        if (isMouseEvent(event)) {

            // Make the event relative by accumulating all of the transformations
            Vector2 origin = m_clientRect.x0y0();
            GuiPane* p = keyFocusGuiControl->m_parent;
            while (p != NULL) {
                origin += p->m_clientRect.x0y0();
                p = p->m_parent;
            }
            consumed = keyFocusGuiControl->onEvent(makeRelative(event, origin)) || consumed;

        } else {
            consumed = keyFocusGuiControl->onEvent(event) || consumed;
        }
    }

    return consumed;
}

void GuiWindow::close() {
    switch (m_closeAction) {
    case NO_CLOSE:
      debugAssertM(false, "Internal Error");
      break;
    
    case HIDE_ON_CLOSE:
        setVisible(false);
        // Intentionally fall through

    case IGNORE_CLOSE:
        {
            GEvent e;
            e.guiClose.type = GEventType::GUI_CLOSE;
            e.guiClose.window = this;
            fireEvent(e);
        }
        break;
    
    case REMOVE_ON_CLOSE:
        {
            GEvent e;
            e.guiClose.type = GEventType::GUI_CLOSE;
            e.guiClose.window = NULL;
            fireEvent(e);
        }
        break;
    }
}


void GuiWindow::pack() {
    m_rootPane->pack();
    increaseBounds(m_rootPane->rect().wh());
}


void GuiWindow::render(RenderDevice* rd) {
    
    if (m_morph.active) {
        m_morph.update(this);
    }
    
    m_skin->beginRendering(rd);
    {
        bool hasClose = m_closeAction != NO_CLOSE;

        if (m_style != NO_FRAME_STYLE) {
            m_skin->renderWindow(m_rect, focused(), hasClose, m_closeButton.down,
                               m_closeButton.mouseOver, m_text, GuiSkin::WindowStyle(m_style));
        } else {
            debugAssertM(m_closeAction == NO_CLOSE, "Windows without frames cannot have a close button.");
        }
        
        m_skin->pushClientRect(m_clientRect);
            m_rootPane->render(rd, m_skin);
  /*
  // Code for debugging window sizes
rd->endPrimitive();
rd->pushState();
rd->setTexture(0, NULL);
Draw::rect2D(Rect2D::xywh(-100, -100, 1000, 1000), rd, Color3::red());
rd->popState();
rd->beginPrimitive(RenderDevice::QUADS);
*/
        m_skin->popClientRect();
        /*
  // Code for debugging window sizes
rd->endPrimitive();
rd->pushState();
rd->setTexture(0, NULL);
Draw::rect2D(m_rect + Vector2(20,0), rd, Color3::blue());
rd->popState();
rd->beginPrimitive(RenderDevice::QUADS);
*/
    }
    m_skin->endRendering();
    
}

void GuiWindow::moveToCenter() {
    if (window() != NULL) {
        setRect(Rect2D::xywh((window()->dimensions().wh() - rect().wh()) / 2.0f, rect().wh()));
    }
}

////////////////////////////////////////////////////////////
// Modal support

void GuiWindow::showModal(GWindow* osWindow) {
    modal = new Modal(osWindow);
    modal->run(this);
    delete modal;
    modal = NULL;
}


void GuiWindow::showModal(GuiWindow::Ref parent) {
    showModal(parent->window());
}


GuiWindow::Modal::Modal(GWindow* osWindow) : osWindow(osWindow) {
    manager = WidgetManager::create(osWindow);
    renderDevice = osWindow->renderDevice();
    userInput = new UserInput(osWindow);

    viewport = renderDevice->viewport();

    // Grab the screen texture
    if (GLCaps::supports_GL_ARB_texture_non_power_of_two()) {
        image = Texture::createEmpty("Old screen image", (int)viewport.width(), (int)viewport.height(), 
                                     TextureFormat::RGB8(), Texture::DIM_2D_NPOT, Texture::Settings::video());
    } else {
        image = Texture::createEmpty("Old screen image", 512, 512,
                                     TextureFormat::RGB8(), Texture::DIM_2D, Texture::Settings::video());
    }

    image->copyFromScreen(viewport, false);
}


GuiWindow::Modal::~Modal() {
    delete userInput;
}


void GuiWindow::Modal::run(GuiWindow::Ref dialog) {
    this->dialog = dialog.pointer();
    manager->add(dialog);
    manager->setFocusedWidget(dialog);
    dialog->setVisible(true);

    if (osWindow->requiresMainLoop()) {
        osWindow->pushLoopBody(loopBody, this);
    } else {
        do {
            oneFrame();
        } while (dialog->visible());
    }

}

void GuiWindow::Modal::loopBody(void* me) {
    reinterpret_cast<GuiWindow::Modal*>(me)->oneFrame();
}

void GuiWindow::Modal::oneFrame() {
    double desiredFrameDuration = 1 / 60.0;

    processEventQueue();

    manager->onUserInput(userInput);

    manager->onNetwork();

    {
        // Pretend that we acheive our frame rate
        RealTime rdt = desiredFrameDuration;
        SimTime  sdt = desiredFrameDuration;
        SimTime  idt = desiredFrameDuration;

        manager->onSimulation(rdt, sdt, idt);
    }

    // Logic
    manager->onLogic();

    // Sleep to keep the frame rate at about the desired frame rate
    System::sleep(0.9 * desiredFrameDuration);

    // Graphics
    renderDevice->beginFrame();
    {
        renderDevice->push2D();
        {
            renderDevice->setTexture(0, image);
            Draw::fastRect2D(viewport, renderDevice, Color3::white() * 0.5f);
            
            // Desaturate the image by drawing white over it
            renderDevice->setTexture(0, NULL);
            renderDevice->setBlendFunc(RenderDevice::BLEND_ONE, RenderDevice::BLEND_ONE);
            Draw::fastRect2D(viewport, renderDevice, Color3::white() * 0.3f);            
        }
        renderDevice->pop2D();

        renderDevice->pushState();
        {
            Array<PosedModel::Ref> posedArray, opaque, transparent; 
            Array<PosedModel2DRef> posed2DArray;

            manager->onPose(posedArray, posed2DArray);

            // 3D
            if (posedArray.size() > 0) {
                Vector3 lookVector = renderDevice->getCameraToWorldMatrix().lookVector();
                PosedModel::sort(posedArray, lookVector, opaque, transparent);
                
                for (int i = 0; i < opaque.size(); ++i) {
                    opaque[i]->render(renderDevice);
                }
                
                for (int i = 0; i < transparent.size(); ++i) {
                    transparent[i]->render(renderDevice);
                }
            }
            
            if (posed2DArray.size() > 0) {
                renderDevice->push2D();
                PosedModel2D::sort(posed2DArray);
                for (int i = 0; i < posed2DArray.size(); ++i) {
                    posed2DArray[i]->render(renderDevice);
                }
                renderDevice->pop2D();
            }
        }
        renderDevice->popState();
    }
    renderDevice->endFrame();
        
    if (! dialog->visible() && osWindow->requiresMainLoop()) {
        osWindow->popLoopBody();
    }
}

void GuiWindow::Modal::processEventQueue() {
    userInput->beginEvents();

    // Event handling
    GEvent event;
    while (osWindow->pollEvent(event)) {
        if (WidgetManager::onEvent(event, manager)) {
            continue;
        }

        switch(event.type) {
        case GEventType::QUIT:
            exit(0);
            break;

        case GEventType::VIDEO_RESIZE:
            {
                renderDevice->notifyResize(event.resize.w, event.resize.h);
                Rect2D full =  Rect2D::xywh(0, 0, renderDevice->width(), 
                                            renderDevice->height());
                renderDevice->setViewport(full);
            }
            break;

        default:;
        }

        userInput->processEvent(event);
    }

    userInput->endEvents();
}

///////////////////////////////////////////////////////////////////////


GuiWindow::Posed::Posed(GuiWindow* gui) : gui(gui) {}

Rect2D GuiWindow::Posed::bounds () const {
    return gui->m_rect;
}

float GuiWindow::Posed::depth () const {
    return 0;
}

void GuiWindow::Posed::render (RenderDevice *rd) const {
    gui->render(rd);
}

}
