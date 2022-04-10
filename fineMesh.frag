#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;


// Inputs the color from the Vertex Shader
in vec3 color;
in vec3 intercept;

uniform sampler2D texture1;


void main()
{
	float intensity = 1.f;
	//FragColor = vec4((color), 1.0);
	vec2 intercepts = vec2(intercept.x,intercept.z);
	float map = max(dot(color, vec3(0, 1, 0)) * 100 - 0.5, 0);
	float r = texture(texture1, vec2(map)).r;
	float g = texture(texture1, vec2(map)).g;
	float b = texture(texture1, vec2(map)).b;
	r *= intensity;
	g *= intensity;
	b *= intensity;
	float alpha = (r + g + b) / 3.f;
	FragColor = vec4(vec3(1.), alpha);
	//FragColor = vec4(intercept,1.0f);
}