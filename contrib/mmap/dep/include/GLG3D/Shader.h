/**
 @file Shader.h
  
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2004-04-25
 @edited  2007-07-04
 */

#ifndef G3D_SHADER_H
#define G3D_SHADER_H

#include "GLG3D/glheaders.h"
#include "GLG3D/Texture.h"
#include "G3D/Set.h"
#include "G3D/Vector4.h"
#include "G3D/CoordinateFrame.h"
#include "G3D/Vector3.h"
#include "G3D/Color4.h"
#include "G3D/Color3.h"
#include <string>

namespace G3D {

typedef ReferenceCountedPointer<class VertexAndPixelShader>  VertexAndPixelShaderRef;

#ifdef _DEBUG
    #define DEBUG_SHADER true
#else
    #define DEBUG_SHADER false
#endif

/** Argument to G3D::VertexAndPixelShader and G3D::Shader create methods */
enum UseG3DUniforms {DEFINE_G3D_UNIFORMS, DO_NOT_DEFINE_G3D_UNIFORMS};


/**
  A compatible vertex and pixel shader.  Used internally by G3D::Shader; see that 
  class for more information.

  Only newer graphics cards with recent drivers (e.g. GeForceFX cards with driver version 57 or greater)
  support this API.  Use the VertexAndPixelShader::fullySupported method to determine at run-time
  if your graphics card is compatible.

  For purposes of shading, a "pixel" is technically a "fragment" in OpenGL terminology.

  Pixel and vertex shaders are loaded as text strings written in 
  <A HREF="http://developer.3dlabs.com/openGL2/specs/GLSLangSpec.Full.1.10.59.pdf">GLSL</A>, the high-level
  OpenGL shading language.  

  Typically, the G3D::Shader sets up constants like the object-space position
  of the light source and the object-to-world matrix.  The vertex shader transforms
  input vertices to homogeneous clip space and computes values that are interpolated
  across the surface of a triangle (e.g. reflection vector).  The pixel shader
  computes the final color of a pixel (it does not perform alpha-blending, however).

  Multiple VertexAndPixelShaders may share object, vertex, and pixel shaders.

  Uniform variables that begin with 'gl_' are ignored because they are assumed to 
  be GL built-ins.

  @cite http://oss.sgi.com/projects/ogl-sample/registry/ARB/shader_objects.txt
  @cite http://oss.sgi.com/projects/ogl-sample/registry/ARB/vertex_shader.txt

@deprecated
 */
class VertexAndPixelShader : public ReferenceCountedObject {
public:
    friend class Shader;

    class UniformDeclaration {
    public:
        /** If true, this variable is declared but unused */
        bool                dummy;

        /** Register location */
        int                 location;

        /** Name of the variable.  May include [] and . (e.g.
            "foo[1].normal")*/
        std::string         name;

        /** OpenGL type of the variable (e.g. GL_INT) */
        GLenum              type;

        /** Unknown... appears to always be 1 */
        int                 size;

        /**
         Index of the texture unit in which this value
         is stored.  -1 for uniforms that are not textures. */  
        int                 textureUnit;
    };

protected:

    class GPUShader {
    protected:
        
        /** argument for output on subclasses */
        static std::string          ignore;
        
        std::string                 _name;
        std::string                 _code;
        bool                        fromFile;
        
        GLhandleARB                 _glShaderObject;
        
        bool                        _ok;
        std::string                 _messages;
        
        /** Returns true on success.  Called from init. */
        void compile();
        
        /** Initialize a shader object and returns object.  
            Called from subclass create methods. */
        static GPUShader*           init(GPUShader* shader, bool debug);
        
        /** Set to true when name and code both == "" */
        bool			_fixedFunction;
        
        GLenum                      _glShaderType;
        
        std::string                 _shaderType;
        
        /**
           Replaces all instances of 
           <code>"g3d_sampler2DSize(name)"</code> with 
           <code>"     (g3d_sz2D_name.xy)"</code> and
           
           <code>"g3d_sampler2DInvSize(name)"</code> with
           <code>"        (g3d_sz2D_name.zw)"</code> 
           
          Note that both replacements will leave column numbers the same in error messages.  The
          <code>()</code> wrapper ensures that <code>.xy</code> fields are accessible using
          normal syntax off the result; it is the same as the standard practice of wrapping macros
          in parentheses.

          and adds "uniform vec4 g3d_sz2D_name;" to the uniform string.

          Called from init.
         */
        void replaceG3DSize(std::string& code, std::string& uniformString);

