#include <iostream>
#include <string>
#include <chrono>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "init.h"

struct Vec3
{
    float x, y, z;

    Vec3 normalize()
    {
        float len = this->length();
        return Vec3{ x / len,y / len,z / len };
    }
    float length()
    {
       return sqrt(x * x + y * y + z * z);
    }
    Vec3 abs()
    {
        return Vec3{ sqrt(x * x),sqrt(y * y),sqrt(z * z) };
    }

    float dot(Vec3 other)
    {
        return x * other.x + y * other.y + z * other.z;
    }

    Vec3 operator-(Vec3 other)
    {
        return Vec3{ x - other.x , y - other.y, z - other.z };
    }

    Vec3 operator+(Vec3 other)
    {
        return Vec3{ x + other.x , y + other.y, z + other.z };
    }

    Vec3 operator*(float other)
    {
        return Vec3{ x * other,y * other,z * other };
    }

};

struct Sphere
{
    Vec3 pos;
    float rad;
    float mass = 0;
    Vec3 vel = { 0,0,0 };
    float restitution = 1.0;
};

Sphere mainSphere = Sphere{ {0,0,0},2,1,{0,0,4},1.0 };
Sphere staticSpheres[10] = 
{
    Sphere{ {2,2,10},3},
    Sphere{ {2,2,-10},3},
    Sphere{ {6,2,10},3},
    Sphere{ {-2,2,10},3},
    Sphere{ {2,6,10},3},
    Sphere{ {6,-2,10},3},
    Sphere{ {6,2,-10},3},
    Sphere{ {-2,2,-10},3},
    Sphere{ {2,6,-10},3},
    Sphere{ {2,-2,-10},3}
};
double totalTime = 0;

float modulo(float x, float y)
{
    return x - y * floor(x / y); //https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/mod.xhtml
}

bool isColiding(int i)
{
    return (mainSphere.pos - staticSpheres[i].pos).length() < mainSphere.rad + staticSpheres[i].rad;
}

// https://gamedevelopment.tutsplus.com/tutorials/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331
void resolveCollision(int i)
{
    Vec3 dist = staticSpheres[i].pos - mainSphere.pos;
    Vec3 n = dist.normalize();
    float j = -(1.0 + mainSphere.restitution) * (-mainSphere.vel.dot(n)) / (1.0 / mainSphere.mass);
    mainSphere.vel = mainSphere.vel - n * (j / mainSphere.mass);
    mainSphere.pos = mainSphere.pos + n * (dist.length() - mainSphere.rad - staticSpheres[i].rad);
}

void update(int program, std::chrono::high_resolution_clock::time_point &previousTime)
{
    GlCheckError();
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    double deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - previousTime).count()/1000.0;
    previousTime = std::chrono::high_resolution_clock::now();
    totalTime += deltaTime;

    mainSphere.pos = mainSphere.pos + mainSphere.vel * (float)deltaTime;
    mainSphere.pos.x = modulo(mainSphere.pos.x, 4.0);
    mainSphere.pos.y = modulo(mainSphere.pos.y, 4.0);

    for (int i = 0; i < 10; i++)
    {
        if (isColiding(i))
        {
            resolveCollision(i);
        }
    }

    GLint posLoc = glGetUniformLocation(program, "u_pos");
    if (posLoc != -1)
    {

        glUniform3f(posLoc, mainSphere.pos.x, mainSphere.pos.y, mainSphere.pos.z);
    }

    GLint rotLoc = glGetUniformLocation(program, "u_lookRot");
    if (rotLoc != -1)
    {
        glUniform2f(rotLoc, 0.0,totalTime);
    }
}


int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(screenSizeX, screenSizeY, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if(glewInit()!=GLEW_OK)
    {
        std::cout << "glew not ok" << std::endl;
    }

    glfwSetWindowSizeCallback(window, WindowResizeCallback);

   int program = setupSquare();
   std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window))
    {
        
        update(program,startTime);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}