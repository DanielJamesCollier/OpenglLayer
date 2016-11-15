//
//  OpenGLContext.cpp
//  OpenGLExamples
//
//  Created by Daniel Collier on 16/10/2016.
//  Copyright © 2016 Daniel Collier. All rights reserved.
//

#include "OpenglLayer.hpp"



#include <iostream>

#ifdef DEBUG
#define GL_CHECK(stmt) do { stmt; checkOpenGLError(#stmt,__FILE__,__LINE__); } while(0)
#else
#define GL_CHECK(stmt) stmt
#endif

/*
  class opengl context
 
    - only one object of this class must exist
 */

//-----------------------------------------------//
OpenglLayer::OpenglLayer()
{
    
}

//-----------------------------------------------//
OpenglLayer::~OpenglLayer()
{
    dispose();
}

//-----------------------------------------------//
void
OpenglLayer::init()
{
    if(m_initialised) return;
    m_initialised = true;
    
}

//-----------------------------------------------//
void
OpenglLayer::dispose()
{
    if(!m_initialised) return;
    m_initialised = false;
    for(auto p : m_programs)
    {
        GL_CHECK(glDeleteProgram(p.id));
    }
    
    for(auto s : m_shaderObjects)
    {
        GL_CHECK(glDeleteShader(s.id));
    }
}

//-----------------------------------------------//
ShaderProgram
OpenglLayer::createShaderProgram() const
{
    ShaderProgram shaderProgram;
    
    GL_CHECK(shaderProgram.id = glCreateProgram());
    
    // TODO: add program to program list - used for deleting at dispose time.
    return shaderProgram;
}

//-----------------------------------------------//
ShaderObject
OpenglLayer::createShaderObject(ShaderObjectType const & type) const
{
    ShaderObject shaderObject;
    
    GL_CHECK(shaderObject.id = glCreateShader(static_cast<GLenum>(type))); // conversion used to restrist values passed to glCreateShader
    
    return shaderObject;
}

//-----------------------------------------------//
void
OpenglLayer::attachShaderObjectToProgram(ShaderProgram const & program, ShaderObject const & object) const
{
    assert(object.isCompiled);
    assert(program.id == 0);
    
    GL_CHECK(glAttachShader(program.id,object.id));
}

//-----------------------------------------------//
void
OpenglLayer::detachShaderObjectFromProgram(ShaderProgram const & program, ShaderObject const & object) const
{
    assert(program.id == 0);
    //TODO: maybe some weird things happening if trying to detach something that isnt attached
    GL_CHECK(glDetachShader(program.id,object.id));
}

//-----------------------------------------------//
void
OpenglLayer::detachAllShaderObjectsFromProgram(ShaderProgram const & program) const
{
    assert(program.id == 0);
    
    if(program.shaderObjects[0] != -1)
    {
        GL_CHECK(glDetachShader(program.id, program.shaderObjects[0]));
    }
    else if(program.shaderObjects[1] != -1)
    {
        GL_CHECK(glDetachShader(program.id, program.shaderObjects[1]));
    }
    else if(program.shaderObjects[2] != -1)
    {
        GL_CHECK(glDetachShader(program.id, program.shaderObjects[2]));
    }
}

//-----------------------------------------------//
void
OpenglLayer::compileShaderObject(ShaderObject & shaderObject) const
{
    assert(shaderObject.isCompiled);
    assert(shaderObject.type == ShaderObjectType::INVALID);
    
    const GLchar* shaderSourceStrings[1];
    GLint shaderSourceStringLengths[1];
    
    shaderSourceStrings[0] = shaderObject.source.c_str();
    shaderSourceStringLengths[0] = static_cast<GLint>(shaderObject.source.length());
    
    GL_CHECK(glShaderSource(shaderObject.id, 1, shaderSourceStrings, shaderSourceStringLengths));
    GL_CHECK(glCompileShader(shaderObject.id));
    
    // check for a sucessful compile
    if (!isShaderObjectOkay(shaderObject, GL_COMPILE_STATUS, "ShaderOBJ: implement shader names probs in a higher abstraction!! - Error: Failed to Compile"))
    {
        shaderObject.isCompiled = false;
        return;
    }
 
    shaderObject.isCompiled = true;
}

