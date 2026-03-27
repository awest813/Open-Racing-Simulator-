#version 330 core
// HDR tone-mapping (Reinhard) combined with bloom.
out vec4 fragColor;
in  vec2 fragTexCoord;

uniform sampler2D hdrBuffer;   // scene HDR colour (unit 0)
uniform sampler2D bloomBlur;   // blurred bright-pass (unit 1)
uniform float     exposure;    // exposure multiplier

const float GAMMA = 2.2;

void main() {
    vec3 hdr   = texture(hdrBuffer, fragTexCoord).rgb;
    vec3 bloom = texture(bloomBlur, fragTexCoord).rgb;

    // Additive bloom
    hdr += bloom;

    // Reinhard tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdr * exposure);

    // Gamma correction
    mapped = pow(mapped, vec3(1.0 / GAMMA));

    fragColor = vec4(mapped, 1.0);
}
