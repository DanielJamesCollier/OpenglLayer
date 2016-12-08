//
//  OpenglTextureLayer.h
//  OpenglFramework
//
//  Created by Daniel Collier on 05/12/2016.
//  Copyright © 2016 Daniel Collier. All rights reserved.
//

#ifndef OpenglTextureLayer_h
#define OpenglTextureLayer_h


#ifdef __APPLE__
#include <OpenGL/gl3.h> // includes 3.2 + functionality
#include <OpenGL/gl3ext.h>
#elif _WIN32
#include "GL/glew.h"
#endif

#include <vector>
#include <type_traits>
#include <assert.h>
#include <exception>

#define INVALID_ID 0
#define TEXTURE_NOT_BOUND 0

#ifdef DEBUG
#define GL_CHECK(stmt) do { stmt; checkOpenGLError(#stmt,__FILE__,__LINE__); } while(0)
#else
#define GL_CHECK(stmt) stmt
#endif

#define ANISOTROPIC_FILTER
//#undef ANISOTROPIC_FILTER // disable anisotropic filtering - some platforms do not support it

/*
 //TODO//
 - add a default texture
 - if pixel vector passed in to create texture function is .empty() then assign default texture
 - make functions pass by ref
 - make mipmapping an ifdef
 - add glTexStorage functions
 - add asserts to check for textures power of two
 - make singleton
 - add DSA - or add the option to not track binds internally and bind every time
 */


enum class TexturePixelFormat {
    // normal colour formats
    RGB                          = GL_RGB,
    RGB8                         = GL_RGB8,
    RGBA                         = GL_RGBA,
    //reverse formats
    BGR                          = GL_BGR,
    BGRA                         = GL_BGRA,
    // special colour formats
    R3_G3_B2                     = GL_R3_G3_B2,
    RGB5_A1                      = GL_RGB5_A1,
    RGB10_A2                     = GL_RGB10_A2,
    RGB10_A2UI                   = GL_RGB10_A2UI,
    R11F_G11F_B10F               = GL_R11F_G11F_B10F,
    RGB9_E5                      = GL_RGB9_E5,
    // sRGB colour space
    SRGB8                        = GL_SRGB8,
    SRGB8_ALPHA8                 = GL_SRGB8_ALPHA8,
    // compressed formats
    // TODO
};

/* filter used when no mip maps are generated */
//-----------------------------------------------//
enum class TextureFilterNormal {
    LINEAR                       = GL_LINEAR,
    NEAREST                      = GL_NEAREST,
};
//-----------------------------------------------//

/* filter used when mipmaps are generated */
//-----------------------------------------------//
enum class TextureFilterMipmap {
    LINEAR_MIPMAP_LINEAR         = GL_LINEAR_MIPMAP_LINEAR,
    LINEAR_MIPMAP_NEAREST        = GL_LINEAR_MIPMAP_NEAREST,
    NEAREST_MIPMAP_NEAREST       = GL_NEAREST_MIPMAP_NEAREST,
    NEAREST_MIPMAP_LINEAR        = GL_NEAREST_MIPMAP_LINEAR,
};
//-----------------------------------------------//

enum class TextureWrapMode {
    REPEAT                       = GL_REPEAT,
    MIRRORED_REPEAT              = GL_MIRRORED_REPEAT,
    CLAMP_TO_EDGE                = GL_CLAMP_TO_EDGE,
    CLAMP_TO_BORDER              = GL_CLAMP_TO_BORDER,
};

class Texture1D {
    friend class OpenglTextureLayer;
public:
    bool operator==(Texture1D const & rhs) {
        return(this->id == rhs.id);
    }
private:
    GLuint          id = INVALID_ID;
    GLuint unitBoundTo = TEXTURE_NOT_BOUND;
};

class Texture2D {
    friend class OpenglTextureLayer;
public:
    bool operator==(Texture2D const & rhs) {
        return(this->id == rhs.id);
    }
private:
    GLuint          id = INVALID_ID;
    GLuint unitBoundTo = TEXTURE_NOT_BOUND;
};

class Texture3D {
    friend class OpenglTextureLayer;
public:
    bool operator==(Texture3D const & rhs) {
        return(this->id == rhs.id);
    }
private:
    GLuint          id = INVALID_ID;
    GLuint unitBoundTo = TEXTURE_NOT_BOUND;
};

class Texture1DArray {
    friend class OpenglTextureLayer;
public:
    bool operator==(Texture1DArray const & rhs) {
        return(this->id == rhs.id);
    }
private:
    GLuint          id = INVALID_ID;
    GLuint unitBoundTo = TEXTURE_NOT_BOUND;
};

class Texture2DArray {
    friend class OpenglTextureLayer;
public:
    bool operator==(Texture2DArray const & rhs) {
        return(this->id == rhs.id);
    }
private:
    GLuint          id = INVALID_ID;
    GLuint unitBoundTo = TEXTURE_NOT_BOUND;
};

class TextureCubemap {
    friend class OpenglTextureLayer;
public:
    bool operator==(TextureCubemap const & rhs) {
        return(this->id == rhs.id);
    }
private:
    GLuint          id = INVALID_ID;
    GLuint unitBoundTo = TEXTURE_NOT_BOUND;
};



