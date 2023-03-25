#include "Box.h"
#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <common/model.h>
#include <common/texture.h>
#include <common/util.h>
using namespace glm;



Box::Box(float to_scale) {
    size = to_scale;
    Floor = new Drawable("floor.obj");
    Left_Wall = new Drawable("left_wall.obj");
    Right_Wall = new Drawable("right_wall.obj");
    Back_Wall = new Drawable("back_wall.obj");
    Front_Wall = new Drawable("front_wall.obj");
    Ceiling = new Drawable("ceiling.obj");
	drawer = new Drawable("drawrer.obj");
	portal = new Drawable("portal.obj");
	inf_portal = new Drawable("inf_portal.obj");
	bed_frame = new Drawable("bed_frame.obj");
	bed_mattress = new Drawable("bed_mattress.obj");
	wooden_toy = new Drawable("wooden_car.obj");
}

Box::~Box() {
    delete Floor;
    delete Left_Wall;
    delete Right_Wall;
    delete Back_Wall;
    delete Front_Wall;
    delete Ceiling;
}



void Box::createContext() {

    FloorDiffuseTexture = loadBMP("Wooden_floor.bmp");
    FloorSpecularTexture = loadBMP("Wooden_floor_specular.bmp");
    FloorNormalTexture = loadBMP("Wooden_floor_normal.bmp");

    WallDiffuseTexture = loadBMP("Brick_Wall.bmp");
    WallSpecularTexture = loadBMP("Brick_Wall_Spec.bmp");
    WallNormalTexture = loadBMP("Brick_Wall_normal.bmp");

    CeilingDiffuseTexture = loadBMP("Ceiling.bmp");
    CeilingSpecularTexture = loadBMP("Ceiling_Spec.bmp");

	bedframeDiffuseTexture = loadBMP("drawrer_diff.bmp");
	bedframeSpecTexture = loadBMP("drawrer_spec.bmp");

	bedmattressDiffTexture = loadBMP("bed_mattress_diff.bmp");
	woodentoyDiff = loadSOIL("wooden_toy_diff.bmp");
	drawrerDiffuseTexture = loadBMP("drawrer_diff.bmp");
	drawrerSpecularTexture = loadBMP("drawrer_spec.bmp");

    int i = 0;
    for (i = 0; i < Floor->uvs.size(); i++) {
        Floor->uvs[i] = vec2(Floor->uvs[i].x * size/2, Floor->uvs[i].y);
        Ceiling->uvs[i] = vec2(Ceiling->uvs[i].x * size / 2, Ceiling->uvs[i].y* size / 2);
        Left_Wall->uvs[i] = vec2(Left_Wall->uvs[i].x * size / 2, Left_Wall->uvs[i].y * size / 2);
        Right_Wall->uvs[i] = vec2(Right_Wall->uvs[i].x * size / 2, Right_Wall->uvs[i].y * size / 2);
        Back_Wall->uvs[i] = vec2(Back_Wall->uvs[i].x * size / 2, Back_Wall->uvs[i].y * size / 2);
        Front_Wall->uvs[i] = vec2(Front_Wall->uvs[i].x * size / 2, Front_Wall->uvs[i].y * size / 2);
    }
    Floor = new Drawable(Floor->vertices, Floor->uvs, Floor->normals, Floor->tangents);
    Ceiling = new Drawable(Ceiling->vertices, Ceiling->uvs, Ceiling->normals, Ceiling->tangents);
    Left_Wall = new Drawable(Left_Wall->vertices, Left_Wall->uvs, Left_Wall->normals, Left_Wall->tangents);
    Right_Wall = new Drawable(Right_Wall->vertices, Right_Wall->uvs, Right_Wall->normals, Right_Wall->tangents);
    Back_Wall = new Drawable(Back_Wall->vertices, Back_Wall->uvs, Back_Wall->normals, Back_Wall->tangents);
    Front_Wall = new Drawable(Front_Wall->vertices, Front_Wall->uvs, Front_Wall->normals, Front_Wall->tangents);


    
}

void Box::draw_down(unsigned int drawable) {
    Floor->bind();
    Floor->draw();
}

void Box::draw_left(unsigned int drawable) {
    Left_Wall->bind();
    Left_Wall->draw();
}

void Box::draw_right(unsigned int drawable) {
    Right_Wall->bind();
    Right_Wall->draw();
}

