//
//  OpenGLContext.hpp
//  OpenGLExamples
//
//  Created by Daniel Collier on 16/10/2016.
//  Copyright © 2016 Daniel Collier. All rights reserved.

/*
 class information
 
 - opengl must be linked with this class
 - on windows glew must be linked with this class
 - a context must be created before any of the methods in this class are used
 - the init() function must be called before any other function in this class
 */
#ifndef OpenGLContext_hpp
#define OpenGLContext_hpp

#include <stdio.h>
#include <Array>
#include <string>
#include <vector>

#define OPENGL_MAJOR_VERSION  4
#define OPENGL_MINOR_VERSION  1
#define OPENGL_MIN_REQUIRED_VERSION_MAJOR 3
#define OPENGL_MIN_REQUIRED_VERSION_MINOR 2
#define OPENGL_INVALID_OBJECT 0

#ifdef __APPLE__
//#include <OpenGL/gl.h> // includes 2.1 functionality
#include <OpenGL/gl3.h> // includes 3.2 functionality
#elif _WIN32
#include "GL/glew.h"
#endif



//assert(obj.boolean) = if boolean is false then fail
//assert(1 == 0) if false then fail kinda annoying

/*
 TODO
 - add ifdefs for opengl versions
 - add context creation into layer so it is based on the version defines.
 - think about max objects per program
 */

namespace OpenglLayer
{
    /* Shader Classes */
    /*----------------------------------------------------------------------------------------------*/
    enum class ShaderObjectType
    {
        VERTEX_SHADER          = GL_VERTEX_SHADER,
#if OPENGL_MAJOR_VERSION >= 4
        TESS_CONTROL_SHADER    = GL_TESS_CONTROL_SHADER,
        TESS_EVALUATION_SHADER = GL_TESS_EVALUATION_SHADER,
#endif
        GEOMETRY_SHADER        = GL_GEOMETRY_SHADER,
        FRAGMENT_SHADER        = GL_FRAGMENT_SHADER,
#if OPENGL_MAJOR_VERSION >= 4 && OPENGL_MINOR_VERSION >= 3
        COMPUTE_SHADER         = GL_COMPUTE_SHADER
#endif
    };
    
    class ShaderObject
    {
        friend class OpenglLayer;
        
    public:
        ShaderObject()
        :
        m_id(OPENGL_INVALID_OBJECT),
        m_type(ShaderObjectType::VERTEX_SHADER),
        m_hasSource(false),
        m_isCompiled(false)
        {
        }
        
        //TODO make opporators only visable to OpenglLayer
        bool operator==(ShaderObject const & rhs) { return(this->m_id == rhs.m_id); }
        bool operator!=(ShaderObject const & rhs) { return(!(this->m_id == rhs.m_id)); }
        operator int() const { return m_id; }
        
    private:
        GLuint           m_id;
        ShaderObjectType m_type;
        bool             m_hasSource;
        bool             m_isCompiled;
    };
    
    class ShaderProgram
    {
        friend class OpenglLayer;
        
    public:
        ShaderProgram()
        :
        m_id(OPENGL_INVALID_OBJECT),
        m_linked(false)
        {
        }
        
        //TODO make opporators only visable to OpenglLayer
        bool operator==(ShaderProgram const & rhs) { return(this->m_id == rhs.m_id);}
        bool operator!=(ShaderProgram const & rhs) { return(!(this->m_id == rhs.m_id));}
        operator int() const { return m_id;}
        
    private:
        GLuint                    m_id;
        std::vector<ShaderObject> m_shaderObjects;
        bool                      m_linked;
    };
    /*----------------------------------------------------------------------------------------------*/
    
    
    /*Geometry Classes*/
    /*----------------------------------------------------------------------------------------------*/
    enum class VertexBufferType
    {
        STATIC_DRAW   = GL_STATIC_DRAW,
        DYNAMIC_DRAW  = GL_DYNAMIC_DRAW,
    };
    