	public:

            void init(
                      const std::string& name,
                      const std::string& code,
                      bool               fromFile,
                      bool		 debug,
                      GLenum		 glType,
                      const std::string& type,
                      UseG3DUniforms     u);
            
		/** Deletes the underlying glShaderObject.  Between GL's reference
			counting and G3D's reference counting, an underlying object
			can never be deleted while in use. */
		~GPUShader();

		/** Shader type, e.g. GL_VERTEX_SHADER_ARB */
		inline GLenum glShaderType() const {
			return _glShaderType;
		}

		inline const std::string& shaderType() const {
			return _shaderType;
		}

		/** Why compilation failed, or any compiler warnings if it succeeded.*/
		inline const std::string& messages() const {
			return _messages;
		}

		/** Returns true if compilation and loading succeeded.  If they failed,
			check the message string.*/
		inline bool ok() const {
			return _ok;
		}

		/** Returns the underlying OpenGL shader object for this shader */
		inline GLhandleARB glShaderObject() const {
			return _glShaderObject;
		}

		inline bool fixedFunction() const {
			return _fixedFunction;
		}
	};

    static std::string      ignore;

    GPUShader	            vertexShader;
    GPUShader	            geometryShader;
    GPUShader		    pixelShader;

    GLhandleARB             _glProgramObject;

    bool                    _ok;
    std::string             _messages;

    std::string             _fragCompileMessages;
    std::string             _vertCompileMessages;
    std::string             _linkMessages;

    int                     lastTextureUnit;

    /** Converts from int and bool types to float types (e.g. GL_INT_VEC2_ARB -> GL_FLOAT_VEC2_ARB).
        Other types are left unmodified.*/
    static GLenum canonicalType(GLenum e);

    /** Computes the uniformArray from the current
        program object.  Called from the constructor */
    void computeUniformArray();

    /** Finds any uniform variables in the code that are not already in 
        the uniform array that OpenGL returned and adds them to that array.
        This causes VertexAndPixelShader to surpress warnings about 
        setting variables that have been compiled away--those warnings
        are annoying when temporarily commenting out code. */
    void addUniformsFromCode(const std::string& code);

    /** Does not contain g3d_ uniforms if they were compiled away */
    Array<UniformDeclaration>   uniformArray;

    /** Does not contain g3d_ uniforms if they were compiled away */
    Set<std::string>            uniformNames;

    /** Returns true for types that are textures (e.g., GL_TEXTURE_2D) */
    static bool isSamplerType(GLenum e);

    VertexAndPixelShader
    (
     const std::string&  vsCode,
     const std::string&  vsFilename,
     bool                vsFromFile,
     const std::string&  psCode,
     const std::string&  psFilename,
     bool                psFromFile,
     bool                debug,
     UseG3DUniforms      u);

public:

    /** Thrown by validateArgList */
    class ArgumentError {
    public:
        std::string             message;
        
        ArgumentError(const std::string& m) : message(m) {}
    };
    
    static VertexAndPixelShaderRef fromStrings
    (
     const std::string& vertexShader,
     const std::string& pixelShader,       
     UseG3DUniforms u = DO_NOT_DEFINE_G3D_UNIFORMS,
     bool debugErrors = DEBUG_SHADER);
    
    /**
       To use the default/fixed-function pipeline for part of the
       shader, pass an empty string.
    */
    static VertexAndPixelShaderRef fromStrings
        (
         const std::string& vertexShaderName,
         const std::string& vertexShader,
         const std::string& geometryShaderName,
         const std::string& geometryShader,       
         const std::string& pixelShaderName,
         const std::string& pixelShader,       
         UseG3DUniforms u = DO_NOT_DEFINE_G3D_UNIFORMS,
         bool debugErrors = DEBUG_SHADER);
    
	/**
	 To use the fixed function pipeline for part of the
	 shader, pass an empty string.

     @param debugErrors If true, a debugging dialog will
        appear when there are syntax errors in the shaders.
        If false, failures will occur silently; check
        VertexAndPixelShader::ok() to see if the files
        compiled correctly.
	 */
	static VertexAndPixelShaderRef fromFiles(
		const std::string& vertexShader,
		const std::string& pixelShader,
        UseG3DUniforms u = DO_NOT_DEFINE_G3D_UNIFORMS,
        bool debugErrors = DEBUG_SHADER);

