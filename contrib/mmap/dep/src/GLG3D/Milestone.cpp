/**
  @file Milestone.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-08-09
  @edited  2003-08-12
*/

#include "GLG3D/Milestone.h"
#include "GLG3D/RenderDevice.h"

namespace G3D {

Array<Milestone::GLFence> Milestone::factory;

Milestone::Milestone(const std::string& n) : _name(n), isSet(false) {

    if (glGenFencesNV) {
        if (factory.size() == 0) {
            int num = 10;
            factory.resize(num, DONT_SHRINK_UNDERLYING_ARRAY);
            glGenFencesNV(num, factory.getCArray());
        }
        
        // Grab a preallocated fence
        glfence = factory.pop();
    }
}


Milestone::~Milestone() {
    if (glDeleteFencesNV) {
        // The global factory might have been deallocated already on shutdown
        if (factory.getCArray() != NULL) {
            // Save it for later
            factory.push(glfence);

            if (factory.size() > 20) {
                // We've deallocated an awful lot, so free some
                glDeleteFencesNV(factory.size() - 5, &(factory.last()) - 5);
                factory.resize(20);
            }

        } else {
            glDeleteFencesNV(1, &glfence);
        }
    }
}


void Milestone::set() {
    if (glSetFenceNV) {
        glSetFenceNV(glfence, GL_ALL_COMPLETED_NV);
    }

    isSet = true;
}


void Milestone::wait() {
    debugAssertM(isSet, std::string("Waiting for a milestone (\"") + _name + "\") that was not set!");

    if (glFinishFenceNV) {
        glFinishFenceNV(glfence);
    } else {
        // With no way of waiting for a specific fence, we must wait for everything.
        glFinish();
    }

    isSet = false;
}

}
