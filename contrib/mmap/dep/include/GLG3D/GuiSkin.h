/**
  @file GuiSkin.h
  @maintainer Morgan McGuire, morgan@cs.williams.edu
  Copyright 2001-2007, Morgan McGuire
 */

#ifndef G3D_GUISKIN_H
#define G3D_GUISKIN_H

#include "G3D/platform.h"
#include "G3D/Rect2D.h"
#include "GLG3D/Texture.h"
#include "GLG3D/GFont.h"
#include "G3D/Table.h"
#include "GLG3D/GuiCaption.h"

namespace G3D {

typedef ReferenceCountedPointer<class GuiSkin> GuiSkinRef;

/**
   Renders UI elements.
   Loads and renders G3D GUI .skn files, which can be used with G3D::Gui.  .skn files
   can be found in the data module data/gui directory.

   Can also be used to explicitly render a UI without actual controls behind it, for example:

  <pre>
   skin->beginRendering(rd);
       skin->renderWindow(rd, Rect2D::xywh(80, 70, 550, 250), false, false, false, "Window", GuiSkin::NORMAL_WINDOW_STYLE);
       skin->renderWindow(rd, Rect2D::xywh(500, 120, 50, 150), true, true, true, "Tools", GuiSkin::TOOL_WINDOW_STYLE);
       skin->renderCheckBox(rd, Rect2D::xywh(100, 100, 20, 20), true, true, true, "Check box");
       skin->renderRadioButton(rd, Rect2D::xywh(100, 120, 20, 20), true, false, false, "Radio button");
       skin->renderButton(rd, Rect2D::xywh(100, 160, 80, 27), true, true, true, "Button");
       skin->setFont(timesFont, 12, Color3::black(), Color4::clear());
       skin->renderButton(rd, Rect2D::xywh(200, 160, 80, 27), true, true, true, "New Font");
       skin->setFont(arialFont, 10, Color3::black(), Color4::clear());
       skin->renderHorizontalSlider(rd, Rect2D::xywh(100, 200, 150, 27), 0.5f, true, false, "Slider");

       int s = 30;
       skin->renderButton(rd, Rect2D::xywh(100 + s * 0, 230, 30, 30), true, false, false, GuiCaption("7", iconFont));
       skin->renderButton(rd, Rect2D::xywh(100 + s * 1, 230, 30, 30), true, false, false, GuiCaption("4", iconFont));
       skin->renderButton(rd, Rect2D::xywh(100 + s * 2, 230, 30, 30), true, false, false, GuiCaption("=", iconFont));
       skin->renderButton(rd, Rect2D::xywh(100 + s * 3, 230, 30, 30), true, false, true, GuiCaption(";", iconFont));
       skin->renderButton(rd, Rect2D::xywh(100 + s * 4, 230, 30, 30), true, false, false, GuiCaption("<", iconFont));
       skin->renderButton(rd, Rect2D::xywh(100 + s * 5, 230, 30, 30), true, false, false, GuiCaption("8", iconFont));
       skin->setFont(arialFont, 10, Color3::black(), Color4::clear());
    skin->endRendering(rd);
   </pre>
  @sa G3D::GuiWindow
*/
class GuiSkin : public ReferenceCountedObject {
public:

    typedef ReferenceCountedPointer<class GuiSkin> Ref;

    enum WindowStyle {
        NORMAL_WINDOW_STYLE,
        TOOL_WINDOW_STYLE,
        DIALOG_WINDOW_STYLE,
        DRAWER_WINDOW_STYLE,

        WINDOW_STYLE_COUNT
    };
    
    enum PaneStyle {
        SIMPLE_PANE_STYLE,
        ORNATE_PANE_STYLE,

        PANE_STYLE_COUNT
    };

    enum ButtonStyle {
        NORMAL_BUTTON_STYLE,
        TOOL_BUTTON_STYLE,
        NO_BUTTON_STYLE,

        BUTTON_STYLE_COUNT
    };

private:

    /**
     Default style information for captions.
     */
    class TextStyle {
    public:
        GFont::Ref          font;
        Color4              color;
        Color4              outlineColor;
        float               size;

        /**
         @param path Path to search for font files
         */
        void deserialize(const std::string& path, const std::string& name, TextInput& t);
    };

