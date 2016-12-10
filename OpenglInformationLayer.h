//
//  OpenglInformationLayer.h
//  OpenglFramework
//
//  Created by Daniel Collier on 10/12/2016.
//  Copyright © 2016 Daniel Collier. All rights reserved.
//

#ifndef OpenglInformationLayer_h
#define OpenglInformationLayer_h

//generic includes
#include <iostream>

// platform dependent includes
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#elif _WIN32
#include "GL/glew.h"
#endif

// defines
#define INVALID_ID 0
#define TEXTURE_NOT_BOUND 0

#ifndef GL_CHECK
#ifdef DEBUG
#define GL_CHECK(stmt) do { stmt; checkOpenGLError(#stmt,__FILE__,__LINE__); } while(0)
#else
#define GL_CHECK(stmt) stmt
#endif//DEBUG
#endif//GL_CHECK

class OpenglInformationLayer {
    
public:
    
    void init() {
        /* Context information */
        //------------------------------------------------------------------------------------------------------//
        GLint mask;
        glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &mask);
        if((mask & GL_CONTEXT_CORE_PROFILE_BIT) == 0) {
            m_isCoreProfile = false;
            std::cout << "is comp profile" << std::endl;
        } else {
            m_isCoreProfile = true;
        }
        
        GLint flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if((flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT) != 0) {
            m_isProfileForwardCompatible = true;
        } else {
            m_isProfileForwardCompatible = false;
        }
        
        // Check Vendor
        /*
         It could be "ATI Technologies", "NVIDIA Corporation", "INTEL" and so on. Note that there's no guarantee that the string for a specific vendor will remain the same in future
         implementations. On Windows, if it says "Microsoft" then you are using the Windows software renderer or the Windows Direct3D wrapper. You probably haven't installed the graphics
         drivers yet in that case.
         */
        m_vendor = (const char *)glGetString(GL_VENDOR);
        
        //check renderer
        /*
         This string is often the name of the GPU. In the case of Mesa3d, it would be i.e "Gallium 0.4 on NVA8".
         It might even say "Direct3D" if the Windows Direct3D wrapper is being used.
         */
        m_renderer = (const char *)glGetString(GL_RENDERER);
        
        // get context major and minor version
        GL_CHECK(glGetIntegerv(GL_MAJOR_VERSION, &m_majorVersion)); // function works for opengl 3.0 + only
        GL_CHECK(glGetIntegerv(GL_MINOR_VERSION, &m_minorVersion)); // function works for opengl 3.0 + only
        
        
        /* texture information */
        //------------------------------------------------------------------------------------------------------//
        GL_CHECK(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,   &m_maxCombinedTextureImageUnits));
        GL_CHECK(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,            &m_maxTextureImageUnits));
        GL_CHECK(glGetFloatv  (GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,     &m_maxAnisotropy));
        GL_CHECK(glGetIntegerv(GL_MAX_TEXTURE_SIZE,                   &m_maxTextureSize));
        
        
        // build information string
        //------------------------------------------------------------------------------------------------------//
        m_concatedInfo += "--------------------------------------\n";
        m_concatedInfo += "      OpenGL Information Layer        \n";
        m_concatedInfo += "--------------------------------------\n";
        m_concatedInfo += "------------------                    \n";
        m_concatedInfo += "Context Info\n";
        m_concatedInfo += "------------------                    \n";
        m_concatedInfo += "Version:                          " + std::to_string(m_majorVersion) + '.' + std::to_string(m_minorVersion) + '\n';
        m_concatedInfo += "Vendor:                           " + std::string(m_vendor) + '\n';
        m_concatedInfo += "Renderer:                         " + std::string(m_renderer) + "\n";
        m_concatedInfo += "------------------                    \n\n";
        
        
        m_concatedInfo += "------------------                    \n";
        m_concatedInfo += "Texture Info\n";
        m_concatedInfo += "------------------                    \n";
        m_concatedInfo += "Max Texture Size:                 " + std::to_string(m_maxTextureSize) + '\n';
        m_concatedInfo += "Max Texture Image Units:          " + std::to_string(m_maxTextureImageUnits) + '\n';
        m_concatedInfo += "Max CombindedTexture Image Units: " + std::to_string(m_maxCombinedTextureImageUnits) + '\n';
        m_concatedInfo += "Max Anisotropy:                   " + std::to_string(m_maxAnisotropy) + '\n';
        m_concatedInfo += "------------------                    \n\n";
        
        m_concatedInfo += "------------------                    \n";
        m_concatedInfo += "Shader Info\n";
        m_concatedInfo += "------------------                    \n";
        m_concatedInfo += "TODO                                    " + std::to_string(m_maxTextureSize) + '\n';
        m_concatedInfo += "------------------                    \n";
        m_concatedInfo += "--------------------------------------";
    }
    
    void printInfo() {
        std::cout << m_concatedInfo << std::endl;
    }
    
    std::string const & getInfoString() const {
        return m_concatedInfo;
    }
    
private:
    //---------------Context------------------//
    bool         m_isCoreProfile;
    bool         m_isProfileForwardCompatible;
    GLint        m_majorVersion;
    GLint        m_minorVersion;
    const char * m_vendor;
    const char * m_renderer;
    //----------------------------------------//
    
    //---------------Texture------------------//
    GLfloat      m_maxAnisotropy;
    GLint        m_maxTextureImageUnits;
    GLint        m_maxCombinedTextureImageUnits;
    GLint        m_maxTextureSize;
    //----------------------------------------//
    
    std::string m_concatedInfo;
    
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

#endif /* OpenglInformationLayer_h */