    /**
     Bindings of values to uniform variables for a VertexAndPixelShader.
     Be aware that 
     the uniform namespace is global across the pixel and vertex shader.

     GL_BOOL_ARB and GL_INT_ARB-based values are coerced to floats
     automatically by the arg list.
     */
    class ArgList {
    private:
        friend class VertexAndPixelShader;

        class Arg {
        public:

            /** Row-major */ 
            Vector4                    vector[4];

            Texture::Ref                 texture;

            GLenum                     type;
        };

        Table<std::string, Arg>        argTable;

    public:

        void set(const std::string& var, const Texture::Ref& val);
        void set(const std::string& var, const CoordinateFrame& val);
        void set(const std::string& var, const Matrix4& val);
        void set(const std::string& var, const Matrix3& val);
        void set(const std::string& var, const Color4& val);
        void set(const std::string& var, const Color3& val);
        void set(const std::string& var, const Vector4& val);
        void set(const std::string& var, const Vector3& val);
        void set(const std::string& var, const Vector2& val);
        void set(const std::string& var, float          val);

        /**
         GLSL does not natively support arrays and structs in the uniform binding API.  Instead, each
         element of an array is treated as a separate element.  This method expands out to setting
         each element of an array.  You can instead set them using <CODE>args.set("arry[3]", myVal)</CODE>.
         Likewise for structs, <CODE>args.set("str.foo.bar", myVal)</CODE>.
         */
        template<class T> void set(const std::string& arrayName, const G3D::Array<T>& arrayVal) {
            for (int i = 0; i < arrayVal.size(); ++i) {
                set(format("%s[%d]", arrayName.c_str(), i), arrayVal[i]);
            }
        }

        template<class T> void set(const std::string& arrayName, const std::vector<T>& arrayVal) {
            for (int i = 0; i < arrayVal.size(); ++i) {
                set(format("%s[%d]", arrayName.c_str(), i), arrayVal[i]);
            }
        }
        
        void clear();
    };

    ~VertexAndPixelShader();

    /**
     Returns GLCaps::supports_GL_ARB_shader_objects() && 
        GLCaps::supports_GL_ARB_shading_language_100() &&
        GLCaps::supports_GL_ARB_fragment_shader() &&
        GLCaps::supports_GL_ARB_vertex_shader()
    */
    static bool fullySupported();

    inline bool ok() const {
        return _ok;
    }

    /**
     All compilation and linking messages, with additional formatting.
     For details about a specific part of the process, see 
     vertexErrors, pixelErrors, and linkErrors.
     */
    inline const std::string& messages() const {
        return _messages;
    }

    inline const std::string& vertexErrors() const {
        return _vertCompileMessages;
    }

    inline const std::string& pixelErrors() const {
        return _fragCompileMessages;
    }

    inline const std::string& linkErrors() const {
        return _linkMessages;
    }

    /** The underlying OpenGL object for the vertex/pixel shader pair.

        To bind a shader with RenderDevice, call renderDevice->setShader(s);
        To bind a shader <B>without</B> RenderDevice, call
        glUseProgramObjectARB(s->glProgramObject());

    */
    GLhandleARB glProgramObject() const {
        return _glProgramObject;
    }

    int numArgs() const {
        return uniformArray.size();
    }

    /** Checks the actual values of uniform variables against those 
        expected by the program.
        If one of the arguments does not match, an ArgumentError
        exception is thrown.
    */
    void validateArgList(const ArgList& args) const;

    /**
       Makes renderDevice calls to bind this argument list.
       Calls validateArgList.
     */
    void bindArgList(class RenderDevice* rd, const ArgList& args) const;

