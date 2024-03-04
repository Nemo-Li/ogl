#version 410 core

uniform sampler2D u_diffuse_texture;
uniform sampler2DArray textureArray;
uniform bool billborad;

in vec2 vTexcoord;
flat in int vTexid;

layout (location = 0) out vec4 FragTex;

void main() {
    vec4 textureColor = vec4(1.0);
    if (billborad) {
        textureColor = texture(u_diffuse_texture, vTexcoord);
    } else {
        vec3 texcoord = vec3(vTexcoord, vTexid);
        if (texcoord.p >= 0.0) {
            textureColor = texture(textureArray, texcoord);
        }
    }

    if (textureColor.a < 0.3) {
        discard;
    }
    if (billborad) {
        if (textureColor.r > 0.6 && textureColor.g > 0.6 && textureColor.b > 0.6) {
            discard;
        }
    }

    FragTex = textureColor;
}