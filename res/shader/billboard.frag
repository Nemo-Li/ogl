#version 410 core

uniform sampler2D u_diffuse_texture;

in vec2 vTexcoord;

layout (location = 0) out vec4 FragTex;

void main() {
    vec4 textureColor = texture(u_diffuse_texture, vTexcoord);
    if (textureColor.a < 0.3) {
        discard;
    }
    if (textureColor.r > 0.6 && textureColor.g > 0.6 && textureColor.b > 0.6) {
        discard;
    }
    FragTex = textureColor;
}