class OpenglTextureLayer {
public:
    
    OpenglTextureLayer() {
        
    }
    
    ~OpenglTextureLayer()
    {
        dispose();
    }
    
    bool init() {
        if(m_init) {
            return true;
        }
        m_init = true;
        
        GL_CHECK(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,   &m_maxCombinedTextureImageUnits));
        GL_CHECK(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,            &m_maxTextureImageUnits));
        GL_CHECK(glGetFloatv  (GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,     &m_maxAnisotropy));
        GL_CHECK(glGetIntegerv(GL_MAX_TEXTURE_SIZE,                   &m_maxTextureSize));

        if(m_maxCombinedTextureImageUnits == -1 || m_maxTextureImageUnits == -1) {
            return false;
        }
        
        try {
            m_allTexture1D.reserve(m_maxCombinedTextureImageUnits); // could have more textures than bindable so reserve not resize
            m_allBoundTexture1D.resize(m_maxCombinedTextureImageUnits);  // only a specific bindable amount of textures
            
            m_allTexture2D.reserve(m_maxCombinedTextureImageUnits); // could have more textures than bindable so reserve not resize
            m_allBoundTexture2D.resize(m_maxCombinedTextureImageUnits);  // only a specific bindable amount of textures
            
            m_allTexture3D.reserve(m_maxCombinedTextureImageUnits); // could have more textures than bindable so reserve not resize
            m_allBoundTexture3D.resize(m_maxCombinedTextureImageUnits);  // only a specific bindable amount of textures
            
            m_allTexture1DArray.reserve(m_maxCombinedTextureImageUnits); // could have more textures than bindable so reserve not resize
            m_allBoundTexture1DArray.resize(m_maxCombinedTextureImageUnits);  // only a specific bindable amount of textures
            
            m_allTexture2DArray.reserve(m_maxCombinedTextureImageUnits); // could have more textures than bindable so reserve not resize
            m_allBoundTexture2DArray.resize(m_maxCombinedTextureImageUnits);  // only a specific bindable amount of textures
            
            m_allTextureCubemap.reserve(m_maxCombinedTextureImageUnits); // could have more textures than bindable so reserve not resize
            m_allBoundTextureCubemap.resize(m_maxCombinedTextureImageUnits);  // only a specific bindable amount of textures
        } catch (...) {
            return false;
        }
      
        // set active unit to 0 - used for avoiding activating the same unit > 1 times in a row
        m_activeUnit = 0;
        GL_CHECK(glActiveTexture(GL_TEXTURE0 + m_activeUnit));
        //
        
        return true;
    };
    
    void dispose() {
        if(!m_init) {
            return;
        }
        m_init = false;
        
        // delete all texture 1D
        for(auto & tex : m_allTexture1D) {
            if(tex.id != INVALID_ID) {
                GL_CHECK(glDeleteTextures(1, &tex.id));
            }
            m_allTexture1D.clear();
        }
        //...
        
        // delete all texture 2D
        for(auto & tex : m_allTexture2D) {
            if(tex.id != INVALID_ID) {
                GL_CHECK(glDeleteTextures(1, &tex.id));
            }
            m_allTexture2D.clear();
        }
        //...
        
        // delete all texture 3D
        for(auto & tex : m_allTexture3D) {
            if(tex.id != INVALID_ID) {
                GL_CHECK(glDeleteTextures(1, &tex.id));
            }
            m_allTexture3D.clear();
        }
        //...
        
        // delete all texture 1D ARRAY
        for(auto & tex : m_allTexture1DArray) {
            if(tex.id != INVALID_ID) {
                GL_CHECK(glDeleteTextures(1, &tex.id));
            }
            m_allTexture1DArray.clear();
        }
        //...
        
        // delete all texture 2D ARRAY
        for(auto & tex : m_allTexture2DArray) {
            if(tex.id != INVALID_ID) {
                GL_CHECK(glDeleteTextures(1, &tex.id));
            }
            m_allTexture2DArray.clear();
        }
        //...
        
        // delete all texture Cubemap
        for(auto & tex : m_allTextureCubemap) {
            if(tex.id != INVALID_ID) {
                GL_CHECK(glDeleteTextures(1, &tex.id));
            }
            m_allTextureCubemap.clear();
        }
        //...
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
    
    // texture 1D //
    //------------------------------------------------------------------------------------------------------------------------------------//
    // mippmapped by default
    template<typename pixelDataType>
    Texture1D createTexture1D(std::vector<pixelDataType> pixels, GLuint width, TexturePixelFormat pFormat, TextureWrapMode s) {
        static_assert(std::is_same<pixelDataType,           int>::value       ||
                      std::is_same<pixelDataType,         float>::value       ||
                      std::is_same<pixelDataType,        double>::value       ||
                      std::is_same<pixelDataType,         short>::value       ||
                      std::is_same<pixelDataType, unsigned char>::value
                      , "unsupported pixel data type");
         assert(width <= m_maxTextureSize);
        
        // Create and bind the texture for editing
        Texture1D tex1d;
        GL_CHECK(glGenTextures(1, &tex1d.id));
        GL_CHECK(glBindTexture(GL_TEXTURE_1D, tex1d.id));
        //...
        
        //upload texture data - glFunction parameters depend this->function params
        GLenum pixelFormat = static_cast<GLenum>(pFormat);
        if(std::is_same<pixelDataType, int>::value) {
            glTexImage1D(GL_TEXTURE_1D, 0, pixelFormat, width, 0, pixelFormat, GL_INT, &pixels);
        } else if(std::is_same<pixelDataType, float>::value) {
            glTexImage1D(GL_TEXTURE_1D, 0, pixelFormat, width, 0, pixelFormat, GL_FLOAT, &pixels);
        } else if (std::is_same<pixelDataType, double>::value) {
            glTexImage1D(GL_TEXTURE_1D, 0, pixelFormat, width, 0, pixelFormat, GL_DOUBLE, &pixels);
        } else if(std::is_same<pixelDataType, short>::value) {
            glTexImage1D(GL_TEXTURE_1D, 0, pixelFormat, width, 0, pixelFormat, GL_SHORT, &pixels);
        } else if(std::is_same<pixelDataType, unsigned char>::value) {
            glTexImage1D(GL_TEXTURE_1D, 0, pixelFormat, width, 0, pixelFormat, GL_UNSIGNED_BYTE, &pixels);

        }
        //...
        
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_1D)); // after pixel data is uploaded to the GPU generate the mipmaps
        
        // set texture parameters - (min & mag filer, wrap function, anisotropic filtering)
        GL_CHECK(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(TextureFilterMipmap::LINEAR_MIPMAP_LINEAR)));
        GL_CHECK(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(TextureFilterNormal::LINEAR)));
        GL_CHECK(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S,     static_cast<GLenum>(s)));
