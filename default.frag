#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;


// Inputs the color from the Vertex Shader
in vec3 Normal;
in vec3 FragPos;
// Inputs the texture coordinates from the Vertex Shader
in vec2 texCoord;

// Gets the Texture Unit from the main function
uniform sampler2D tex0;
uniform vec3 viewPos; 

// placeholder values
// might want to change light color to something more accurate to the sun later on
vec3 lightColor = vec3(1.);
// water color
vec3 objectColor = vec3(0., 100.f/255.f, 1.);
// light placed to the side to see reflections better for now
vec3 lightPos = vec3 (0, 5, -10);

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    //vec3 lightDir = vec3(0, 1,0);
    float diff = max(dot(norm, lightDir), 0.0);
    // second component is different since it aims to make the water more green looking at steeper angles 
    vec3 diffuse = vec3(diff * lightColor.r, clamp(diff * (1 + 2.f * (1 - diff)), 0, 1) * lightColor.g, diff * lightColor.b);
    
    // specular
    float specularStrength = 1.;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
    vec3 specular = specularStrength * spec * lightColor;  
    // extra step to get the bright specular on the water surface, when left out it looks quite flat.
    if ((specular.r + specular.g + specular.b) / 3.f > 0.3) specular = vec3(1.);

    vec3 result = ((ambient + diffuse) * objectColor + specular) ;

    //float r = objectColor.r;
    //float g = objectColor.g;
    //float b = objectColor.b;
    //
    //float alpha = (r + g + b) / 3.f;
    

	FragColor = vec4(result, diff + specular);
	//FragColor = vec4(vec3(0.75), 1.);
}