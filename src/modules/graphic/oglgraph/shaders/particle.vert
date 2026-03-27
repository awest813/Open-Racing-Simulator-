#version 330 core
// Point-sprite particle rendering.
// Per-particle data is packed in a VBO:
//   attrib 0 – position (vec3)
//   attrib 1 – colour   (vec4, alpha encodes lifetime fade)
//   attrib 2 – size     (float, in pixels)
layout(location = 0) in vec3  aPos;
layout(location = 1) in vec4  aColor;
layout(location = 2) in float aSize;

uniform mat4 view;
uniform mat4 projection;

out vec4 vColor;

void main() {
    vColor = aColor;
    gl_PointSize = aSize;
    gl_Position  = projection * view * vec4(aPos, 1.0);
}