#ifdef ANISOTROPIC_FILTER
        GL_CHECK(glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_maxAnisotropy));
#endif
        //...
        
        GL_CHECK(glBindTexture(GL_TEXTURE_1D, 0)); // unbind after finish editing
        
        m_allTexture1D.push_back(tex1d); // add to array of all texture2d for tracking lifetimes
        
        return tex1d;
    }
    
    void deleteTexture1D(Texture1D & texture) {
        assert(texture.id != INVALID_ID);
        
        unbindTexture1D(texture); // unbind before delete
        
        GL_CHECK(glDeleteTextures(1, &texture.id)); // delete texture from gpu
        texture.id = INVALID_ID;
        
        // search and remove the texture from the texture store - search will never equal.end() .... probably..... fails if this is called on a texture not created with createTexture2D
        auto textureSearch = std::find(m_allTexture1D.begin(), m_allTexture1D.end(), texture);
        m_allTexture1D.erase(textureSearch);
    }
    
    void bindTexture1D(GLuint unit, Texture1D & texture) {
        assert(texture.id != INVALID_ID || unit > (GL_TEXTURE0 + unit));
        
        if(m_allBoundTexture1D.at(unit) != texture.id) {
            if(m_activeUnit != unit) {
                GL_CHECK(glActiveTexture(GL_TEXTURE0 + unit));
                m_activeUnit = unit;
                std::cout << "activate unit" << std::endl;
                
            }
            std::cout << "bound " << std::endl;
            glBindTexture(GL_TEXTURE_1D, texture.id);
            m_allBoundTexture1D.at(unit) = texture.id;
            texture.unitBoundTo = unit;
        }
    }
    
    void unbindTexture1D(Texture1D & texture)
    {
        assert(texture.id != INVALID_ID || texture.unitBoundTo != TEXTURE_NOT_BOUND);
        
        if(m_activeUnit != texture.unitBoundTo){
            GL_CHECK(glActiveTexture(GL_TEXTURE0 + texture.unitBoundTo));
            m_activeUnit = texture.unitBoundTo;
        }
        GL_CHECK(glBindTexture(GL_TEXTURE_1D, 0));
        m_allBoundTexture1D.at(texture.unitBoundTo) = TEXTURE_NOT_BOUND;
        texture.unitBoundTo = TEXTURE_NOT_BOUND;
    }
    //------------------------------------------------------------------------------------------------------------------------------------//
    
    
    // texture 2D //
    //------------------------------------------------------------------------------------------------------------------------------------//
    // mippmapped by default
    template<typename pixelDataType>
    Texture2D createTexture2D(std::vector<pixelDataType> pixels, GLuint width, GLuint height, TexturePixelFormat pFormat, TextureWrapMode s, TextureWrapMode t) {
        static_assert(std::is_same<pixelDataType,           int>::value ||
                      std::is_same<pixelDataType,         float>::value ||
                      std::is_same<pixelDataType,        double>::value ||
                      std::is_same<pixelDataType,         short>::value ||
                      std::is_same<pixelDataType, unsigned char>::value
                      , "unsupported pixel data type");
         assert(width <= m_maxTextureSize && height <= m_maxTextureSize);
        
        // Create and bind the texture for editing
        Texture2D tex2d;
        GL_CHECK(glGenTextures(1, &tex2d.id));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, tex2d.id));
        //...
        
        //upload texture data - glFunction parameters depend this->function params
        GLenum pixelFormat = static_cast<GLenum>(pFormat);
        if(std::is_same<pixelDataType, int>::value) {
             glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, width, height, 0, pixelFormat, GL_INT, &pixels);
        } else if(std::is_same<pixelDataType, float>::value) {
            glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, width, height, 0, pixelFormat, GL_FLOAT, &pixels);
        } else if (std::is_same<pixelDataType, double>::value) {
             glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, width, height, 0, pixelFormat, GL_DOUBLE, &pixels);
        } else if(std::is_same<pixelDataType, short>::value) {
             glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, width, height, 0, pixelFormat, GL_SHORT, &pixels);
        } else if(std::is_same<pixelDataType, unsigned char>::value) {
             glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, width, height, 0, pixelFormat, GL_UNSIGNED_BYTE, &pixels);
        }
        //...
        
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));  // after pixel data is uploaded to the GPU generate the mipmaps
        
        // set texture parameters - (min & mag filer, wrap function, anisotropic filtering)
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(TextureFilterMipmap::LINEAR_MIPMAP_LINEAR)));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(TextureFilterNormal::LINEAR)));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     static_cast<GLenum>(s)));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     static_cast<GLenum>(t)));
#ifdef ANISOTROPIC_FILTER
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_maxAnisotropy));
#endif
        //...
        
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0)); // unbind after finish editing
        
        m_allTexture2D.push_back(tex2d); // add to array of all texture2d for tracking lifetimes
        
        return tex2d;
    }
    
    void deleteTexture2D(Texture2D & texture) {
        assert(texture.id != INVALID_ID);
        
        unbindTexture2D(texture); // unbind before delete
        
        GL_CHECK(glDeleteTextures(1, &texture.id)); // delete texture from gpu
        texture.id = INVALID_ID;
        
        // search and remove the texture from the texture store - search will never equal.end() .... probably..... fails if this is called on a texture not created with createTexture2D
        auto textureSearch = std::find(m_allTexture2D.begin(), m_allTexture2D.end(), texture);
        m_allTexture2D.erase(textureSearch);
        
      
    }
    
    void bindTexture2D(GLuint unit, Texture2D & texture) {
        assert(texture.id != INVALID_ID || unit > (GL_TEXTURE0 + unit));
        
        if(m_allBoundTexture2D.at(unit) != texture.id) {
            if(m_activeUnit != unit) {
                GL_CHECK(glActiveTexture(GL_TEXTURE0 + unit));
                m_activeUnit = unit;
                std::cout << "activate unit" << std::endl;
               
            }
            std::cout << "bound " << std::endl;
            glBindTexture(GL_TEXTURE_2D, texture.id);
            m_allBoundTexture2D.at(unit) = texture.id;
            texture.unitBoundTo = unit;
        }
    }
    
    void unbindTexture2D(Texture2D & texture)
    {
        assert(texture.id != INVALID_ID || texture.unitBoundTo != TEXTURE_NOT_BOUND);
        
        if(m_activeUnit != texture.unitBoundTo){
            GL_CHECK(glActiveTexture(GL_TEXTURE0 + texture.unitBoundTo));
            m_activeUnit = texture.unitBoundTo;
        }
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
        m_allBoundTexture2D.at(texture.unitBoundTo) = TEXTURE_NOT_BOUND;
        texture.unitBoundTo = TEXTURE_NOT_BOUND;
    }
    //------------------------------------------------------------------------------------------------------------------------------------//
    
    // texture 3D //
    //------------------------------------------------------------------------------------------------------------------------------------//
    // mippmapped by default
    template<typename pixelDataType>
    Texture3D createTexture3D(std::vector<pixelDataType> pixels, GLuint width, GLuint height, TexturePixelFormat pFormat, TextureWrapMode s, TextureWrapMode t, TextureWrapMode r) {
        static_assert(std::is_same<pixelDataType,           int>::value ||
                      std::is_same<pixelDataType,         float>::value ||
                      std::is_same<pixelDataType,        double>::value ||
                      std::is_same<pixelDataType,         short>::value ||
                      std::is_same<pixelDataType, unsigned char>::value
                      , "unsupported pixel data type");
        assert(width <= m_maxTextureSize && height <= m_maxTextureSize);
        
        // Create and bind the texture for editing
        Texture3D tex3d;
        GL_CHECK(glGenTextures(1, &tex3d.id));
        GL_CHECK(glBindTexture(GL_TEXTURE_3D, tex3d.id));
        //...
        
        //upload texture data - glFunction parameters depend this->function params
        GLenum pixelFormat = static_cast<GLenum>(pFormat);
        if(std::is_same<pixelDataType, int>::value) {
            glTexImage3D(GL_TEXTURE_3D, 0, pixelFormat, width, height, 0, pixelFormat, GL_INT, &pixels);
        } else if(std::is_same<pixelDataType, float>::value) {
            glTexImage3D(GL_TEXTURE_3D, 0, pixelFormat, width, height, 0, pixelFormat, GL_FLOAT, &pixels);
        } else if (std::is_same<pixelDataType, double>::value) {
            glTexImage3D(GL_TEXTURE_3D, 0, pixelFormat, width, height, 0, pixelFormat, GL_DOUBLE, &pixels);
        } else if(std::is_same<pixelDataType, short>::value) {
            glTexImage3D(GL_TEXTURE_3D, 0, pixelFormat, width, height, 0, pixelFormat, GL_SHORT, &pixels);
        } else if(std::is_same<pixelDataType, unsigned char>::value) {
            glTexImage3D(GL_TEXTURE_3D, 0, pixelFormat, width, height, 0, pixelFormat, GL_UNSIGNED_BYTE, &pixels);
        }
        //...
        
         GL_CHECK(glGenerateMipmap(GL_TEXTURE_3D)); // after pixel data is uploaded to the GPU generate the mipmaps
        
        // set texture parameters - (min & mag filer, wrap function, anisotropic filtering)
        GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(TextureFilterMipmap::LINEAR_MIPMAP_LINEAR)));
        GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(TextureFilterNormal::LINEAR)));
        GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S,     static_cast<GLenum>(s)));
        GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T,     static_cast<GLenum>(t)));
        GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T,     static_cast<GLenum>(r)));
