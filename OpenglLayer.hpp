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
 
 TODO
 - add ifdefs for opengl versions
 - add context creation into layer so it is based on the version defines.??
 - add master function for shader creation
 - figure out some template magic for the TexturePixelData class and then pass it into the create texture function !(:
 - if defs for DSA opengl
 - finish of createTexture function - currently only works for certain texture types
 - replace glew with something ?? for the windows version
 */

#ifndef OpenglLayer_hpp
#define OpenglLayer_hpp

#include <stdio.h>
#include <string>
#include <vector>

#define OPENGL_MAJOR_VERSION  4
#define OPENGL_MINOR_VERSION  1
#define OPENGL_MIN_REQUIRED_VERSION_MAJOR 3
#define OPENGL_MIN_REQUIRED_VERSION_MINOR 2
#define OPENGL_INVALID_OBJECT 0

#ifdef __APPLE__
#include <OpenGL/gl3.h> // includes 3.2 + functionality
#elif _WIN32
#include "GL/glew.h"
#endif

namespace gl
{
    
    /* General Classes */
    /*----------------------------------------------------------------------------------------------*/
    
    /*----------------------------------------------------------------------------------------------*/
    
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
    
    
    /*----------------------------------------------------------------------------------------------*/
    /* OpenglLayer - main class */
    /*----------------------------------------------------------------------------------------------*/
    class OpenglLayer
    {
    public:
        
        /*creation and destruction*/
        /*----------------------------------------------------------------------------------------------*/
        OpenglLayer();
        ~OpenglLayer();
        bool init();
        void dispose();
        /*----------------------------------------------------------------------------------------------*/
        
        /*Info functions*/
        /*----------------------------------------------------------------------------------------------*/
        constexpr int getMajor() const;
        constexpr int getMinor() const;
        constexpr int getMinimumRequiredVersionMajor() const;
        constexpr int getMinimumRequiredVersionMinor() const;
        std::string   getInfo()  const;
        /*----------------------------------------------------------------------------------------------*/
        
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
        
        /*Geometry functions*/
        /*----------------------------------------------------------------------------------------------*/
        VertexBufferObject createVertexBufferObject(VertexBufferType const & type, GLfloat points[], int numberOfPoints);
        VertexArrayObject createVertexArrayObject();
        /*----------------------------------------------------------------------------------------------*/
        
        
    private:
        
        // for tracking creation and destruction
        //-----------------------------------------------//
        //std::vector<ShaderProgram>         m_programs;
        //std::vector<ShaderObject>          m_shaderObjects;
        //std::vector<VertexBufferObject>    m_vertexBuffersObjects;
        //std::vector<VertexArrayObject>     m_vertexArrayObjects;
        //-----------------------------------------------//
        
        // shader section variables
        //-----------------------------------------------//
        ShaderProgram                        m_boundProgram;
        //-----------------------------------------------//
        
        // texture variables
        //-----------------------------------------------//
        
        //-----------------------------------------------//
        
        // general information
        //-----------------------------------------------//
        int                                  m_majorVersion;
        int                                  m_minorVersion;
        bool                                 m_initialised;
        std::string                          m_openglLayerInfo;
        //-----------------------------------------------//
        
        
        // private functions
        //-----------------------------------------------//
        void checkOpenGLError(const char * stmt, const char * fname, int line) const;
        //-----------------------------------------------//
    };
}
/*----------------------------------------------------------------------------------------------*/
#endif /* OpenglLayer_hpp */
