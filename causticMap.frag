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
    
    float temp = tex.x-offsetX;
    float startX = max(temp, 0.f);
    
    temp = tex.y-offsetY;
    float startY = max(temp, 0.f);

    temp = tex.x+offsetX;
    float endX = min(temp, 1.f);
    
    temp = tex.y+offsetY;
    float endY = min(temp, 1.f);

    float intensity;
    float caustic;
    //startX = tex.x-offsetX;startY = tex.y-offsetY;endX = tex.x+offsetX;endY = tex.y+offsetY;

    for (float x = startX; x <= endX; x+=0.25) {
        for (float y = startY; y <= endY; y+= 0.25) {
            if (y < 0.f || x < 0.f || x > 1.f || y > 1.f) continue;
            //vec3 norm = normalize(Normal);
            vec3 normTex = normalize(texture(gNormal, vec2(x, y)).rgb);
            //FragColor = vec4(ourColor, 1.0);
            //float ambient = 1.0f;
            intensity = 1.f - distance(tex, vec2(x, y));
            float map = max(pow(dot(normTex, vec3(0, 1, 0)), 100) - 0.5, 0);
	        caustic += texture(texture1, vec2(map)).r * 0.1 * intensity;
            //FragColor = vec4(vec3(r, g, b), 1.);
        }
        
    }
    
    FragColor = caustic;

    //FragColor = texture(gNormal, TexCoords).r;

    //FragColor = texelFetch(texture2, 0,0);
}