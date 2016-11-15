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

#ifdef __APPLE__
#include <OpenGL/GL.h>
#elif _WIN32
#include "GL/glew.h"
#endif

#define OPENGL_MAJOR_VERSION  4
#define OPENGL_MINOR_VERSION  1
#define OPENGL_INVALID_OBJECT 0

//assert(obj.boolean) = if boolean is false then fail
//assert(1 == 0) if false then fail kinda annoying

/*
 TODO
 - add ifdefs for opengl versions
 - add context creation into layer so it is based on the version defines.
 */

struct ShaderProgram
{
    bool operator==(ShaderProgram const & rhs)
    {
        return(this->id == rhs.id);
    }
    
    bool operator!=(ShaderProgram const & rhs)
    {
        return(!(this->id == rhs.id));
    }
    
    operator int() const
    {
        return id;
    }
    
    GLuint id = OPENGL_INVALID_OBJECT;
    std::array<int, 3> shaderObjects = {OPENGL_INVALID_OBJECT,
        OPENGL_INVALID_OBJECT,
        OPENGL_INVALID_OBJECT}; // TODO: review this - sets a max limit on numeber of shaderObjects per shader program ??
    bool linked = false;
};

enum class ShaderObjectType
{
    VERTEX_SHADER   = GL_VERTEX_SHADER,
    FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
};

struct ShaderObject
{
    bool operator==(ShaderObject const & rhs)
    {
        return(this->id == rhs.id);
    }
    
    bool operator!=(ShaderObject const & rhs)
    {
        return(!(this->id == rhs.id));
    }
    
    operator int() const
    {
        return id;
    }
    
    std::string source{""}; // TODO: move into other location to redude shader object size, messes with the alignment
    GLuint id = OPENGL_INVALID_OBJECT;
    ShaderObjectType type = ShaderObjectType::VERTEX_SHADER;
    bool isCompiled = false;
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
    void compileShaderObject(ShaderObject & shaderObject) const;
    void attachShaderObjectToProgram(ShaderProgram const & program, ShaderObject const & object) const;
    void detachShaderObjectFromProgram(ShaderProgram const & program, ShaderObject const & object) const;
    void detachAllShaderObjectsFromProgram(ShaderProgram const & program) const;
    void linkProgram(ShaderProgram & program) const;
    void deleteShaderProgram(ShaderProgram const & program) const;
    void deleteShaderObject(ShaderObject const & object) const;
    bool isShaderObjectOkay(ShaderObject const & shader, GLenum flag, const std::string& errorMessage) const;
    bool isShaderProgramOkay(ShaderProgram const & program, GLenum flag, std::string const & errorMessage) const;
    void bindShaderProgram(ShaderProgram const & program);
    /*----------------------------------------------------------------------------------------------*/
    
    /*
     *Order of texture creation*
     -
     */
    
    /*Texture functions*/
    /*----------------------------------------------------------------------------------------------*/
    
    /*----------------------------------------------------------------------------------------------*/
    
    
private:
    std::vector<ShaderProgram> m_programs;
    std::vector<ShaderObject>  m_shaderObjects;
    int  m_majorVersion;
    int  m_minorVersion;
    bool m_initialised;
    
    ShaderProgram m_boundProgram;
};
#endif /* OpenGLContext_hpp */
