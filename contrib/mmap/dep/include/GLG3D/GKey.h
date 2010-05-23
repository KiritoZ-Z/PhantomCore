/**
  @file GLG3D/GKey.h

  @maintainer Morgan McGuire, morgan@graphics3d.com
  @created 2006-10-20
  @edited  2007-01-27
*/
#ifndef GLG3D_GKEY_H
#define GLG3D_GKEY_H

#include "G3D/platform.h"
#include "G3D/enumclass.h"
#include "G3D/g3dmath.h"
#include <string>

namespace G3D {

// winnt.h creates a DELETE macro    
#ifdef DELETE
#undef DELETE
#endif

/** 
   A mapping of every physical key on the keyboard and other input devices
   in a (mostly) platform independent manner.

   International keyboards use the range 0xA1 - 0xFF
   as international virtual keycodes.

   The scan codes have been chosen to map to ASCII for most printable characters.

   Uses the "Intelligent Enum" design pattern 
   http://www.codeguru.com/cpp/cpp/cpp_mfc/article.php/c4001/
 */ 
class GKey {
public:
    /** Don't use this enum; use GKey instances instead. */
    enum Value {
	    UNKNOWN	= 0,
	    FIRST		= 0,
	    BACKSPACE	= 8,
	    TAB		= 9,
	    CLEAR		= 12,
	    RETURN		= 13,
	    PAUSE		= 19,
	    ESCAPE		= 27,
	    SPACE		= 32,
	    EXCLAIM		= 33,
	    QUOTEDBL	= 34,
	    HASH		= 35,
	    DOLLAR		= 36,
	    AMPERSAND	= 38,
	    QUOTE		= 39,
	    LEFTPAREN	= 40,
	    RIGHTPAREN	= 41,
	    ASTERISK	= 42,
	    PLUS		= 43,
	    COMMA		= 44,
	    MINUS		= 45,
	    PERIOD		= 46,
	    SLASH		= 47,
	    // 0 through 9 are '0' through '9'
	    COLON		= 58,
	    SEMICOLON	= 59,
	    LESS		= 60,
	    EQUALS		= 61,
	    GREATER		= 62,
	    QUESTION	= 63,
	    AT			= 64,
	    /* 
	       Skip uppercase letters
	     */
	    LEFTBRACKET	= 91,
	    BACKSLASH	= 92,
	    RIGHTBRACKET= 93,
	    CARET	= 94,
	    UNDERSCORE	= 95,
	    BACKQUOTE	= 96,
            // a through z are 'a' through 'z'

	    DELETE      = 127,
	    /* End of ASCII mapped keysyms */

	    /* International keyboard syms */
	    WORLD_0     = 160,		/* 0xA0 */
	    WORLD_1	= 161,
	    WORLD_2     = 162,
	    WORLD_3		= 163,
	    WORLD_4		= 164,
	    WORLD_5		= 165,
	    WORLD_6		= 166,
	    WORLD_7		= 167,
	    WORLD_8		= 168,
	    WORLD_9		= 169,
	    WORLD_10	= 170,
	    WORLD_11	= 171,
	    WORLD_12	= 172,
	    WORLD_13	= 173,
	    WORLD_14	= 174,
	    WORLD_15	= 175,
	    WORLD_16	= 176,
	    WORLD_17	= 177,
	    WORLD_18	= 178,
	    WORLD_19	= 179,
	    WORLD_20	= 180,
	    WORLD_21	= 181,
	    WORLD_22	= 182,
	    WORLD_23	= 183,
	    WORLD_24	= 184,
	    WORLD_25	= 185,
	    WORLD_26	= 186,
	    WORLD_27	= 187,
	    WORLD_28	= 188,
	    WORLD_29	= 189,
	    WORLD_30	= 190,
	    WORLD_31	= 191,
	    WORLD_32	= 192,
	    WORLD_33	= 193,
	    WORLD_34	= 194,
	    WORLD_35	= 195,
	    WORLD_36	= 196,
	    WORLD_37	= 197,
	    WORLD_38	= 198,
	    WORLD_39	= 199,
	    WORLD_40	= 200,
	    WORLD_41	= 201,
	    WORLD_42	= 202,
	    WORLD_43	= 203,
	    WORLD_44	= 204,
	    WORLD_45	= 205,
	    WORLD_46	= 206,
	    WORLD_47	= 207,
	    WORLD_48	= 208,
	    WORLD_49	= 209,
	    WORLD_50	= 210,
	    WORLD_51	= 211,
	    WORLD_52	= 212,
	    WORLD_53	= 213,
	    WORLD_54	= 214,
	    WORLD_55	= 215,
	    WORLD_56	= 216,
	    WORLD_57	= 217,
	    WORLD_58	= 218,
	    WORLD_59	= 219,
	    WORLD_60	= 220,
	    WORLD_61	= 221,
	    WORLD_62	= 222,
	    WORLD_63	= 223,
	    WORLD_64	= 224,
	    WORLD_65	= 225,
	    WORLD_66	= 226,
	    WORLD_67	= 227,
	    WORLD_68	= 228,
	    WORLD_69	= 229,
	    WORLD_70	= 230,
	    WORLD_71	= 231,
	    WORLD_72	= 232,
	    WORLD_73	= 233,
	    WORLD_74	= 234,
	    WORLD_75	= 235,
	    WORLD_76	= 236,
	    WORLD_77	= 237,
	    WORLD_78	= 238,
	    WORLD_79	= 239,
	    WORLD_80	= 240,
	    WORLD_81	= 241,
	    WORLD_82	= 242,
	    WORLD_83	= 243,
	    WORLD_84	= 244,
	    WORLD_85	= 245,
	    WORLD_86	= 246,
	    WORLD_87	= 247,
	    WORLD_88	= 248,
	    WORLD_89	= 249,
	    WORLD_90	= 250,
	    WORLD_91	= 251,
	    WORLD_92	= 252,
	    WORLD_93	= 253,
	    WORLD_94	= 254,
	    WORLD_95	= 255,		/* 0xFF */

