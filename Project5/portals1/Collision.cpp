#include "Collision.h"
#include "Box.h"
#include "Sphere.h"
#include "Obb.h"
#include <common/camera.h>
using namespace glm;

void handleBoxSphereCollision(Box& box, Sphere& sphere);

bool checkForBoxSphereCollision(glm::vec3 &pos, const float& r,
                                const float& size, glm::vec3& n);

bool Cam_with_room_collision(glm::vec3& obb_pos,const float&r,const float& size);
bool Cam_with_obj_collision(const float& r, glm::vec3& cam_pos, float minx, float maxz, float minz, float maxx);
bool Sphere_with_obj_collision(vec3& pos, const float& r, vec3 min, vec3 max,vec3 &n);



void handleObjectsCameraCollision(Camera& camera , OBB& cam_obb, float minx,float maxz,float minz,float maxx) {
    camera.speed = 3.0f;
    if (Cam_with_obj_collision(cam_obb.r, camera.position, minx, maxz, minz, maxx)) {
        camera.speed = 0;
    }
}

bool Cam_with_obj_collision(const float& r,glm::vec3& cam_pos,float minx, float maxz, float minz,float maxx) {
    if ((r >= abs(cam_pos.x - minx))&& (cam_pos.z<maxz && cam_pos.z >minz)) {
        cam_pos.x -= 0.1;
        return true;
    }
    if (r >= abs(cam_pos.z - maxz) && (cam_pos.x<maxx && cam_pos.x >minx)) {
        cam_pos.z += 0.1;
        return true;
    }
    if (r >= abs(cam_pos.z - minz) && (cam_pos.x<maxx && cam_pos.x >minx)) {
        cam_pos.z -= 0.1;
        return true;
    }
    if (r >= abs(cam_pos.x - maxx) && (cam_pos.z<maxz && cam_pos.x >minz)) {
        cam_pos.x += 0.1;
        return true;
    }
    else {
        return false;
    }
}

void handleSphereObjCollision(Sphere& sphere, vec3 min_xyz, vec3 max_xyz) {
    vec3 n;
    if (Sphere_with_obj_collision(sphere.x, sphere.r,min_xyz,max_xyz, n)) {
        // Task 2b: define the velocity of the sphere after the collision
        sphere.v = sphere.v - n * dot(sphere.v, n) * 1.99f;
        sphere.P = sphere.m * sphere.v;
    }
}

bool Sphere_with_obj_collision(vec3& pos, const float& r, vec3 min, vec3 max, vec3& n) {
    if ((r >= abs(pos.x - min.x)) && (pos.z<max.z && pos.z >min.z)&&(pos.y<max.y && pos.y >min.y)) {
        float dis = 0.01;
        pos = pos - vec3(dis, 0, 0);
        n = vec3(-1, 0, 0);
        return true;
    }
    if (r >= abs(pos.z - max.z) && (pos.x<max.x && pos.x >min.x) && (pos.y<max.y && pos.y >min.y)) {
        float dis = 0.01;
        pos = pos + vec3(0, 0, dis);
        n = vec3(0, 0, 1);
        return true;
    }
    if (r >= abs(pos.z - min.z) && (pos.x<max.x && pos.x >min.x) && (pos.y<max.y && pos.y >min.y)) {
        float dis = 0.01;
        pos = pos - vec3(0, 0, dis);
        n = vec3(0, 0, -1);
        return true;
    }
    if (r >= abs(pos.x - max.x) && (pos.z<max.z && pos.z >min.z) && (pos.y<max.y && pos.y >min.y)) {
        float dis = 0.01;
        pos = pos + vec3(dis, 0, 0);
        n = vec3(1, 0, 0);
        return true;
    }
    if (r >= abs(pos.y - max.y) && (pos.x<max.x && pos.x >min.x) && (pos.z<max.z && pos.z >min.z)) {
        float dis = 0.01;
        pos = pos + vec3(0, dis, 0);
        n = vec3(0, 1, 0);
        return true;
    }
    if (r >= abs(pos.y - min.y) && (pos.x<max.x && pos.x >min.x) && (pos.z<max.z && pos.z >min.z)) {
        float dis = 0.01;
        pos = pos + vec3(0, dis, 0);
        n = vec3(0, 1, 0);
        return true;
    }
    else {
        return false;
    }
}


void handleRoomCameraCollision(Box& box, OBB& cam_obb,Camera& camera){
    camera.speed = 3.0f;
    if (Cam_with_room_collision(camera.position, cam_obb.r, box.size)) {
        camera.speed = 0;
    }
}
bool Cam_with_room_collision(glm::vec3& cam_pos, const float& r, const float& size) {
    if (r >= abs(abs(cam_pos.x) - size)) {
        if (cam_pos.x < 0) {
            cam_pos.x +=0.001;
        }
        else if (cam_pos.x > 0) {
            cam_pos.x -= 0.001;  
        }
        return true;
        
    }
    if (r >= abs(abs(cam_pos.z) - size)) {
        if (cam_pos.z < 0)cam_pos.z += 0.001;
        else if (cam_pos.z > 0)cam_pos.z -= 0.001;
        return true;
    }
    else {
        return false;
    }
}

void handleBoxSphereCollision(Box& box, Sphere& sphere) {
    vec3 n;
    if (checkForBoxSphereCollision(sphere.x, sphere.r, box.size, n)) {
        // Task 2b: define the velocity of the sphere after the collision
        sphere.v =sphere.v-n*dot(sphere.v,n)*1.99f;
        sphere.P = sphere.m * sphere.v;
    }
}


bool checkForBoxSphereCollision(vec3& pos, const float& r,const float& size, vec3& n) {
    if (r >= abs(abs(pos.x) - size)) {
        if (pos.x < 0) {
            float dis = 0.01;
            pos = pos + vec3(dis, 0, 0);
            n = vec3(1, 0, 0);
        }
        else if (pos.x > 0) {
            float dis = -0.01;
            pos = pos + vec3(dis, 0, 0);
            n = vec3(-1, 0, 0);
        }
        return true;
    }
    if (r >= abs(pos.y)) {
        float dis = 0.01;
        pos = pos + vec3(0, dis, 0);
        n = vec3(0, 1, 0);
        return true;
    }
    else if (r >= abs(pos.y - 2 * size)) {
        float dis = -0.01;
        pos = pos + vec3(0, dis, 0);
        n = vec3(0, -1, 0);
        return true;
    }
    if (r >= abs(abs(pos.z) - size)) {
        if (pos.z < 0) {
            float dis = 0.01;
            pos = pos + vec3(0, 0, dis);
            n = vec3(0, 0, 1);
        }
        else if (pos.z > 0) {
            float dis = -0.01;
            pos = pos + vec3(0, 0, dis);
            n = vec3(0, 0, -1);
        }
        return true;
    }
    else {
        return false;
    }
}