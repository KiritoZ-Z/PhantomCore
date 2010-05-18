/**
  @file PixelProgram.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-04-13
  @edited  2003-09-09
*/

#include "GLG3D/PixelProgram.h"

namespace G3D {


PixelProgram::PixelProgram(const std::string& name, const std::string& filename) : 
    GPUProgram(name, filename) {
}


PixelProgramRef PixelProgram::fromFile(const std::string& _filename) {
    PixelProgram* v = new PixelProgram(_filename, _filename);
    v->reload(std::string(""));
    return v;
}


PixelProgramRef PixelProgram::fromCode(const std::string& name, const std::string& code) {
    PixelProgram* v = new PixelProgram(name, std::string(""));
    v->reload(code);
    return v;
}

}