//-----------------------------------------------//
bool
OpenglLayer::isShaderProgramOkay(ShaderProgram const & program, GLenum flag, const std::string& errorMessage) const
{
    GLint success = 0;
    GLchar error[1024] = { 0 };
    
    GL_CHECK(glGetProgramiv(program.id, flag, &success));
   
    if (!success)
    {
        GL_CHECK(glGetProgramInfoLog(program.id, sizeof(error), NULL, error));
        std::cerr << errorMessage << error << std::endl;
        return(false);
    }
    return(true);
}

//-----------------------------------------------//
bool
OpenglLayer::isShaderObjectOkay(ShaderObject const & object, GLenum flag, const std::string& errorMessage) const
{
    GLint success = 0;
    GLchar error[1024] = { 0 };
    
    GL_CHECK(glGetShaderiv(object.id, flag, &success));
    
    if (!success)
    {
        GL_CHECK(glGetShaderInfoLog(object.id, sizeof(error), NULL, error));
        std::cerr << errorMessage << error << std::endl;
        return(false);
    }
    return(true);
}

//-----------------------------------------------//
void
OpenglLayer::deleteShaderProgram(ShaderProgram const & program) const
{
    assert(program.id == 1);
    GL_CHECK(glDeleteProgram(program.id));
}

//-----------------------------------------------//
void
OpenglLayer::deleteShaderObject(ShaderObject const & object) const
{
    assert(object.id == 1);
    GL_CHECK(glDeleteShader(object.id));
}

//-----------------------------------------------//
void OpenglLayer::linkProgram(ShaderProgram & program) const
{
    assert(program.linked);
    
    
    // TODO: there are only some ways there can be one shader object attatched. most of the time there has to be two. this if statement must account for that.
    if(program.shaderObjects[0] != 0 ||
       program.shaderObjects[1] != 0 ||
       program.shaderObjects[2] != 0)
    {
        GL_CHECK(glLinkProgram(program.id));
        
        if (!isShaderProgramOkay(program, GL_LINK_STATUS, "Program: " + std::to_string(program.id)))
        {
            program.linked = false;
            detachAllShaderObjectsFromProgram(program);
            return;
        }
        
        detachAllShaderObjectsFromProgram(program);
        
        program.linked = true;
        
        return;
    }
   
}

//-----------------------------------------------//
void
OpenglLayer::checkOpenGLError(const char * stmt, const char * fname, int line) const
{
    GLenum err;
    
    while((err = glGetError()) != GL_NO_ERROR)
    {
        std::string errorType;
        
        switch(err)
        {
            case GL_INVALID_OPERATION:
            {
                errorType = "INVALID_OPERATION";
                break;
            }
            case GL_INVALID_ENUM:
            {
                errorType = "INVALID_ENUM";
                break;
            }
                
            case GL_INVALID_VALUE:
            {
               errorType = "INVALID_VALUE";
               break;
            }
            case GL_OUT_OF_MEMORY:
            {
                errorType = "OUT_OF_MEMORY";
                break;
            }
            case GL_INVALID_FRAMEBUFFER_OPERATION:
            {
                 errorType = "INVALID_FRAMEBUFFER_OPERATION";
                 break;
            }
            default:
            {
                errorType = "UKNOWN ERROR";
                break;
            }
        }
        
        std::cerr << "-----------------------------------------------------------------------------\nOPENGL ERROR: "<< errorType
                  << "\nFilename: " << fname
                  << "\nline: " << line
                  << "\nerror on: " << stmt
                  << "\n-----------------------------------------------------------------------------"
                  << std::endl;
    }
}