    /** Returns information about one of the arguments expected
        by this VertexAndPixelShader.  There are VertexAndPixelShader::numArgs()
        total.*/
    const UniformDeclaration& arg(int i) const {
        return uniformArray[i];
    }
};

class Shader;
typedef ReferenceCountedPointer<Shader> ShaderRef;

/**
  Abstraction of the programmable hardware pipeline.  
  Use with G3D::RenderDevice::setShader.
  G3D::Shader allows you to specify C++ code (by overriding
  the methods) that executes for each group of primitives and
  OpenGL Shading Language (<A HREF="http://developer.3dlabs.com/openGL2/specs/GLSLangSpec.Full.1.10.59.pdf">GLSL</A>) 
  code that executes for each vertex and each pixel.
    
  <P>
  Uses G3D::VertexAndPixelShader internally.  What we call pixel shaders
  are really "fragment shaders" in OpenGL terminology.

  <P>
  
  Unless DO_NOT_DEFINE_G3D_UNIFORMS is specified to the static constructor, the
  following additional variables will be available inside the shaders:

  <PRE>
    uniform mat4 g3d_WorldToObjectMatrix;
    uniform mat4 g3d_ObjectToWorldMatrix;
    uniform mat4 g3d_WorldToCameraMatrix;
    uniform mat4 g3d_CameraToWorldMatrix;
    uniform mat3 g3d_ObjectToWorldNormalMatrix; // Upper 3x3 matrix (assumes that the transformation is RT so that the inverse transpose of the upper 3x3 is just R)
    uniform mat3 g3d_WorldToObjectNormalMatrix; // Upper 3x3 matrix (assumes that the transformation is RT so that the inverse transpose of the upper 3x3 is just R)
    uniform int  g3d_NumLights;        // 1 + highest index of the enabled lights
    uniform int  g3d_NumTextures;      // 1 + highest index of the enabled textures
    uniform vec4 g3d_ObjectLight0;     // g3d_WorldToObject * g3d_CameraToWorld * gl_LightState[0].position
    uniform vec4 g3d_WorldLight0;      // g3d_CameraToWorld * gl_LightState[0].position

    vec2 g3d_sampler2DSize(sampler2D t);        // Returns the x and y dimensions of t
    vec2 g3d_sampler2DInvSize(sampler2D t);     // Returns vec2(1.0, 1.0) / g3d_size(t) at no additional cost
  </PRE>

  The macros <code>G3D_OSX, G3D_WIN32, G3D_FREEBSD, G3D_LINUX,
  G3D_ATI, G3D_NVIDIA, G3D_MESA</code> are defined on the relevant
  platforms.

  <code>g3d_sampler2DSize</code> and <code>g3d_sampler2DInvSize</code>
  require that there be no additional space between the function name
  and parens and no space between the parens and sampler name.  There
  is no cost for definining and then not using any of these; unused
  variables do not increase the runtime cost of the shader.


  If your GLSL 1.1 shader begins with <CODE>#include</CODE> or <CODE>#define</CODE> the
  line numbers will be off by 1 to 3 in error messages because the G3D uniforms are 
  inserted on the first line.  GLSL 1.2 shaders do not have this problem.

  <P>

  <P>
  ATI's RenderMonkey is an excellent tool for writing GLSL shaders under an IDE.
  http://www.ati.com/developer/rendermonkey/index.html
  You can then load those shaders into G3D::Shader and use them with your application.

  <B>Example</B>

  The following example computes lambertian + ambient shading in camera space,
  on the vertex processor.  Although the shader could easily have been loaded from a file,
  the example loads it from a string (which is conveniently created with the STR macro)

  Vertex
  shaders are widely supported, so this will run on any graphics card produced since 2001
  (e.g. GeForce3 and up).  Pixel shaders are available on "newer" cards 
  (e.g. GeForceFX 5200 and up).

  <PRE>
   IFSModelRef model;
   ShaderRef   lambertian;

   ...

   // Initialization
   model = IFSModel::create(app->dataDir + "ifs/teapot.ifs");
   lambertian = Shader::creates(Shader::VERTEX_STRING, STR(

     uniform vec3 k_A;

     void main(void) {
        gl_Position = ftransform();
        gl_FrontColor.rgb = max(dot(gl_Normal, g3d_ObjectLight0.xyz), 0.0) * gl_LightSource[0].diffuse + k_A;
     }));

    ...

    // Rendering loop
    app->renderDevice->setLight(0, GLight::directional(Vector3(1,1,1), Color3::white() - Color3(.2,.2,.3)));

    app->renderDevice->setShader(lambertian);
    lambertian->args.set("k_A", Color3(.2,.2,.3));
    model->pose()->render(app->renderDevice);
  </PRE>

  This example explicitly sets the ambient light color to demonstrate how
  arguments are used.  That could instead have been read from gl_LightModel.ambient.
  Note the use of g3d_ObjectLight0.  Had we not used that variable, Shader would
  not have computed or set it.

  See also: http://developer.download.nvidia.com/opengl/specs/GL_EXT_geometry_shader4.txt  

  <B>BETA API</B>
  This API is subject to change.
 */
class Shader  : public ReferenceCountedObject {
protected:

