/**
  @file GPUProgram.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-04-13
  @edited  2006-10-24
*/

#include "GLG3D/GPUProgram.h"
#include "GLG3D/getOpenGLState.h"
#include "G3D/debugAssert.h"
#include "G3D/debug.h"
#include "G3D/prompt.h"
#include "G3D/platform.h"
#include "G3D/fileutils.h"
#include "G3D/Log.h"
#include "GLG3D/RenderDevice.h"

#if defined(_MSC_VER) && !defined(_NDEBUG)
// Disable unreachable code warning in debug mode
#   pragma warning( push )
#   pragma warning( disable : 4702 )
#endif

namespace G3D {

void GPUProgram::ArgList::set(const std::string& var, const Texture::Ref& val) {

    alwaysAssertM(! argTable.containsKey(var), std::string("Cannot set variable \"") + var + "\" more than once");

    Arg arg;

    switch (val->dimension()) {
    case Texture::DIM_2D_NPOT:
    case Texture::DIM_2D:
	arg.type = SAMPLER2D;
	break;

    case Texture::DIM_3D:
	arg.type = SAMPLER3D;
	break;

    case Texture::DIM_2D_RECT:
	arg.type = SAMPLERRECT;
	break;

    case Texture::DIM_CUBE_MAP_NPOT:
    case Texture::DIM_CUBE_MAP:
	arg.type = SAMPLERCUBE;
	break;
    }

    arg.texture = val;
    argTable.set(var, arg);

}


void GPUProgram::ArgList::set(const std::string& var, const CoordinateFrame& val) {
    set(var, Matrix4(val));
}


void GPUProgram::ArgList::set(const std::string& var, const Matrix4& val) {
    alwaysAssertM(! argTable.containsKey(var), std::string("Cannot set variable \"") + var + "\" more than once");

    Arg arg;
    arg.type = FLOAT4X4;
    for (int r = 0; r < 4; ++r) {
        arg.vector[r] = val.getRow(r);
    }

    argTable.set(var, arg);
}


void GPUProgram::ArgList::set(const std::string& var, const Vector4& val) {
    alwaysAssertM(! argTable.containsKey(var), std::string("Cannot set variable \"") + var + "\" more than once");

    Arg arg;
    arg.type = FLOAT4;
    arg.vector[0] = val;
    argTable.set(var, arg);
}


void GPUProgram::ArgList::set(const std::string& var, const Vector3& val) {
    alwaysAssertM(! argTable.containsKey(var), std::string("Cannot set variable \"") + var + "\" more than once");

    Arg arg;
    arg.type = FLOAT3;
    arg.vector[0] = Vector4(val, 0);
    argTable.set(var, arg);

}


void GPUProgram::ArgList::set(const std::string& var, const Vector2& val) {
    alwaysAssertM(! argTable.containsKey(var), std::string("Cannot set variable \"") + var + "\" more than once");

    Arg arg;
    arg.type = FLOAT2;
    arg.vector[0] = Vector4(val, 0, 0);
    argTable.set(var, arg);
}


void GPUProgram::ArgList::set(const std::string& var, float          val) {
    alwaysAssertM(! argTable.containsKey(var), std::string("Cannot set variable \"") + var + "\" more than once");

    Arg arg;
    arg.type = FLOAT1;
    arg.vector[0] = Vector4(val, 0, 0, 0);
    argTable.set(var, arg);
}


void GPUProgram::ArgList::clear() {
    argTable.clear();
}

///////////////////////////////////////////////////


GPUProgram::GPUProgram(
    const std::string&  _name,
    const std::string&  _filename) : name(_name), filename(_filename) {
}


GLenum GPUProgram::getUnitFromCode(const std::string& code, Extension& extension) {

    if (beginsWith(code, "!!ARBvp1.0")) {

		extension = ARB;

        return GL_VERTEX_PROGRAM_ARB;

    } if (beginsWith(code, "!!ARBfp1.0")) {

		extension = ARB;

        return GL_FRAGMENT_PROGRAM_ARB;

    } if (beginsWith(code, "!!VP2.0")) {

		extension = NVIDIA;

        return GL_VERTEX_PROGRAM_NV;


    } if (beginsWith(code, "!!VP1.0")) {

		extension = NVIDIA;

        return GL_VERTEX_PROGRAM_NV;


    } if (beginsWith(code, "!!FP1.0")) {

		extension = NVIDIA;

        return GL_FRAGMENT_PROGRAM_NV;


    } if (beginsWith(code, "!!FP1.1")) {

		extension = NVIDIA;

        return GL_FRAGMENT_PROGRAM_NV;


    } if (beginsWith(code, "!!FP2.0")) {

		extension = NVIDIA;

        return GL_FRAGMENT_PROGRAM_NV;


    } else {
        return GL_NONE;
    }
}


void GPUProgram::genPrograms(int num, unsigned int* id) const {

    switch (extension) {
    case NVIDIA:
        alwaysAssertM(glGenProgramsNV != NULL, "Requires an NVIDIA card with the GL_NV_vertex_program extension");
        glGenProgramsNV(num, id);
        break;

    case ARB:
        glGenProgramsARB(num, id);
        break;
    }
}


void GPUProgram::deletePrograms(int num, unsigned int* id) const {

    switch (extension) {
    case NVIDIA:
        glDeleteProgramsNV(num, id);
        break;

    case ARB:
        glDeleteProgramsARB(num, id);
        break;
    }
}


void GPUProgram::bindProgram(int unit, unsigned int glProgram) const {
    switch (extension) {
    case NVIDIA:
        bindingTable.nvBind(unit);
        glBindProgramNV(unit, glProgram);
        break;

    case ARB:
        bindingTable.arbBind(unit);
        glBindProgramARB(unit, glProgram);
        break;
    }
}


void GPUProgram::loadProgram(const std::string& code) const {
    switch (extension) {
    case NVIDIA:
        glLoadProgramNV(unit, glProgram, code.size(), (const unsigned char*)code.c_str());
        break;

    case ARB:
        glProgramStringARB(unit, GL_PROGRAM_FORMAT_ASCII_ARB, code.size(), code.c_str());
        break;
    }
}


void GPUProgram::loadConstant(int slot, const Vector4& value) const {
    switch (extension) {
    case NVIDIA:
        glProgramParameter4fvNV(unit, slot, value);
        break;

    case ARB:
        glProgramLocalParameter4fvARB(unit, slot, value);
        break;
    }
}


void GPUProgram::getProgramError(int& pos, const unsigned char*& msg) const {

    switch (extension) {
    case NVIDIA:
        pos = glGetInteger(GL_PROGRAM_ERROR_POSITION_NV);
        msg = glGetString(GL_PROGRAM_ERROR_STRING_NV);
        break;

    case ARB:
        pos = glGetInteger(GL_PROGRAM_ERROR_POSITION_ARB);
        msg = glGetString(GL_PROGRAM_ERROR_STRING_ARB);
        break;
    }
}


void GPUProgram::reload(const std::string& _code) {
    std::string code = _code;

    // If a syntax error occurs while loading the shader we want to break.  
    // However, it makes no sense to break in this loading code when the
    // error is really in the shader code.  To hack this under MSVC we print
    // out the error as if it were a MSVC error so double clicking will take
    // us there, then break in this code.  To reload the shader we jump back
    // to the top of the loading routine and try again.
     
    bool reloadFromFile = (code == "");

    bool ignore = false;

LOADSHADER:

    if (reloadFromFile) {
        alwaysAssertM(fileExists(filename), std::string("Cannot locate file \"") + filename + "\" to reload it.");
        code = readWholeFile(filename);
    }

    unit = getUnitFromCode(code, extension);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(unit);

    genPrograms(1, &glProgram);
    bindProgram(unit, glProgram);
    // Clear the error flag.
    glGetError();

    loadProgram(code);

    // Check for load errors
    if ((glGetError() == GL_INVALID_OPERATION) && (! ignore)) {

        int                  pos = 0;
        const unsigned char* msg = NULL;
        getProgramError(pos, msg);

        deletePrograms(1, &glProgram);

        int line = 1;
        int col  = 1;

        // Find the line and column position.
        int x = 0;
        for (x = 0, col = 1; x < pos; ++x, ++col) {
            if (code[x] == '\n') {
                ++line;
                col = 1;
            }
        }

        if (col > 1) {
            --col;
        }

        // Count forward to the end of the line
        int endCol = col;
        while ((x < (int)code.size()) && (code[x] != '\n') && (code[x] != '\r')) {
            ++x;
            ++endCol;
        }

        // Extract the line
        std::string codeLine = code.substr(pos - col + 1, endCol - col);

        // Show the line
        std::string text = format("%s (%d:%d) : %s%s%s", filename.c_str(), line, col, msg, NEWLINE, NEWLINE);
        text += codeLine + NEWLINE;
        for (int i = 0; i < col - 1; ++i) {
            text += " ";
        }

        text += "^";

        #ifdef G3D_WIN32
        {
            // Print the error message in MSVC format
            std::string fullFilename = resolveFilename(filename);
            debugPrintf("%s%s(%d) : GPU Program Error : %s%s%s",
                   NEWLINE, fullFilename.c_str(), line, msg, NEWLINE, NEWLINE);
        }
        #endif

        #ifndef _DEBUG
            Log::common()->print("\n******************************\n");
            Log::common()->print(text);
            exit(-1);
        #endif

        const char* choice[] = {"Debug", "Ignore", "Ignore All", "Exit"};

        switch (prompt("Error Loading Program", text.c_str(), choice, 4, true)) {
        case 0:
            // Debug
            {
                ////////////////////////////////////////////////////////////////////////////
                //                                                                        //
                //                              PUSH  F4                                  //
                //                                                                        // 
                //   If your program breaks on this line in debug mode under Windows,     //
                //   go to the MSVC Debug window and click on the error message (or       //
                //   just press F4 be taken to the error line in your shader.             //
                //                                                                        //
                //   When you change it and press continue, G3D will try to reload your   //
                //   shader (if it came from a file).                                     //
                //                                                                        //
                ////////////////////////////////////////////////////////////////////////////
                debugBreak();
                reloadFromFile = true;
                goto LOADSHADER;
                break;
            }

        case 1:
            // Ignore
            break;

        case 2:
            // Ignore all
            ignore = true;
            break;

        case 3:
            // Exit
            exit(-1);
        }
    }
    bindingTable.parse(code);

    glPopAttrib();
}


GPUProgram::~GPUProgram() {
    deletePrograms(1, &glProgram);
    glProgram = 0;
}


GLuint GPUProgram::getOpenGLID() const {
    return glProgram;
}


void GPUProgram::bind() {
    glEnable(unit);
    bindProgram(unit, glProgram);
}


void GPUProgram::disable() {
    glDisable(unit);
}


void GPUProgram::setArgs(RenderDevice* renderDevice, const ArgList& args) {
    int numVariables = 0;

    // Iterate through formal bindings
    for (int b = 0; b < bindingTable.bindingArray.size(); ++b) {
        const BindingTable::Binding& binding = bindingTable.bindingArray[b];
        
        if (binding.source == VARIABLE) {
            ++numVariables;
            alwaysAssertM(args.argTable.containsKey(binding.name),
                std::string("No value provided for GPUProgram variable \"") + binding.name + "\"");

            const ArgList::Arg& arg = args.argTable[binding.name];

            // check the type
            alwaysAssertM(arg.type == binding.type,
                std::string("Variable \"") + binding.name +
                "\" was declared as type " + GPUProgram::toString(binding.type) + " and the values supplied" +
                " at runtime had type " + GPUProgram::toString(arg.type));

            if (binding.slot != BindingTable::Binding::UNASSIGNED) {
                switch (binding.type) {
                case SAMPLER1D:
			    case SAMPLER2D:
			    case SAMPLER3D:
			    case SAMPLERCUBE:
			    case SAMPLERRECT:
				    renderDevice->setTexture(binding.slot, arg.texture);
				    break;

                case FLOAT4:
                case FLOAT3:
                case FLOAT2:
                case FLOAT1:
                    loadConstant(binding.slot, arg.vector[0]);
                    break;

                case FLOAT2X2:
                    alwaysAssertM(false, "FLOAT2X2 not implemented.");
                    break;

                case FLOAT3X3:
                    for (int s = 0; s < 3; ++s) {
                        loadConstant(binding.slot + s, arg.vector[s]);
                    }
                    break;

                case FLOAT4X4:
                    for (int s = 0; s < 4; ++s) {
                        loadConstant(binding.slot + s, arg.vector[s]);
                    }
                    break;
                }
            }
        }
    }

    if (numVariables < static_cast<int>(args.argTable.size())) {
        // Some variables were unused.  Figure out which they were.
        Table<std::string, ArgList::Arg>::Iterator arg = args.argTable.begin();
        Table<std::string, ArgList::Arg>::Iterator end = args.argTable.end();

        while (arg != end) {
            // See if this arg was in the formal binding list

            bool foundArgument = false;
            
            for (int b = 0; b < bindingTable.bindingArray.size(); ++b) {
                if (bindingTable.bindingArray[b].name == arg->key) {
                    foundArgument = true;
                    break;
                }
            }

            debugAssertM(foundArgument, 
                std::string("Unused GPU program argument specified: \"") + arg->key + "\".");

            ++arg;
        }
    }

}


////////////////////////////////////////////////////////////////////////////////////////


bool GPUProgram::BindingTable::symbolMatch(const Token& t, const std::string& s) {
    return (t.type() == Token::SYMBOL) && (t.string() == s);
}


bool GPUProgram::BindingTable::consumeSymbol(TextInput& ti, const std::string& s) {
    Token t = ti.peek();
    if (symbolMatch(t, s)) {
        ti.readSymbol(s);
        return true;
    } else {
        return false;
    }
}


void GPUProgram::BindingTable::nvBind(GLenum target) const {
    for (int b = 0; b < bindingArray.size(); ++b) {
        const Binding& binding = bindingArray[b];

        if ((binding.source == CONSTANT) && (binding.type == FLOAT4)) {
            glProgramParameter4fvNV(target, binding.slot, binding.vector);
        }
    }
}



void GPUProgram::BindingTable::arbBind(GLenum target) const {
    for (int b = 0; b < bindingArray.size(); ++b) {
        const Binding& binding = bindingArray[b];

        if ((binding.source == CONSTANT) && (binding.type == FLOAT4)) {
            glProgramLocalParameter4fvARB(target, binding.slot, binding.vector);
        }
    }
}


bool GPUProgram::CgType(const std::string& s, GPUProgram::Type& t) {

	if (s == "float4x4") {
		t = FLOAT4X4;
	} else if (s == "float3x3") {
		t = FLOAT3X3;
	} else if (s == "float2x2") {
		t = FLOAT2X2;
	} else if (s == "float4") {
		t = FLOAT4;
	} else if (s == "float3") {
		t = FLOAT3;
	} else if (s == "float2") {
		t = FLOAT2;
	} else if (s == "float1") {
		t = FLOAT1;
	} else if (s == "float") {
		t = FLOAT1;
	} else if (s == "sampler1D") {
		t = SAMPLER1D;
	} else if (s == "sampler2D") {
		t = SAMPLER2D;
	} else if (s == "sampler3D") {
		t = SAMPLER3D;
	} else if (s == "samplerCUBE") {
		t = SAMPLERCUBE;
	} else if (s == "samplerRECT") {
		t = SAMPLERRECT;
	} else {
		return false;
	}

	return true;
}



std::string GPUProgram::toString(const Type& t) {

	switch (t) {
	case FLOAT4X4:
		return "float4x4";
	case FLOAT3X3:
		return "float3x3";
	case FLOAT2X2:
		return "float2x2";
	case FLOAT4:
		return "float4";
	case FLOAT3:
		return "float3";
	case FLOAT2:
		return "float2";
	case FLOAT1:
		return "float1";
	case SAMPLER1D:
		return "sampler1D";
	case SAMPLER2D:
		return "sampler2D";
	case SAMPLER3D:
		return "sampler3D";
	case SAMPLERCUBE:
		return "samplerCUBE";
	case SAMPLERRECT:
		return "samplerRECT";
	}

	return "** unknown type**";
}


void GPUProgram::BindingTable::parseVariable(TextInput& ti) {
	std::string name;


    // #var float4 osLight :  : c[4] : 1 : 1
    // #var float3 vin.v0 : $vin.POSITION : ATTR0 : 2 : 1

    Token t = ti.peek();
    if (t.type() != Token::SYMBOL) {
		goto abort;
	}
    // get the binding's type
    ti.readSymbol();
    Type type;


    if (! CgType(t.string(), type)) {
        alwaysAssertM(false, std::string("Unsupported type: \"") + t.string() + "\"");
    }
    
    t = ti.peek();
    if (t.type() != Token::SYMBOL) {
		goto abort;
	}
    // read the binding name
    name = ti.readSymbol();

    if (! consumeSymbol(ti, ":")) {
		goto abort;
	}

    // see if it is the vertex or a constant register
    t = ti.peek();

    if (t.type() != Token::SYMBOL) {
		goto abort;
	}

	// Sometimes there is an extra token between the colons
	if (t.string() != ":") {
		ti.readSymbol();
		t = ti.peek();
	}

    if (! consumeSymbol(ti, ":")) {
		goto abort;
	}

    // read the register number
	t = ti.peek();

	if (t.type() != Token::SYMBOL) {
		goto abort;
	}

    // Consume the symbol we just peeked
	ti.readSymbol();

	if (t.string() == "texunit") {
		// We're reading a texture unit

	} else if (t.string() == "c") {
		// We're reading a regular variable; parse the open bracket

		if (! consumeSymbol(ti, "[")) {
			goto abort;
		}

    } else if ((t.type() == Token::SYMBOL) && (t.string() == ":")) {        
        // Unused variable; must be present but is not bound

        Binding binding;
        binding.source = VARIABLE;
        binding.type = type;
        binding.name = name;
        binding.slot = Binding::UNASSIGNED;
        bindingArray.append(binding);
        return;

    } else {

		// Something unexpected happened.
		goto abort;

	}

    t = ti.peek();

    if (t.type() == Token::NUMBER) {
        int slot = iRound(ti.readNumber());
        Binding binding;
        binding.source = VARIABLE;
        binding.type = type;
        binding.name = name;
        binding.slot = slot;
        bindingArray.append(binding);
    }

abort:
	;// Jump here if anything unexpected is encountered during parsing
}


void GPUProgram::BindingTable::parseConstant(TextInput& ti) {
    if (consumeSymbol(ti, "c") && consumeSymbol(ti, "[")) {
        // constant
        Token t = ti.peek();

        if (t.type() == Token::NUMBER) {
            Binding binding;
            binding.source = CONSTANT;
            binding.type   = FLOAT4;
            binding.slot   = iRound(ti.readNumber());

            if (consumeSymbol(ti, "]") && consumeSymbol(ti, "=")) {
                for (int i = 0; i < 4; ++i) {
                    t = ti.peek();
                    if (t.type() == Token::NUMBER) {
                        binding.vector[i] = ti.readNumber(); 
                    }
                }
                bindingArray.append(binding);
            }
        }
    }
}


void GPUProgram::BindingTable::parse(const std::string& code) {
    bindingArray.resize(0);

    TextInput ti(TextInput::FROM_STRING, code);
    
    while (ti.hasMore()) {
        Token t = ti.read();
        // Scan for "#"
        while (! symbolMatch(t, "#") && ti.hasMore()) {
            t = ti.read();
        }
    
        if (ti.hasMore()) {
            // Read the comment line
            Token t = ti.peek();
            if (t.type() == Token::SYMBOL) {
                ti.readSymbol();
                if (t.string() == "var") {

                    parseVariable(ti);

                } else if (t.string() == "const") {
                    
                    parseConstant(ti);

                }
            }
        }
    }
}


}

#if defined(_MSC_VER) && !defined(_NDEBUG)
#   pragma warning( pop )
#endif
