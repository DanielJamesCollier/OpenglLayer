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

#ifdef __APPLE__
#include <AGL/agl.h>
#elif _WIN32
#   include <Wingdi.h>
#endif

namespace gl
{
    /*
     an opengl context must be created before using this class
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
        
#if 0 // thinking about addid context creation within this init
#if _WIN32
        //https://msdn.microsoft.com/en-us/library/windows/desktop/dd374383(v=vs.85).aspx
        
        HGLRC context = wglGetCurrentContext();
        if(context != NULL)
        {
            m_initialised = false;
            return true;
        }
        
        m_initialised = false;
        return false;
#endif
#endif
        
        
        // get info
        {
            m_openglLayerInfo = R"(-----------------------------
            -      OPENGL LAYER INFO     -
            -----------------------------
            
            
            -----------------------------)";
        }
        
        m_initialised = true;
        return true;
    }
    
    //-----------------------------------------------//
    void
    OpenglLayer::dispose()
    {
        if(!m_initialised) return;
        m_initialised = false;
        
        //    for(auto program : m_programs)
        //    {
        //        GL_CHECK(glDeleteProgram(program));
        //    }
        //
        //    for(auto shaderObject : m_shaderObjects)
        //    {
        //        GL_CHECK(glDeleteShader(shaderObject));
        //    }
        //
        //    for(auto vbo : m_vertexBuffersObjects)
        //    {
        //        GL_CHECK(glDeleteBuffers(1, &vbo.id));
        //    }
        //
        //    for(auto vao : m_vertexArrayObjects)
        //    {
        //        GL_CHECK(glDeleteVertexArrays(1, &vao.id));
        //    }
        
        //TODO: opengl context destruction will go here
    }
    
    //-----------------------------------------------//
    constexpr int
    OpenglLayer::getMajor() const
    {
        return(m_majorVersion);
    }
    
    //-----------------------------------------------//
    constexpr int
    OpenglLayer::getMinor() const
    {
        return(m_minorVersion);
    }
    
    //-----------------------------------------------//
    constexpr int
    OpenglLayer::getMinimumRequiredVersionMajor() const
    {
        return OPENGL_MIN_REQUIRED_VERSION_MAJOR;
    }
    
    //-----------------------------------------------//
    constexpr int
    OpenglLayer::getMinimumRequiredVersionMinor() const
    {
        return OPENGL_MIN_REQUIRED_VERSION_MINOR;
    }
    
    //-----------------------------------------------//
    std::string
    OpenglLayer::getInfo() const
    {
        return ""; //TODO - returns major - minor - renderer..... print out all max values ... max texture units ect..
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
        
        GL_CHECK(shaderProgram.m_id = glCreateProgram());
        
        // TODO: add program to program list - used for deleting at dispose time.
        return shaderProgram;
    }
    
    //-----------------------------------------------//
    ShaderObject
    OpenglLayer::createShaderObject(ShaderObjectType const & type) const
    {
        ShaderObject shaderObject;
        
        GL_CHECK(shaderObject.m_id = glCreateShader(static_cast<GLenum>(type))); // conversion used to restrist values passed to glCreateShader
        
        return shaderObject;
    }
    
    //-----------------------------------------------//
    void
    OpenglLayer::attachSourceToShaderObject(ShaderObject & object, std::string const & source) const
    {
        // calling this multiple times in consession will overide the privious attached source - concatinate all source before calling this function
        assert(object != OPENGL_INVALID_OBJECT && "shader object is in invalid state - the shader object must be created with createShaderObject()");
        assert(!(source.empty()) && "source is empty");
        
        const GLchar* shaderSourceStrings[1];
        GLint shaderSourceStringLengths[1];
        
        shaderSourceStrings[0] = source.c_str();
        shaderSourceStringLengths[0] = static_cast<GLint>(source.length());
        
        GL_CHECK(glShaderSource(object, 1, shaderSourceStrings, shaderSourceStringLengths));
        
        object.m_hasSource = true;
    }
    
    //-----------------------------------------------//
    void
    OpenglLayer::attachShaderObjectToProgram(ShaderProgram & program, ShaderObject const & object) const
    {
        assert(program != OPENGL_INVALID_OBJECT && "shader program is in an invalid state");
        assert(object  != OPENGL_INVALID_OBJECT && "shader object is in an invalid state");
        assert(object.m_isCompiled && "shader object is not compiled");
        
        auto find = std::find(program.m_shaderObjects.begin(), program.m_shaderObjects.end(),object);
        
        if(find == program.m_shaderObjects.end())
        {
            GL_CHECK(glAttachShader(program, object));
            program.m_shaderObjects.push_back(object);
        }
        else
        {
            // object is allready attatched
        }
    }
    
    //-----------------------------------------------//
    void
    OpenglLayer::detachShaderObjectFromProgram(ShaderProgram & program, ShaderObject const & object) const
    {
        assert(program != OPENGL_INVALID_OBJECT && "shader program is in an invalid state");
        assert(object  != OPENGL_INVALID_OBJECT && "shader object is in an invalid state");
        
        auto find = std::find(program.m_shaderObjects.begin(), program.m_shaderObjects.end(),object);
        
        if(find != program.m_shaderObjects.end())
        {
            GL_CHECK(glDetachShader(program,object));
            program.m_shaderObjects.erase(find);
        }
        else
        {
            // program doesnt contain shader object
        }
        
    }
    
    //-----------------------------------------------//
    void
    OpenglLayer::detachAllShaderObjectsFromProgram(ShaderProgram & program) const
    {
        assert(program != OPENGL_INVALID_OBJECT && "shader program is in an invalid state");
        
        for(auto i = program.m_shaderObjects.begin(); i < program.m_shaderObjects.end(); i++)
        {
            GL_CHECK(glDetachShader(program,i->m_id));
            program.m_shaderObjects.erase(i);
        }
    }
    
    //-----------------------------------------------//
    void
    OpenglLayer::detachAndDeleteShaderObjectFromProgram(ShaderProgram & program, ShaderObject const & object) const
    {
        assert(program != OPENGL_INVALID_OBJECT && "shader program is in an invalid state");
        assert(object  != OPENGL_INVALID_OBJECT && "shader object is in an invalid state");
        //TODO: maybe some weird things happening if trying to detach something that isnt attached
        
        auto find = std::find(program.m_shaderObjects.begin(), program.m_shaderObjects.end(),object);
        
        if(find != program.m_shaderObjects.end())
        {
            GL_CHECK(glDetachShader(program,object));
            GL_CHECK(glDeleteShader(object));
            program.m_shaderObjects.erase(find);
        }
        else
        {
            // program doesnt contain the specified shader object
        }
    }
    
    //-----------------------------------------------//
    void
    OpenglLayer::detachAndDeleteAllShaderObjectsFromProgram(ShaderProgram & program) const
    {
        assert(program != OPENGL_INVALID_OBJECT && "shader program is in an invalid state");
        
        for(auto i = program.m_shaderObjects.begin(); i < program.m_shaderObjects.end(); i++)
        {
            GL_CHECK(glDetachShader(program,i->m_id));
            GL_CHECK(glDeleteShader(i->m_id));
            program.m_shaderObjects.erase(i);
        }
    }
    
    //-----------------------------------------------//
    void
    OpenglLayer::compileShaderObject(ShaderObject & object) const
    {
        assert(object != OPENGL_INVALID_OBJECT && "the shader object being compiled is in an invald state");
        assert((!(object.m_isCompiled)) && "the shader object being compiled is allready compiled");
        assert(object.m_hasSource && "the shader object has no source code to compile");
        //assert(shaderObject.type == ShaderObjectType::INVALID); TODO: check this out
        
        GL_CHECK(glCompileShader(object));
        
        GLint isCompiled = GL_FALSE;
        
        GL_CHECK(glGetShaderiv(object,GL_COMPILE_STATUS, &isCompiled));
        
        if(isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            GL_CHECK(glGetShaderiv(object, GL_INFO_LOG_LENGTH, &maxLength)); // get the length of the error
            GLchar error[maxLength];  //The maxLength includes the NULL character
            
            GL_CHECK(glGetShaderInfoLog(object, maxLength, &maxLength, error)); // populate the error array with the error string
            
            printf("%s", error);
            
            GL_CHECK(glDeleteShader(object)); // Don't leak the shader.
            
            object.m_isCompiled = false;
            
            return;
            
        }
        object.m_isCompiled = true;
    }
    
    //-----------------------------------------------//
    void
    OpenglLayer::linkProgram(ShaderProgram & program, bool deleteShaderObjects)
    {
        assert(program != OPENGL_INVALID_OBJECT && "shader program is in an invalid state");
        
        // check to see if program contains any objects
        // if contains object check to see if they are valid
        // if valid link program
        
        if(!program.m_shaderObjects.empty())
        {
            printf("program contains shader objects\n");
            // check if the the shader program contains any valid objects - if it does link
            bool isLinkable = true;
            
            for(ShaderObject & so : program.m_shaderObjects)
            {
                if(so == OPENGL_INVALID_OBJECT)
                {
                    isLinkable = false;
                }
            }
            
            if(isLinkable)
            {
                printf("program doesnt contain any invalid shader objects\n");
                GL_CHECK(glLinkProgram(program));
                
                GLint isLinked = GL_FALSE;
                
                GL_CHECK(glGetProgramiv(program,GL_LINK_STATUS,&isLinked));
                
                if(isLinked == GL_FALSE)
                {
                    printf("program link failed");
                    
                    GLint maxLength = 0;
                    GL_CHECK(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength));
                    GLchar error[maxLength]; //The maxLength includes the NULL character
                    
                    GL_CHECK(glGetProgramInfoLog(program, maxLength, &maxLength, error));
                    
                    std::cout << error << std::endl;;
                    
                    deleteShaderProgram(program);
                    
                    
                    
                    //Provide the infolog in whatever manner you deem best.
                    //Exit with failure. TODO: exit with failure or error code?
                    return;
                }
                else // if link succesfull
                {
                    printf("program link successfull\n");
                    
                    if(deleteShaderObjects)
                    {
                        detachAndDeleteAllShaderObjectsFromProgram(program);
                    }
                    else
                    {
                        detachAllShaderObjectsFromProgram(program);
                        //TODO: cache shader objects
                    }
                }
            }
            else
            {
                // shader program contains invalid shader objects: TODO: handle shader object destruction
            }
        }
        else
        {
            printf("the program contains no shader objects\n");
        }
    }
    
    //-----------------------------------------------//
    void
    OpenglLayer::deleteShaderProgram(ShaderProgram & program) const
    {
        assert(program != OPENGL_INVALID_OBJECT && "shader program being deleted is invalid");
        detachAllShaderObjectsFromProgram(program);
        GL_CHECK(glDeleteProgram(program));
        program.m_id = OPENGL_INVALID_OBJECT;
    }
    
    //-----------------------------------------------//
    void
    OpenglLayer::deleteShaderObject(ShaderObject & object) const
    {
        assert(object != OPENGL_INVALID_OBJECT && "shader object being deleted is invalid");
        GL_CHECK(glDeleteShader(object));
        object.m_id = OPENGL_INVALID_OBJECT;
    }
    
    //-----------------------------------------------//
    void
    OpenglLayer::bindShaderProgram(ShaderProgram const & program)
    {
        assert(program != OPENGL_INVALID_OBJECT && "the program being bound is in an invalid state");
        
        GL_CHECK(glUseProgram(program.m_id));
    }
    
    //-----------------------------------------------//
    //    GLuint OpenglLayer::ConvertIndexToTextureTarget(GLuint index) const
    //    {
    //        switch (index)
    //        {
    //            case 0:
    //                return GL_TEXTURE_1D;
    //            case 1:
    //                return GL_TEXTURE_2D;
    //            case 2:
    //                return GL_TEXTURE_3D;
    //            case 3:
    //                return GL_TEXTURE_1D_ARRAY;
    //            case 4:
    //                return GL_TEXTURE_2D_ARRAY;
    //            case 5:
    //                return GL_TEXTURE_RECTANGLE;
    //            case 6:
    //                return GL_TEXTURE_CUBE_MAP;
    //            case 7:
    //                return GL_TEXTURE_CUBE_MAP_ARRAY;
    //            case 8:
    //                return GL_TEXTURE_BUFFER;
    //            case 9:
    //                return GL_TEXTURE_2D_MULTISAMPLE;
    //            case 10:
    //                return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
    //            default:
    //                printf("TextureBinderError - Error: invalid index");
    //                return 11; // to chang
    //        }
    //    }
    //
    //    //-----------------------------------------------//
    //    GLuint OpenglLayer::ConvertTextureTargetToIndex(TextureTarget const & target) const
    //    {
    //
    //        GLenum tar = static_cast<GLenum>(target);
    //
    //        switch (tar)
    //        {
    //            case GL_TEXTURE_1D:
    //                return 0;
    //            case GL_TEXTURE_2D:
    //                return 1;
    //            case GL_TEXTURE_3D:
    //                return 2;
    //            case GL_TEXTURE_1D_ARRAY:
    //                return 3;
    //            case GL_TEXTURE_2D_ARRAY:
    //                return 4;
    //            case GL_TEXTURE_RECTANGLE:
    //                return 5;
    //            case GL_TEXTURE_CUBE_MAP:
    //                return 6;
    //            case GL_TEXTURE_CUBE_MAP_ARRAY:
    //                return 7;
    //            case GL_TEXTURE_BUFFER:
    //                return 8;
    //            case GL_TEXTURE_2D_MULTISAMPLE:
    //                return 9;
    //            case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
    //                return 10;
    //            default:
    //                printf("TextureBinderError - Error: invalid target");
    //
    //                return GL_INVALID_OPERATION;
    //        }
    //    }
    
    //-----------------------------------------------//
    //    void
    //    OpenglLayer::bindTexture(GLuint unit, Texture const & texture)
    //    {
    //        assert(unit <= m_maxCombinedTextureImageUnits);
    //
    //        GLuint targetIndex = ConvertTextureTargetToIndex(texture.m_target);
    //
    //        // check if the texture trying to be bound is allready bound
    //        if(m_boundTextures.at(unit).at(targetIndex) == texture.m_id)
    //        {
    //            return;
    //        }
    //        else
    //        {
    //            if(m_activeUnit != unit)
    //            {
    //                GL_CHECK(glActiveTexture(GL_TEXTURE0 + unit));
    //                m_activeUnit = unit;
    //            }
    //
    //            GL_CHECK(glBindTexture(static_cast<GLenum>(texture.m_target), texture.m_id));
    //        }
    //    }
    
    //-----------------------------------------------//
    VertexBufferObject
    OpenglLayer::createVertexBufferObject(VertexBufferType const & type, GLfloat points[], int numberOfPoints)
    {
        VertexBufferObject vbo;
        
        GL_CHECK(glGenBuffers(1, &vbo.m_id));;
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, numberOfPoints * sizeof(float), points, static_cast<GLenum>(type)));
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
        
        // m_vertexBuffersObjects.push_back(vbo)
        
        return vbo;
    }
    
    //-----------------------------------------------//
    VertexArrayObject
    OpenglLayer::createVertexArrayObject()
    {
        VertexArrayObject vao;
        
        GL_CHECK(glGenVertexArrays(1, &vao.m_id));
        
        
        //m_vertexArrayObjects.push_back(vao);
        
        return vao;
    }
}

