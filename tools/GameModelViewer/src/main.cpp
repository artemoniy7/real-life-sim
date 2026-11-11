#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace
{
    void framebuffer_size_callback(GLFWwindow*, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    std::string read_text_file(const std::string& path)
    {
        std::ifstream file(path);
        if (!file)
        {
            throw std::runtime_error("Cannot open shader: " + path);
        }

        std::ostringstream contents;
        contents << file.rdbuf();
        return contents.str();
    }

    GLuint compile_shader(GLenum type, const std::string& source)
    {
        const GLuint shader = glCreateShader(type);
        const char* source_ptr = source.c_str();
        glShaderSource(shader, 1, &source_ptr, nullptr);
        glCompileShader(shader);

        GLint success = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (success != GL_TRUE)
        {
            GLint log_length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

            std::string log(static_cast<std::size_t>(log_length), '\0');
            glGetShaderInfoLog(shader, log_length, nullptr, log.data());
            glDeleteShader(shader);

            throw std::runtime_error("Shader compilation failed:\n" + log);
        }

        return shader;
    }

    GLuint create_shader_program()
    {
        const std::string shader_dir = GAMEVIEWER_SHADER_DIR;
        const GLuint vertex = compile_shader(
            GL_VERTEX_SHADER,
            read_text_file(shader_dir + "/model.vert")
        );
        const GLuint fragment = compile_shader(
            GL_FRAGMENT_SHADER,
            read_text_file(shader_dir + "/model.frag")
        );

        const GLuint program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);

        glDeleteShader(vertex);
        glDeleteShader(fragment);

        GLint success = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if (success != GL_TRUE)
        {
            GLint log_length = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

            std::string log(static_cast<std::size_t>(log_length), '\0');
            glGetProgramInfoLog(program, log_length, nullptr, log.data());
            glDeleteProgram(program);

            throw std::runtime_error("Shader linking failed:\n" + log);
        }

        return program;
    }
}

int main()
{
    if (glfwInit() != GLFW_TRUE)
    {
        std::cerr << "ERROR: GLFW initialization failed.\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        1280, 720, "GameModelViewer - OpenGL", nullptr, nullptr
    );

    if (window == nullptr)
    {
        std::cerr << "ERROR: Window creation failed. Check OpenGL 3.3 support.\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (gladLoadGL(reinterpret_cast<GLADloadfunc>(glfwGetProcAddress)) == 0)
    {
        std::cerr << "ERROR: GLAD could not load OpenGL.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << '\n';
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';

    try
    {
        constexpr std::array<float, 18> vertices = {
             0.0f,  0.65f, 0.0f,  0.95f, 0.35f, 0.25f,
            -0.65f, -0.50f, 0.0f, 0.25f, 0.75f, 0.95f,
             0.65f, -0.50f, 0.0f,  0.35f, 0.90f, 0.45f
        };

        GLuint vao = 0;
        GLuint vbo = 0;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
            vertices.data(),
            GL_STATIC_DRAW
        );

        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr
        );
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(
            1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
            reinterpret_cast<void*>(3 * sizeof(float))
        );
        glEnableVertexAttribArray(1);

        const GLuint shader_program = create_shader_program();
        glEnable(GL_DEPTH_TEST);

        while (glfwWindowShouldClose(window) == GLFW_FALSE)
        {
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }

            glClearColor(0.075f, 0.085f, 0.11f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUseProgram(shader_program);
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glDeleteProgram(shader_program);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }
    catch (const std::exception& error)
    {
        std::cerr << "ERROR: " << error.what() << '\n';
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
