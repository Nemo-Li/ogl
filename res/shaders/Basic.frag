#version 410

in vec3 o_position;
in vec3 o_normal;
in vec2 o_texcoord;

layout(location = 0) out vec4 fragColor;
uniform sampler2D tex_sampler;
uniform sampler2D tex_sampler1;
uniform vec3 cam_pos;

void main()
{   
	vec3 n = normalize(o_normal);
	vec3 frag_pos = normalize(cam_pos - o_position);
	
	vec3 finalColor = mix(texture(tex_sampler, o_texcoord).xyz,texture(tex_sampler1, o_texcoord).xyz, 0.8);
	finalColor *= max(0.0f, dot(n, frag_pos));
	
	fragColor = vec4(finalColor, 1.0f);
}