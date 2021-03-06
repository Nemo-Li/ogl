#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aTexCoord;

out vec2 TexCoord;
uniform mat4 modelMatrix;

void main()
{
    gl_Position = modelMatrix * vec4(aPos, 1.0);
    TexCoord = aTexCoord.xy;
}