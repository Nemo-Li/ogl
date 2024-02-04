#version 410 core

layout (points) in;
layout (triangle_strip, max_vertices = 3) out;
uniform sampler2D u_distortion_texture;

uniform mat4 viewProjectMatrix;
uniform float time;
uniform mat4 view;

in vec3 tePosition[];
in vec2 v_uv[];
in float Height[];
in vec4 teInfo[];

out vec2 frag_uv;
out float frag_height;
out vec3 vColor;

//#define BottomColor vec3(0.0025, 0.02, 0.005)
#define BottomColor vec3(0.1, 0.2, 0.05)
//#define TopColor vec3(0.1, 0.2, 0.05)
#define TopColor vec3(0.50, 0.62, 0.4)

const float PI = 3.1415926;
#define WindStrength 0.8
#define WindFrequency vec2(0.05, 0.05)

mat3 RotY(float r) {
    float cosR = cos(r);
    float sinR = sin(r);
    return mat3(
    cosR, 0.0, -sinR,
    0.0, 1.0, 0.0,
    sinR, 0.0, cosR
    );
}

mat3 AngleAxis3x3(float angle, vec3 axis) {
    float c = cos(angle), s = sin(angle);

    float t = 1.0 - c;
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    return mat3(
    t * x * x + c, t * x * y + s * z, t * x * z - s * y,
    t * x * y - s * z, t * y * y + c, t * y * z + s * x,
    t * x * z + s * y, t * y * z - s * x, t * z * z + c
    );
}

mat3 GetWindMat(vec2 pos) {
    vec2 uv = pos * 0.01 + WindFrequency * time;
    vec2 windSample = texture(u_distortion_texture, uv).xy * 2.0 - 1.0;
    windSample *= WindStrength;
    vec3 wind = normalize(vec3(windSample.x, windSample.y, 0.0));
    mat3 windRotation = AngleAxis3x3(PI * windSample.x, wind);
    return windRotation;
}

void main() {
    frag_uv = v_uv[0];
    frag_height = Height[0];

    vec3 trans = tePosition[0];
    vec4 info = teInfo[0];

    float rand = info.x + info.y;

    mat3 rotMat = RotY(rand * PI);
    float viewz = (view * vec4(trans, 1.0)).z;
    mat3 windMat = GetWindMat(trans.xz + vec2(viewz));
    rotMat = rotMat * windMat;

    vec2 size = vec2(rand * 0.5 + 0.1, rand * 2.0 + 0.5);
    vec3 vertA = vec3(-size.x, 0.0, 0.0), vertB = -vertA, vertC = vec3(0.0, size.y, 0.0);
    vec2 hbt = vec2(vertA.y, vertC.y) / size.y;

    vertA = rotMat * vertA + trans;
    vertB = rotMat * vertB + trans;
    vertC = rotMat * vertC + trans;

    vColor = mix(BottomColor, TopColor, hbt.x);
    gl_Position = viewProjectMatrix * vec4(vertA, 1.0);
    EmitVertex();

    vColor = mix(BottomColor, TopColor, hbt.x);
    gl_Position = viewProjectMatrix * vec4(vertB, 1.0);
    EmitVertex();

    vColor = mix(BottomColor, TopColor, hbt.y);
    gl_Position = viewProjectMatrix * vec4(vertC, 1.0);
    EmitVertex();

    EndPrimitive();
}