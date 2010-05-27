/**
 @file GuiCaption.cpp
 @author Morgan McGuire, morgan@cs.williams.edu
 */

#include "G3D/platform.h"
#include "GLG3D/GuiCaption.h"

namespace G3D {

GuiCaption::GuiCaption
(const std::string& text, 
 const GFontRef& font, 
 float size, 
 const Color4& color,
 const Color4& outlineColor) : m_text(text), m_font(font), m_size(size), m_color(color), m_outlineColor(outlineColor) {
    
}

GuiCaption::GuiCaption
(const char* text) : m_text(text), m_font(NULL), m_size(-1), m_color(-1,-1,-1,-1), m_outlineColor(-1,-1,-1,-1) {
    
}

/** Provides the value of default values; called by Gui to overwrite the illegal values.*/
void GuiCaption::setDefault(const GFontRef& dfont, float dsize, const Color4& dcolor, const Color4& doutline) {
    if (m_font.isNull()) {
        m_font = dfont;
    }

    if (m_size < 0) {
        m_size = dsize;
    }

    if (m_color.a < 0) {
        m_color = dcolor;
    }
    
    if (m_outlineColor.a < 0) {
        m_outlineColor = doutline;
    }
}


GuiCaption GuiCaption::Symbol::record() {
    return GuiCaption("=", GFont::fromFile(System::findDataFile("icon.fnt")), 16, Color3::red() * 0.5f);
}

GuiCaption GuiCaption::Symbol::play() {
    return GuiCaption("4", GFont::fromFile(System::findDataFile("icon.fnt")), 16);
}

GuiCaption GuiCaption::Symbol::pause() {
    return GuiCaption("3", GFont::fromFile(System::findDataFile("icon.fnt")), 16);
}

GuiCaption GuiCaption::Symbol::stop() {
    return GuiCaption("<", GFont::fromFile(System::findDataFile("icon.fnt")), 16);
}

GuiCaption GuiCaption::Symbol::forward() {
    return GuiCaption("0", GFont::fromFile(System::findDataFile("icon.fnt")), 16);
}

GuiCaption GuiCaption::Symbol::reverse() {
    return GuiCaption("/", GFont::fromFile(System::findDataFile("icon.fnt")), 16);
}

GuiCaption GuiCaption::Symbol::previous() {
    return GuiCaption("1", GFont::fromFile(System::findDataFile("icon.fnt")), 16);
}

GuiCaption GuiCaption::Symbol::next() {
    return GuiCaption("2", GFont::fromFile(System::findDataFile("icon.fnt")), 16);
}


GuiCaption GuiCaption::Symbol::greek(int code) {
    return GuiCaption(std::string() + (char)code, GFont::fromFile(System::findDataFile("symbol.fnt")));
}

GuiCaption GuiCaption::Symbol::leq() {
    return greek(0xA3);
}

GuiCaption GuiCaption::Symbol::geq() {
    return greek(0xB3);
}

GuiCaption GuiCaption::Symbol::pm() {
    return greek(0xB1);
}

GuiCaption GuiCaption::Symbol::heartSuit() {
    return GuiCaption(std::string() + char(0xA9), GFont::fromFile(System::findDataFile("symbol.fnt")), -1, Color3::red() * 0.6f);
}

GuiCaption GuiCaption::Symbol::diamondSuit() {
    return GuiCaption(std::string() + char(0xA8), GFont::fromFile(System::findDataFile("symbol.fnt")), -1, Color3::red() * 0.6f);
}

GuiCaption GuiCaption::Symbol::clubSuit() {
    return greek(0xA7);
}

GuiCaption GuiCaption::Symbol::spadeSuit() {
    return greek(0xAA);
}

GuiCaption GuiCaption::Symbol::approx() {
    return greek(0xBB);
}

GuiCaption GuiCaption::Symbol::partial() {
    return greek(0xB6);
}

}
