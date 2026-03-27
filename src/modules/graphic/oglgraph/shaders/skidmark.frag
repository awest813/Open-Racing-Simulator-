#version 330 core
// Skid-mark fragment shader: renders a dark, semi-transparent mark.
// The alpha uniform allows the renderer to fade strips over time.
in  vec2 fragTexCoord;
out vec4 fragColor;

uniform float alpha;   // global opacity (set to ~0.8 by OGLRenderer)

void main() {
    // Dark rubber colour; edges softened by u-coord proximity to 0 or 1
    // Dark rubber colour (near-black with a slight brown tint).
    // Values are intentionally small to represent worn tyre rubber on asphalt.
    float edgeFade = smoothstep(0.0, 0.1, fragTexCoord.x) *
                     smoothstep(1.0, 0.9, fragTexCoord.x);
    fragColor = vec4(0.05, 0.04, 0.03, alpha * edgeFade);
}
