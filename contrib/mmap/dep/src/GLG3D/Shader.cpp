/**
 @file Shader.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com
 
 @created 2004-04-24
 @edited  2007-03-16
 */

#include "G3D/fileutils.h"
#include "GLG3D/Shader.h"
#include "GLG3D/GLCaps.h"
#include "GLG3D/getOpenGLState.h"
#include "GLG3D/RenderDevice.h"

namespace G3D {

ShaderRef Shader::create(
    ShaderType          type0,
    const std::string&  value0,

    ShaderType          type1,
    const std::string&  value1,

    ShaderType          type2,
    const std::string&  value2) {

    // Sort out the arguments

    std::string pixelShader, geometryShader, vertexShader;
    bool pixelFromFile    = false;
    bool geometryFromFile = false;
    bool vertexFromFile   = false;

    Array<ShaderType> type;
    type.append(type0, type1, type2);

    Array<std::string> value;
    value.append(value0, value1, value2);

    for (int i = 0; i < type.size(); ++i) {
        switch (type[i]) {
        case VERTEX_STRING:
        case VERTEX_FILE:
            vertexFromFile = (type[i] == VERTEX_FILE);
            vertexShader = value[i];
            break;

        case GEOMETRY_STRING:
        case GEOMETRY_FILE:
            geometryFromFile = (type[i] == GEOMETRY_FILE);
            geometryShader = value[i];
            break;

        case PIXEL_STRING:
        case PIXEL_FILE:
            pixelFromFile = (type[i] == PIXEL_FILE);
            pixelShader = value[i];
            break;

        case SHADER_NONE:
            ;
        } // switch
    } // for

    // TODO

    return NULL;
}


bool Shader::hasArgument(const std::string& argname) const {
    const Set<std::string>& uniformNames = _vertexAndPixelShader->uniformNames;
    return uniformNames.contains(argname);
}


bool Shader::ok() const {
    return _vertexAndPixelShader->ok();
}


bool Shader::supportsPixelShaders() {
    return
        GLCaps::supports_GL_ARB_shader_objects() && 
        GLCaps::supports_GL_ARB_shading_language_100() &&
        GLCaps::supports_GL_ARB_fragment_shader();
}


bool Shader::supportsVertexShaders() {
    return
        GLCaps::supports_GL_ARB_shader_objects() && 
        GLCaps::supports_GL_ARB_shading_language_100() &&
        GLCaps::supports_GL_ARB_vertex_shader();
}

bool Shader::supportsGeometryShaders() {
    return
        GLCaps::supports_GL_ARB_shader_objects() && 
        GLCaps::supports_GL_ARB_shading_language_100() &&
        GLCaps::supports_GL_EXT_geometry_shader4();
}


void Shader::beforePrimitive(class RenderDevice* renderDevice) {
    if (_preserveState) {
        renderDevice->pushState();
    }

    if (_useUniforms == DEFINE_G3D_UNIFORMS) {
        const CoordinateFrame& o2w = renderDevice->getObjectToWorldMatrix();
        const CoordinateFrame& c2w = renderDevice->getCameraToWorldMatrix();

        const Set<std::string>& uniformNames = _vertexAndPixelShader->uniformNames;

        // Bind matrices
        if (uniformNames.contains("g3d_ObjectToWorldMatrix")) {
            args.set("g3d_ObjectToWorldMatrix", o2w);
        }

        if (uniformNames.contains("g3d_CameraToWorldMatrix")) {
            args.set("g3d_CameraToWorldMatrix", c2w);
        }

        if (uniformNames.contains("g3d_ObjectToWorldNormalMatrix")) {
            args.set("g3d_ObjectToWorldNormalMatrix", o2w.rotation);
        }

        if (uniformNames.contains("g3d_WorldToObjectNormalMatrix")) {
            args.set("g3d_WorldToObjectNormalMatrix", o2w.rotation.transpose());
        }

        if (uniformNames.contains("g3d_WorldToObjectMatrix")) {
            args.set("g3d_WorldToObjectMatrix", o2w.inverse());
        }

        if (uniformNames.contains("g3d_WorldToCameraMatrix")) {
            args.set("g3d_WorldToCameraMatrix", c2w.inverse());
        }

        if (uniformNames.contains("g3d_NumLights")) {
            if (glGetBoolean(GL_LIGHTING)) {
                // Search for the highest light number
                int i = 7;
                while (i >= 0 && ! glGetBoolean(GL_LIGHT0 + i)) {
                    --i;
                }
                args.set("g3d_NumLights", i + 1);
            } else {
                args.set("g3d_NumLights", 0);
            }
        }

        if (uniformNames.contains("g3d_ObjectLight0")) {
            Vector4 cL;
            // OpenGL lights are already in camera space, so take them
            // from camera to world to object space.
            glGetLightfv(GL_LIGHT0, GL_POSITION, cL);
            args.set("g3d_ObjectLight0", o2w.toObjectSpace(c2w.toWorldSpace(cL)));
        }

        if (uniformNames.contains("g3d_WorldLight0")) {
            Vector4 cL;
            // OpenGL lights are already in camera space, so take them
            // from camera to world to object space.
            glGetLightfv(GL_LIGHT0, GL_POSITION, cL);
            args.set("g3d_WorldLight0", c2w.toWorldSpace(cL));
        }

        if (uniformNames.contains("g3d_NumTextures")) {
            // Search for the highest texture number
            int i = 7;
            while (i >= 0 && ! glGetBoolean(GL_TEXTURE0_ARB + i)) {
                --i;
            }
            args.set("g3d_NumTextures", i + 1);
        }
    }

    renderDevice->setVertexAndPixelShader(_vertexAndPixelShader, args);
}


void Shader::afterPrimitive(class RenderDevice* renderDevice) {
    if (_preserveState) {
        renderDevice->popState();
    }
}


const std::string& Shader::messages() const {
    return _vertexAndPixelShader->messages();
}

///////////////////////////////////////////////////////////////////////////////

void VertexAndPixelShader::GPUShader::replaceG3DSize(std::string& code, std::string& uniformString) {

    // Ensure that uniforms are only declared once
    Set<std::string> newUniforms;

    std::string funcName = "g3d_sampler2DSize(";
    std::string prefix   = "     (";
    std::string postfix  = ".xy)";

    for (int count = 0; count < 2; ++count) {

        size_t last = 0;
        while (true) {
            size_t next = code.find(funcName, last);
            if (next == std::string::npos) {
                break;
            }

            size_t nameStart = next + funcName.length();
            size_t nameEnd   = code.find(')', nameStart);

            alwaysAssertM(nameEnd != std::string::npos,
                "Could not find the end of " + funcName + ") expression");

            std::string name = code.substr(nameStart, nameEnd - nameStart);

            std::string decoratedName = "g3d_sz2D_" + name;

            std::string replacement = prefix + decoratedName + postfix;
            for (size_t i = 0; i < replacement.size(); ++i) {
                code[i + next] = replacement[i];
            }

            newUniforms.insert(decoratedName);

            last = nameEnd;

        } // Iterating through all instances

        // On the second iteration, replace g3d_invSize.
        funcName = "g3d_sampler2DInvSize(";
        prefix   = "        (";
        postfix  = ".zw)";
    }

    // Add the unique uniforms to the header for the code.
    Set<std::string>::Iterator current = newUniforms.begin();
    const Set<std::string>::Iterator end = newUniforms.end();
    while (current != end) {
        uniformString += "uniform vec4 " + *current + "; "; 
        ++current;
    }
}


void VertexAndPixelShader::GPUShader::init
(
 const std::string&	name,
 const std::string&	code,
 bool			_fromFile,
 bool			debug,	
 GLenum			glType,
 const std::string&	type,
 UseG3DUniforms         uniforms) {
    
    std::string uniformString = 
        STR(
        uniform mat4 g3d_WorldToObjectMatrix;
        uniform mat4 g3d_ObjectToWorldMatrix;
        uniform mat3 g3d_WorldToObjectNormalMatrix;
        uniform mat3 g3d_ObjectToWorldNormalMatrix;
        uniform mat4 g3d_WorldToCameraMatrix;
        uniform mat4 g3d_CameraToWorldMatrix;
        uniform int  g3d_NumLights;
        uniform int  g3d_NumTextures;
        uniform vec4 g3d_ObjectLight0;
        uniform vec4 g3d_WorldLight0;
        );

    _name		= name;
    _shaderType		= type;
    _glShaderType	= glType;
    _ok			= true;
    fromFile		= _fromFile;
    _fixedFunction	= (name == "") && (code == "");
    
    if (! _fixedFunction) {
        
        switch (glType) {
        case GL_VERTEX_SHADER_ARB:
            if (! Shader::supportsVertexShaders()) {
                _ok = false;
                _messages = "This graphics card does not support vertex shaders.";
            }
            break;

        case GL_FRAGMENT_SHADER_ARB:
            if (! Shader::supportsPixelShaders()) {
                _ok = false;
                _messages = "This graphics card does not support pixel shaders.";
            }
            break;
        }

        if (fromFile) {
            if (fileExists(_name)) {
                _code = readWholeFile(_name);
            } else {
                _ok = false;
                _messages = format("Could not load shader file \"%s\".", 
                                   _name.c_str());
            }
        } else {
            _code = code;
        }
        
        bool shifted = false;

        // See if the program begins with a version pragma
        std::string versionLine;
        if (beginsWith(code, "#version ")) {
            // Strip off the version line, including the \n
            int pos = code.find("\n") + 1;
            versionLine = code.substr(0, pos);
            _code = code.substr(versionLine.length());
        }

        if ((uniforms == DEFINE_G3D_UNIFORMS) && (_code.size() > 0)) {
            // Replace g3d_size and g3d_invSize with corresponding magic names
            
            switch (GLCaps::enumVendor()) {
            case GLCaps::ATI:
                _code = "#define G3D_ATI\n" + _code;
                break;
            case GLCaps::NVIDIA:
                _code = "#define G3D_NVIDIA\n" + _code;
                break;
            case GLCaps::MESA:
                _code = "#define G3D_MESA\n" + _code;
                break;
            default:;
            }
#           ifdef G3D_OSX 
                _code = "#define G3D_OSX\n" + _code;
#           elif defined(G3D_WIN32)
                _code = "#define G3D_WIN32\n" + _code;
#           elif defined(G3D_LINUX)
                _code = "#define G3D_LINUX\n" + _code;
#           elif defined(G3D_FREEBSD)
                _code = "#define G3D_FREEBSD\n" + _code;
#           endif
                
            replaceG3DSize(_code, uniformString);
            
            std::string lineDirective = "";
            if (versionLine != "") {
                // Fix the line numbers since we inserted code at the top
                lineDirective = "#line 2\n";
            } else {
                // We only need to record that we've shifted line numbers
                // if the version is pre-1.20, since all later version support
                // the #line directive
                shifted = true;
            }

            _code = uniformString + "\n" + lineDirective + _code + "\n";

        } else {
            // Add a newline to the end to ensure that the shader is terminated
            // with one, which is required by GLSL.
            _code += "\n";
        }

        _code = versionLine + _code;

        if (_ok) {
            compile();
        }
        
        if (debug) {
            // Check for compilation errors
            if (! ok()) {
                if (shifted) {
                    debugPrintf("\n[Line numbers in the following shader errors are shifted by one.]\n");
                }
                debugPrintf("%s", messages().c_str());
                alwaysAssertM(ok(), messages());
            }
        }
    }
        
}


void VertexAndPixelShader::GPUShader::compile() {

    GLint compiled = GL_FALSE;
	_glShaderObject = glCreateShaderObjectARB(glShaderType());

    // Compile the shader
	GLint length = _code.length();
    const GLcharARB* codePtr = static_cast<const GLcharARB*>(_code.c_str());

	glShaderSourceARB(_glShaderObject, 1, &codePtr, &length);
	glCompileShaderARB(_glShaderObject);
	glGetObjectParameterivARB(_glShaderObject, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);

    // Read the result of compilation
	GLint	  maxLength;
	glGetObjectParameterivARB(_glShaderObject, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
    GLcharARB* pInfoLog = (GLcharARB *)malloc(maxLength * sizeof(GLcharARB));
	glGetInfoLogARB(_glShaderObject, maxLength, &length, pInfoLog);

	int c = 0;
    // Copy the result to the output string, prepending the filename
	while (pInfoLog[c] != '\0') {
		_messages += _name;
		while (pInfoLog[c] != '\n' && pInfoLog[c] != '\r' && pInfoLog[c] != '\0') {
			_messages += pInfoLog[c];
			++c;
		}

		if (pInfoLog[c] == '\r' && pInfoLog[c + 1] == '\n') {
			// Windows newline
			#ifdef G3D_WIN32
				_messages += "\r\n";
			#else
				_messages += "\n";
			#endif
			c += 2;
		} else if (pInfoLog[c] == '\r' && pInfoLog[c + 1] != '\n') {
			// Dangling \r; treat it as a newline
			_messages += "\r\n";
			++c;
		} else if (pInfoLog[c] == '\n') {
			// Newline
			#ifdef G3D_WIN32
				_messages += "\r\n";
			#else
				_messages += "\n";
			#endif
			++c;
		}
	}
	free(pInfoLog);

    _ok = (compiled == GL_TRUE);
}


VertexAndPixelShader::GPUShader::~GPUShader() {
	if (! _fixedFunction) {
		glDeleteObjectARB(_glShaderObject);
	}
}


////////////////////////////////////////////////////////////////////////////////////

VertexAndPixelShader::VertexAndPixelShader(
	const std::string&  vsCode,
	const std::string&  vsFilename,
	bool                vsFromFile,
	const std::string&  psCode,
	const std::string&  psFilename,
	bool                psFromFile,
    bool                debug,
    UseG3DUniforms      uniforms) :
        _ok(true) {

    if (! GLCaps::supports_GL_ARB_shader_objects()) {
        _messages = "This graphics card does not support GL_ARB_shader_objects.";
        _ok = false;
        return;
    }

	vertexShader.init(vsFilename, vsCode, vsFromFile, debug, GL_VERTEX_SHADER_ARB, "Vertex Shader", uniforms);
	pixelShader.init(psFilename, psCode, psFromFile, debug, GL_FRAGMENT_SHADER_ARB, "Pixel Shader", uniforms);
    
    _vertCompileMessages += vertexShader.messages();
    _messages += 
		std::string("Compiling ") + vertexShader.shaderType() + " " + vsFilename + NEWLINE +
		vertexShader.messages() + NEWLINE + NEWLINE;
    if (! vertexShader.ok()) {
        _ok = false;
    }

    if (! pixelShader.ok()) {
        _ok = false;
    }    
    _fragCompileMessages += pixelShader.messages();
    _messages += 
		std::string("Compiling ") + pixelShader.shaderType() + " " + psFilename + NEWLINE +
		pixelShader.messages() + NEWLINE + NEWLINE;

    lastTextureUnit = -1;

    if (_ok) {
        // Create GL object
        _glProgramObject = glCreateProgramObjectARB();

        // Attach vertex and pixel shaders
        if (! vertexShader.fixedFunction()) {
            glAttachObjectARB(_glProgramObject, vertexShader.glShaderObject());
        }

        if (! pixelShader.fixedFunction()) {
            glAttachObjectARB(_glProgramObject, pixelShader.glShaderObject());
        }

        // Link
        GLint linked = GL_FALSE;
        glLinkProgramARB(_glProgramObject);

        // Read back messages
	    glGetObjectParameterivARB(_glProgramObject, GL_OBJECT_LINK_STATUS_ARB, &linked);
        GLint maxLength = 0, length = 0;
	    glGetObjectParameterivARB(_glProgramObject, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
	    GLcharARB* pInfoLog = (GLcharARB *)malloc(maxLength * sizeof(GLcharARB));
	    glGetInfoLogARB(_glProgramObject, maxLength, &length, pInfoLog);

        _messages += std::string("Linking\n") + std::string(pInfoLog) + "\n";
        _linkMessages += std::string(pInfoLog);
	    free(pInfoLog);
        _ok = _ok && (linked == GL_TRUE);

        if (debug) {
            alwaysAssertM(_ok, _messages);
        }
    }

    if (_ok) {
        uniformNames.clear();
        computeUniformArray();
        // note that the extra uniforms are computed from the original code,
        // not from the code that has the g3d uniforms prepended.
        addUniformsFromCode(vsFromFile ? readWholeFile(vsFilename) : vsCode);
        addUniformsFromCode(psFromFile ? readWholeFile(psFilename) : psCode);

        // Add all uniforms to the name list
        for (int i = uniformArray.size() - 1; i >= 0; --i) {
            uniformNames.insert(uniformArray[i].name);
        }
    }
}


bool VertexAndPixelShader::isSamplerType(GLenum e) {
    return
       (e == GL_SAMPLER_1D_ARB) ||
       (e == GL_SAMPLER_2D_ARB) ||
       (e == GL_SAMPLER_2D_RECT_ARB) ||
       (e == GL_SAMPLER_3D_ARB) ||
       (e == GL_SAMPLER_CUBE_ARB) ||
       (e == GL_SAMPLER_1D_SHADOW_ARB) ||
       (e == GL_SAMPLER_2D_SHADOW_ARB) ||
       (e == GL_SAMPLER_2D_RECT_SHADOW_ARB);
}


/** Converts a type name to a GL enum */
static GLenum toGLType(const std::string& s) {
    if (s == "float") {
        return GL_FLOAT;
    } else if (s == "vec2") {
        return GL_FLOAT_VEC2_ARB;
    } else if (s == "vec3") {
        return GL_FLOAT_VEC3_ARB;
    } else if (s == "vec4") {
        return GL_FLOAT_VEC4_ARB;

    } else if (s == "int") {
        return GL_INT;

    } else if (s == "bool") {
        return GL_BOOL_ARB;

    } else if (s == "mat2") {
        return GL_FLOAT_MAT2_ARB;
    } else if (s == "mat3") {
        return GL_FLOAT_MAT3_ARB;
    } else if (s == "mat4") {
        return GL_FLOAT_MAT4_ARB;

    } else if (s == "sampler1D") {
        return GL_SAMPLER_1D_ARB;
    } else if (s == "sampler2D") {
        return GL_SAMPLER_2D_ARB;
    } else if (s == "sampler3D") {
        return GL_SAMPLER_3D_ARB;
    } else if (s == "samplerCube") {
        return GL_SAMPLER_CUBE_ARB;
    } else if (s == "sampler2DRect") {
        return GL_SAMPLER_2D_RECT_ARB;
    } else if (s == "sampler2DShadow") {
        return GL_SAMPLER_2D_SHADOW_ARB;
    } else if (s == "sampler2DRectShadow") {
        return GL_SAMPLER_2D_RECT_SHADOW_ARB;
    } else {
        debugAssertM(false, std::string("Unknown type in shader: ") + s);
        return 0;
    }
}


void VertexAndPixelShader::addUniformsFromCode(const std::string& code) {
    TextInput ti(TextInput::FROM_STRING, code);

    while (ti.hasMore()) {
        if ((ti.peek().type() == Token::SYMBOL) && (ti.peek().string() == "uniform")) {
            // Read the definition
            ti.readSymbol("uniform");

            // Maybe read "const"
            if ((ti.peek().type() == Token::SYMBOL) && (ti.peek().string() == "const")) {
                ti.readSymbol("const");
            }

            // Read the type
            GLenum type = toGLType(ti.readSymbol());

            // Read the name
            std::string name = ti.readSymbol();

            if ((ti.peek().type() == Token::SYMBOL) && (ti.peek().string() == "[")) {
                ti.readSymbol("[");
                ti.readNumber();
                ti.readSymbol("]");
            }

            // Read the semi-colon
            ti.readSymbol(";");

            // See if this variable is already declared.
            bool found = false;

            for (int i = 0; i < uniformArray.size(); ++i) {
                if (uniformArray[i].name == name) {
                    found = true;
                    break;
                }
            } 

            if (! found) {
                // Add the definition
                uniformArray.next();

                uniformArray.last().dummy = true;
                uniformArray.last().location = -1;
                uniformArray.last().name = name;
                uniformArray.last().size = 1;
                uniformArray.last().type = type;

                // Don't allocate texture units for unused variables
                uniformArray.last().textureUnit = -1;
            }

        } else {
            // Consume the token
            ti.read();
        }
    }
}


void VertexAndPixelShader::computeUniformArray() {
    uniformArray.clear();

    GLint maxLength;
    GLint uniformCount;

    // On ATI cards, we are required to call glUseProgramObjectARB before glGetUniformLocationARB.
    // First, store the old program.
    GLenum oldProgram = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
    glUseProgramObjectARB(glProgramObject());

    // Get the number of uniforms, and the length of the longest name.
    glGetObjectParameterivARB(glProgramObject(), GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB, &maxLength);
    glGetObjectParameterivARB(glProgramObject(), GL_OBJECT_ACTIVE_UNIFORMS_ARB, &uniformCount);

    uniformArray.resize(0);

    GLcharARB* name = (GLcharARB *) malloc(maxLength * sizeof(GLcharARB));
    
    // Loop over glGetActiveUniformARB and store the results away.
    for (int i = 0; i < uniformCount; ++i) {

        GLint size;
        GLenum type;

	    glGetActiveUniformARB(glProgramObject(), 
            i, maxLength, NULL, &size, &type, name);

        uniformArray.next().name = name;

        uniformArray.last().location = glGetUniformLocationARB(glProgramObject(), name);
        
        bool isGLBuiltIn = (uniformArray.last().location == -1) || 
            ((strlen(name) > 3) && beginsWith(std::string(name), "gl_"));

        uniformArray.last().dummy = isGLBuiltIn;

        if (! isGLBuiltIn) {
            uniformArray.last().size = size;
            uniformArray.last().type = type;

            if (isSamplerType(type)) {
                ++lastTextureUnit;
                uniformArray.last().textureUnit = lastTextureUnit;
            } else {
                uniformArray.last().textureUnit = -1;
            }
        }
    }

    free(name);

    glUseProgramObjectARB(oldProgram);
}


VertexAndPixelShaderRef VertexAndPixelShader::fromStrings(
	const std::string& vs,
    const std::string& ps,
    UseG3DUniforms u,
    bool debugErrors) {

    return new VertexAndPixelShader(vs, "", false, ps, "", false, debugErrors, u);
}


VertexAndPixelShaderRef VertexAndPixelShader::fromStrings(
    const std::string& vsName,
	const std::string& vs,
    const std::string& gsName,
    const std::string& gs,
    const std::string& psName,
    const std::string& ps,
    UseG3DUniforms u,
    bool debugErrors) {

    return new VertexAndPixelShader(vs, vsName, false, ps, psName, false, debugErrors, u);
}


VertexAndPixelShaderRef VertexAndPixelShader::fromFiles(
	const std::string& vsFilename,
    const std::string& psFilename,
    UseG3DUniforms u,
    bool debugErrors) {

	std::string vs;
	std::string ps;

	if (vsFilename != "") {
		vs = readWholeFile(vsFilename);
	}

	if (psFilename != "") {
		ps = readWholeFile(psFilename);
	}

    return new VertexAndPixelShader(vs, vsFilename, vsFilename != "", ps, psFilename, (psFilename != ""), debugErrors, u);
}


VertexAndPixelShader::~VertexAndPixelShader() {
    glDeleteObjectARB(_glProgramObject);
}


bool VertexAndPixelShader::fullySupported() {
    return
        GLCaps::supports_GL_ARB_shader_objects() && 
        GLCaps::supports_GL_ARB_shading_language_100() &&
        GLCaps::supports_GL_ARB_fragment_shader() &&
        GLCaps::supports_GL_ARB_vertex_shader();
}


GLenum VertexAndPixelShader::canonicalType(GLenum e) {

    switch (e) {
    case GL_INT:
    case GL_BOOL_ARB:
        return GL_FLOAT;

    case GL_INT_VEC2_ARB:
    case GL_BOOL_VEC2_ARB:
        return GL_FLOAT_VEC2_ARB;

    case GL_INT_VEC3_ARB:
    case GL_BOOL_VEC3_ARB:
        return GL_FLOAT_VEC3_ARB;

    case GL_INT_VEC4_ARB:
    case GL_BOOL_VEC4_ARB:
        return GL_FLOAT_VEC4_ARB;

    case GL_SAMPLER_2D_SHADOW_ARB:
    case GL_SAMPLER_2D_ARB:
        return GL_TEXTURE_2D;
        
    case GL_SAMPLER_CUBE_ARB:
        return GL_TEXTURE_CUBE_MAP_ARB;
        
    case GL_SAMPLER_2D_RECT_SHADOW_ARB:
    case GL_SAMPLER_2D_RECT_ARB:
        return GL_TEXTURE_RECTANGLE_EXT;

    default:
        // Return the input
        return e;    
    }
}


void VertexAndPixelShader::validateArgList(const ArgList& args) const {
    int numVariables = 0;

    // Iterate through formal bindings
    for (int u = 0; u < uniformArray.size(); ++u) {
        const UniformDeclaration& decl = uniformArray[u];

        // Only count variables that need to be set in code
        if (! decl.dummy) {
            ++numVariables;
        }

        bool declared = false;
        bool performTypeCheck = true;

        if (beginsWith(decl.name, "g3d_sz2D_")) {
            // This is the autogenerated dimensions of a texture.
            std::string textureName  = decl.name.substr(9, decl.name.length() - 9);
            declared = args.argTable.containsKey(textureName);
            performTypeCheck = false;

            if (! declared && ! decl.dummy) {
                throw ArgumentError(
                    format("No value provided for VertexAndPixelShader uniform variable %s.",
                        textureName.c_str()));
            }

        } else {

            declared = args.argTable.containsKey(decl.name);

            if (! declared && ! decl.dummy) {
                throw ArgumentError(
                    format("No value provided for VertexAndPixelShader uniform variable %s of type %s.",
                        decl.name.c_str(), GLenumToString(decl.type)));
            }
        }

        if (declared && performTypeCheck) {
            const ArgList::Arg& arg = args.argTable[decl.name];

            // check the type
            if (canonicalType(arg.type) != canonicalType(decl.type)) {
                std::string v1 = GLenumToString(decl.type);
                std::string v2 = GLenumToString(arg.type);
                std::string v1c = GLenumToString(canonicalType(decl.type));
                std::string v2c = GLenumToString(canonicalType(arg.type));
                throw ArgumentError(
                format("Variable %s was declared as type %s (%s) and the value provided at runtime had type %s (%s).",
                        decl.name.c_str(), v1.c_str(), v1c.c_str(), v2.c_str(), v2.c_str()));
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
            
            for (int u = 0; u < uniformArray.size(); ++u) {
                if (uniformArray[u].name == arg->key) {
                    foundArgument = true;
                    break;
                }
            }

            if (! foundArgument) {
                throw ArgumentError(
                std::string("Extra VertexAndPixelShader uniform variable provided at runtime: ") + arg->key + ".");
            }

            ++arg;
        }
    }

}


void VertexAndPixelShader::bindArgList(RenderDevice* rd, const ArgList& args) const {
    rd->forceVertexAndPixelShaderBind();

    validateArgList(args);

    // Iterate through the formal parameter list
    for (int u = 0; u < uniformArray.size(); ++u) {
        const UniformDeclaration& decl  = uniformArray[u];
    
        if (decl.dummy) {
            // Don't set this variable; it is unused.
            continue;
        }

        int location = uniformArray[u].location;

        if (beginsWith(decl.name, "g3d_sz2D_")) {
            // This is the autogenerated dimensions of a texture.

            std::string textureName  = decl.name.substr(9, decl.name.length() - 9);

            const ArgList::Arg& value = args.argTable.get(textureName); 

            // Compute the vector of size and inverse size
            float w = value.texture->texelWidth();
            float h = value.texture->texelHeight();
            Vector4 v(w, h, 1.0f / w, 1.0f / h);

            glUniform4fvARB(location, 1, v);

        } else {

            // Normal user defined variable

            const ArgList::Arg&       value = args.argTable.get(decl.name); 

            // Bind based on the declared type
            switch (canonicalType(decl.type)) {
            case GL_TEXTURE_1D:
                debugAssertM(false, "1D texture binding not implemented");
                break;

            case GL_TEXTURE_2D:
            case GL_TEXTURE_CUBE_MAP_ARB:
            case GL_TEXTURE_RECTANGLE_EXT:
            case GL_TEXTURE_3D:
                // Textures are bound as if they were integers.  The
                // value of the texture is the texture unit into which
                // the texture is placed.
                debugAssert(decl.textureUnit >= 0);
                glUniform1iARB(location, decl.textureUnit);
                rd->setTexture(decl.textureUnit, value.texture);
                break;

            case GL_FLOAT:
                glUniform1fvARB(location, 1, value.vector[0]);
                break;

            case GL_FLOAT_VEC2_ARB:
                glUniform2fvARB(location, 1, value.vector[0]);
                break;

            case GL_FLOAT_VEC3_ARB:
                glUniform3fvARB(location, 1, value.vector[0]);
                break;

            case GL_FLOAT_VEC4_ARB:
                glUniform4fvARB(location, 1, value.vector[0]);
                break;

            case GL_INT:
            case GL_BOOL_ARB:
                // OpenGL allows us to treat bools as ints, but not ints as floats
                glUniform1iARB(location, (int)value.vector[0][0]);
                break;

            case GL_INT_VEC2_ARB:
            case GL_BOOL_VEC2_ARB:
                glUniform2iARB(location, (int)value.vector[0].x, (int)value.vector[0].y);
                break;

            case GL_INT_VEC3_ARB:
            case GL_BOOL_VEC3_ARB:
                glUniform3iARB(location, (int)value.vector[0].x, (int)value.vector[0].y,
                    (int)value.vector[0].z);
                break;

            case GL_INT_VEC4_ARB:
            case GL_BOOL_VEC4_ARB:
                glUniform4iARB(location, (int)value.vector[0].x, (int)value.vector[1].y,
                    (int)value.vector[2].z, (int)value.vector[3].w);
                break;

            case GL_FLOAT_MAT2_ARB:
                debugAssertM(false, "GL_FLOAT_MAT2_ARB binding not implemented");
                break;

            case GL_FLOAT_MAT3_ARB:
                {
                    float m[9];
                    for (int i = 0, c = 0; c < 3; ++c) {
                        for (int r = 0; r < 3; ++r, ++i) {
                            m[i] = value.vector[r][c];
                        }
                    }
                    glUniformMatrix3fvARB(location, 1, GL_FALSE, m);
                }            
                break;

            case GL_FLOAT_MAT4_ARB:
                {
                    float m[16];
                    for (int i = 0, c = 0; c < 4; ++c) {
                        for (int r = 0; r < 4; ++r, ++i) {
                            m[i] = value.vector[r][c];
                        }
                    }
                    glUniformMatrix4fvARB(location, 1, GL_FALSE, m);
                }
                break;

            default:
                alwaysAssertM(false, format("Unsupported argument type: %s", GLenumToString(decl.type)));
            } // switch on type
        } // if g3d_sz2D_ variable
    }
}


////////////////////////////////////////////////////////////////////////

void VertexAndPixelShader::ArgList::set(const std::string& var, const Texture::Ref& val) {
    Arg arg;
    debugAssert(val.notNull());
    arg.type    = val->openGLTextureTarget();
    arg.texture = val;
    argTable.set(var, arg);

}


void VertexAndPixelShader::ArgList::set(const std::string& var, const CoordinateFrame& val) {
    set(var, Matrix4(val));
}


void VertexAndPixelShader::ArgList::set(const std::string& var, const Matrix4& val) {
    Arg arg;
    arg.type = GL_FLOAT_MAT4_ARB;
    for (int r = 0; r < 4; ++r) {
        arg.vector[r] = val.getRow(r);
    }

    argTable.set(var, arg);
}

void VertexAndPixelShader::ArgList::set(const std::string& var, const Matrix3& val) {
    Arg arg;
    arg.type = GL_FLOAT_MAT3_ARB;
    for (int r = 0; r < 3; ++r) {
        arg.vector[r] = Vector4(val.getRow(r), 0);
    }

    argTable.set(var, arg);
}


void VertexAndPixelShader::ArgList::set(const std::string& var, const Vector4& val) {
    Arg arg;
    arg.type = GL_FLOAT_VEC4_ARB;
    arg.vector[0] = val;
    argTable.set(var, arg);
}


void VertexAndPixelShader::ArgList::set(const std::string& var, const Vector3& val) {
    Arg arg;
    arg.type = GL_FLOAT_VEC3_ARB;
    arg.vector[0] = Vector4(val, 0);
    argTable.set(var, arg);
}


void VertexAndPixelShader::ArgList::set(const std::string& var, const Color4& val) {
    Arg arg;
    arg.type = GL_FLOAT_VEC4_ARB;
    arg.vector[0] = Vector4(val.r, val.g, val.b, val.a);
    argTable.set(var, arg);
}


void VertexAndPixelShader::ArgList::set(const std::string& var, const Color3& val) {
    Arg arg;
    arg.type = GL_FLOAT_VEC3_ARB;
    arg.vector[0] = Vector4(val.r, val.g, val.b, 0);
    argTable.set(var, arg);

}

void VertexAndPixelShader::ArgList::set(const std::string& var, const Vector2& val) {
    Arg arg;
    arg.type = GL_FLOAT_VEC2_ARB;
    arg.vector[0] = Vector4(val, 0, 0);
    argTable.set(var, arg);
}


void VertexAndPixelShader::ArgList::set(const std::string& var, float          val) {
    Arg arg;
    arg.type = GL_FLOAT;
    arg.vector[0] = Vector4(val, 0, 0, 0);
    argTable.set(var, arg);
}


void VertexAndPixelShader::ArgList::clear() {
    argTable.clear();
}


}