    /** When a caption appears on the left of a control, inset the control by this amount */
    enum {LEFT_CAPTION_WIDTH = 90};
    
    /** Used for delayed text rendering. */
    class Text {
    public:
        Vector2             position;
        std::string         text;
        GFont::XAlign       xAlign;
        GFont::YAlign       yAlign;
        float               size;
        Color4              color;
        Color4              outlineColor;
    };

    /** Delayed text, organized by the associated font.*/
    Table<GFontRef, Array<Text> >       delayedText;

    /** Number of values in delayedText's arrays.  Used to 
        detect when we need to enter font rendering mode. */
    int                                 delayedTextCount;


    /** Clears the delayedText array. */
    void drawDelayedText() const;

    /** Postpones rendering the specified text until later. Switching
        between the GUI texture and the font texture is relatively
        slow, so delaying text rendering until a few controls have
        been put down increases performance.

        Note that delayed text must be drawn before the clipping region is changed or another window is rendered.
    */
    void addDelayedText(GFontRef font, const std::string& text, const Vector2& position, float size, 
                        const Color4& color, const Color4& outlineColor, GFont::XAlign xalign, 
                        GFont::YAlign yalign = GFont::YALIGN_CENTER) const;

    enum {TEXTURE_UNIT = 0};

    enum StretchMode {TILE, STRETCH};

    // All internal coordinates are stored in texture coordinates

    /** */
    class Fill {
    public:
        StretchMode     horizontalMode;
        StretchMode     verticalMode;

        /** Source rectangle.  This does not have to correspond to the 
            dimensions of the destination rectangle at all.  When 
            rendered, mode will be used to fill the destination rectangle
            up to the correct dimensions if source is too small.  If too source
            is too large it is cropped on the bottom and right.
         */
        Rect2D          rect;

        void deserialize(const std::string& name, TextInput& b);
        void render(class RenderDevice* rd, const Rect2D& bounds, const Vector2& texOffset) const;        
    };

    /** 
        Horizontal stretch--two caps and a center fill
     */
    class StretchRectH {
    public:
        Rect2D         left;
        Fill           center;
        Rect2D         right;

        /** Name is prepended onto Left, Center, Right */
        void deserialize(const std::string& name, TextInput& b);

        /** Bounds height must match left.height and right.height */
        void render(class RenderDevice* rd, const Rect2D& bounds, const Vector2& texOffset) const;
        
        inline float height() const {
            return left.height();
        }
    };

    /** 
        Vertical stretch--two caps and a center fill
     */
    class StretchRectV {
    public:
        Rect2D         top;
        Rect2D         bottom;
        Fill           center;
        void deserialize(TextInput& b);
        void render(class RenderDevice* rd, const Rect2D& bounds, const Vector2& texOffset) const;
    };
    
    /** Stretchable in horizontal and vertical direction. */
    class StretchRectHV {
    public:
        StretchRectH       top;
        Fill               centerLeft;
        Fill               centerCenter;
        Fill               centerRight;
        StretchRectH       bottom;

        void deserialize(const std::string& name, TextInput& b);
        void render(class RenderDevice* rd, const Rect2D& bounds, const Vector2& texOffset) const;
    };

    class Pad {
    public:
        Vector2      topLeft;
        Vector2      bottomRight;
        
        void deserialize(const std::string& name, TextInput& b);
        
        /** Net width and height of the padding */
        inline Vector2 wh() const {
            return topLeft + bottomRight;
        }
    };


    class DropDownList {
    public:
        StretchRectH        base;

        /** Offsets from base of area for text display */
        Pad                 textPad;
        
        class Pair {
        public:
            /** For use during selection */
            Vector2         down;

            Vector2         up;
            void deserialize(const std::string& name, TextInput& b);
        };
        
        class Focus {
        public:
            Pair            focused;
            Vector2         defocused;
            void deserialize(const std::string& name, TextInput& b);
        };
 
        Focus               enabled;
        Vector2             disabled;

        TextStyle           textStyle;
        TextStyle           disabledTextStyle;

        void deserialize(const std::string& name, const std::string& path, TextInput& b);
        void render(RenderDevice* rd, const Rect2D& bounds, bool enabled, bool focused, bool pushed) const;
    };

