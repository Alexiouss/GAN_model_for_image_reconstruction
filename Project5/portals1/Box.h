#ifndef BOX_H
#define BOX_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glfw3.h>

class Drawable;

/**
 * Represents the bounding box
 */
class Box {
public:
    
    struct objects_room1
    {
        glm::mat4 MM;
        glm::vec3 min_xyz;
        glm::vec3 max_xyz;
        glm::vec3 center;
    }drawer1, bed1, bed1_back, mattress1, toy1, portal1, portal2, inf_portal1, inf_portal2;
    void createMMforObj_room1();
    void createAABBWorldSpace_room1();
    Drawable* Floor;
    Drawable* Left_Wall;
    Drawable* Right_Wall;
    Drawable* Back_Wall;
    Drawable* Front_Wall;
    Drawable* Ceiling;
    Drawable* drawer;
    Drawable* bed_frame;
    Drawable* bed_mattress;
    Drawable* wooden_toy;
    Drawable* portal;
    Drawable* inf_portal;

    GLuint FloorDiffuseTexture, FloorSpecularTexture, FloorNormalTexture;
    GLuint WallDiffuseTexture, WallSpecularTexture, WallNormalTexture;
    GLuint CeilingDiffuseTexture, CeilingSpecularTexture;

    GLuint drawrerDiffuseTexture, drawrerSpecularTexture;
    GLuint bedframeDiffuseTexture, bedframeSpecTexture, bedmattressDiffTexture;
    GLuint woodentoyDiff;


    float size;
    glm::mat4 DownMM;
    glm::mat4 LeftMM;
    glm::mat4 RightMM;
    glm::mat4 BackMM;
    glm::mat4 FrontMM;
    glm::mat4 UpMM;


    Box(float to_scale);
    ~Box();

    void draw_down(unsigned int drawable = 0);
    void draw_left(unsigned int drawable = 0);
    void draw_right(unsigned int drawable = 0);
    void draw_back(unsigned int drawable = 0);
    void draw_front(unsigned int drawable = 0);
    void draw_up(unsigned int drawable = 0);

    void update();


    void createContext();
};

#endif
