#version 330 core
// Point-sprite particle fragment shader.
// gl_PointCoord ranges [0,1] over the point square; we map it to a soft disc.
in  vec4 vColor;
out vec4 fragColor;

void main() {
    // Map point coord to [-1, 1] and compute squared distance from centre
    vec2  coord = gl_PointCoord * 2.0 - 1.0;
    float dist2 = dot(coord, coord);
    if (dist2 > 1.0) discard;                        // clip to circle

    float alpha = vColor.a * (1.0 - dist2);          // smooth fade at edge
    fragColor   = vec4(vColor.rgb, alpha);
}
