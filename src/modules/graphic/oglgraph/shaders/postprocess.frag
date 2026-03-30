#version 330 core
// HDR tone-mapping combined with bloom.
out vec4 fragColor;
in  vec2 fragTexCoord;

uniform sampler2D hdrBuffer;   // scene HDR colour (unit 0)
uniform sampler2D bloomBlur;   // blurred bright-pass (unit 1)
uniform float     exposure;    // exposure multiplier
uniform float     bloomStrength;

const float GAMMA = 2.2;

vec3 acesTonemap(vec3 color) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0, 1.0);
}

void main() {
    vec3 hdr   = texture(hdrBuffer, fragTexCoord).rgb;
    vec3 bloom = texture(bloomBlur, fragTexCoord).rgb;

    // Blend bloom into the HDR scene before filmic tone mapping.
    vec3 mapped = acesTonemap(max(hdr + bloom * bloomStrength, vec3(0.0)) * exposure);

    // Gamma correction
    mapped = pow(mapped, vec3(1.0 / GAMMA));

    fragColor = vec4(mapped, 1.0);
}
