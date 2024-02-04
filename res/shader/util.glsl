const vec4 RAND_FACTOR = vec4(12.9898, 78.233, 45.164, 94.673);
const float PI = 3.1415926;

float random(vec3 seed, float i){
    vec4 seed4 = vec4(seed, i);
    float dotProduct = dot(seed4, RAND_FACTOR);
    return fract(sin(dotProduct) * 43758.5453);
}

mat3 RotY(float r) {
    float cosR = cos(r);
    float sinR = sin(r);
    return mat3(
    cosR, 0.0, -sinR,
    0.0, 1.0, 0.0,
    sinR, 0.0, cosR
    );
}