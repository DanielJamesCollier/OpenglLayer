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
#include <assert.h>
#include <vector>

#ifdef __APPLE__
    #include <OpenGL/GL.h>
#elif _WIN32
    #include "GL/glew.h"
#endif

struct ShaderProgram
{
    GLuint id = 0;
    std::array<int, 3> shaderObjects = {0,0,0}; // TODO: review this - sets a max limit on numeber of shaderObjects per shader program ??
    bool linked = false;
};

enum class ShaderObjectType
{
    VERTEX_SHADER   = GL_VERTEX_SHADER,
    FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
    INVALID         = GL_INVALID_VALUE
};


struct ShaderObject
{
    std::string source{""}; // TODO: move into other location to redude shader object size, messes with the alignment
    GLuint id = 0;
    ShaderObjectType type = ShaderObjectType::INVALID;
    bool isCompiled = false;
}; // contains padding


class OpenglLayer
{
public:
    
     OpenglLayer();
    ~OpenglLayer();
    
    void init();
    void dispose();
    

    void checkOpenGLError(const char * stmt, const char * fname, int line) const;

    
    /*
     create a shader progarm
     create a shader object
     compile shader object
     attach objects to program
     link program
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
    /*----------------------------------------------------------------------------------------------*/
   
    bool dan();
    
private:

    std::vector<ShaderProgram> m_programs;
    std::vector<ShaderObject> m_shaderObjects;
   
    
    int m_majorVersion;
    int m_minorVersion;
    
    bool m_initialised;
    
   // copy constructor
    
};
#endif /* OpenGLContext_hpp */