    class VertexBufferObject
    {
        friend class OpenglLayer;
        
    public:
        VertexBufferObject()
        :
        m_id(OPENGL_INVALID_OBJECT)
        {
        }
        
        bool operator==(VertexBufferObject const & rhs) { return(this->m_id == rhs.m_id); }
        bool operator!=(VertexBufferObject const & rhs) { return(!(this->m_id == rhs.m_id)); }
        operator int() const { return m_id; }
        
    private:
        GLuint m_id;
    };
    
    class VertexArrayObject
    {
        friend class OpenglLayer;
        
    public:
        VertexArrayObject()
        :
        m_id(OPENGL_INVALID_OBJECT)
        {
        }
        
        bool operator==(VertexArrayObject const & rhs) { return(this->m_id == rhs.m_id); }
        bool operator!=(VertexArrayObject const & rhs) { return(!(this->m_id == rhs.m_id)); }
        operator int() const { return m_id; }
        
    private:
        GLuint m_id;
    };
    /*----------------------------------------------------------------------------------------------*/
    
    
    /*Texture classes*/
    /*----------------------------------------------------------------------------------------------*/
    
    enum class TextureTarget
    {
        TEXTURE_1D                   = GL_TEXTURE_1D,
        TEXTURE_2D                   = GL_TEXTURE_2D,
        TEXTURE_3D                   = GL_TEXTURE_3D,
        TEXTURE_1D_ARRAY             = GL_TEXTURE_1D_ARRAY,
        TEXTURE_2D_ARRAY             = GL_TEXTURE_2D_ARRAY,
        TEXTURE_RECTANGLE            = GL_TEXTURE_RECTANGLE,
        TEXTURE_CUBE_MAP             = GL_TEXTURE_CUBE_MAP,
        TEXTURE_CUBEMAP_ARRAY        = GL_TEXTURE_CUBE_MAP_ARRAY,
        TEXUTRE_BUFFER               = GL_TEXTURE_BUFFER,
        TEXTURE_2D_MULTISAMPLE       = GL_TEXTURE_2D_MULTISAMPLE,
        TEXTURE_2D_MULTISAMPLE_ARRAY = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
    };
    
    enum class TextureFilter
    {
        LINEAR                       = GL_LINEAR,
        NEAREST                      = GL_NEAREST,
        LINEAR_MIPMAP_LINEAR         = GL_LINEAR_MIPMAP_LINEAR,
        LINEAR_MIPMAP_NEAREST        = GL_LINEAR_MIPMAP_NEAREST,
        NEAREST_MIPMAP_NEAREST       = GL_NEAREST_MIPMAP_NEAREST,
        NEAREST_MIPMAP_LINEAR        = GL_NEAREST_MIPMAP_LINEAR,
    };
    
    enum class TextureFormat
    {
        // normal colour formats
        
        // special colour formats
        R3_G3_B2 = GL_R3_G3_B2,
        RGB5_A1 = GL_RGB5_A1,
        RGB10_A2 = GL_RGB10_A2,
        RGB10_A2UI = GL_RGB10_A2UI,
        R11F_G11F_B10F = GL_R11F_G11F_B10F,
        RGB9_E5 = GL_RGB9_E5,
        // sRGB colour space
        SRGB8 = GL_SRGB8,
        SRGB8_ALPHA8 = GL_SRGB8_ALPHA8,
        // compressed formats
        // TODO
    };
    
    class TextureSampler
    {
        friend class OpenglLayer;
        
    public:
        TextureSampler(TextureFilter magFilter = TextureFilter::LINEAR, TextureFilter minFilter = TextureFilter::LINEAR, bool genMipmaps = false)
        :
        m_id(OPENGL_INVALID_OBJECT),
        m_magFilter(magFilter),
        m_minFilter(minFilter),
        m_genMipmaps(genMipmaps)
        {
        }
    private:
        GLuint        m_id;
        TextureFilter m_magFilter;
        TextureFilter m_minFilter;
        bool          m_genMipmaps;
    };
    
