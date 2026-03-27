#version 330 core
// Two-pass separable Gaussian blur used for bloom.
// Reuses postprocess.vert for the full-screen quad.
out vec4 fragColor;
in  vec2 fragTexCoord;

uniform sampler2D image;
uniform int       horizontal;   // 1 = horizontal pass, 0 = vertical pass

// 5-tap half-kernel weights.  Full kernel sum:
// WEIGHT[0] + 2*(WEIGHT[1]+WEIGHT[2]+WEIGHT[3]+WEIGHT[4]) = 1.0
const float WEIGHT[5] = float[](0.2270270270,
                                0.1945945946,
                                0.1216216216,
                                0.0540540541,
                                0.0162162162);

void main() {
    vec2 texel  = 1.0 / vec2(textureSize(image, 0));
    vec3 result = texture(image, fragTexCoord).rgb * WEIGHT[0];

    if (horizontal == 1) {
        for (int i = 1; i < 5; i++) {
            result += texture(image, fragTexCoord + vec2(texel.x * float(i), 0.0)).rgb * WEIGHT[i];
            result += texture(image, fragTexCoord - vec2(texel.x * float(i), 0.0)).rgb * WEIGHT[i];
        }
    } else {
        for (int i = 1; i < 5; i++) {
            result += texture(image, fragTexCoord + vec2(0.0, texel.y * float(i))).rgb * WEIGHT[i];
            result += texture(image, fragTexCoord - vec2(0.0, texel.y * float(i))).rgb * WEIGHT[i];
        }
    }

    fragColor = vec4(result, 1.0);
}
