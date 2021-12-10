#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform vec2 resolution;
uniform vec4 drawColor;
uniform bool useTexture;
uniform sampler2D ourTexture;

void main()
{
    if (useTexture) {
        FragColor = texture(ourTexture, TexCoord);
    } else {
        FragColor = drawColor;
    }
}
