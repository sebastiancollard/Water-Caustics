#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;


// Inputs the color from the Vertex Shader
in vec3 color;
in vec3 intercept;
in vec2 tex;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D gCausticBlurred;
uniform sampler2D gp;
uniform float groundOffset;

//uniform sampler2D gp;
//uniform sampler2D gpp;
//uniform sampler2D texture1;
//uniform sampler2D texture2;
//uniform sampler2D gCausticBlurred;


void main()
{
	float caustic = texture(gCausticBlurred, tex).r;
	vec3 norm = normalize(color);
	vec3 waterColor = vec3(0., 112.5f/255.f, 1.);
	float intensity = 1.f;
	////FragColor = vec4((color), 1.0);
	//vec2 intercepts = vec2(intercept.x,intercept.z);
	//float map = max(pow(dot(norm, vec3(0, 1, 0)), 30) - 0.5, 0);
	//float r = texture(texture1, vec2(map)).r;
	//float g = texture(texture1, vec2(map)).g;
	//float b = texture(texture1, vec2(map)).b;
	//r *= intensity;
	//g *= intensity;
	//b *= intensity;
	//float alpha = (r + g + b) / 3.f;
	//FragColor = vec4(vec3(1.), alpha);
	//FragColor = vec4(texture(texture1,vec2((intercepts+2)/4)).rgb,1.0f);
	//FragColor = vec4(texture(texture1, vec2(0.5f,0.5f)).rgb,1.0f);
	//FragColor = vec4(intercept,1.0f);
	float a = 0.1f;
	float b = 0.1f;
	float attenuation = 2.f / ( a * groundOffset * groundOffset + b * groundOffset+ 1.f );
	
	vec3 res = (texture(texture2,tex).rgb * 0.9f + texture(texture2,tex).rgb * vec3(caustic)) * min(attenuation, 1.f);
	res = min(res, texture(texture2,tex).rgb * 1.4) + waterColor * 0.25f;

	FragColor = vec4(res ,1.0f);

//	FragColor = texelFetch(texture3,1,0);
//	FragColor = vec4(texture(gp,tex).rgb,1.0);
	//FragColor = vec4(vec3(caustic),1.0);
}