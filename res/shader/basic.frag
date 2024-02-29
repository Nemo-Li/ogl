#version 410 core

in vec3 o_position;
in vec3 o_normal;
in vec2 o_texcoord;
flat in int vTexid;

layout(location = 0) out vec4 fragColor;
uniform sampler2DArray textureArray;
uniform vec3 cam_pos;

void main()
{
    vec3 n = normalize(o_normal);
    vec3 frag_pos = normalize(cam_pos - o_position);


    vec3 texcoord = vec3(o_texcoord, vTexid);

    vec3 finalColor = vec3(1.0);
    if (texcoord.p >= 0.0) {
        finalColor = texture(textureArray, texcoord).xyz;
    }
    finalColor *= max(0.0f, dot(n, frag_pos));

    fragColor = vec4(finalColor, 1.0f);
}