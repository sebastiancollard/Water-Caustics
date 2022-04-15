#version 330 core
out float FragColor;

//out vec4 FragColor;  
in vec3 Normal;

// env map
uniform sampler2D texture1;

  
void main()
{
    vec3 norm = normalize(Normal);
    //FragColor = vec4(ourColor, 1.0);
    //float ambient = 1.0f;
    float map = max(pow(dot(norm, vec3(0, 1, 0)), 100) - 0.5, 0);
	float caustic = texture(texture1, vec2(map)).r;
    //FragColor = vec4(vec3(r, g, b), 1.);
    
    
    FragColor = caustic;

    //FragColor = texelFetch(texture2, 0,0);
}