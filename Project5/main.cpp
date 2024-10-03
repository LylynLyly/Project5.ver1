#include<iostream>
#include <KHR/khrplatform.h>
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include"Particel.h"
#include"SP.h"
#include<random>
#include"Time.h"
#include "Class.h"
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * vec4(aPos, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";
float radius = 10.0f; // Відстань від центру сцени
float cameraAngleX = 0.0f; // Кут обертання навколо осі X
float cameraAngleY = 0.0f; // Кут обертання навколо осі Y
float cameraSpeed = 0.02f; // Швидкість обертання камери

// Оновлюємо положення камери, щоб вона оберталася навколо центру
glm::vec3 updateCameraPosition() {
    float camX = radius * cos(cameraAngleY) * sin(cameraAngleX);
    float camY = radius * sin(cameraAngleY);
    float camZ = radius * cos(cameraAngleY) * cos(cameraAngleX);

    return glm::vec3(camX, camY, camZ);
}
void cleanup(std::vector<Particel*>& particles)
{
    for (Particel* particle : particles)
    {
        delete particle;
    }
    particles.clear();
}
void Edge(Particel* A, glm::vec3 size_border_min, glm::vec3 size_border_max)
{
    glm::vec3 pos = A->GetPosition();
    glm::vec3 velocity = A->GetVelocity();
    float e = 0.2f;
 
    if (pos.x < size_border_min.x || pos.x > size_border_max.x)
    {
        velocity.x = -velocity.x*e; // Змінюємо знак швидкості по осі X
        pos.x = glm::clamp(pos.x, size_border_min.x, size_border_max.x);
    }

    if (pos.y < size_border_min.y || pos.y > size_border_max.y)
    {
        velocity.y = -velocity.y*e; // Змінюємо знак швидкості по осі Y
        pos.y = glm::clamp(pos.y, size_border_min.y, size_border_max.y);
    }

    if (pos.z < size_border_min.z || pos.z > size_border_max.z)
    {
        velocity.z = -velocity.z*e; // Змінюємо знак швидкості по осі Z
        pos.z = glm::clamp(pos.z, size_border_min.z, size_border_max.z);
    }

    // Оновлюємо швидкість частинки
    A->SetPosition(pos);
    A->SetVelocity(velocity);
}
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL for Ravesli.com", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }


    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);


    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);


    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float sr = 0.1f, ckf = 1 / (pow(sr, 3)), mass = pow(sr,3)*1000;

	std::vector<Particel*> f;
    float minPos = -1.0;
    float maxPos = 1.0 - sr;
    float spacing = 0.1;
    int countPerSide = static_cast<int>((maxPos - minPos) / spacing);
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<float> velcoity_nums(-0.01f, 0.01f);
    glm::vec3 max_velocity(0, 0, 0);
    int x1 = 0;
    for (int x = 0; x < countPerSide; ++x)
    {
        for (int y = 0; y < countPerSide; ++y)
        {
            for (int z = 0; z < countPerSide; ++z)
            {

                glm::vec3 position(minPos + x * spacing, minPos + y * spacing, minPos + z * spacing);
                glm::vec3 velocity(velcoity_nums(rng), velcoity_nums(rng), velcoity_nums(rng));
                if (glm::length(max_velocity) < glm::length(velocity))
                {
                    max_velocity = velocity;
                }
                f.push_back(new Particel(position,velocity,mass, ckf, sr, 1000,glm::vec3(0,0,0),x1));
                x1 += 1;
            }
        }
    }
    float t = 0.1;
    std::vector<float> vertics;
    SP SPATILHASH(0.2f);
    SPATILHASH.SetSize(10, 10, 10);
    SPATILHASH.StoreGrid(f);
    Time c_time(t,0.0f);
    c_time.ComputeCFL(max_velocity,sr);

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);


    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertics), vertics.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glBindVertexArray(0);
    std::cout << f.size() << '\n';
    glPointSize(0.2);

  
    int iterator = 100;
    int c = 100;
    while(!glfwWindowShouldClose(window))
         {
        vertics.clear();
        
        processInput(window);
        glm::vec3 cameraPos = updateCameraPosition();
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); // Центр сцени
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);  // Напрямок "вверх"

        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, up);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // Передача матриць у шейдер
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
 
             glm::vec3 max_velocity = glm::vec3(0, 0, 0);
             for (int i = 0; i < f.size(); i++)
             {
                 SPATILHASH.found_neighbor(f[i], sr);
                 
             }
             
             float s = 0, s1 = 0;
             for (int i = 0; i < f.size(); i++)
             {
                 //f[i]->Neighboors();
                 f[i]->ComputeDensity();
                 f[i]->ComputePressure();
                 if (glm::length(max_velocity) < glm::length(
                     f[i]->GetVelocity()))
                 {
                     max_velocity = f[i]->GetVelocity();
                 }
                 if (c == iterator)
                 {
                   // std::cout << "D" << i << "   " << f[i]->GetDensity() << '\n';
                   // std::cout << "P" << i << "   " << f[i]->GetPressure() << '\n';
                   // c = 0;
                 }
                 c += 1;
             }
             c_time.ComputeCFL(max_velocity, sr);
             glm::vec3 af(0, 0, 0);
             for (int i = 0; i < f.size(); i++)
             {
                f[i]->ComputeColision(0.5f);
                 Edge(f[i], glm::vec3(-1.1, -1.1, -1.1), glm::vec3(1.1, 1.1, 1.1));
                 f[i]->ComputeFPressure();
                 f[i]->ComputeFVicosiy();
                 f[i]->ComputeAc();
                 f[i]->Transform(t);
                 f[i]->ConvertToOpenGL(vertics);
             }
             if (c_time.IsCFLC() == true)
             {
                 c_time.IncrementTime_s(t);
             }
             else
             {
                 c_time.DecrementTime_s(t);
             }
             
             //std::cout << SPATILHASH.AVG(f.size()) << '\n';
             glBindBuffer(GL_ARRAY_BUFFER, VBO);
             glBufferData(GL_ARRAY_BUFFER, vertics.size() * sizeof(float), vertics.data(), GL_STATIC_DRAW);


             glUseProgram(shaderProgram);
             glBindVertexArray(VAO);
             glDrawArrays(GL_POINTS, 0, f.size());

             glfwSwapBuffers(window);
             glfwPollEvents();
             
             //std::cout << " " << s / f.size() << '\n';
             //std::cout << " " << s1 / f.size() << '\n';
         }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    cleanup(f);
    glfwTerminate();
    return 0;
}
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Зміна кута обертання камери на основі натискань клавіш
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        cameraAngleX -= cameraSpeed; // Обертання вліво
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        cameraAngleX += cameraSpeed; // Обертання вправо
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        cameraAngleY += cameraSpeed; // Обертання вгору
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        cameraAngleY -= cameraSpeed; // Обертання вниз
    }

    cameraAngleY = glm::clamp(cameraAngleY, -glm::half_pi<float>(), glm::half_pi<float>());
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
   
    glViewport(0, 0, width, height);
}