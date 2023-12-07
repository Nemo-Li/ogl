#version 410

in vec3 o_position;
in vec3 o_normal;
in vec2 o_texcoord;

layout(location = 0) out vec4 fragColor;
uniform sampler2D tex_sampler;
uniform vec3 mesh_color;
uniform vec3 cam_pos;
uniform bool useColor;

void main()
{
    vec3 n = normalize(o_normal);
    vec3 frag_pos = normalize(cam_pos - o_position);

    vec3 finalColor = mesh_color;

    if (!useColor) {
        finalColor = texture(tex_sampler, o_texcoord).xyz;
        finalColor *= max(0.0f, dot(n, frag_pos));
    }

    fragColor = vec4(finalColor, 1.0f);
}