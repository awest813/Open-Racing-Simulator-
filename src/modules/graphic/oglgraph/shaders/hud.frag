#version 330 core
// fragTexCoord is kept to satisfy the postprocess.vert interface; this
// shader uses a flat uniform colour and does not sample any texture.
in  vec2 fragTexCoord;
out vec4 fragColor;

uniform vec4 color;   // RGBA colour set per draw call by OGLRenderer::renderHUD

void main() {
    fragColor = color;
}
