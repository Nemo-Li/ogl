#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float uAlpha;

void main()
{
    FragColor = vec4(texture(screenTexture, TexCoords).rgb, uAlpha);
}