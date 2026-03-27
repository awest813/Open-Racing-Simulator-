#version 330 core
// Skid-mark triangle-strip rendering.
// Vertices are already in world space (skidmarks follow the track surface).
// attrib 0 – position  (vec3)
// attrib 1 – normal    (vec3)  – not used for lighting but kept for layout compatibility
// attrib 2 – texcoord  (vec2)
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 view;
uniform mat4 projection;

out vec2 fragTexCoord;

void main() {
    fragTexCoord = aTexCoord;
    gl_Position  = projection * view * vec4(aPos, 1.0);
}
