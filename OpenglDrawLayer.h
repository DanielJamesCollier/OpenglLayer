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
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#elif _WIN32
#include "GL/glew.h"
#endif

//local includes
#include "OpenglVertexDataLayer.h"
#include "OpenglTextureLayer.h"
#include "OpenglShaderLayer.h"

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

namespace glLayer {
    
    enum class DrawType {
        TRIANGLES      = GL_TRIANGLES,
        TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
        POINTS         = GL_POINTS,
    };
    
    class DrawCommand {
        friend class OpenglDrawLayer;
    public:
        DrawCommand(ShaderProgram const & program, Texture const & texture, VertexArrayObject const & vao, DrawType const & drawType = DrawType::TRIANGLES, bool wireFrame = false)
        :
        m_program(program)
        , m_texture(texture)
        , m_vao(vao)
        , m_drawType(drawType)
        , m_wireFrame(wireFrame)
        {
        }
    private:
        ShaderProgram m_program;
        Texture       m_texture;
        GLuint        m_vao;
        DrawType      m_drawType;
        bool          m_wireFrame;
    };
    
    class OpenglDrawLayer {
        
    public:
        
        OpenglDrawLayer()
        : m_boundShaderProgram(OPENGL_INVALID_OBJECT)
        {
        }
        
        void addDrawCommad(DrawCommand const & command) {
            m_commands.push_back(command);
        }
        
        float r,g,b;
        void processDrawCommands() {
            sortCommandsByVaoAndThenTexture();
            
            r+= 0.001f;
            g+= 0.01f;
            b+= 0.0001f;
            glClearColor(sin(r), sin(g),sin(b), 1);
            glClear(GL_COLOR_BUFFER_BIT);
            
            
            for(auto & command: m_commands) {
                bindShaderProgram(command.m_program);
                bindTexture(0, command.m_texture);
                
                //TODO: remove this branch in future
                if(!command.m_wireFrame) {
                    glDrawArrays(static_cast<GLenum>(command.m_drawType), 0, 3);
                } else {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glDrawArrays(static_cast<GLenum>(command.m_drawType), 0, 3);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);
                }
                
            }
        }
        
        void clearDrawCommands() {
            m_commands.clear();
        }
        
    private:
        std::vector<DrawCommand> m_commands;
        GLuint                   m_boundShaderProgram;
        
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
            if(m_boundShaderProgram == program.m_id) {
                return;
            } else {
                GL_CHECK(glUseProgram(program.m_id));
                m_boundShaderProgram = program.m_id;
            }
        }
        
        void unbindShaderProgram() {
            GL_CHECK(glUseProgram(0));
        }
        
        void bindTexture(GLuint unit, Texture const & texture) {
            GL_CHECK(glActiveTexture(GL_TEXTURE0 + unit));
            GL_CHECK(glBindTexture(static_cast<GLenum>(texture.m_target), texture.m_id));
        }
        
        void bindVertexArrayObject(VertexArrayObject const & vao) {
            GL_CHECK(glBindVertexArray(vao.m_id));
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
