#ifndef COLLISION_H
#define COLLISION_H

#include <glm/glm.hpp>

class Box;
class Sphere;
class OBB;
class Camera;
void handleBoxSphereCollision(Box& box, Sphere& sphere);
void handleRoomCameraCollision(Box& box, OBB& cam_obb, Camera& camera);
void handleObjectsCameraCollision(Camera& camera, OBB& cam_obb,float minx,float maxz,float minz,float maxx);
void handleSphereObjCollision(Sphere& sphere, glm::vec3 min_xyz, glm::vec3 max_xyz);
bool Cam_with_obj_collision(const float& r, glm::vec3& cam_pos, float minx, float maxz, float minz, float maxx);
#endif
