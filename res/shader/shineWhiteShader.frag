#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
//修改这个值，可以控制曝光的程度
uniform float uAdditionalColor;

void main()
{
    vec4 color = texture(screenTexture, TexCoords);
    FragColor = vec4(color.r + uAdditionalColor, color.g + uAdditionalColor, color.b + uAdditionalColor, color.a);
}