#version 330 core
// 2-D HUD rendering (speed bar, gear indicator, etc.).
// Quad vertices are supplied in screen pixels; the projection uniform
// converts them to NDC via an orthographic matrix built by OGLRenderer.
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 projection;

out vec2 fragTexCoord;

void main() {
    fragTexCoord = aTexCoord;
    gl_Position  = projection * vec4(aPos, 0.0, 1.0);
}
