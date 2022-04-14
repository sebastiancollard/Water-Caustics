#version 330 core
out float FragColor;

in vec2 tex;

uniform sampler2D gCaustic;
uniform int blurIntensity;

// https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/9.ssao/9.ssao_blur.fs
// blur caustic map
void main() 
{
    if (blurIntensity < 1) { 
        FragColor = texture(gCaustic, tex).r;
        return;
    }
    vec2 texelSize = 1.0 / vec2(textureSize(gCaustic, 0));
    float result = 0.0;
    for (int x = -blurIntensity; x < blurIntensity; ++x) 
    {
        for (int y = -blurIntensity; y < blurIntensity; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(gCaustic, tex + offset).r;
        }
    }
    FragColor = result / (4 * blurIntensity * blurIntensity);
}  