    class Button {
    public:
        /**
           General texture coordinates for any mode of the button
         */
        StretchRectHV        base;

        /**
           Displacement from bounds center.
         */
        Vector2              textOffset;

        /** Defaults */
        TextStyle            textStyle;

        TextStyle            disabledTextStyle;

        class Pair {
        public:
            /** TexOffset from base of this image */
            Vector2          down;

            /** TexOffset from base of this image */
            Vector2          up;
            void deserialize(const std::string& name, TextInput& b);
        };

        class Focus {
        public:
            Pair      focused;
            Pair      defocused;
            void deserialize(const std::string& name, TextInput& b);
        };
    
        Focus         enabled;
        Pair          disabled;

        void deserialize(const std::string& name, const std::string& path, TextInput& b);
        void render(RenderDevice* rd, const Rect2D& bounds, bool enabled, bool focused, bool pushed) const;
    };


    class TextBox {
    public:
        /**
           General texture coordinates for any mode
         */
        StretchRectHV        base;

        Pad                  textPad;

        /** Defaults */
        TextStyle            textStyle;

        TextStyle            disabledTextStyle;

        /** For the user value*/
        TextStyle            contentStyle;

        class Focus {
        public:
            Vector2    focused;
            Vector2    defocused;
            void deserialize(const std::string& name, TextInput& b);
        };
    
        Focus         enabled;
        Vector2       disabled;

        void deserialize(const std::string& name, const std::string& path, TextInput& b);
        void render(RenderDevice* rd, const Rect2D& bounds, bool enabled, bool focused) const;
    };


    /** Used for radio and check boxes */
    class Checkable {
    public:
        class Pair {
        public:
            Rect2D    checked;
            Rect2D    unchecked;
            void deserialize(const std::string& name, TextInput& b);
        };

        class Focus {
        public:
            Pair      focused;
            Pair      defocused;
            void deserialize(const std::string& name, TextInput& b);
        };
    
        Focus         enabled;
        Pair          disabled;

        Vector2       textOffset;

        /** Defaults */
        TextStyle            textStyle;
        TextStyle            disabledTextStyle;

        void deserialize(const std::string& name, const std::string& path, TextInput& b);
        void render(RenderDevice* rd, const Rect2D& bounds, bool enabled, bool focused, bool checked) const;
        
        inline float width() const {
            return disabled.checked.width();
        }

        inline float height() const {
            return disabled.checked.height();
        }
    };

    /** Window close, minimize, maximize */
    class WindowButton {
    public:
        Rect2D       base;
        
        Vector2      focusedUp;
        Vector2      focusedDown;
        Vector2      defocused;
        Vector2      windowDefocused;

        void deserialize(const std::string& name, TextInput& b);
    };


    class Window {
    public:
        StretchRectHV    base;

        /** Distance from edge to border */
        Pad              borderPad;

        /** Thickness of border on each side (not counting rounded corners) */
        Pad              borderThickness;

        /** Distance from border exterior to client area */
        Pad              netClientPad;

        Vector2          focused;
        Vector2          defocused;

        /** Defaults */
        TextStyle        textStyle;
        TextStyle        defocusedTextStyle;

        void deserialize(const std::string& name, const std::string& path, TextInput& b);

        /** Pass the bounds outside the border; the borderPad will automatically be added. */
        void render(RenderDevice* rd, const Rect2D& bounds, bool focused) const;
    };


    class HSlider {
    public:
        class Bar {
        public:
            StretchRectH     base;
            Vector2          enabled;
            Vector2          disabled;
            void deserialize(const std::string& name, TextInput& b);
        };

        class Thumb {
        public:
            Rect2D           base;
            class Focus {
            public:
                Vector2      focused;
                Vector2      defocused;
                void deserialize(const std::string& name, TextInput& b);
            };

            Focus            enabled;
            Vector2          disabled;

            void deserialize(const std::string& name, TextInput& b);
        };

        Bar                  bar;
        Thumb                thumb;

        /** Defaults */
        TextStyle            textStyle;
        TextStyle        disabledTextStyle;
        void deserialize(const std::string& name, const std::string& path, TextInput& b);

        /** Renders along the center of the vertical bounds and stretches to fill horizontally.*/
        void render(RenderDevice* rd, const Rect2D& bounds, float thumbPos, bool enabled, bool focused) const;

