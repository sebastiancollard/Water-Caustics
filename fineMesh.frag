#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;


// Inputs the color from the Vertex Shader
in vec3 color;
in vec3 intercept;

uniform sampler2D texture1;


void main()
{
	//FragColor = vec4((color), 1.0);
	vec2 intercepts = vec2(intercept.x,intercept.z);
	float r = texture(texture1, intercepts*0.8f).r;
	float g = texture(texture1, intercepts*0.8f).g;
	float b = texture(texture1, intercepts*0.8f).b;
	float alpha = (r + g + b) / 3.f;
	FragColor = vec4(vec3(1.), alpha);
	//FragColor = vec4(intercept,1.0f);
}