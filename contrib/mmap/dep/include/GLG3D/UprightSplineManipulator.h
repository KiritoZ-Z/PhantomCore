/**
  @file UprightSplineManipulator.h

  @maintainer Morgan McGuire, morgan@cs.williams.edu

  @created 2002-07-28
  @edited  2006-08-10
*/
#ifndef G3D_UPRIGHTSPLINEMANIPULATOR_H
#define G3D_UPRIGHTSPLINEMANIPULATOR_H

#include "G3D/platform.h"
#include "G3D/Color3.h"
#include "G3D/UprightFrame.h"
#include "G3D/g3dmath.h"
#include "GLG3D/Widget.h"

namespace G3D {

typedef ReferenceCountedPointer<class UprightSplineManipulator> UprightSplineManipulatorRef;

/**
 Allows recording and playback of a G3D::UprightSpline based on a G3D::GCamera.
 Used by G3D::CameraControlWindow.

 @sa G3D::UprightFrame, G3D::UprightSpline, G3D::FirstPersonManipulator
 */
class UprightSplineManipulator : public Manipulator {
public:
    typedef ReferenceCountedPointer<class UprightSplineManipulator> Ref;
    
    /**
       RECORD_INTERVAL_MODE: Automatically add one frame per
       second to the spline, reading from the camera.

       RECORD_KEY_MODE: Add one frame every time the recordKey() is
       pressed.

       PLAY_MODE: Move the frame() through the spline over time.  Does not mutate the
       camera.

       INACTIVE_MODE: Do nothing.
       
       The manipulator begins in INACTIVE_MODE.
     */
    enum Mode {RECORD_INTERVAL_MODE, RECORD_KEY_MODE, PLAY_MODE, INACTIVE_MODE};

protected:

    UprightSpline          m_spline;

    /** Current time during playback. */
    RealTime               m_time;
    
    Mode                   m_mode;

    class GCamera*         m_camera;

    CoordinateFrame        m_currentFrame;

    bool                   m_showPath;

    Color3                 m_pathColor;

    /** samples per second */
    const float            m_sampleRate;

    GKey                   m_recordKey;

    UprightSplineManipulator();

public:
    
    static Ref create(class GCamera* c = NULL);
    
    virtual void onNetwork() {}
    virtual void onLogic() {}

    /** Returns the underlying spline. */
    const UprightSpline& spline() const {
        return m_spline;
    }

    void setSpline(const UprightSpline& s) {
        m_spline = s;
    }
    
    GKey recordKey() const {
        return m_recordKey;
    }

    void setRecordKey(GKey k) {
        m_recordKey = k;
    }
    
    void setCyclic(bool b) {
        m_spline.cyclic = b;
    }

    /** Does the spline wrap around to the first control point. */
    bool cyclic() const {
        return m_spline.cyclic;
    }

    /** If set to true, the camera's path is shown as a glowing curve. */
    void setShowPath(bool b) {
        m_showPath  = b;
    }

    bool showPath() const {
        return m_showPath;
    }
    
    void setPathColor(const Color3& c) {
        m_pathColor = c;
    }

    const Color3& pathColor() const {
        return m_pathColor;
    }

    /** Setting the mode to RECORD_KEY_MODE and RECORD_INTERVAL_MODE
        require that a non-NULL camera have previously been set. When
        in INACTIVE_MODE, time does not elapse for the manipulator.*/
    void setMode(Mode m);

    Mode mode() const;

    /** Number of control points */
    int splineSize() const {
        return m_spline.control.size();
    }

    GCamera* camera() const {
        return m_camera;
    }
    
    /** Destroys the current path */
    void clear();

    /** Time since the beginning of recording or playback. */
    void setTime(double t);

    double time() const {
        return m_time;
    }

    /**
       Sets the camera from which the frame will be read when recording.  
       Argument may be NULL, but recording cannot proceed until this is non-null.

       This camera will also be used for playback
     */
    void setCamera(GCamera* c) {
        m_camera = c;
    }

    GCamera* camera() {
        return m_camera;
    }

    virtual CoordinateFrame frame() const;
    virtual void getFrame(CoordinateFrame &c) const;
    virtual void onPose(Array< PosedModelRef > &posedArray, Array< PosedModel2DRef > &posed2DArray);
    virtual bool onEvent (const GEvent &event);
    virtual void onSimulation (RealTime rdt, SimTime sdt, SimTime idt);
    virtual void onUserInput (UserInput *ui);
    
};

}

#endif