#ifdef ANISOTROPIC_FILTER
        GL_CHECK(glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_maxAnisotropy));
#endif
        //...
        
        GL_CHECK(glBindTexture(GL_TEXTURE_3D, 0)); // unbind after finish editing
        
        m_allTexture3D.push_back(tex3d); // add to array of all texture2d for tracking lifetimes
        
        return tex3d;
    }
    
    void deleteTexture3D(Texture3D & texture) {
        assert(texture.id != INVALID_ID);
        
        unbindTexture3D(texture); // unbind before delete
        
        GL_CHECK(glDeleteTextures(1, &texture.id)); // delete texture from gpu
        texture.id = INVALID_ID;
        
        // search and remove the texture from the texture store - search will never equal.end() .... probably..... fails if this is called on a texture not created with createTexture2D
        auto textureSearch = std::find(m_allTexture3D.begin(), m_allTexture3D.end(), texture);
        m_allTexture3D.erase(textureSearch);
    }
    
    void bindTexture3D(GLuint unit, Texture3D & texture) {
        assert(texture.id != INVALID_ID || unit > (GL_TEXTURE0 + unit));
        
        if(m_allBoundTexture3D.at(unit) != texture.id) {
            if(m_activeUnit != unit) {
                GL_CHECK(glActiveTexture(GL_TEXTURE0 + unit));
                m_activeUnit = unit;
                std::cout << "activate unit" << std::endl;
                
            }
            std::cout << "bound " << std::endl;
            glBindTexture(GL_TEXTURE_3D, texture.id);
            m_allBoundTexture3D.at(unit) = texture.id;
            texture.unitBoundTo = unit;
        }
    }
    
    void unbindTexture3D(Texture3D & texture)
    {
        assert(texture.id != INVALID_ID || texture.unitBoundTo != TEXTURE_NOT_BOUND);
        
        if(m_activeUnit != texture.unitBoundTo){
            GL_CHECK(glActiveTexture(GL_TEXTURE0 + texture.unitBoundTo));
            m_activeUnit = texture.unitBoundTo;
        }
        GL_CHECK(glBindTexture(GL_TEXTURE_3D, 0));
        m_allBoundTexture3D.at(texture.unitBoundTo) = TEXTURE_NOT_BOUND;
        texture.unitBoundTo = TEXTURE_NOT_BOUND;
    }
    //------------------------------------------------------------------------------------------------------------------------------------//
    
    // texture 1D Array//
    //------------------------------------------------------------------------------------------------------------------------------------//
    // mippmapped by default
    template<typename pixelDataType>
    Texture1DArray createTexture1DArray(std::vector<std::vector<pixelDataType>> textureArray, GLuint width, TexturePixelFormat pFormat, TextureWrapMode s) {
        static_assert(std::is_same<pixelDataType,           int>::value ||
                      std::is_same<pixelDataType,         float>::value ||
                      std::is_same<pixelDataType,        double>::value ||
                      std::is_same<pixelDataType,         short>::value ||
                      std::is_same<pixelDataType, unsigned char>::value
                      , "unsupported pixel data type");
        assert(width <= m_maxTextureSize);
        
        // Create and bind the texture for editing
        Texture1DArray tex1d;
        GL_CHECK(glGenTextures(1, &tex1d.id));
        GL_CHECK(glBindTexture(GL_TEXTURE_1D, tex1d.id));
        //...
        
        GLenum pixelFormat = static_cast<GLenum>(pFormat); // convert pFormat to opengl type
        GLsizei mipLevels  = static_cast<GLsizei>(1 + floor(log2(width))); // calculates the number of mipmap levels needed for the given texture
        
        //             target             , mipCount, type     , width, height, array size
        glTexStorage2D(GL_TEXTURE_1D_ARRAY, mipLevels, pixelFormat, width, textureArray.size()); // allocate memory on the GPU for the GL_TEXTURE_2D_ARRAY
        
        auto uploadTextures = [&](auto pixDataType) -> void {
            int i = 0;
            for(auto & texture : textureArray) {
                glTexSubImage2D(GL_TEXTURE_1D_ARRAY, 0, 0, 0, i++, width, 1, pixelFormat, pixDataType, &textureArray[0]);
            }
        };
        
        //upload texture data - glFunction parameters depend this->function params
        
        if(std::is_same<pixelDataType, int>::value) {
            uploadTextures(GL_INT);
        } else if(std::is_same<pixelDataType, float>::value) {
            uploadTextures(GL_FLOAT);
        } else if (std::is_same<pixelDataType, double>::value) {
            uploadTextures(GL_DOUBLE);
        } else if(std::is_same<pixelDataType, short>::value) {
            uploadTextures(GL_SHORT);
        } else if(std::is_same<pixelDataType, unsigned char>::value) {
            uploadTextures(GL_UNSIGNED_BYTE);
        }
        //...
        
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_1D)); // after pixel data is uploaded to the GPU generate the mipmaps
        
        // set texture parameters - (min & mag filer, wrap function, anisotropic filtering)
        GL_CHECK(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(TextureFilterMipmap::LINEAR_MIPMAP_LINEAR)));
        GL_CHECK(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(TextureFilterNormal::LINEAR)));
        GL_CHECK(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S,     static_cast<GLenum>(s)));