void Box::draw_back(unsigned int drawable) {
    Back_Wall->bind();
    Back_Wall->draw();
}

void Box::draw_front(unsigned int drawable) {
    Front_Wall->bind();
    Front_Wall->draw();
}

void Box::draw_up(unsigned int drawable) {
    Ceiling->bind();
    Ceiling->draw();
}



void Box::update() {
    DownMM = translate(mat4(), vec3(0, 0, 0)) * scale(mat4(), vec3(size));
    LeftMM = translate(mat4(), vec3(-size , size, 0)) * rotate(mat4(), radians(90.0f), vec3(1, 0, 0)) * scale(mat4(), vec3(size));
    RightMM = translate(mat4(), vec3(size  , size, 0)) * rotate(mat4(), radians(90.0f), vec3(1, 0, 0)) * scale(mat4(), vec3(size));
    BackMM = translate(mat4(), vec3(0, size,-size)) * scale(mat4(), vec3(size));
    FrontMM = translate(mat4(), vec3(0, size , size)) * rotate(mat4(), radians(180.0f), vec3(0, 0, 1)) * scale(mat4(), vec3(size));
    UpMM = translate(mat4(), vec3(0, 2*size ,0)) * scale(mat4(), vec3(size));
}

void Box::createMMforObj_room1() {
	drawer1.MM = translate(mat4(), vec3(8.0, 0.0, -8.0)) * scale(mat4(), vec3(2.0, 2.0, 2.0));
	bed1.MM = translate(mat4(), vec3(-8, 0.0, -5.5)) * scale(mat4(), vec3(4.0, 4.0, 4.0));
	mattress1.MM = translate(mat4(), vec3(-8, 0.0, -5.5)) * scale(mat4(), vec3(4.0, 4.0, 4.0));
	toy1.MM = translate(mat4(), vec3(5, 0, 3))  * scale(mat4(), vec3(0.5, 0.5, 0.5));
	drawer1.center = vec3(8.0, 0.0, -8.0);
	bed1.center = vec3(-8, 0.0, -5.5);
	mattress1.center = vec3(-8, 0.0, -5.5);
	toy1.center = vec3(5, 0, 3);
	portal1.MM = translate(mat4(), vec3(0, 6, -9.9)) * rotate(mat4(), radians(180.0f), vec3(0, 0, 1)) * rotate(mat4(), radians(90.0f), vec3(1, 0, 0)) * scale(mat4(), vec3(3, 1, 6));
	portal2.MM = translate(mat4(), vec3(-9.9, 6, 5)) * rotate(mat4(), radians(90.0f), vec3(0, 1, 0)) * rotate(mat4(), radians(90.0f), vec3(1, 0, 0)) * scale(mat4(), vec3(3, 1, 6));
	inf_portal1.MM = translate(mat4(), vec3(0, 0, -5)) * scale(mat4(), vec3(2, 1, 2));
	inf_portal2.MM = translate(mat4(), vec3(0, 20, -5)) * scale(mat4(), vec3(2, 1, 2));
}

