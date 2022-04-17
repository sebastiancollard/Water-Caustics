#version 330 core
out float FragColor;

//out vec4 FragColor;  
in vec3 Normal;
in vec2 TexCoords;

// env map
uniform sampler2D texture1;

uniform sampler2D gNormal;

// uniform variables
uniform int sampleSteps;
uniform int sunDistance;
uniform float baseIntensity;
  
void main()
{
    vec2 tex = abs(TexCoords);
    tex.y = 1.f - tex.y;

    // sample from space is entire normal texture
    float offsetX = 0.5f;
    float offsetY = 0.5f;
    
    // get start and end bounds for sampling
    float startX = tex.x-offsetX;
    float startY = tex.y-offsetY;
    float endX = tex.x+offsetX;
    float endY = tex.y+offsetY;

    // calculate increment for double for-loop
    float incrementX = (endX-startX) / sampleSteps;
    float incrementY = (endY-startY) / sampleSteps;

    //vec3 norm = normalize(Normal);

    // refracted vector
    vec3 refrac;

    // used to apply weighting based on how far the current sample is from being directly above
    float distanceIntensity;

    for (float x = startX; x <= endX; x+=incrementX) {
        for (float y = startY; y <= endY; y+=incrementY) {
            // calculate 2d direction of the current sample relative to the current frag position
            vec2 dir2D = normalize(tex+vec2(x, y));

            // the starting wave plane is 1.5 units above the ground,
            // this only approximates the vector as it does not account
            // for the added height variation of the wave equation.
            vec3 dir3D = normalize(vec3(dir2D.x, 1.5, dir2D.y));

            // get sample normal from texture
            vec3 normTex = normalize(texture(gNormal, vec2(x, y)).rgb);

            // get refracted direction
            refrac = normalize(refract(dir3D, normTex, 1.333));

            // calculate text coord mapping based on the angle between the refracted angle and the 
            // up vector (sun is directly above at all locations).
            float map = max(pow(dot(normTex, vec3(0, 1, 0)), sunDistance) - 0.5, 0);

            // calaulate distance weighting
            distanceIntensity = pow(1.f - distance(tex, vec2(x, y)), 4);
	        
            // add the mapping on the sun texture multiplied by intensity variables
            // accumulate this for each sample.
            FragColor += texture(texture1, vec2(map)).r  * baseIntensity * distanceIntensity;
        }
    }

    // for debugging caustic 
    //FragColor = tex.x < 0.5f && tex.y < 0.5f ? texture(gNormal, tex).r : 0.f;
}