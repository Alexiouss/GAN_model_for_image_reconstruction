#include <glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"

using namespace glm;

Camera::Camera(GLFWwindow* window) : window(window) {
    position = vec3(0, 4, 5);
    horizontalAngle = 3.14f;
    verticalAngle = 0.0f;
    FoV = 60.0f;
    speed = 3.0f;
    mouseSpeed = 0.001f;
    direction = vec3(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );
    right = vec3(0, 0, 0);
}

void Camera::update() {
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Get mouse position
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Reset mouse position for next frame
    glfwSetCursorPos(window, width / 2, height / 2);

    horizontalAngle += mouseSpeed * float(width / 2 - xPos);
    verticalAngle += mouseSpeed * float(height / 2 - yPos);
    direction = vec3(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

        // Right vector
    right = vec3(
        sin(horizontalAngle - 3.14f / 2.0f),
        0,
        cos(horizontalAngle - 3.14f / 2.0f)
    );
        // Up vector
    vec3 walking = vec3(direction.x, 0, direction.z);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position += walking * deltaTime * speed;
    }
        // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position -= walking * deltaTime * speed;
    }
        // Strafe right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += right * deltaTime * speed;
    }
        // Strafe left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position -= right * deltaTime * speed;
    }
    vec3 up = cross(right, direction);
        // Task 5.7: construct projection and view matrices
        //*/
    projectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 0.05f, 1000.0f);
    viewMatrix = lookAt(
        position,
        position + direction,
        up
    );
    lastTime = currentTime;
}