    VertexAndPixelShaderRef         _vertexAndPixelShader;
    UseG3DUniforms                  _useUniforms;
    bool                            _preserveState;

    inline Shader(VertexAndPixelShaderRef v, UseG3DUniforms u) : 
        _vertexAndPixelShader(v), _useUniforms(u), _preserveState(true) {}

    /** For subclasses to invoke */
    inline Shader() {}

public:

    /** Arguments to the vertex and pixel shader.  You may change these either
        before or after the shader is set on G3D::RenderDevice-- either way
        they will take effect immediately.*/
    VertexAndPixelShader::ArgList   args;

    /** Returns true if this shader is declared to accept the specified argument. */
    bool hasArgument(const std::string& argname) const;

    /** Describe the arguments to Shader::create */
    enum ShaderType {
        SHADER_NONE,
        VERTEX_STRING, 
        VERTEX_FILE,
        GEOMETRY_STRING, 
        GEOMETRY_FILE,
        PIXEL_STRING, 
        PIXEL_FILE};

    /**
      This routine is a placeholder for 7.01 geometry shader API.  It is not implemented.

      @beta

     Create a new GLSL shader.  Arguments are in pairs. The first argument in each pair is an ArgumentType
     specifying the type of shader (vertex, geometry, or pixel) and whether it comes from a string or a file.
     The second argument in each pair is the filename or shader source itself.

     Example:
     <pre>
     std::string vs = STR(
         void main() {
            gl_Position = ftransform();
         });

     ShaderRef myEffect = Shader::create(
        Shader::VERTEX_STRING, vs, 
        Shader::GEOMETRY_FILE, "effect.geom.glsl",
        Shader::PIXEL_FILE, "effect.pixel.glsl");

     </pre>
     */
    static ShaderRef create(
        ShaderType          type0,
        const std::string&  value0,

        ShaderType          type1 = SHADER_NONE,
        const std::string&  value1 = "",

        ShaderType          type2 = SHADER_NONE,
        const std::string&  value2 = "");

        
    inline static ShaderRef fromFiles(
        const std::string& vertexFile, 
        const std::string& pixelFile,
        UseG3DUniforms u = DEFINE_G3D_UNIFORMS) {
        return new Shader(VertexAndPixelShader::fromFiles(vertexFile, pixelFile, u, DEBUG_SHADER), u);
    }

    inline static ShaderRef fromStrings(
        const std::string& vertexCode,
        const std::string& pixelCode,
        UseG3DUniforms u = DEFINE_G3D_UNIFORMS) {
        return new Shader(VertexAndPixelShader::fromStrings(vertexCode, pixelCode, u, DEBUG_SHADER), u);
    }

    /** Names are purely for debugging purposes */
    inline static ShaderRef fromStrings(
        const std::string& vertexName,
        const std::string& vertexCode,
        const std::string& pixelName,
        const std::string& pixelCode,
        UseG3DUniforms u = DEFINE_G3D_UNIFORMS) {
        return new Shader(VertexAndPixelShader::fromStrings(vertexName, vertexCode, "", "", pixelName, pixelCode, u, DEBUG_SHADER), u);
    }

    /** When true, any RenderDevice state that the shader configured before a primitive it restores at
        the end of the primitive.  When false, the shader is allowed to corrupt state.  Setting to false can
        lead to faster operation
        when you know that the next primitive will also be rendered with a shader, since shaders tend
        to set all of the state that they need.

        Defaults to true */
    virtual void setPreserveState(bool s) {
        _preserveState = s;
    }

    virtual bool preserveState() const {
        return _preserveState;
    }

    virtual bool ok() const;

    /** Returns true if this card supports vertex shaders */
    static bool supportsVertexShaders();

    /** Returns true if this card supports geometry shaders */
    static bool supportsGeometryShaders();

    /** Returns true if this card supports pixel shaders.  */
    static bool supportsPixelShaders();

	/**
	 Invoked by RenderDevice immediately before a primitive group.
	 Override to set state on the RenderDevice (including the underlying
     vertex and pixel shader).

     If overriding, do not call RenderDevice::setShader from this routine.

     Default implementation pushes state, sets the g3d_ uniforms,
     and loads the vertex and pixel shader.
	 */
    virtual void beforePrimitive(class RenderDevice* renderDevice);

    /**
     Default implementation pops state.
     */
    virtual void afterPrimitive(class RenderDevice* renderDevice);

    virtual const std::string& messages() const;
};


}

#endif
