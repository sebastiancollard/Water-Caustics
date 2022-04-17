#version 330 core
layout (location = 0) in vec3 position;   // the position variable has attribute position 0
layout (location = 1) in vec3 normal; // the color variable has attribute position 1
layout (location = 2) in vec2 texcoord; 
  
out vec3 Normal; // output a color to the fragment shader
out vec2 TexCoords;

uniform mat4 model;
void main()
{
    gl_Position = model * vec4(position, 1.0);
    Normal = normal;
    TexCoords = texcoord;
}  