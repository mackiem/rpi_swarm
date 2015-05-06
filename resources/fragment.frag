in vec3 frag_color;
out vec4 final_color;

int main() {
    final_color = vec4(frag_color, 1.0);
}