        /** Given the bounds on the graphical part of the slider, returns the bounds on the track.*/
        Rect2D trackBounds(const Rect2D& sliderBounds) const;
        Rect2D thumbBounds(const Rect2D& sliderBounds, float pos) const;
    };

    class Pane {
    public:
        /** Defaults */
        TextStyle        textStyle;
        TextStyle        disabledTextStyle;
        StretchRectHV    frame;
        Pad              clientPad;
        void deserialize(const std::string& name, const std::string& path, TextInput& b);
    };

    Checkable         m_checkBox;
    Checkable         m_radioButton;
    Button            m_button[BUTTON_STYLE_COUNT];

    Window            m_window[WINDOW_STYLE_COUNT];
    HSlider           m_hSlider;
    Pane              m_pane[PANE_STYLE_COUNT];
    WindowButton      m_closeButton;
    TextBox           m_textBox;
    DropDownList      m_dropDownList;

    /** If true, the close button is on the left.  If false, it is on the right */
    bool              m_osxWindowButtons;

    /** Defaults */
    TextStyle         m_textStyle;
    TextStyle         m_disabledTextStyle;

    TextureRef        texture;

    /**
       The transformation matrix used to scale texture coordinates
       to the resolution of the texture.
       
       OpenGL-style matrix
     */
    float             guiTextureMatrix[16];

    float             fontTextureMatrix[16];
    
    /** The font and gui share a single texture unit (this is faster than
        using two units and selectively disabling).
     */
    int               fontTextureID;

    /** True between beginRendering and endRendering */
    bool              inRendering;

    RenderDevice*     rd;

    /** Used by push/popClientRect */
    Array<Rect2D>     scissorStack;

    /** Used by push/popClientRect */
    Array<CoordinateFrame> coordinateFrameStack;

    static StretchMode readStretchMode(TextInput& t);

    static void drawRect(const Rect2D& vertex, const Rect2D& texCoord, RenderDevice* rd);
    
    void drawCheckable(const Checkable& control, const Rect2D& bounds, bool enabled, bool focused,
                       bool selected, const GuiCaption& text) const;

    void drawWindow(const Window& window, const Rect2D& bounds, bool focused, 
                    bool close, bool closeDown, bool closeIsFocused, const GuiCaption& text) const;

    static Rect2D readRect2D(const std::string& name, TextInput& b);

    static Vector2 readVector2(const std::string& name, TextInput& b);

    /** Only used for testing the formatting of text during skin creation */
    GuiSkin();

    GuiSkin(const std::string& filename,
        const GFont::Ref&   fallbackFont,
        float               fallbackSize, 
        const Color4&       fallbackColor, 
        const Color4&       fallbackOutlineColor);
    
    void deserialize(const std::string& path, BinaryInput& b);

    /** Called from deserialize(BinaryInput) */
    void deserialize(const std::string& path, TextInput& t);

    /** Call before GFont::send2DQuads */
    void beginText() const;   
 
    /** Call after GFont::send2DQuads */
    void endText() const;    
    
    Rect2D horizontalSliderToSliderBounds(const Rect2D& bounds) const;
    Rect2D closeButtonBounds(const Window& window, const Rect2D& bounds) const;

public:

    /** 
     May return a cached copy.  

     @param filename Name of the .skn file.
    */
    static GuiSkinRef fromFile(
        const std::string&  filename, 
        const GFont::Ref&   fallbackFont,
        float               fallbackSize = 11, 
        const Color4&       fallbackColor = Color3::black(), 
        const Color4&       fallbackOutlineColor = Color4::clear());

    /** Call before all other render methods. 
        @param offset Offset all positions by this amount (convenient for rendering 
        relative to a containing control or window.)
     */
    void beginRendering(class RenderDevice* rd);

    /** 
      Offsets all subsequent rendering by r.x0y0() and sets the clipping region to r.
      Call only between beginRendering and endRendering.
      */
    void pushClientRect(const Rect2D& r);
    void popClientRect();

    /** Call after all other render methods. */
    void endRendering();

    /** Only call between beginRendering and endRendering */
    void renderCheckBox(const Rect2D& bounds, bool enabled, bool focused, 
                        bool checked, const GuiCaption& text) const;