    class Texture
    {
        friend class OpenglLayer;
        
    public:
        Texture()
        :
        m_id(OPENGL_INVALID_OBJECT),
        m_width(0),
        m_height(0),
        m_target(TextureTarget::TEXTURE_2D),
        m_sampler(/*use default args*/)
        {
        }
        
    private:
        GLuint         m_id;
        GLuint         m_width;
        GLuint         m_height;
        TextureTarget  m_target;
        TextureSampler m_sampler;
    };
    /*----------------------------------------------------------------------------------------------*/
    
    
    class OpenglLayer
    {
    public:
        OpenglLayer();
        ~OpenglLayer();
        
        bool init();
        void dispose();
        
        /*Info functions*/
        /*----------------------------------------------------------------------------------------------*/
        constexpr int getMajor() const;
        constexpr int getMinor() const;
        constexpr int getMinimumRequiredVersionMajor() const;
        constexpr int getMinimumRequiredVersionMinor() const;
        std::string getInfo() const;
        /*----------------------------------------------------------------------------------------------*/
        
        void checkOpenGLError(const char * stmt, const char * fname, int line) const;
        
        /*
         *Order of shader Program creation*
         - create a shader progarm
         - create a shader object
         - compile shader object
         - attach objects to program
         - bind attrib locations ? optional
         - link program
         */
        
        /*Shader functions*/
        /*----------------------------------------------------------------------------------------------*/
        ShaderProgram createShaderProgram() const;
        ShaderObject createShaderObject(ShaderObjectType const & type) const;
        void attachSourceToShaderObject(ShaderObject & object, std::string const & source) const;
        void compileShaderObject(ShaderObject & object) const;
        void attachShaderObjectToProgram(ShaderProgram & program, ShaderObject const & object) const;
        void detachShaderObjectFromProgram(ShaderProgram & program, ShaderObject const & object) const;
        void detachAllShaderObjectsFromProgram(ShaderProgram & program) const;
        void detachAndDeleteShaderObjectFromProgram(ShaderProgram & program, ShaderObject const & object) const;
        void detachAndDeleteAllShaderObjectsFromProgram(ShaderProgram & program) const;
        void linkProgram(ShaderProgram & program, bool deleteShaderObjects = true);
        void deleteShaderProgram(ShaderProgram & program) const;
        void deleteShaderObject(ShaderObject & object) const;
        void bindShaderProgram(ShaderProgram const & program);
        /*----------------------------------------------------------------------------------------------*/
        
        /*
         *Order of texture creation*
         -
         */
        
        /*Texture functions*/
        /*----------------------------------------------------------------------------------------------*/
        Texture createTexture(unsigned char * data, TextureTarget const & target, TextureSampler const & sampler, TextureFormat const & type) const;
        /*----------------------------------------------------------------------------------------------*/
        
        
        /*
         *Order of vertex creation*
         -
         */
        
        /*Geometry functions*/
        /*----------------------------------------------------------------------------------------------*/
        VertexBufferObject createVertexBufferObject(VertexBufferType const & type, GLfloat points[], int numberOfPoints);
        VertexArrayObject createVertexArrayObject();
        /*----------------------------------------------------------------------------------------------*/
        
        
    private:
        //std::vector<ShaderProgram>      m_programs;
        //std::vector<ShaderObject>       m_shaderObjects;
        //std::vector<VertexBufferObject> m_vertexBuffersObjects;
        //std::vector<VertexArrayObject>  m_vertexArrayObjects;
        
        int  m_majorVersion;
        int  m_minorVersion;
        bool m_initialised;
        
        ShaderProgram m_boundProgram;
    };
}
#endif /* OpenGLContext_hpp */