void Box :: createAABBWorldSpace_room1() {
	//drawer AABB
	float max_y_drawer = drawer->vertices[0].y; float min_y_drawer = drawer->vertices[0].y;
	float max_x_drawer = drawer->vertices[0].x; float min_x_drawer = drawer->vertices[0].x;
	float max_z_drawer = drawer->vertices[0].z; float min_z_drawer = drawer->vertices[0].z;

	for (int i = 0; i < drawer->vertices.size(); i++) {
		if (drawer->vertices[i].y >= max_y_drawer)max_y_drawer = drawer->vertices[i].y;
		if (drawer->vertices[i].y < min_y_drawer)min_y_drawer = drawer->vertices[i].y;
		if (drawer->vertices[i].x >= max_x_drawer)max_x_drawer = drawer->vertices[i].x;
		if (drawer->vertices[i].x < min_x_drawer)min_x_drawer = drawer->vertices[i].x;
		if (drawer->vertices[i].z >= max_z_drawer)max_z_drawer = drawer->vertices[i].z;
		if (drawer->vertices[i].z < min_z_drawer)min_z_drawer = drawer->vertices[i].z;
	}
	drawer1.max_xyz = vec3(drawer1.MM * vec4(max_x_drawer, max_y_drawer, max_z_drawer, 1.0f));
	drawer1.min_xyz = vec3(drawer1.MM * vec4(min_x_drawer, min_y_drawer, min_z_drawer, 1.0f));



	//bed frame AABB
	float max_y_bed_frame = bed_frame->vertices[0].y; float min_y_bed_frame = bed_frame->vertices[0].y;
	float max_x_bed_frame = bed_frame->vertices[0].x; float min_x_bed_frame = bed_frame->vertices[0].x;
	float max_z_bed_frame = bed_frame->vertices[0].z; float min_z_bed_frame = bed_frame->vertices[0].z;
	for (int i = 0; i < bed_frame->vertices.size(); i++) {
		if (bed_frame->vertices[i].y >= max_y_bed_frame)max_y_bed_frame = bed_frame->vertices[i].y;
		if (bed_frame->vertices[i].y < min_y_bed_frame)min_y_bed_frame = bed_frame->vertices[i].y;
		if (bed_frame->vertices[i].x >= max_x_bed_frame)max_x_bed_frame = bed_frame->vertices[i].x;
		if (bed_frame->vertices[i].x < min_x_bed_frame)min_x_bed_frame = bed_frame->vertices[i].x;
		if (bed_frame->vertices[i].z >= max_z_bed_frame)max_z_bed_frame = bed_frame->vertices[i].z;
		if (bed_frame->vertices[i].z < min_z_bed_frame)min_z_bed_frame = bed_frame->vertices[i].z;
	}

	bed1.max_xyz = vec3(bed1.MM * vec4(max_x_bed_frame, max_y_bed_frame, max_z_bed_frame, 1.0f));
	bed1.min_xyz = vec3(bed1.MM * vec4(min_x_bed_frame, min_y_bed_frame, min_z_bed_frame, 1.0f));

	//bed mattress AABB
	float max_y_bed_mattress = bed_mattress->vertices[0].y; float min_y_bed_mattress = bed_mattress->vertices[0].y;
	float max_x_bed_mattress = bed_mattress->vertices[0].x; float min_x_bed_mattress = bed_mattress->vertices[0].x;
	float max_z_bed_mattress = bed_mattress->vertices[0].z; float min_z_bed_mattress = bed_mattress->vertices[0].z;

	for (int i = 0; i < bed_mattress->vertices.size(); i++) {
		if (bed_mattress->vertices[i].y >= max_y_bed_mattress)max_y_bed_mattress = bed_mattress->vertices[i].y;
		if (bed_mattress->vertices[i].y < min_y_bed_mattress)min_y_bed_mattress = bed_mattress->vertices[i].y;
		if (bed_mattress->vertices[i].x >= max_x_bed_mattress)max_x_bed_mattress = bed_mattress->vertices[i].x;
		if (bed_mattress->vertices[i].x < min_x_bed_mattress)min_x_bed_mattress = bed_mattress->vertices[i].x;
		if (bed_mattress->vertices[i].z >= max_z_bed_mattress)max_z_bed_mattress = bed_mattress->vertices[i].z;
		if (bed_mattress->vertices[i].z < min_z_bed_mattress)min_z_bed_mattress = bed_mattress->vertices[i].z;
	}

	mattress1.max_xyz = vec3(mattress1.MM * vec4(max_x_bed_mattress, max_y_bed_mattress, max_z_bed_mattress, 1.0f));
	mattress1.min_xyz = vec3(mattress1.MM * vec4(min_x_bed_mattress, min_y_bed_mattress, min_z_bed_mattress, 1.0f));

	//bed_back AABB
	bed1_back.max_xyz = vec3(bed1.max_xyz.x, bed1.max_xyz.y, mattress1.min_xyz.z);
	bed1_back.min_xyz = vec3(bed1.min_xyz.x, bed1.min_xyz.y, bed1.min_xyz.z);

	//toy AABB
	float max_y_toy = wooden_toy->vertices[0].y; float min_y_toy = wooden_toy->vertices[0].y;
	float max_x_toy = wooden_toy->vertices[0].x; float min_x_toy = wooden_toy->vertices[0].x;
	float max_z_toy = wooden_toy->vertices[0].z; float min_z_toy = wooden_toy->vertices[0].z;

	for (int i = 0; i < wooden_toy->vertices.size(); i++) {
		if (wooden_toy->vertices[i].y >= max_y_toy)max_y_toy = wooden_toy->vertices[i].y;
		if (wooden_toy->vertices[i].y < min_y_toy)min_y_toy = wooden_toy->vertices[i].y;
		if (wooden_toy->vertices[i].x >= max_x_toy)max_x_toy = wooden_toy->vertices[i].x;
		if (wooden_toy->vertices[i].x < min_x_toy)min_x_toy = wooden_toy->vertices[i].x;
		if (wooden_toy->vertices[i].z >= max_z_toy)max_z_toy = wooden_toy->vertices[i].z;
		if (wooden_toy->vertices[i].z < min_z_toy)min_z_toy = wooden_toy->vertices[i].z;
	}
	toy1.max_xyz = vec3(toy1.MM * vec4(max_x_toy, max_y_toy, max_z_toy, 1));
	toy1.min_xyz = vec3(toy1.MM * vec4(min_x_toy, min_y_toy, min_z_toy, 1));
	float max_y_portal = portal->vertices[0].y; float min_y_portal = portal->vertices[0].y;
	float max_x_portal = portal->vertices[0].x; float min_x_portal = portal->vertices[0].x;
	float max_z_portal = portal->vertices[0].z; float min_z_portal = portal->vertices[0].z;
	for (int i = 0; i < portal->vertices.size(); i++) {
		if (portal->vertices[i].y >= max_y_portal)max_y_portal = portal->vertices[i].y;
		if (portal->vertices[i].y < min_y_portal)min_y_portal = portal->vertices[i].y;
		if (portal->vertices[i].x >= max_x_portal)max_x_portal = portal->vertices[i].x;
		if (portal->vertices[i].x < min_x_portal)min_x_portal = portal->vertices[i].x;
		if (portal->vertices[i].z >= max_z_portal)max_z_portal = portal->vertices[i].z;
		if (portal->vertices[i].z < min_z_portal)min_z_portal = portal->vertices[i].z;
	}
	portal1.max_xyz = vec3(portal1.MM * vec4(max_x_portal, max_y_portal, max_z_portal, 1));
	portal1.min_xyz = vec3(portal1.MM * vec4(min_x_portal, min_y_portal, min_z_portal, 1));

	portal2.min_xyz = vec3(portal2.MM * vec4(max_x_portal, max_y_portal, max_z_portal, 1));
	portal2.max_xyz = vec3(portal2.MM * vec4(min_x_portal, min_y_portal, min_z_portal, 1));


	float max_y_portal_inf = inf_portal->vertices[0].y; float min_y_portal_inf = inf_portal->vertices[0].y;
	float max_x_portal_inf = inf_portal->vertices[0].x; float min_x_portal_inf = inf_portal->vertices[0].x;
	float max_z_portal_inf = inf_portal->vertices[0].z; float min_z_portal_inf = inf_portal->vertices[0].z;
	for (int i = 0; i < inf_portal->vertices.size(); i++) {
		if (inf_portal->vertices[i].y >= max_y_portal_inf)max_y_portal_inf = inf_portal->vertices[i].y;
		if (inf_portal->vertices[i].y < min_y_portal_inf)min_y_portal_inf = inf_portal->vertices[i].y;
		if (inf_portal->vertices[i].x >= max_x_portal_inf)max_x_portal_inf = inf_portal->vertices[i].x;
		if (inf_portal->vertices[i].x < min_x_portal_inf)min_x_portal_inf = inf_portal->vertices[i].x;
		if (inf_portal->vertices[i].z >= max_z_portal_inf)max_z_portal_inf = inf_portal->vertices[i].z;
		if (inf_portal->vertices[i].z < min_z_portal_inf)min_z_portal_inf = inf_portal->vertices[i].z;
	}
	inf_portal1.max_xyz = vec3(inf_portal1.MM * vec4(max_x_portal_inf, max_y_portal_inf, max_z_portal_inf, 1));
	inf_portal1.min_xyz = vec3(inf_portal1.MM * vec4(min_x_portal_inf, min_y_portal_inf, min_z_portal_inf, 1));

	inf_portal2.min_xyz = vec3(inf_portal2.MM * vec4(max_x_portal_inf, max_y_portal_inf, max_z_portal_inf, 1));
	inf_portal2.max_xyz = vec3(inf_portal2.MM * vec4(min_x_portal_inf, min_y_portal_inf, min_z_portal_inf, 1));
}
