//
//  OpenglDrawLayer.h
//  OpenglFramework
//
//  Created by Daniel Collier on 08/12/2016.
//  Copyright © 2016 Daniel Collier. All rights reserved.
//

#ifndef OpenglDrawLayer_h
#define OpenglDrawLayer_h

// platform dependent includes
#ifdef __APPLE__
#include <OpenGL/gl3.h> // includes 3.2 + functionality
#include <OpenGL/gl3ext.h>
#elif _WIN32
#include "GL/glew.h"
#endif

struct OpenglDrawCommand {
    
};

class OpenglDrawLayer {
public:
    
    OpenglDrawLayer() {
        
    }
    
    void addDrawCommad(DrawCommand const & dCommand) {
        
    }
    
    void Draw() {
        
    }
    
private:
    
};


#endif /* OpenglDrawLayer_h */
