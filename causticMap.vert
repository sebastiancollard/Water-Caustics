#version 330 core
layout (location = 0) in vec3 position;   // the position variable has attribute position 0
layout (location = 1) in vec3 normal; // the color variable has attribute position 1
  
out vec3 Normal; // output a color to the fragment shader

uniform mat4 model;
void main()
{
    gl_Position = model * vec4(position, 1.0);
    Normal = normal;
}  