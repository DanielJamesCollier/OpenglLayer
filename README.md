# OpenglLayer

A header only explicit wrapper around the OpenGL API

Notess
- make sure that the opengl context is created before any of the functions are called

###Shader Program Creation
'''C++

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
'''

###Texture Creation
'''C++

'''

###Texture Creation
'''C++

'''