#ifdef ANISOTROPIC_FILTER
        GL_CHECK(glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_maxAnisotropy));
#endif
        //...
        
        GL_CHECK(glBindTexture(GL_TEXTURE_1D, 0)); // unbind after finish editing
        
        m_allTexture1DArray.push_back(tex1d); // add to array of all texture2d for tracking lifetimes
        
        return tex1d;
    }
    
    void deleteTexture1DArray(Texture1DArray & texture) {
        assert(texture.id != INVALID_ID);
        
        unbindTexture1DArray(texture); // unbind before delete
        
        GL_CHECK(glDeleteTextures(1, &texture.id)); // delete texture from gpu
        texture.id = INVALID_ID;
        
        // search and remove the texture from the texture store - search will never equal.end() .... probably..... fails if this is called on a texture not created with createTexture2D
        auto textureSearch = std::find(m_allTexture1DArray.begin(), m_allTexture1DArray.end(), texture);
        m_allTexture1DArray.erase(textureSearch);
    }
    
    void bindTexture1DArray(GLuint unit, Texture1DArray & texture) {
        assert(texture.id != INVALID_ID || unit > (GL_TEXTURE0 + unit));
        
        if(m_allBoundTexture1DArray.at(unit) != texture.id) {
            if(m_activeUnit != unit) {
                GL_CHECK(glActiveTexture(GL_TEXTURE0 + unit));
                m_activeUnit = unit;
                std::cout << "activate unit" << std::endl;
            }
            std::cout << "bound " << std::endl;
            glBindTexture(GL_TEXTURE_1D, texture.id);
            m_allBoundTexture1DArray.at(unit) = texture.id;
            texture.unitBoundTo = unit;
        }
    }
    
    void unbindTexture1DArray(Texture1DArray & texture)
    {
        assert(texture.id != INVALID_ID || texture.unitBoundTo != TEXTURE_NOT_BOUND);
        
        if(m_activeUnit != texture.unitBoundTo){
            GL_CHECK(glActiveTexture(GL_TEXTURE0 + texture.unitBoundTo));
            m_activeUnit = texture.unitBoundTo;
        }
        GL_CHECK(glBindTexture(GL_TEXTURE_1D, 0));
        m_allBoundTexture1D.at(texture.unitBoundTo) = TEXTURE_NOT_BOUND;
        texture.unitBoundTo = TEXTURE_NOT_BOUND;
    }
    //------------------------------------------------------------------------------------------------------------------------------------//
    
    // texture 2D Array//
    //------------------------------------------------------------------------------------------------------------------------------------//
    // mippmapped by default
    template<typename pixelDataType>
    Texture2DArray createTexture2DArray(std::vector<std::vector<pixelDataType>> textureArray, GLuint width, GLuint height, TexturePixelFormat pFormat, TextureWrapMode s, TextureWrapMode t) {
        static_assert(std::is_same<pixelDataType,           int>::value ||
                      std::is_same<pixelDataType,         float>::value ||
                      std::is_same<pixelDataType,        double>::value ||
                      std::is_same<pixelDataType,         short>::value ||
                      std::is_same<pixelDataType, unsigned char>::value
                      , "unsupported pixel data type");
        assert(width <= m_maxTextureSize && height <= m_maxTextureSize);
        
        // Create and bind the texture for editing
        Texture2DArray tex1d;
        GL_CHECK(glGenTextures(1, &tex1d.id));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D_ARRAY, tex1d.id));
        //...
        
        GLenum  pixelFormat = static_cast<GLenum>(pFormat); // convert pFormat to opengl type
        GLsizei mipLevels   = static_cast<GLsizei>(1 + floor(log2(std::max(width, height)))); // calculates the number of mipmap levels needed for the given texture
        
        //             target             , mipCount, type     , width, height, array size
        glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevels, pixelFormat, width, height, textureArray.size()); // allocate memory on the GPU for the GL_TEXTURE_2D_ARRAY
        
        auto uploadTextures = [&](auto pixDataType) -> void {
            int i = 0;
            for(auto & texture : textureArray) {
                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i++, width, height, 1, pixelFormat, pixDataType, &texture[0]);
            }
        };
        
        if(std::is_same<pixelDataType, int>::value) {
            uploadTextures(GL_INT);
        } else if(std::is_same<pixelDataType, float>::value) {
            uploadTextures(GL_FLOAT);
        } else if (std::is_same<pixelDataType, double>::value) {
            uploadTextures(GL_DOUBLE);
        } else if(std::is_same<pixelDataType, short>::value) {
            uploadTextures(GL_SHORT);
        } else if(std::is_same<pixelDataType, unsigned char>::value) {
            uploadTextures(GL_UNSIGNED_BYTE);
        }
        //...
        
        GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D_ARRAY));  // after pixel data is uploaded to the GPU generate the mipmaps
        
        // set texture parameters - (min & mag filer, wrap function, anisotropic filtering)
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(TextureFilterMipmap::LINEAR_MIPMAP_LINEAR)));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(TextureFilterNormal::LINEAR)));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S,     static_cast<GLenum>(s)));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S,     static_cast<GLenum>(t)));
#ifdef ANISOTROPIC_FILTER
        GL_CHECK(glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_maxAnisotropy));
