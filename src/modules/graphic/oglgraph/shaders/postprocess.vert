#version 330 core
// Full-screen quad pass: shared by postprocess.frag and bloom.frag.
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 fragTexCoord;

void main() {
    fragTexCoord = aTexCoord;
    gl_Position  = vec4(aPos, 0.0, 1.0);
}
