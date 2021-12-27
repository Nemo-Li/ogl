#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
//颜色的偏移距离
uniform float uTextureCoordOffset;

void main()
{
    vec4 blue = texture(screenTexture, TexCoords);
    vec4 green = texture(screenTexture, vec2(TexCoords.x + uTextureCoordOffset, TexCoords.y + uTextureCoordOffset));
    vec4 red = texture(screenTexture, vec2(TexCoords.x - uTextureCoordOffset, TexCoords.y - uTextureCoordOffset));
    FragColor = vec4(red.x, green.y, blue.z, blue.w);
}