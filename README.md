# OpenglLayer

A header only explicit wrapper around the OpenGL API

Notess
- make sure that the opengl context is created before any of the functions are called

###Shader Program Creation
```cpp

using namespace glLayer;

// vertex shder
std::string vertexCode = R"(

#version 400
in vec3 vp;

void main() {
    gl_Position = vec4(vp, 1.0);
}
)";

// fragment shader

std::string fragmentCode = R"(

#version 400
out vec4 frag_colour;

void main() {
    frag_colour = vec4(0.5, 0.0, 0.5, 1.0);
}
)";

ShaderProgram program  = glShaderLayer.createShaderProgram();
ShaderObject  vertex   = glShaderLayer.createShaderObject(ShaderObjectType::VERTEX_SHADER);
ShaderObject  fragment = glShaderLayer.createShaderObject(ShaderObjectType::FRAGMENT_SHADER);

glShaderLayer.attachSourceToShaderObject(vertex, vertexCode);
glShaderLayer.attachSourceToShaderObject(fragment, fragmentCode);

glShaderLayer.compileShaderObject(vertex);
glShaderLayer.compileShaderObject(fragment);

glShaderLayer.attachShaderObjectToProgram(program, vertex);
glShaderLayer.attachShaderObjectToProgram(program, fragment);

glShaderLayer.linkProgram(program);

glShaderLayer.bindShaderProgram(program);
```

###Texture Creation

A vector of floats is displayed in this example but any integral or floating point data type can be used

```cpp
using namespace glLayer;

std::vector<float> pixels = {/**/0.0f, 0.0f, 0.0f,/**/1.0f, 1.0f, 1.0f,
                             /**/1.0f, 1.0f, 1.0f,/**/0.0f, 0.0f, 0.0f};

OpenglTextureLayer glTextureLayer;

if(glTextureLayer.init()) {
    std::cout << "texture layer is okay" << std::endl;
} else {
    //handle error
}

Texture tex1d = glTextureLayer.createTexture1D(pixels, 4 , TexturePixelFormat::RGB, TextureWrapMode::CLAMP_TO_EDGE);
Texture tex2d = glTextureLayer.createTexture2D(pixels, 2, 2, TexturePixelFormat::RGB, TextureWrapMode::REPEAT, TextureWrapMode::REPEAT);

```

###How to draw ?
To render something you must use the OpenglDrawLayer, commands are passed to the draw layer using 
addDrawCommand(..). once the comand queue is pupulated proccessDrawCommands() can then be called.
After processDrawCommands() is called you must swap the back buffer.
```cpp
using namespace glLayer;
```
