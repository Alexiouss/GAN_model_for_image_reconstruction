#pragma once
#ifndef OBB_H
#define OBB_H
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glfw3.h>
class Drawable;


class OBB{
public:
	glm::vec3 pos_of_cam_obb;
	float r;
	void Cam_OBB(glm::vec3 obb_pos);
};


#endif // !OBB_H
