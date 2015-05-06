#include <stdlib.h>
#include <stdio.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// ---------------- shader stuff ----------------------//

typedef enum { VERTEX, FRAGMENT, GEOMETRY } ShaderType;

int read_file(const char* filename, char** buffer) {
    FILE* fp = fopen(filename, "r");
    if (!fp)
        goto f_read_error;


    // get length of eof
    size_t file_len;

    if (fseek(fp, 0L, SEEK_END) == 0) {
        file_len = ftell(fp);
        if (file_len == -1) {
            goto f_read_error;
        }
        if (fseek(fp, 0L, SEEK_SET) != 0) {
            goto f_read_error;
        }
        *buffer = (char*) malloc(sizeof(char) * (file_len + 1));
        printf("file len: %d\n", file_len);
        int output = fread(*buffer, sizeof(char), file_len, fp);
        printf("output : %d\n", output);
        if (output != file_len) {
            goto f_read_error;
        };
    } else {
        f_read_error: ;
        char error_msg[1024];
        sprintf(error_msg, "Unable to read file : %s", filename);
        perror(error_msg);
        if (buffer) {
            free(buffer);
        }
        if (fp) {
            fclose(fp);
        }
        return -1;
    }
    return 1;
}

static void compile_shader(const char* filename, ShaderType shader_type) {
    char* source = NULL;
    read_file(filename, &source);
    if (source) {
        printf("%s\n", source);
        free(source);
    }

}

// --------------- shader stuff ----------------------//


int main(int argc, char** argv)
{
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);

    compile_shader("resources/vert.vert", VERTEX);

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef((float) glfwGetTime() * 50.f * 10.f, 0.f, 0.f, 1.f);
        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f);
        glVertex3f(-0.6f, -0.4f, 0.f);
        glColor3f(0.f, 1.f, 0.f);
        glVertex3f(0.6f, -0.4f, 0.f);
        glColor3f(0.f, 0.f, 1.f);
        glVertex3f(0.f, 0.6f, 0.f);
        glEnd();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
