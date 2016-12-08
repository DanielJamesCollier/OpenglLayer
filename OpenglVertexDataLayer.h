//
//  OpenglVertexDataLayer.h
//  OpenglFramework
//
//  Created by Daniel Collier on 08/12/2016.
//  Copyright © 2016 Daniel Collier. All rights reserved.
//

#ifndef OpenglVertexDataLayer_h
#define OpenglVertexDataLayer_h

//generic includes
#include <vector>
#include <iostream>

// platform dependent includes
#ifdef __APPLE__
#include <OpenGL/gl3.h> // includes 3.2 + functionality
#include <OpenGL/gl3ext.h>
#elif _WIN32
#include "GL/glew.h"
#endif /* __APPLE__ */

//defines
#ifndef GL_CHECK
    #ifdef DEBUG
        #define GL_CHECK(stmt) do { stmt; checkOpenGLError(#stmt,__FILE__,__LINE__); } while(0)
    #else
        #define GL_CHECK(stmt) stmt
    #endif /* DEBUG */
#endif /* GL_CHECK */


enum class VertexBufferType {
    STATIC_DRAW   = GL_STATIC_DRAW,
    DYNAMIC_DRAW  = GL_DYNAMIC_DRAW,
};

class VertexArrayObject {
    friend class OpenglVertexDataLayer;
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

class VertexBufferObject {
    friend class OpenglVertexDataLayer;
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

class OpenglVertexDataLayer {
    
public:
    OpenglVertexDataLayer() {
        
    }
    
    VertexBufferObject createVertexBufferObject(VertexBufferType const & type, GLfloat points[], int numberOfPoints) {
        VertexBufferObject vbo;
        
        GL_CHECK(glGenBuffers(1, &vbo.m_id));;
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));
        GL_CHECK(glBufferData(GL_ARRAY_BUFFER, numberOfPoints * sizeof(float), points, static_cast<GLenum>(type)));
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
        
        // m_vertexBuffersObjects.push_back(vbo)
        
        return vbo;
    }
    
    VertexArrayObject createVertexArrayObject() {
        VertexArrayObject vao;
        
        GL_CHECK(glGenVertexArrays(1, &vao.m_id));
        
        //m_vertexArrayObjects.push_back(vao);
        
        return vao;
    }

private:
    std::vector<VertexBufferObject>    m_vertexBuffersObjects;
    std::vector<VertexArrayObject>     m_vertexArrayObjects;
    
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


#endif /* OpenglVertexDataLayer_h */
