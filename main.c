#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>


#define SUCCESS_CODE 1
#define ERROR_CODE -1
#define ERROR_MSG_LEN 1024

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

char* handle_file_read_error(FILE* fp, char* buffer, const char* filename) {
    char error_msg[ERROR_MSG_LEN];
    // let's trunc filenames larger than 255, to not overflow error_msg buffer
    /*char safe_filename[256];*/
    /*memcpy(safe_filename, filename, 255);*/
    /*safe_filename[255] = '\0';*/
    snprintf(error_msg, ERROR_MSG_LEN, "Unable to read file : %s", filename);
    if (buffer) {
        free(buffer);
    }
    if (fp) {
        fclose(fp);
    }
    return NULL;
}

char* read_file(const char* filename) {
    char* buffer;
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        return handle_file_read_error(fp, buffer, filename);
    }

    long file_len;
    if (fseek(fp, 0L, SEEK_END) != 0) {
        return handle_file_read_error(fp, buffer, filename);
    }
    file_len = ftell(fp);
    if (file_len == -1) {
        return handle_file_read_error(fp, buffer, filename);
    }
    if (fseek(fp, 0L, SEEK_SET) != 0) {
        return handle_file_read_error(fp, buffer, filename);
    }
    buffer = (char*) calloc((file_len + 1), sizeof(char));
    int output = fread(buffer, sizeof(char), file_len, fp);

    if (output != file_len) {
        return handle_file_read_error(fp, buffer, filename);
    };
    // calloc takes care of \0
    /*(*buffer, filename)[file_len] = '\0';*/
    return buffer;
}

int handle_GL_error() {
    GLuint error_code;
    if ((error_code = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "Error in GL code : %s", glewGetErrorString(error_code));
        return ERROR_CODE;
    }
    return SUCCESS_CODE;
}

int compile_shader(const char* filename, ShaderType shader_type, GLuint* shader) {
    char* source;
    source = read_file(filename);

    if (!source) {
        char error_msg[ERROR_MSG_LEN];
        snprintf(error_msg, ERROR_MSG_LEN, "%s : read_file()", filename);
        perror(error_msg);
        return ERROR_CODE;
    }

    GLuint shader_id;
    switch (shader_type) {
        case VERTEX: {
                         shader_id = glCreateShader(GL_VERTEX_SHADER);
                         break;
                     }
        case FRAGMENT: {
                           shader_id = glCreateShader(GL_FRAGMENT_SHADER);
                           break;
                       }
        default: {

                 }
    }
    glShaderSource(shader_id, 1, &source, NULL);
    if (!handle_GL_error()) {
        return ERROR_CODE;
    }
    glCompileShader(shader_id);
    GLint status;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        GLint log_length;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
        char log[log_length];
        glGetShaderInfoLog(shader_id, log_length, NULL, log);
        fprintf(stderr, "Shader %s compilation error: %s", filename, log);
        return ERROR_CODE;
    } else {
        *shader = shader_id;
        return SUCCESS_CODE;
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

    GLuint vtx_shader;
    compile_shader("resource/vert.vert", VERTEX, &vtx_shader);

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
