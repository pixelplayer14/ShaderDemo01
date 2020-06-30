#include <iostream>
#include <string>
#include <fstream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "init.h"

const int screenSizeX = 500;
const int screenSizeY = 500;
int program;

void WindowResizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    GLint resLoc = glGetUniformLocation(program, "u_resolution");
    if (resLoc != -1)
    {
        glUniform2f(resLoc, (float)width, (float)height);
    }
}

void GlClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

void GlCheckError()
{
    while (GLenum error = glGetError())
    {
        std::cout << error << std::endl;
    }
}

std::string readShader()
{

    std::string line;
    std::string shader;

    std::ifstream file("shaders/spheres.shader");

    if (file.is_open())
    {
        while (getline(file, line))
        {
            shader += line + "\n";
        }
    }

    file.close();
    return shader;
}

int CompileShader(const std::string& source, unsigned int type)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);//nullptr means it is assumed to be null terminated
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE)
    {
        int length;
        std::cout << "failed to compile" << std::endl;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

int CreateShader(const std::string source)
{
    unsigned int program = glCreateProgram();
    unsigned int fs = CompileShader(source, GL_FRAGMENT_SHADER);
    glAttachShader(program, fs);
    glLinkProgram(program);

    return program;
}

int setupSquare()
{
    float verts[8]
    {
        -1.0,1.0,
        -1.0,-1.0,
         1.0,-1.0,
         1.0,1.0

    };
    //Gluint
    unsigned int vertBuffer;
    glGenBuffers(1, &vertBuffer); //create
    glBindBuffer(GL_ARRAY_BUFFER, vertBuffer); // bind (select)
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind

    unsigned int indices[6] =
    {
        0,1,2,
        0,2,3
    };

    unsigned int indBuffer;
    glGenBuffers(1, &indBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    std::string vs = readShader();
    program = CreateShader(vs);
    glUseProgram(program);

    GLint resLoc = glGetUniformLocation(program, "u_resolution");
    if (resLoc != -1)
    {
        glUniform2f(resLoc, (float)screenSizeX, (float)screenSizeX);
    }


    return program;
}
