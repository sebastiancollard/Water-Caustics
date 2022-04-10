#version 330 core
out vec4 FragColor;  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D tex0;
  
void main()
{
    //FragColor = vec4(ourColor, 1.0);
    float ambient = 1.f;
    FragColor = vec4(ambient * texture(tex0, TexCoord).xyz, 1.0);
}