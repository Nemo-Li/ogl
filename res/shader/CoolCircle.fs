#version 330 core
#extension GL_OES_standard_derivatives : enable

precision highp float;

out vec4 FragColor;

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

const float pi = 3.14;

float random (vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898,78.233)))*43758.5453123);
}


void main( void )
{
	vec2 uv = (gl_FragCoord.xy - 0.5 * resolution.xy) / resolution.y;
	vec3 color = vec3(0.0);
	color += 0.001 /length(uv);
	float stp = 3.14 * 2.0 / 60.0;
	for(float i = 0.0; i < 63.0; i++)
	{
		float factor = (sin(time) * 0.5 + 0.5) + 0.3;
		float ux = 0.25 * cos(stp);
		float uy = 0.25 * sin(stp);
		color += 0.001 /length(uv - vec2(ux, uy));
		color += 0.001 * factor / length(uv - vec2(ux + sin(i * time / 100.0 * (0.02 + random(vec2(ux)))), uy - cos(i * time * 0.02 + random(vec2(ux)))));
		stp += 0.1;
	}

	color += vec3(0.1, 0.3, 0.5) * sin(time * 0.02);

	FragColor = vec4(color, 1.0);
}