    /** Render a single-line text box. Only call between beginRendering and endRendering.
        Automatically shifts text so that a cursor at character index given by 
        cursorPosition is visible on screen.
     */
    void renderTextBox(
        const Rect2D&           bounds, 
        bool                    enabled,
        bool                    focused, 
        const GuiCaption&       caption,
        const GuiCaption&       text,
        const GuiCaption&       cursor,
        int                     cursorPosition) const;

    void renderDropDownList
    (
     const Rect2D&        bounds,
     bool                 enabled,
     bool                 focused,
     bool                 menuOpen,
     const GuiCaption&    contentText,
     const GuiCaption&    text) const;

    /** Only call between beginRendering and endRendering */
    void renderRadioButton(const Rect2D& bounds, bool enabled, bool focused, 
                           bool checked, const GuiCaption& text) const;

    /** Only call between beginRendering and endRendering */
    void renderButton(const Rect2D& bounds, bool enabled, bool focused, 
                      bool pushed, const GuiCaption& text, ButtonStyle buttonStyle) const;

    /** Only call between beginRendering and endRendering.
        @param bounds Corresponds to the footprint of the window; dropshadows and glows may
         still render outside this area.*/
    void renderWindow(const Rect2D& bounds, bool focused, 
                      bool hasCloseButton, bool closeButtonIsDown, bool closeIsFocused,
                      const GuiCaption& text, WindowStyle frameStyle) const;

    /** Given the bounds of a full dropDownList and caption, returns
        the bounds around just the control itself. */
    Rect2D dropDownListToClickBounds(const Rect2D& bounds) const;
    Rect2D textBoxToClickBounds(const Rect2D& bounds) const;

    /** Given the bounds on a window's borders, returns the bounds of
     the area inside the window where controls will appear.*/
    Rect2D windowToClientBounds(const Rect2D& bounds, WindowStyle frameStyle) const;

    /** Given the area that controls should appear in for a window,
        returns the bounds that should be used to draw the window.
        Note that the window's shadow or other effects may exceed
        these bounds when rendering.*/
    Rect2D clientToWindowBounds(const Rect2D& bounds, WindowStyle frameStyle) const;
    Rect2D windowToTitleBounds(const Rect2D& bounds, WindowStyle frameStyle) const;
    Rect2D windowToCloseButtonBounds(const Rect2D& bounds, WindowStyle frameStyle) const;

    /** Returns the position of the thumb button, which is needed for processing
        UI events for the slider. */
    Rect2D horizontalSliderToThumbBounds(const Rect2D& bounds, float pos) const;
    Rect2D horizontalSliderToTrackBounds(const Rect2D& bounds) const;

    Rect2D paneToClientBounds(const Rect2D& bounds, PaneStyle paneStyle) const;
    Rect2D clientToPaneBounds(const Rect2D& bounds, PaneStyle paneStyle) const;

    /** Only call between beginRendering and endRendering.
        Label is on the right, slider is aligned with the left edge
        @param pos 0 = left edge, 1 = right edge*/
    void renderHorizontalSlider(const Rect2D& bounds, float pos, bool enabled, bool focused, 
                                const GuiCaption& text) const;

    /** Only call between beginRendering and endRendering */
    void renderLabel(const Rect2D& bounds, const GuiCaption& text, 
                     GFont::XAlign xalign, GFont::YAlign yalign,
                     bool enabled) const;

    void renderPane(const Rect2D& bounds, PaneStyle paneStyle) const;
    
    /** 
        Create a .skn file from source files.  Used as a preprocess
        when creating new skins.  You probably don't need to call this
        because most people will want to use pre-existing skins
        (making a skin is a time-consuming artistic process!)

        @param sourceDir Must end in a slash. 
        @param whiteName Image of controls on a white background, must be in sourceDir
        @param blackName Image of controls on a black background, must be in sourceDir
        @param coordsFile Text file describing the location of controls
        @param destFile Output .skn file, not relative to sourceDir
    */
    static void makeSkinFromSourceFiles(
              const std::string& sourceDir,
              const std::string& whiteName,
              const std::string& blackName,
              const std::string& coordsFile,
              const std::string& destFile);

}; // GuiSkin

} // namespace G3D

#endif // G3D_GUISKIN_H