#endif
        //...
        
        GL_CHECK(glBindTexture(GL_TEXTURE_2D_ARRAY, 0)); // unbind after finish editing
        
        m_allTexture2DArray.push_back(tex1d); // add to array of all texture2d for tracking lifetimes
        
        return tex1d;
    }
    
    void deleteTexture2DArray(Texture1D & texture) {
        assert(texture.id != INVALID_ID);
        
        unbindTexture1D(texture); // unbind before delete
        
        GL_CHECK(glDeleteTextures(1, &texture.id)); // delete texture from gpu
        texture.id = INVALID_ID;
        
        // search and remove the texture from the texture store - search will never equal.end() .... probably..... fails if this is called on a texture not created with createTexture2D
        auto textureSearch = std::find(m_allTexture1D.begin(), m_allTexture1D.end(), texture);
        m_allTexture1D.erase(textureSearch);
    }
    
    void bindTexture2DArray(GLuint unit, Texture1D & texture) {
        assert(texture.id != INVALID_ID || unit > (GL_TEXTURE0 + unit));
        
        if(m_allBoundTexture1D.at(unit) != texture.id) {
            if(m_activeUnit != unit) {
                GL_CHECK(glActiveTexture(GL_TEXTURE0 + unit));
                m_activeUnit = unit;
            }
            glBindTexture(GL_TEXTURE_1D, texture.id);
            m_allBoundTexture1D.at(unit) = texture.id;
            texture.unitBoundTo = unit;
        }
    }
    
    void unbindTexture2DArray(Texture1D & texture)
    {
        assert(texture.id != INVALID_ID || texture.unitBoundTo != TEXTURE_NOT_BOUND);
        
        if(m_activeUnit != texture.unitBoundTo){
            GL_CHECK(glActiveTexture(GL_TEXTURE0 + texture.unitBoundTo));
            m_activeUnit = texture.unitBoundTo;
        }
        GL_CHECK(glBindTexture(GL_TEXTURE_1D, 0));
        m_allBoundTexture1D.at(texture.unitBoundTo) = TEXTURE_NOT_BOUND;
        texture.unitBoundTo = TEXTURE_NOT_BOUND;
    }
    //------------------------------------------------------------------------------------------------------------------------------------//

    
    // texture cubemap //
    //------------------------------------------------------------------------------------------------------------------------------------//
    // mippmapped by default
    template<typename pixelDataType>
    TextureCubemap createTextureCubemap(std::vector<pixelDataType> front,
                                        std::vector<pixelDataType> back,
                                        std::vector<pixelDataType> top,
                                        std::vector<pixelDataType> bottom,
                                        std::vector<pixelDataType> left,
                                        std::vector<pixelDataType> right,
                                        GLuint                     size,
                                        TexturePixelFormat pFormat) {
        static_assert(std::is_same<pixelDataType,           int>::value ||
                      std::is_same<pixelDataType,         float>::value ||
                      std::is_same<pixelDataType,        double>::value ||
                      std::is_same<pixelDataType,         short>::value ||
                      std::is_same<pixelDataType, unsigned char>::value
                      , "unsupported pixel data type");
        assert(size <= m_maxTextureSize);
        
        // Create and bind the texture for editing
        TextureCubemap texCubemap;
        GL_CHECK(glGenTextures(1, &texCubemap.id));
        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, texCubemap.id));
        //...
        
        // set the texture wraping mode
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, static_cast<GLenum>(TextureWrapMode::CLAMP_TO_EDGE));
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, static_cast<GLenum>(TextureWrapMode::CLAMP_TO_EDGE));
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, static_cast<GLenum>(TextureWrapMode::CLAMP_TO_EDGE));
        //...
    
        auto uploadCubemapTextures = [&](auto pixDataType) -> void {
            GLenum pixelFormat = static_cast<GLenum>(pFormat);
            GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, pixelFormat, size, size, 0, pixelFormat, pixDataType, &front[0]));  // front
            GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, pixelFormat, size, size, 0, pixelFormat, pixDataType, &back[0]));   // back
            GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, pixelFormat, size, size, 0, pixelFormat, pixDataType, &top[0]));    // top
            GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, pixelFormat, size, size, 0, pixelFormat, pixDataType, &bottom[0])); // bottom
            GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, pixelFormat, size, size, 0, pixelFormat, pixDataType, &left[0]));   // left
            GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, pixelFormat, size, size, 0, pixelFormat, pixDataType, &right[0]));  // right
        };
        
        //upload texture data - glFunction parameters depend this->function params
        if(std::is_same<pixelDataType, int>::value) {
            uploadCubemapTextures(GL_INT);
        } else if(std::is_same<pixelDataType, float>::value) {
            uploadCubemapTextures(GL_FLOAT);
        } else if (std::is_same<pixelDataType, double>::value) {
            uploadCubemapTextures(GL_DOUBLE);
        } else if(std::is_same<pixelDataType, short>::value) {
            uploadCubemapTextures(GL_SHORT);
        } else if(std::is_same<pixelDataType, unsigned char>::value) {
            uploadCubemapTextures(GL_UNSIGNED_BYTE);
        }
        //...
        
         GL_CHECK(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));  // after pixel data is uploaded to the GPU generate the mipmaps
        
        // set texture parameters - (min & mag filer, wrap function, anisotropic filtering)
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(TextureFilterMipmap::LINEAR_MIPMAP_LINEAR)));
        GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(TextureFilterNormal::LINEAR)));
