#include <glfw3.h>
#include <iostream>
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include "light.h"
using namespace glm;
Light::Light(GLFWwindow* window, 
             glm::vec4 init_La,
             glm::vec4 init_Ld,
             glm::vec4 init_Ls,
             glm::vec3 init_position,
             float init_power) : window(window) {
    La = init_La;
    Ld = init_Ld;
    Ls = init_Ls;
    power = init_power;
    lightPosition_worldspace = init_position;
    // setting near and far plane affects the detail of the shadow
    nearPlane = 1.0;
    farPlane = 25.0;

    direction = normalize(targetPosition-lightPosition_worldspace);

    lightSpeed = 0.01f;
    targetPosition = glm::vec3(0.0, 0, 0);


    projectionMatrix =  perspective(radians(90.0f), 4.0f/3.0f, nearPlane, farPlane);

}


mat4 Light::lightVP() {
    return projectionMatrix * viewMatrix;
}