	    /* Numeric keypad */
	    KP0		= 256,
	    KP1		= 257,
	    KP2		= 258,
	    KP3		= 259,
	    KP4		= 260,
	    KP5		= 261,
	    KP6		= 262,
	    KP7		= 263,
	    KP8		= 264,
	    KP9		= 265,
	    KP_PERIOD	= 266,
	    KP_DIVIDE	= 267,
	    KP_MULTIPLY	= 268,
	    KP_MINUS	= 269,
	    KP_PLUS		= 270,
	    KP_ENTER	= 271,
	    KP_EQUALS	= 272,

	    /* Arrows + Home/End pad */
	    UP			= 273,
	    DOWN		= 274,
	    RIGHT		= 275,
	    LEFT		= 276,
	    INSERT		= 277,
	    HOME		= 278,
	    END		= 279,
	    PAGEUP		= 280,
	    PAGEDOWN		= 281,

	    /* Function keys */
	    F1			= 282,
	    F2			= 283,
	    F3			= 284,
	    F4			= 285,
	    F5			= 286,
	    F6			= 287,
	    F7			= 288,
	    F8			= 289,
	    F9			= 290,
	    F10		= 291,
	    F11		= 292,
	    F12		= 293,
	    F13		= 294,
	    F14		= 295,
	    F15		= 296,

	    /* Key state modifier keys */
	    NUMLOCK		= 300,
	    CAPSLOCK		= 301,
	    SCROLLOCK		= 302,
	    RSHIFT		= 303,
	    LSHIFT		= 304,
	    RCTRL		= 305,
	    LCTRL		= 306,
	    RALT		= 307,
	    LALT		= 308,
	    RMETA		= 309,
	    LMETA		= 310,
	    LSUPER		= 311,		/* Left "Windows" key */
	    RSUPER		= 312,		/* Right "Windows" key */
	    MODE		= 313,		/* "Alt Gr" key */
	    COMPOSE		= 314,		/* Multi-key compose key */

	    /* Miscellaneous function keys */
	    HELP		= 315,
	    PRINT		= 316,
	    SYSREQ		= 317,
	    BREAK		= 318,
	    MENU		= 319,
	    POWER		= 320,		/* Power Macintosh power key */
	    EURO		= 321,		/* Some european keyboards */
	    UNDO		= 322,		/* Atari keyboard has Undo */

            // The order of the mouse buttons is intentionally chosen to match SDL's button codes
            // and cannot be changed.
            LEFT_MOUSE        = 324,
            MIDDLE_MOUSE,
            RIGHT_MOUSE,
            MOUSE_WHEEL_UP,
            MOUSE_WHEEL_DOWN,

            /* Add any other keys here */
            
            LAST};

private:
    
    Value value;

public:

    G3D_DECLARE_ENUM_CLASS_METHODS(GKey);


    /** Creates a key code from a standardized description. */
    static GKey fromString(const std::string& s);

    /** Converts to a standarized description; convenient for 
        displaying key codes in a configuration window. */
    std::string toString() const;
};
} // namespace G3D

G3D_DECLARE_ENUM_CLASS_HASHCODE(G3D::GKey);

#endif