#ifdef ANISOTROPIC_FILTER
        GL_CHECK(glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_maxAnisotropy));
#endif
        //
        
        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0)); // unbind after finish editing
        
        m_allTextureCubemap.push_back(texCubemap); // add to array of all texture2d for tracking lifetimes
        
        return texCubemap;
    }
    
    void deleteTextureCubemap(TextureCubemap & texture) {
        assert(texture.id != INVALID_ID);
        
        unbindTextureCubemap(texture); // unbind before delete
        
        GL_CHECK(glDeleteTextures(1, &texture.id)); // delete texture from gpu
        texture.id = INVALID_ID;
        
        // search and remove the texture from the texture store - search will never equal.end() .... probably..... fails if this is called on a texture not created with createTexture2D
        auto textureSearch = std::find(m_allTextureCubemap.begin(), m_allTextureCubemap.end(), texture);
        m_allTextureCubemap.erase(textureSearch);
    }
    
    void bindTextureCubemap(GLuint unit, TextureCubemap & texture) {
        assert(texture.id != INVALID_ID || unit > (GL_TEXTURE0 + unit));
        
        if(m_allBoundTextureCubemap.at(unit) != texture.id) {
            if(m_activeUnit != unit) {
                GL_CHECK(glActiveTexture(GL_TEXTURE0 + unit));
                m_activeUnit = unit;
                std::cout << "activate unit" << std::endl;
                
            }
            std::cout << "bound " << std::endl;
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);
            m_allBoundTextureCubemap.at(unit) = texture.id;
            texture.unitBoundTo = unit;
        }
    }
    
    void unbindTextureCubemap(TextureCubemap & texture)
    {
        assert(texture.id != INVALID_ID || texture.unitBoundTo != TEXTURE_NOT_BOUND);
        
        if(m_activeUnit != texture.unitBoundTo){
            GL_CHECK(glActiveTexture(GL_TEXTURE0 + texture.unitBoundTo));
            m_activeUnit = texture.unitBoundTo;
        }
        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
        m_allBoundTexture3D.at(texture.unitBoundTo) = TEXTURE_NOT_BOUND;
        texture.unitBoundTo = TEXTURE_NOT_BOUND;
    }
    //------------------------------------------------------------------------------------------------------------------------------------//

    
private:
    std::vector<Texture1D>       m_allTexture1D;
    std::vector<GLuint>          m_allBoundTexture1D;
    std::vector<Texture2D>       m_allTexture2D;
    std::vector<GLuint>          m_allBoundTexture2D; // size == max units
    std::vector<Texture3D>       m_allTexture3D;
    std::vector<GLuint>          m_allBoundTexture3D;
    std::vector<Texture1DArray>  m_allTexture1DArray;
    std::vector<GLuint>          m_allBoundTexture1DArray;
    std::vector<Texture2DArray>  m_allTexture2DArray;
    std::vector<GLuint>          m_allBoundTexture2DArray;
    std::vector<TextureCubemap>  m_allTextureCubemap;
    std::vector<GLuint>          m_allBoundTextureCubemap;
    GLuint                       m_activeUnit;
    
    GLfloat                      m_maxAnisotropy;
    GLint                        m_maxTextureImageUnits;
    GLint                        m_maxCombinedTextureImageUnits;
    GLint                        m_maxTextureSize;
    
    bool m_init;
};


#endif /* OpenglTextureLayer_h */
