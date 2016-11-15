//
//  OpenGLContext.cpp
//  OpenGLExamples
//
//  Created by Daniel Collier on 16/10/2016.
//  Copyright © 2016 Daniel Collier. All rights reserved.
//

#include "OpenglLayer.hpp"
#include <iostream>
#include <assert.h>

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
:
m_initialised(false),
m_majorVersion(OPENGL_MAJOR_VERSION),
m_minorVersion(OPENGL_MAJOR_VERSION)
{
}

//-----------------------------------------------//
OpenglLayer::~OpenglLayer()
{
    dispose();
}

//-----------------------------------------------//
bool
OpenglLayer::init()
{
    if(m_initialised) return true;
    m_initialised = true;
    
    // TODO: opengl context creation will go here - return false on fail
    
    return true;
}

//-----------------------------------------------//
void
OpenglLayer::dispose()
{
    if(!m_initialised) return;
    m_initialised = false;
    
    for(auto p : m_programs)
    {
        GL_CHECK(glDeleteProgram(p));
    }
    
    for(auto s : m_shaderObjects)
    {
        GL_CHECK(glDeleteShader(s));
    }
    
    //TODO: opengl context destruction will go here
}

//-----------------------------------------------//
int
OpenglLayer::getMajor() const
{
    return(m_majorVersion);
}

//-----------------------------------------------//
int
OpenglLayer::getMinor() const
{
    return(m_minorVersion);
}

//-----------------------------------------------//
std::string
OpenglLayer::getInfo() const
{
    return ""; //TODO - returns major - minor - renderer.....
}

//-----------------------------------------------//
void
OpenglLayer::checkOpenGLError(const char * stmt, const char * fname, int line) const
{
    // TODO: add assert functionality
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
    assert(program != OPENGL_INVALID_OBJECT && "shader program is in an invalid state");
    assert(object  != OPENGL_INVALID_OBJECT && "shader object is in an invalid state");
    assert(object.isCompiled && "shader object is not compiled");
    
    GL_CHECK(glAttachShader(program, object));
}

//-----------------------------------------------//
void
OpenglLayer::detachShaderObjectFromProgram(ShaderProgram const & program, ShaderObject const & object) const
{
    assert(program != OPENGL_INVALID_OBJECT && "shader program is in an invalid state");
    assert(object  != OPENGL_INVALID_OBJECT && "shader object is in an invalid state");
    //TODO: maybe some weird things happening if trying to detach something that isnt attached
    GL_CHECK(glDetachShader(program,object));
}

//-----------------------------------------------//
void
OpenglLayer::detachAllShaderObjectsFromProgram(ShaderProgram const & program) const
{
    assert(program != OPENGL_INVALID_OBJECT && "shader program is in an invalid state");
    
    if(program.shaderObjects[0] != OPENGL_INVALID_OBJECT)
    {
        GL_CHECK(glDetachShader(program, program.shaderObjects[0]));
    }
    else if(program.shaderObjects[1] != OPENGL_INVALID_OBJECT)
    {
        GL_CHECK(glDetachShader(program, program.shaderObjects[1]));
    }
    else if(program.shaderObjects[2] != OPENGL_INVALID_OBJECT)
    {
        GL_CHECK(glDetachShader(program, program.shaderObjects[2]));
    }
}

//-----------------------------------------------//
void
OpenglLayer::compileShaderObject(ShaderObject & shaderObject) const
{
    assert(shaderObject != OPENGL_INVALID_OBJECT && "the shader object being compiled is in an invald state");
    assert(shaderObject.isCompiled && "the shader object being compiled is allready compiled");
    //assert(shaderObject.type == ShaderObjectType::INVALID); TODO: check this out
    
    const GLchar* shaderSourceStrings[1];
    GLint shaderSourceStringLengths[1];
    
    shaderSourceStrings[0] = shaderObject.source.c_str();
    shaderSourceStringLengths[0] = static_cast<GLint>(shaderObject.source.length());
    
    GL_CHECK(glShaderSource(shaderObject, 1, shaderSourceStrings, shaderSourceStringLengths));
    GL_CHECK(glCompileShader(shaderObject));
    
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
    assert(program != OPENGL_INVALID_OBJECT && "shader program is in an invalid state");
    assert(flag == GL_LINK_STATUS && "invalid flag"); // limit flags
    
    GLint success = 0;
    GLchar error[1024] = { 0 };
    
    GL_CHECK(glGetProgramiv(program, flag, &success));
    
    if (!success)
    {
        GL_CHECK(glGetProgramInfoLog(program, sizeof(error), NULL, error));
        std::cerr << errorMessage << error << std::endl;
        return(false);
    }
    return(true);
}

//-----------------------------------------------//
bool
OpenglLayer::isShaderObjectOkay(ShaderObject const & object, GLenum flag, const std::string& errorMessage) const
{
    assert(object != OPENGL_INVALID_OBJECT && "shader object is in an invald state");
    assert(flag == GL_LINK_STATUS && "invalid flag"); // limit flags
    
    GLint success = 0;
    GLchar error[1024] = { 0 };
    
    GL_CHECK(glGetShaderiv(object, flag, &success));
    
    if (!success)
    {
        GL_CHECK(glGetShaderInfoLog(object, sizeof(error), NULL, error));
        std::cerr << errorMessage << error << std::endl;
        return(false);
    }
    return(true);
}

//-----------------------------------------------//
void
OpenglLayer::deleteShaderProgram(ShaderProgram const & program) const
{
    assert(program != OPENGL_INVALID_OBJECT && "shader program being deleted is invalid");
    GL_CHECK(glDeleteProgram(program));
}

//-----------------------------------------------//
void
OpenglLayer::deleteShaderObject(ShaderObject const & object) const
{
    assert(object != OPENGL_INVALID_OBJECT && "shader object being deleted is invalid");
    GL_CHECK(glDeleteShader(object));
}

//-----------------------------------------------//
void
OpenglLayer::linkProgram(ShaderProgram & program) const
{
    assert(program != OPENGL_INVALID_OBJECT && "shader program is in an invalid state");
    
    // TODO: there are only some ways there can be one shader object attatched. most of the time there has to be two. this if statement must account for that.
    if(program.shaderObjects[0] != OPENGL_INVALID_OBJECT ||
       program.shaderObjects[1] != OPENGL_INVALID_OBJECT ||
       program.shaderObjects[2] != OPENGL_INVALID_OBJECT)
    {
        GL_CHECK(glLinkProgram(program));
        
        if (!isShaderProgramOkay(program, GL_LINK_STATUS, "Program: " + std::to_string(program)))
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
OpenglLayer::bindShaderProgram(ShaderProgram const & program)
{
    assert(program != OPENGL_INVALID_OBJECT && "the program being bound is in an invalid state");
    
    if(m_boundProgram != program)
    {
        GL_CHECK(glUseProgram(program));
    }
}
