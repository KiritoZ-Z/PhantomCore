/** 
  @file Color1uint8.h
 
  @maintainer Morgan McGuire, graphics3d.com
 
  @created 2007-01-30
  @edited  2007-01-30

  Copyright 2000-2007, Morgan McGuire.
  All rights reserved.
 */

#ifndef G3D_COLOR1UINT8_H
#define G3D_COLOR1UINT8_H

#include "G3D/platform.h"
#include "G3D/g3dmath.h"

namespace G3D {

/**
 Represents a Color1 as a packed integer.  Convenient
 for creating unsigned int vertex arrays. 

 <B>WARNING</B>: Integer color formats are different than
 integer vertex formats.  The color channels are automatically
 scaled by 255 (because OpenGL automatically scales integer
 colors back by this factor).  So Color3(1,1,1) == Color3uint8(255,255,255)
 but Vector3(1,1,1) == Vector3int16(1,1,1).

 */

#if defined(G3D_WIN32)
    // Switch to tight alignment
    #pragma pack(push, 1)
#endif

class Color1uint8 {
private:
    // Hidden operators
    bool operator<(const Color1uint8&) const;
    bool operator>(const Color1uint8&) const;
    bool operator<=(const Color1uint8&) const;
    bool operator>=(const Color1uint8&) const;

public:

    uint8       value;

    Color1uint8() : value(0) {}

    explicit Color1uint8(const uint8 _v) : value(_v) {}

    Color1uint8(const class Color1& c);

    Color1uint8(class BinaryInput& bi);

    void serialize(class BinaryOutput& bo) const;

    void deserialize(class BinaryInput& bi);

    inline bool operator==(const Color1uint8& other) const {
        return value == other.value;
    }

    inline bool operator!=(const Color1uint8& other) const {
        return value != other.value;
    }

}

#if defined(G3D_LINUX) || defined(G3D_OSX)
    __attribute((aligned(1)))
#endif

;

#ifdef G3D_WIN32
  #pragma pack(pop)
#endif

}

#endif
