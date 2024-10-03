#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
class Camera
{
public:
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : Position(position), WorldUp(up), Yaw(yaw), Pitch(pitch), MovementSpeed(2.5f), MouseSensitivity(0.1f), Zoom(45.0f)
    {
        updateCameraVectors();
    }

    // Повертає матрицю вигляду
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // Оновлює позицію камери на основі введення клавіатури
    void ProcessKeyboard(int direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == GLFW_KEY_W)
            Position += Front * velocity;
        if (direction == GLFW_KEY_S)
            Position -= Front * velocity;
        if (direction == GLFW_KEY_A)
            Position -= Right * velocity;
        if (direction == GLFW_KEY_D)
            Position += Right * velocity;
    }

    // Оновлює напрямок камери на основі введення миші
    void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true)
    {
        xOffset *= MouseSensitivity;
        yOffset *= MouseSensitivity;

        Yaw += xOffset;
        Pitch += yOffset;

        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        updateCameraVectors();
    }

private:
    // Атрибути камери
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Ейлерові кути
    float Yaw;
    float Pitch;

    // Налаштування камери
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // Оновлення векторів камери
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
