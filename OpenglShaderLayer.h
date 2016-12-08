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

// generic includes
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <assert.h>

// platform dependent includes
#ifdef __APPLE__
#include <OpenGL/gl3.h> // includes 3.2 + functionality
#include <OpenGL/gl3ext.h>
#elif _WIN32
#include "GL/glew.h"
#endif

//defines
#define OPENGL_MAJOR_VERSION  4
#define OPENGL_MINOR_VERSION  1
#define OPENGL_MIN_REQUIRED_VERSION_MAJOR 3
#define OPENGL_MIN_REQUIRED_VERSION_MINOR 2
#define OPENGL_INVALID_OBJECT 0

#ifndef GL_CHECK
#ifdef DEBUG
#define GL_CHECK(stmt) do { stmt; checkOpenGLError(#stmt,__FILE__,__LINE__); } while(0)
#else
#define GL_CHECK(stmt) stmt
#endif // DEBUG
#endif // GL_CHECK

namespace gl
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
        friend class OpenglShaderLayer;
        
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
        friend class OpenglShaderLayer;
        
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
    /* OpenglLayer - main class */
    /*----------------------------------------------------------------------------------------------*/
    class OpenglShaderLayer
    {
    public:
        
        /*creation and destruction*/
        /*----------------------------------------------------------------------------------------------*/
        OpenglShaderLayer()
        :
        m_initialised(false),
        m_majorVersion(OPENGL_MAJOR_VERSION),
        m_minorVersion(OPENGL_MAJOR_VERSION)
        {
        }
        
        ~OpenglShaderLayer() {
            dispose();
        }
        
        bool init() {
            if(m_initialised) {
                assert(false && "double init you noob");
                return false;
            }
            
            
            m_initialised = true;
            return m_initialised;
        }
        
        void dispose() {
            if(!m_initialised) {
                return;
            }
            m_initialised = false;
        }
        
        /*----------------------------------------------------------------------------------------------*/
        
        /*Info functions*/
        /*----------------------------------------------------------------------------------------------*/
        constexpr int getMajor() const {
            return m_majorVersion;
        }
        
        constexpr int getMinor() const {
            return m_minorVersion;
        }
        
        std::string getInfo()  const {
            return m_openglLayerInfo;
        }
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
        ShaderProgram createShaderProgram() const {
            ShaderProgram shaderProgram;
            
            GL_CHECK(shaderProgram.m_id = glCreateProgram());
            
            // TODO: add program to program list - used for deleting at dispose time.
            return shaderProgram;
        }
        
        ShaderObject createShaderObject(ShaderObjectType const & type) const {
            ShaderObject shaderObject;
            
            GL_CHECK(shaderObject.m_id = glCreateShader(static_cast<GLenum>(type))); // conversion used to restrist values passed to glCreateShader
            
            return shaderObject;
        }
        
        void attachSourceToShaderObject(ShaderObject & object, std::string const & source) const {
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
        
        void compileShaderObject(ShaderObject & object) const {
            assert(object != OPENGL_INVALID_OBJECT && "the shader object being compiled is in an invald state");
            assert((!(object.m_isCompiled))        && "the shader object being compiled is allready compiled");
            assert(object.m_hasSource              && "the shader object has no source code to compile");
            //assert(shaderObject.type == ShaderObjectType::INVALID); TODO: check this out
            
            GL_CHECK(glCompileShader(object));
            
            GLint isCompiled = GL_FALSE;
            
            GL_CHECK(glGetShaderiv(object,GL_COMPILE_STATUS, &isCompiled));
            
            if(isCompiled == GL_FALSE) {
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
        
        void attachShaderObjectToProgram(ShaderProgram & program, ShaderObject const & object) const {
            assert(program != OPENGL_INVALID_OBJECT && "shader program is in an invalid state");
            assert(object  != OPENGL_INVALID_OBJECT && "shader object is in an invalid state");
            assert(object.m_isCompiled && "shader object is not compiled");
            
            auto find = std::find(program.m_shaderObjects.begin(), program.m_shaderObjects.end(),object);
            
            if(find == program.m_shaderObjects.end()) {
                GL_CHECK(glAttachShader(program, object));
                program.m_shaderObjects.push_back(object);
            } else {
                // object is allready attatched
            }
        }
        
        void detachShaderObjectFromProgram(ShaderProgram & program, ShaderObject const & object) const {
            assert(program != OPENGL_INVALID_OBJECT && "shader program is in an invalid state");
            assert(object  != OPENGL_INVALID_OBJECT && "shader object is in an invalid state");
            
            auto find = std::find(program.m_shaderObjects.begin(), program.m_shaderObjects.end(),object);
            
            if(find != program.m_shaderObjects.end()) {
                GL_CHECK(glDetachShader(program,object));
                program.m_shaderObjects.erase(find);
            } else {
                // program doesnt contain shader object
            }
        }
        
        void detachAllShaderObjectsFromProgram(ShaderProgram & program) const {
            assert(program != OPENGL_INVALID_OBJECT && "shader program is in an invalid state");
            
            for(auto i = program.m_shaderObjects.begin(); i < program.m_shaderObjects.end(); i++) {
                GL_CHECK(glDetachShader(program,i->m_id));
                program.m_shaderObjects.erase(i);
            }
        }
        
        void detachAndDeleteShaderObjectFromProgram(ShaderProgram & program, ShaderObject const & object) const {
            assert(program != OPENGL_INVALID_OBJECT && "shader program is in an invalid state");
            assert(object  != OPENGL_INVALID_OBJECT && "shader object is in an invalid state");
            //TODO: maybe some weird things happening if trying to detach something that isnt attached
            
            auto find = std::find(program.m_shaderObjects.begin(), program.m_shaderObjects.end(),object);
            
            if(find != program.m_shaderObjects.end()) {
                GL_CHECK(glDetachShader(program,object));
                GL_CHECK(glDeleteShader(object));
                program.m_shaderObjects.erase(find);
            } else {
                // program doesnt contain the specified shader object
            }
        }
        
        void detachAndDeleteAllShaderObjectsFromProgram(ShaderProgram & program) const {
            assert(program != OPENGL_INVALID_OBJECT && "shader program is in an invalid state");
            
            for(auto i = program.m_shaderObjects.begin(); i < program.m_shaderObjects.end(); i++) {
                GL_CHECK(glDetachShader(program,i->m_id));
                GL_CHECK(glDeleteShader(i->m_id));
                program.m_shaderObjects.erase(i);
            }
        }
        
        void linkProgram(ShaderProgram & program, bool deleteShaderObjects = true) {
            assert(program != OPENGL_INVALID_OBJECT && "shader program is in an invalid state");
            
            // check to see if program contains any objects
            // if contains object check to see if they are valid
            // if valid link program
            
            if(!program.m_shaderObjects.empty())
            {
                printf("program contains shader objects\n");
                // check if the the shader program contains any valid objects - if it does link
                bool isLinkable = true;
                
                for(ShaderObject & so : program.m_shaderObjects) {
                    if(so == OPENGL_INVALID_OBJECT) {
                        isLinkable = false;
                    }
                }
                
                if(isLinkable) {
                    printf("program doesnt contain any invalid shader objects\n");
                    GL_CHECK(glLinkProgram(program));
                    
                    GLint isLinked = GL_FALSE;
                    
                    GL_CHECK(glGetProgramiv(program,GL_LINK_STATUS,&isLinked));
                    
                    if(isLinked == GL_FALSE) {
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
                    } else { // if link succesfull
                        printf("program link successfull\n");
                        
                        if(deleteShaderObjects) {
                            detachAndDeleteAllShaderObjectsFromProgram(program);
                        } else {
                            detachAllShaderObjectsFromProgram(program);
                            //TODO: cache shader objects
                        }
                    }
                } else {
                    // shader program contains invalid shader objects: TODO: handle shader object destruction
                }
            } else {
                printf("the program contains no shader objects\n");
            }
        }
        
        void deleteShaderProgram(ShaderProgram & program) const {
            assert(program != OPENGL_INVALID_OBJECT && "shader program being deleted is invalid");
            detachAllShaderObjectsFromProgram(program);
            GL_CHECK(glDeleteProgram(program));
            program.m_id = OPENGL_INVALID_OBJECT;
        }
        
        void deleteShaderObject(ShaderObject & object) const {
            assert(object != OPENGL_INVALID_OBJECT && "shader object being deleted is invalid");
            GL_CHECK(glDeleteShader(object));
            object.m_id = OPENGL_INVALID_OBJECT;
        }
        
        void bindShaderProgram(ShaderProgram const & program) {
            assert(program != OPENGL_INVALID_OBJECT && "the program being bound is in an invalid state");
            
            GL_CHECK(glUseProgram(program.m_id));
        }
        /*----------------------------------------------------------------------------------------------*/
        
    private:
        // shader section variables
        //-----------------------------------------------//
        ShaderProgram                        m_boundProgram;
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
        void checkOpenGLError(const char * stmt, const char * fname, int line) const {
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
    };
}
/*----------------------------------------------------------------------------------------------*/
#endif /* OpenglLayer_hpp */
