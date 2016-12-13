#version 400
 
in vec3 vp;
in vec3 pos;

void main() {
  gl_Position = vec4(vp + pos, 1.0);
}