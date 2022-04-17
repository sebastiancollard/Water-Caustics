#version 330 core
out float FragColor;

//out vec4 FragColor;  
in vec3 Normal;
in vec2 TexCoords;

// env map
uniform sampler2D texture1;

uniform sampler2D gNormal;

  
void main()
{
    vec2 tex = abs(TexCoords);
    tex.y = 1.f - tex.y;

    float offsetX = 0.5f;
    float offsetY = 0.5f;
    
    float startX = tex.x-offsetX;
    

    float startY = tex.y-offsetY;


    float endX = tex.x+offsetX;
    

    float endY = tex.y+offsetY;

    vec3 norm = normalize(Normal);
    float intensity;
    float caustic;
    //startX = tex.x-offsetX;startY = tex.y-offsetY;endX = tex.x+offsetX;endY = tex.y+offsetY;

    for (float x = startX; x <= endX; x+=0.25) {
        for (float y = startY; y <= endY; y+= 0.25) {
            vec3 normTex = normalize(texture(gNormal, vec2(x, y)).rgb);
            intensity = pow(1.f - distance(tex, vec2(x, y)), 4);
            float map = max(pow(dot(normTex, vec3(0, 1, 0)), 1024) - 0.5, 0);
	        caustic += texture(texture1, vec2(map)).r  * 0.5 * intensity;
        }
        
    }
    
    FragColor = caustic;

    //FragColor = tex.x < 0.5f && tex.y < 0.5f ? texture(gNormal, tex).r : 0.f;

    //FragColor = texelFetch(texture2, 0,0);
}