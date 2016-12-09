//
//  OpenglDrawLayer.h
//  OpenglFramework
//
//  Created by Daniel Collier on 08/12/2016.
//  Copyright © 2016 Daniel Collier. All rights reserved.
//

#ifndef OpenglDrawLayer_h
#define OpenglDrawLayer_h

//general includes
#include <vector>

// platform dependent includes
#ifdef __APPLE__
#include <OpenGL/gl3.h> // includes 3.2 + functionality
#include <OpenGL/gl3ext.h>
#elif _WIN32
#include "GL/glew.h"
#endif

// defines
#ifndef GL_CHECK
#ifdef DEBUG
#define GL_CHECK(stmt) do { stmt; checkOpenGLError(#stmt,__FILE__,__LINE__); } while(0)
#else
#define GL_CHECK(stmt) stmt
#endif//DEBUG
#endif//GL_CHECK

/*
 TODO list
 
 - add a deferred path
 - add a forward path
 */

#include "OpenglVertexDataLayer.h"
#include "OpenglTextureLayer.h"
#include "OpenglShaderLayer.h"

namespace glLayer {
    
    class OpenglDrawCommand {
        friend class OpenglDrawLayer;
    public:
        OpenglDrawCommand(ShaderProgram const & program, Texture const & texture, VertexArrayObject const & vao)
        :
        m_program(program)
        , m_texture(texture)
        , m_vao(vao)
        {
        }
    private:
        ShaderProgram m_program;
        Texture m_texture;
        GLuint        m_vao;
    };
    
    class OpenglDrawLayer {
        
    public:
        
        OpenglDrawLayer() {
            
        }
        
        void addDrawCommad(OpenglDrawCommand const & command) {
            m_commands.push_back(command);
        }
        
        void processDrawCommands() {
            sortCommandsByVaoAndThenTexture();
        }
        
        void clearDrawCommands() {
            m_commands.clear();
        }
        
    private:
        std::vector<OpenglDrawCommand> m_commands;
        
        void sortCommandsByVaoAndThenTexture() {
            if(m_commands.size() == 0 || m_commands.size() == 1) {
                return;
            }
        }
        
        void sortCommandsByTextureAndThenVao() {
            if(m_commands.size() == 0 || m_commands.size() == 1) {
                return;
            }
        }
        
        void bindShaderProgram(ShaderProgram const & program) {
            
        }
        
        void bindTexture(GLuint unit, Texture const & texture) {
            GL_CHECK(glActiveTexture(GL_TEXTURE0 + unit));
            GL_CHECK(glBindTexture(static_cast<GLenum>(texture.m_target), texture.m_id));
        }
        
        void bindVertexArrayObject() {
            
        }
        
        void checkOpenGLError(const char * stmt, const char * fname, int line) const {
            // TODO: add assert functionality
            GLenum err;
            
            while((err = glGetError()) != GL_NO_ERROR) {
                std::string errorType;
                
                switch(err) {
                    case GL_INVALID_OPERATION:             errorType = "INVALID_OPERATION";             break;
                    case GL_INVALID_ENUM:                  errorType = "INVALID_ENUM";                  break;
                    case GL_INVALID_VALUE:                 errorType = "INVALID_VALUE";                 break;
                    case GL_OUT_OF_MEMORY:                 errorType = "OUT_OF_MEMORY";                 break;
                    case GL_INVALID_FRAMEBUFFER_OPERATION: errorType = "INVALID_FRAMEBUFFER_OPERATION"; break;
                    default:                               errorType = "UKNOWN ERROR - FUCK SAKE";      break;
                }
                
                std::cerr << "-----------------------------------------------------------------------------\nOPENGL ERROR: "<< errorType
                << "\nFilename: " << fname
                << "\nline: " << line
                << "\nerror on: " << stmt
                << "\n-----------------------------------------------------------------------------"
                << std::endl;
            }
        }
    };
}
#endif /* OpenglDrawLayer_h */
