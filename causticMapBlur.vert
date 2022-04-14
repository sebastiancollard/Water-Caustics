#version 330 core
layout (location = 0) in vec3 position;   
layout (location = 1) in vec3 normal; 
  
out vec2 tex;

uniform mat4 model;
void main()
{
    // assumes plane is 4 x 4m
    tex = vec2((position.x+2.f)/4.f, (position.z+2.f)/4.f);
    gl_Position = model * vec4(position, 1.0);
}  