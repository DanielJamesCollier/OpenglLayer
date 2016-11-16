//
//  OpenGLContext.hpp
//  OpenGLExamples
//
//  Created by Daniel Collier on 16/10/2016.
//  Copyright © 2016 Daniel Collier. All rights reserved.
//

#ifndef OpenGLContext_hpp
#define OpenGLContext_hpp

#include <stdio.h>
#include "Utils.h"
#include <Array>
#include <string>
#include <vector>

#define OPENGL_MAJOR_VERSION  4
#define OPENGL_MINOR_VERSION  1
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
 */

class ShaderProgram
{
    friend class OpenglLayer;
    
public:
    ShaderProgram()
    :
    id(OPENGL_INVALID_OBJECT),
    shaderObjects({OPENGL_INVALID_OBJECT,OPENGL_INVALID_OBJECT,OPENGL_INVALID_OBJECT}),
    linked(false)
    {
    }
    
    bool operator==(ShaderProgram const & rhs) { return(this->id == rhs.id);}
    bool operator!=(ShaderProgram const & rhs) { return(!(this->id == rhs.id));}
    operator int() const { return id;}
    
private:
    GLuint id;;
    std::array<int, 3> shaderObjects;
    bool linked;
};

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
    CUMPUTE_SHADER         = GL_COMPUTE_SHADER
#endif
};

class ShaderObject
{
    friend class OpenglLayer;
    
public:
    ShaderObject()
    :
    id(OPENGL_INVALID_OBJECT),
    type(ShaderObjectType::VERTEX_SHADER),
    hasSource(false),
    isCompiled(false)
    {
    }
    
    bool operator==(ShaderObject const & rhs) { return(this->id == rhs.id); }
    bool operator!=(ShaderObject const & rhs) { return(!(this->id == rhs.id)); }
    operator int() const { return id; }
    
private:
    GLuint id;
    ShaderObjectType type;
    bool hasSource;
    bool isCompiled;
};

enum class VertexBufferType
{
    OPENGL_STATIC_DRAW   = GL_STATIC_DRAW,
    OPENGL_DYNAMIC_DRAW  = GL_DYNAMIC_DRAW,
};

class VertexBufferObject
{
    friend class OpenglLayer;
    
public:
    VertexBufferObject()
    :
    id(OPENGL_INVALID_OBJECT)
    {
    }
    
    bool operator==(VertexBufferObject const & rhs) { return(this->id == rhs.id); }
    bool operator!=(VertexBufferObject const & rhs) { return(!(this->id == rhs.id)); }
    operator int() const { return id; }
    
private:
    GLuint id;
};

class VertexArrayObject
{
    friend class OpenglLayer;
    
public:
    VertexArrayObject()
    :
    id(OPENGL_INVALID_OBJECT)
    {
    }
    
    bool operator==(VertexArrayObject const & rhs) { return(this->id == rhs.id); }
    bool operator!=(VertexArrayObject const & rhs) { return(!(this->id == rhs.id)); }
    operator int() const { return id; }
    
private:
    GLuint id;
};

class OpenglLayer
{
public:
    OpenglLayer();
    ~OpenglLayer();
    
    bool init();
    void dispose();
    
    /*Info functions*/
    /*----------------------------------------------------------------------------------------------*/
    int getMajor() const;
    int getMinor() const;
    std::string getInfo() const;
    /*----------------------------------------------------------------------------------------------*/
    
    void checkOpenGLError(const char * stmt, const char * fname, int line) const;
    
    /*
     *Order of shader Program creation*
     - create a shader progarm
     - create a shader object
     - compile shader object
     - attach objects to program
     - link program
     */
    
    /*Shader functions*/
    /*----------------------------------------------------------------------------------------------*/
    ShaderProgram createShaderProgram() const;
    ShaderObject createShaderObject(ShaderObjectType const & type) const;
    void attachSourceToShaderObject(ShaderObject & object, std::string const & source) const;
    void compileShaderObject(ShaderObject & object) const;
    void attachShaderObjectToProgram(ShaderProgram const & program, ShaderObject const & object) const;
    void detachShaderObjectFromProgram(ShaderProgram const & program, ShaderObject const & object) const;
    void detachAllShaderObjectsFromProgram(ShaderProgram const & program) const;
    void linkProgram(ShaderProgram & program);
    void deleteShaderProgram(ShaderProgram const & program) const;
    void deleteShaderObject(ShaderObject const & object) const;
    void bindShaderProgram(ShaderProgram const & program);
    /*----------------------------------------------------------------------------------------------*/
    
    /*
     *Order of texture creation*
     -
     */
    
    /*Texture functions*/
    /*----------------------------------------------------------------------------------------------*/
    
    /*----------------------------------------------------------------------------------------------*/
    
    
    /*
     *Order of vertex creation*
     -
     */
    
    /*Texture functions*/
    /*----------------------------------------------------------------------------------------------*/
    VertexBufferObject createVertexBufferObject(VertexBufferType const & type, GLfloat points[], int numberOfPoints);
    VertexArrayObject createVertexArrayObject();
    /*----------------------------------------------------------------------------------------------*/
    
    
private:
    std::vector<ShaderProgram>      m_programs;
    std::vector<ShaderObject>       m_shaderObjects;
    std::vector<VertexBufferObject> m_vertexBuffersObjects;
    std::vector<VertexArrayObject>  m_vertexArrayObjects;
    
    int  m_majorVersion;
    int  m_minorVersion;
    bool m_initialised;
    
    ShaderProgram m_boundProgram;
};
#endif /* OpenGLContext_hpp */
