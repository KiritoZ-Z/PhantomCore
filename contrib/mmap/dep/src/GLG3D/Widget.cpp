/**
 @file Widget.cpp

 @maintainer Morgan McGuire, morgan3d@users.sourceforge.net

 @created 2006-04-22
 @edited  2007-05-31
*/

#include "GLG3D/Widget.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/GEvent.h"

namespace G3D {

void Widget::fireEvent(const GEvent& event) {
    m_manager->fireEvent(event);
}


GWindow* Widget::window() const {
    return m_manager->window();
}


GWindow* WidgetManager::window() const {
    return m_window;
}


WidgetManager::Ref WidgetManager::create(GWindow* window) {
    WidgetManager* m = new WidgetManager();
    m->m_window = window;
    return m;
}


void WidgetManager::fireEvent(const GEvent& event) {
    m_window->fireEvent(event);
}


WidgetManager::WidgetManager() : 
    m_locked(false) {
}


int WidgetManager::size() const {
    return m_moduleArray.size();
}


const Widget::Ref& WidgetManager::operator[](int i) const {
    return m_moduleArray[i];
}


void WidgetManager::beginLock() {
    debugAssert(! m_locked);
    m_locked = true;
}


void WidgetManager::endLock() {
    debugAssert(m_locked);
    m_locked = false;

    for (int e = 0; e < m_delayedEvent.size(); ++e) {
        DelayedEvent& event = m_delayedEvent[e];
        switch (event.type) {
        case DelayedEvent::REMOVE_ALL:
            clear();
            break;

        case DelayedEvent::REMOVE:
            remove(event.module);
            break;

        case DelayedEvent::ADD:
            add(event.module);
            break;
            
        case DelayedEvent::SET_FOCUS:
            setFocusedWidget(event.module);
            break;

        case DelayedEvent::SET_DEFOCUS:
            defocusWidget(event.module);
            break;
        }
    }

    m_delayedEvent.fastClear();
}


void WidgetManager::remove(const Widget::Ref& m) {
    if (m_locked) {
        m_delayedEvent.append(DelayedEvent(DelayedEvent::REMOVE, m));
    } else {
        if (m_focusedModule == m) {
            m_focusedModule = NULL;
        }
        int j = m_moduleArray.findIndex(m);
        if (j != -1) {
            m->setManager(NULL);
            m_moduleArray.remove(j);
            return;
        }
        debugAssertM(false, "Removed a Widget that was not in the manager.");
    }
}


void WidgetManager::add(const Widget::Ref& m) {
    debugAssert(m.notNull());
    if (m_locked) {
        m_delayedEvent.append(DelayedEvent(DelayedEvent::ADD, m));
    } else {
        if (m_focusedModule.notNull()) {
            // Cannot replace the focused module at the top of the priority list
            m_moduleArray[m_moduleArray.size() - 1] = m;
            m_moduleArray.append(m_focusedModule);
        } else {
            m_moduleArray.append(m);
        }
        m->setManager(this);
    }
}


Widget::Ref WidgetManager::focusedWidget() const {
    return m_focusedModule;
}


void WidgetManager::defocusWidget(const Widget::Ref& m) {
   if (m_locked) {
        m_delayedEvent.append(DelayedEvent(DelayedEvent::SET_DEFOCUS, m));
   } else if (focusedWidget().pointer() == m.pointer()) {
       setFocusedWidget(NULL);
   }    
}


void WidgetManager::setFocusedWidget(const Widget::Ref& m) {
    if (m_locked) {
        m_delayedEvent.append(DelayedEvent(DelayedEvent::SET_FOCUS, m));
    } else {

        debugAssert(m.isNull() || m_moduleArray.contains(m));

        if (m.notNull()) {
            // Move to the first event position
            int i = m_moduleArray.findIndex(m);
            m_moduleArray.remove(i);
            m_moduleArray.append(m);
        }

        m_focusedModule = m;
    }
}


void WidgetManager::clear() {
    if (m_locked) {
        m_delayedEvent.append(DelayedEvent(DelayedEvent::REMOVE_ALL));
    } else {
        m_moduleArray.clear();
        m_focusedModule = NULL;
    }
}

// Iterate through all modules in priority order.
// This same iteration code is used to implement
// all Widget methods concisely.
#define ITERATOR(body)\
    beginLock();\
        for (int i = m_moduleArray.size() - 1; i >=0; --i) {\
            body;\
        }\
    endLock();


void WidgetManager::onPose(
    Array<PosedModel::Ref>& posedArray, 
    Array<PosedModel2DRef>& posed2DArray) {

    beginLock();
    for (int i = 0; i < m_moduleArray.size(); ++i) {
        m_moduleArray[i]->onPose(posedArray, posed2DArray);
    }
    endLock();

}


void WidgetManager::onSimulation(RealTime rdt, SimTime sdt, SimTime idt) {
    ITERATOR(m_moduleArray[i]->onSimulation(rdt, sdt, idt));
}


bool WidgetManager::onEvent(const GEvent& event) {
    bool motionEvent = 
        (event.type == GEventType::MOUSE_MOTION) ||
        (event.type == GEventType::JOY_AXIS_MOTION) ||
        (event.type == GEventType::JOY_HAT_MOTION) ||
        (event.type == GEventType::JOY_BALL_MOTION);

    // if the event is ever consumed, abort iteration
    ITERATOR(if (m_moduleArray[i]->onEvent(event) && ! motionEvent) { endLock(); return true; });
    return false;
}

void WidgetManager::onUserInput(UserInput* ui) {
    ITERATOR(m_moduleArray[i]->onUserInput(ui));
}

void WidgetManager::onNetwork() {
    ITERATOR(m_moduleArray[i]->onNetwork());
}

void WidgetManager::onLogic() {
    ITERATOR(m_moduleArray[i]->onLogic());
}

#undef ITERATOR

bool WidgetManager::onEvent(const GEvent& event, 
                             WidgetManager::Ref& a) {
    static WidgetManager::Ref x(NULL);
    return onEvent(event, a, x);
}


bool WidgetManager::onEvent(const GEvent& event, WidgetManager::Ref& a, WidgetManager::Ref& b) {
    a->beginLock();
    if (b.notNull()) {
        b->beginLock();
    }

    int numManagers = (b.isNull()) ? 1 : 2;

    // Process each
    for (int k = 0; k < numManagers; ++k) {
        Array<Widget::Ref>& array = 
            (k == 0) ?
            a->m_moduleArray :
            b->m_moduleArray;
                
        for (int i = array.size() - 1; i >= 0; --i) {
            if (array[i]->onEvent(event)) {
                if (b.notNull()) {
                    b->endLock();
                }
                
                a->endLock();
                return true;
            }
        }
    }
    
    if (b.notNull()) {
        b->endLock();
    }
    a->endLock();

    return false;
}


} // G3D
