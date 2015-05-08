uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

in vec3 vert;
in vec3 vert_color;

out vec3 frag_color;

void main() {
    frag_color = vert_color;
    gl_Position = proj * view * model * vec4(vert, 1.0);
}

