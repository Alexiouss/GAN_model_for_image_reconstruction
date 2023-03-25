#include "Obb.h"
#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <common/model.h>
#include <common/texture.h>
#include <common/util.h>
using namespace glm;


void OBB::Cam_OBB(vec3 obb_pos) {
	pos_of_cam_obb = obb_pos;
	r = 0.1;
}

