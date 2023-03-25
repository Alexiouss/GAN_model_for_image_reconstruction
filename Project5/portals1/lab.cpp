// Include C++ headers
#include <iostream>
#include <string>
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Shader loading utilities and other
#include <common/shader.h>
#include <common/util.h>
#include <common/camera.h>
#include <common/model.h>
#include <common/texture.h>
#include <common/light.h> 
#include "Box.h"
#include "Obb.h"
#include "Collision.h"
#include "Sphere.h"
using namespace std;
using namespace glm;

// Function prototypes
void initialize();
void createContext();
void mainLoop();
void free();

#define W_WIDTH 1536
#define W_HEIGHT 1152
#define TITLE "Portals1"

#define SHADOW_WIDTH 2048
#define SHADOW_HEIGHT 2048


// Creating a structure to store the material parameters of an object
struct Material {
	vec4 Ka;
	vec4 Kd;
	vec4 Ks;
	float Ns;
};



// Global Variables
GLFWwindow* window;

Camera* camera;
Camera* camera_portal_1;
Camera* camera_portal_2;
Camera* camera_down_inf;
Camera* camera_up_inf;

Sphere* sphere;
Light* light;

Box* box;
OBB* cam_obb;
GLuint shaderProgram, depthProgram;
Drawable* lightsphere1;


GLuint cam_pos;

GLuint depthFrameBuffer, depthTexture;


// locations for shaderProgram
GLuint viewMatrixLocation;
GLuint projectionMatrixLocation;
GLuint modelMatrixLocation;
GLuint KaLocation, KdLocation, KsLocation, NsLocation;
GLuint LaLocation, LdLocation, LsLocation;
GLuint lightPositionLocation;
GLuint lightPositionLocationd;
GLuint lightPowerLocation;
GLuint diffuseColorSampler;
GLuint specularColorSampler;
GLuint normalColorSampler;
GLuint useTextureLocation;
GLuint depthMapSampler;
GLuint lightVPLocation;


GLuint farplaneloc;
GLuint farplaneloc1;

// locations for depthProgram
GLuint shadowViewProjectionLocation[6];
//GLuint shadowViewProjectionLocation;
GLuint shadowModelLocation;



// Create two sample materials
const Material polishedSilver{
	vec4{0.23125, 0.23125, 0.23125, 1},
	vec4{0.2775, 0.2775, 0.2775, 1},
	vec4{0.773911, 0.773911, 0.773911, 1},
	89.6f
};


// Creating a function to upload (make uniform) the light parameters to the shader program
void uploadLight(const Light& light) {
	glUniform4f(LaLocation, light.La.r, light.La.g, light.La.b, light.La.a);
	glUniform4f(LdLocation, light.Ld.r, light.Ld.g, light.Ld.b, light.Ld.a);
	glUniform4f(LsLocation, light.Ls.r, light.Ls.g, light.Ls.b, light.Ls.a);
	glUniform3f(lightPositionLocation, light.lightPosition_worldspace.x,
		light.lightPosition_worldspace.y, light.lightPosition_worldspace.z);
	glUniform1f(lightPowerLocation, light.power);
}

// Creating a function to upload the material parameters of a model to the shader program
void uploadMaterial(const Material& mtl) {
	glUniform4f(KaLocation, mtl.Ka.r, mtl.Ka.g, mtl.Ka.b, mtl.Ka.a);
	glUniform4f(KdLocation, mtl.Kd.r, mtl.Kd.g, mtl.Kd.b, mtl.Kd.a);
	glUniform4f(KsLocation, mtl.Ks.r, mtl.Ks.g, mtl.Ks.b, mtl.Ks.a);
	glUniform1f(NsLocation, mtl.Ns);
}
#define g 9.80665f

float to_scale = 10.0;


float near_plane = 1.0f;
float far_plane = 100.0f;

glm::mat4 lightProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH/(float)SHADOW_HEIGHT, near_plane, far_plane);
std::vector<glm::mat4> shadowTransforms;

vec3 lightPos;

void createContext() {

	// Create and complile our GLSL program from the shader
	shaderProgram = loadShaders("ShadowMapping.vertexshader", "ShadowMapping.fragmentshader");

	depthProgram = loadShaders("Depth.vertexshader", "Depth.fragmentshader","Depth.geom");

	// --- shaderProgram ---
	projectionMatrixLocation = glGetUniformLocation(shaderProgram, "P");
	viewMatrixLocation = glGetUniformLocation(shaderProgram, "V");
	modelMatrixLocation = glGetUniformLocation(shaderProgram, "M");
	

	cam_pos = glGetUniformLocation(shaderProgram,"cam_pos");
	// for phong lighting
	KaLocation = glGetUniformLocation(shaderProgram, "mtl.Ka");
	KdLocation = glGetUniformLocation(shaderProgram, "mtl.Kd");
	KsLocation = glGetUniformLocation(shaderProgram, "mtl.Ks");
	NsLocation = glGetUniformLocation(shaderProgram, "mtl.Ns");
	LaLocation = glGetUniformLocation(shaderProgram, "light.La");
	LdLocation = glGetUniformLocation(shaderProgram, "light.Ld");
	LsLocation = glGetUniformLocation(shaderProgram, "light.Ls");
	lightPositionLocation = glGetUniformLocation(shaderProgram, "light.lightPosition_worldspace");
	lightPowerLocation = glGetUniformLocation(shaderProgram, "light.power");
	diffuseColorSampler = glGetUniformLocation(shaderProgram, "diffuseColorSampler");
	specularColorSampler = glGetUniformLocation(shaderProgram, "specularColorSampler");
	normalColorSampler = glGetUniformLocation(shaderProgram, "normalColorSampler");

	farplaneloc1 = glGetUniformLocation(shaderProgram, "far_plane");

	// Task 1.4
	useTextureLocation = glGetUniformLocation(shaderProgram, "useTexture");

	// locations for shadow rendering
	depthMapSampler = glGetUniformLocation(shaderProgram, "shadowMapSampler");


	// --- depthProgram ---
	shadowViewProjectionLocation[0] = glGetUniformLocation(depthProgram, "shadowMatrices[0]" );
	shadowViewProjectionLocation[1] = glGetUniformLocation(depthProgram, "shadowMatrices[1]");
	shadowViewProjectionLocation[2] = glGetUniformLocation(depthProgram, "shadowMatrices[2]");
	shadowViewProjectionLocation[3] = glGetUniformLocation(depthProgram, "shadowMatrices[3]");
	shadowViewProjectionLocation[4] = glGetUniformLocation(depthProgram, "shadowMatrices[4]");
	shadowViewProjectionLocation[5] = glGetUniformLocation(depthProgram, "shadowMatrices[5]");

	farplaneloc = glGetUniformLocation(depthProgram, "far_plane");
	lightPositionLocationd = glGetUniformLocation(depthProgram, "lightPos");
	shadowModelLocation = glGetUniformLocation(depthProgram, "M");

	
	box = new Box(to_scale);
	box->createContext();

	// 1. Using Drawable to load suzanne
	cam_obb = new OBB();

	sphere = new Sphere(vec3(0, 3.5, 0), vec3(0,0,0), 0.3, 1);
	
	cam_obb->Cam_OBB(camera->position);

	int i = 0;
	lightsphere1 = new Drawable("bulb.obj");
	for (i = 0; i < lightsphere1->normals.size(); i++)lightsphere1->normals[i] = -lightsphere1->normals[i];
	lightsphere1 = new Drawable(lightsphere1->vertices, lightsphere1->uvs, lightsphere1->normals);


	glGenFramebuffers(1, &depthFrameBuffer);
	// create depth cubemap texture
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthTexture);
	for (unsigned int i = 0; i < 6; ++i)glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		glfwTerminate();
		throw runtime_error("Frame buffer not initialized correctly");
	}

	// Binding the default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//
}


void light_VP(vec3 LightPos) {
	shadowTransforms.push_back(lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(lightProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
}

void free() {
	// Delete Shader Programs
	glDeleteProgram(shaderProgram);
	glDeleteProgram(depthProgram);
	box->~Box();
	delete box->drawer;
	delete box->bed_frame;
	delete box->bed_mattress;
	delete box->wooden_toy;
	sphere->~Sphere();
	glfwTerminate();
}

void CreateBox(GLuint modelMatrixloc) {
	//glDisable(GL_CULL_FACE);

	glUniform1i(useTextureLocation, 2);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, box->FloorDiffuseTexture);
	glUniform1i(diffuseColorSampler, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, box->FloorSpecularTexture);
	glUniform1i(specularColorSampler, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, box->FloorNormalTexture);
	glUniform1i(normalColorSampler, 3);

	box->update();

	glUniformMatrix4fv(modelMatrixloc, 1, GL_FALSE, &box->DownMM[0][0]);
	box->draw_down();

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, box->WallDiffuseTexture);
	glUniform1i(diffuseColorSampler, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, box->WallSpecularTexture);
	glUniform1i(specularColorSampler, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, box->WallNormalTexture);
	glUniform1i(normalColorSampler, 3);


	glUniformMatrix4fv(modelMatrixloc, 1, GL_FALSE, &box->BackMM[0][0]);
	box->draw_back();

	glUniformMatrix4fv(modelMatrixloc, 1, GL_FALSE, &box->FrontMM[0][0]);
	box->draw_front();

	glUniformMatrix4fv(modelMatrixloc, 1, GL_FALSE, &box->LeftMM[0][0]);
	box->draw_left();

	glUniformMatrix4fv(modelMatrixloc, 1, GL_FALSE, &box->RightMM[0][0]);
	box->draw_right();

	glUniform1i(useTextureLocation, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, box->CeilingDiffuseTexture);
	glUniform1i(diffuseColorSampler, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, box->CeilingSpecularTexture);
	glUniform1i(specularColorSampler, 2);

	glUniformMatrix4fv(modelMatrixloc, 1, GL_FALSE, &box->UpMM[0][0]);
	box->draw_up();
}

void depth_pass(std::vector<glm::mat4> shadowTransforms, vec3 lightPos, float far_plane,int flag) {

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(depthProgram);
	for (unsigned int i = 0; i < 6; ++i) glUniformMatrix4fv(shadowViewProjectionLocation[i], 1, GL_FALSE, &shadowTransforms[i][0][0]);
	glUniform3fv(lightPositionLocationd,1,&lightPos[0]);
	glUniform1f(farplaneloc, far_plane);
	
	glCullFace(GL_FRONT);
	
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &box->drawer1.MM[0][0]);
	box->drawer->bind();
	box->drawer->draw();

	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &box->bed1.MM[0][0]);
	box->bed_frame->bind();
	box->bed_frame->draw();

	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &box->mattress1.MM[0][0]);
	box->bed_mattress->bind();
	box->bed_mattress->draw();

	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &box->toy1.MM[0][0]);
	box->wooden_toy->bind();
	box->wooden_toy->draw();

	if (flag == 1 || flag==2) {
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &sphere->modelMatrix[0][0]);
		sphere->draw();
	}
	
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void portal_draw(mat4 viewMatrix,mat4 projectionMatrix,mat4 portal_MM,Drawable* portal) {
	glEnable(GL_STENCIL_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glStencilFunc(GL_NEVER, 1, 0xFF);
	glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
	glViewport(0, 0, W_WIDTH, W_HEIGHT);
	//glStencilOp(GL_INCR, GL_KEEP, GL_KEEP);
	glUseProgram(shaderProgram);
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	glStencilMask(0xFF);
	glClear(GL_STENCIL_BUFFER_BIT);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &portal_MM[0][0]);
	portal->bind();
	portal->draw();
}

void portal_draw_2(mat4 viewMatrix, mat4 projectionMatrix,mat4 portal_MM,Drawable* portal) {
	glEnable(GL_STENCIL_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glStencilFunc(GL_NEVER, 1, 0xFF);
	glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
	glStencilMask(0xFF);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &portal_MM[0][0]);
	//uploadMaterial(turquoise);
	portal->bind();
	portal->draw();
}

void portalPass(mat4 viewMatrix, mat4 projectionMatrix, int flag, Camera* cam) {
	glEnable(GL_DEPTH_TEST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	//glViewport(0, 0, W_WIDTH, W_HEIGHT);
	glStencilMask(0x00);
	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glUseProgram(shaderProgram);
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniform1f(farplaneloc1, far_plane);
	glUniform3f(cam_pos, cam->position.x, cam->position.y, cam->position.z);

	// Task 4.1 Display shadows on the 
	//*/
	// Sending the shadow texture to the shaderProgram
	glCullFace(GL_BACK);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthTexture);
	//glBindTexture(GL_TEXTURE_2D, depthTexture);
	glUniform1i(depthMapSampler, 0);

	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &box->drawer1.MM[0][0]);
	glUniform1i(useTextureLocation, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, box->drawrerDiffuseTexture);
	glUniform1i(diffuseColorSampler, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, box->drawrerSpecularTexture);
	glUniform1i(specularColorSampler, 2);
	box->drawer->bind();
	box->drawer->draw();


	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &box->bed1.MM[0][0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, box->bedframeDiffuseTexture);
	glUniform1i(diffuseColorSampler, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, box->bedframeSpecTexture);
	glUniform1i(specularColorSampler, 2);
	box->bed_frame->bind();
	box->bed_frame->draw();


	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &box->mattress1.MM[0][0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, box->bedmattressDiffTexture);
	glUniform1i(diffuseColorSampler, 1);
	box->bed_mattress->bind();
	box->bed_mattress->draw();


	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &box->toy1.MM[0][0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, box->woodentoyDiff);
	glUniform1i(diffuseColorSampler, 1);
	box->wooden_toy->bind();
	box->wooden_toy->draw();
	if (flag != 0) {
		glUniform1i(useTextureLocation, 0);
		uploadMaterial(polishedSilver);
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &sphere->modelMatrix[0][0]);
		sphere->draw();
	}
	glEnable(GL_CULL_FACE);
	CreateBox(modelMatrixLocation);
	glDisable(GL_CULL_FACE);
	//1
	glUniform1i(useTextureLocation, 0);
	uploadMaterial(polishedSilver);
	mat4 lightsphere1_model = translate(mat4(), vec3(light->lightPosition_worldspace)) * scale(mat4(), vec3(0.5f, 0.5f, 0.5f));
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &lightsphere1_model[0][0]);
	lightsphere1->bind();
	lightsphere1->draw();

}

void portals_depth(mat4 viewMatrix, mat4 projectionMatrix, mat4 portal1_MM, mat4 portal2_MM, Drawable* portal) {
	glUseProgram(shaderProgram);
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniform1f(farplaneloc1, far_plane);
	glDisable(GL_STENCIL_TEST);
	glStencilMask(0x00);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_ALWAYS);
	glClear(GL_DEPTH_BUFFER_BIT);
	//*
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &portal1_MM[0][0]);
	portal->bind();
	portal->draw();

	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &portal2_MM[0][0]);
	portal->bind();
	portal->draw();
	//*/
	glDepthFunc(GL_LESS);
}

void lighting_pass(mat4 viewMatrix, mat4 projectionMatrix,int flag) {
	// Step 1: Binding a frame buffer
	glViewport(0, 0, W_WIDTH, W_HEIGHT);
	glEnable(GL_STENCIL_TEST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glStencilMask(0x00);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

	glUseProgram(shaderProgram);
	// Making view and projection matrices uniform to the shader program
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniform1f(farplaneloc1, far_plane);
	glUniform3f(cam_pos, camera->position.x, camera->position.y, camera->position.z);
	// uploading the light parameters to the shader program
	uploadLight(*light);


	// Task 4.1 Display shadows on the 
	//*/
	// Sending the shadow texture to the shaderProgram
	glCullFace(GL_BACK);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthTexture);
	//glBindTexture(GL_TEXTURE_2D, depthTexture);
	glUniform1i(depthMapSampler, 0);

	// Sending the light View-Projection matrix to the shader program
	//mat4 lightVP = light->projectionMatrix * light->viewMatrix;
	//glUniformMatrix4fv(lightVPLocation, 1, GL_FALSE, &lightVP[0][0]);

	glStencilFunc(GL_GEQUAL, 1, 0xFF);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &box->drawer1.MM[0][0]);
	glUniform1i(useTextureLocation, 1);
	glActiveTexture(GL_TEXTURE1);								
	glBindTexture(GL_TEXTURE_2D, box->drawrerDiffuseTexture);			
	glUniform1i(diffuseColorSampler, 1);						

	glActiveTexture(GL_TEXTURE2);								
	glBindTexture(GL_TEXTURE_2D,box->drawrerSpecularTexture);
	glUniform1i(specularColorSampler, 2);
	box->drawer->bind();
	box->drawer->draw();

	
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &box->bed1.MM[0][0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, box->bedframeDiffuseTexture);
	glUniform1i(diffuseColorSampler, 1);
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, box->bedframeSpecTexture);
	glUniform1i(specularColorSampler, 2);
	box->bed_frame->bind();
	box->bed_frame->draw();
	

	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &box->mattress1.MM[0][0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, box->bedmattressDiffTexture);
	glUniform1i(diffuseColorSampler, 1);
	box->bed_mattress->bind();
	box->bed_mattress->draw();
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &box->toy1.MM[0][0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, box->woodentoyDiff);
	glUniform1i(diffuseColorSampler, 1);
	box->wooden_toy->bind();
	box->wooden_toy->draw();
	if (flag!=0) {
		glStencilFunc(GL_GEQUAL, 1, 0xFF);
		glUniform1i(useTextureLocation, 0);
		uploadMaterial(polishedSilver);
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &sphere->modelMatrix[0][0]);
		sphere->draw();
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	}
	glDisable(GL_CULL_FACE);
	CreateBox(modelMatrixLocation);
	glEnable(GL_CULL_FACE);
	//1
	glUniform1i(useTextureLocation, 0);

	glStencilFunc(GL_GEQUAL, 1, 0xFF);
	mat4 lightsphere1_model = translate(mat4(), vec3(light->lightPosition_worldspace)) * scale(mat4(), vec3(0.5f, 0.5f, 0.5f));
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &lightsphere1_model[0][0]);
	lightsphere1->bind();
	lightsphere1->draw();
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
}

void calc_portal_cameras(int inf) {

	if (inf == 1) {
		camera_up_inf->position = vec3(0, 25, -5);
		camera_up_inf->direction = vec3(0, -1, 0);

		camera_up_inf->right = vec3(
			-cos(camera->horizontalAngle),
			0,
			-sin(camera->horizontalAngle)
		);

		vec3 up_inf_up = cross(camera_up_inf->right, camera_up_inf->direction);
		//vec3 up_inf_up = cross(camera_up_inf->right, camera_up_inf->direction);
		camera_up_inf->projectionMatrix = perspective(radians(60.0f), 4.0f / 3.0f, 0.1f, 1000.0f);
		camera_up_inf->viewMatrix = lookAt(
			camera_up_inf->position,
			camera_up_inf->position + camera_up_inf->direction,
			up_inf_up
		);
	}
	else {
		camera_up_inf->position = vec3(0, 6, 10);
		camera_up_inf->direction = vec3(0, 0,-1);

		camera_up_inf->right = vec3(
			-cos(camera->horizontalAngle),
			0,
			-sin(camera->horizontalAngle)
		);

		vec3 up_inf_up = cross(camera_up_inf->right, camera_up_inf->direction);
		
		camera_up_inf->projectionMatrix = perspective(radians(60.0f), 4.0f / 3.0f, 0.1f, 1000.0f);
		camera_up_inf->viewMatrix = lookAt(
			camera_up_inf->position,
			camera_up_inf->position + camera_up_inf->direction,
			up_inf_up
		);

		camera_down_inf->position = vec3(0, 6, -10);
		camera_down_inf->direction = vec3(0, 0, 1);

		camera_down_inf->right = vec3(
			cos(camera->horizontalAngle),
			0,
			sin(camera->horizontalAngle)
		);

		vec3 up_inf_down = cross(camera_down_inf->right, camera_down_inf->direction);
		
		camera_down_inf->projectionMatrix = perspective(radians(60.0f), 4.0f / 3.0f, 0.1f, 1000.0f);
		camera_down_inf->viewMatrix = lookAt(
			camera_down_inf->position,
			camera_down_inf->position + camera_down_inf->direction,
			up_inf_down
		);

	}

	camera_portal_1->position = vec3(-2 * 9.9 - camera->position.z, 4, 5 + camera->position.x);
	camera_portal_1->direction = vec3(
		cos(camera->verticalAngle) * sin(camera->horizontalAngle - 3.14f / 2.0f),
		sin(camera->verticalAngle),
		cos(camera->verticalAngle) * cos(camera->horizontalAngle - 3.14f / 2.0f));
	camera_portal_1->right = vec3(
		sin(camera->horizontalAngle - 3.14f),
		0,
		cos(camera->horizontalAngle - 3.14f)
	);
	//*/
	//*
	vec3 up = cross(camera_portal_1->right, camera_portal_1->direction);
	// Task 5.7: construct projection and view matrices

	camera_portal_1->projectionMatrix = perspective(radians(60.0f), 4.0f / 3.0f, 0.1f, 1000.0f);

	camera_portal_1->viewMatrix = lookAt(
		camera_portal_1->position,
		camera_portal_1->position + camera_portal_1->direction,
		up
	);


	camera_portal_2->position = vec3(-5 + camera->position.z, 4, -2 * 9.9 - camera->position.x);
	camera_portal_2->direction = vec3(
		cos(camera->verticalAngle) * sin(camera->horizontalAngle + 3.14f / 2.0f),
		sin(camera->verticalAngle),
		cos(camera->verticalAngle) * cos(camera->horizontalAngle + 3.14f / 2.0f));
	camera_portal_2->right = vec3(
		sin(camera->horizontalAngle),
		0,
		cos(camera->horizontalAngle)
	);
	vec3 up_2 = cross(camera_portal_2->right, camera_portal_2->direction);
	// Task 5.7: construct projection and view matrices

	camera_portal_2->projectionMatrix = perspective(radians(60.0f), 4.0f / 3.0f, 0.1f, 1000.0f);

	camera_portal_2->viewMatrix = lookAt(
		camera_portal_2->position,
		camera_portal_2->position + camera_portal_2->direction,
		up_2
	);
}

void mainLoop() {

	light_VP(light->lightPosition_worldspace);
	box->createMMforObj_room1();
	box->createAABBWorldSpace_room1();

	//depth_pass(light_view, light_proj);
	int flag = 0;
	float vel = 0;
	float t = glfwGetTime();
	vec3 direction_of_sphere;
	vec3 temp_camera_position=vec3(-9.9,4,5);
	vec3 temp_dir = camera->right;
	vec3 temp_right = -camera->direction;

	vec3 last_cam_pos = vec3(0,0,0);
	float portal_time= 0;
	float portal_time_inf = 0;
	float portal_time_player = 0;
	int inf = 0;

	
	do {
		box->createMMforObj_room1();
		camera->update();
		float distance_from_drawer = abs(length(camera->position - box->drawer1.center));
		float distance_from_bed = abs(length(camera->position - box->bed1.center));
		float distance_from_toy = abs(length(camera->position - box->toy1.center));
		if (inf != 0)handleRoomCameraCollision(*box, *cam_obb, *camera);
		if (((camera->position.x < -3.0 || camera->position.x>3.0) || (camera->position.z > -9.7) || (camera->position.y > 12.0)) &&
			((camera->position.z > box->portal2.max_xyz.z || camera->position.z < box->portal2.min_xyz.z) || (camera->position.x > -9.7) || (sphere->x.y > 12.0))) {
			if (((camera->position.z<box->drawer1.max_xyz.z && camera->position.z > box->drawer1.min_xyz.z) ||
				(camera->position.x<box->drawer1.max_xyz.x && camera->position.x >box->drawer1.min_xyz.x)) && ((distance_from_drawer < distance_from_bed) && (distance_from_drawer < distance_from_toy))) {
				handleObjectsCameraCollision(*camera, *cam_obb, box->drawer1.min_xyz.x, box->drawer1.max_xyz.z, box->drawer1.min_xyz.z, box->drawer1.max_xyz.x);
			}
			else {
				handleRoomCameraCollision(*box, *cam_obb, *camera);
			}
			if (((camera->position.z<box->bed1.max_xyz.z && camera->position.z > box->bed1.min_xyz.z) ||
				(camera->position.x<box->bed1.max_xyz.x && camera->position.x >box->bed1.min_xyz.x)) && ((distance_from_bed < distance_from_drawer) && (distance_from_bed < distance_from_toy) && camera->position.x > -9.7)) {
				handleObjectsCameraCollision(*camera, *cam_obb, box->bed1.min_xyz.x, box->bed1.max_xyz.z, box->bed1.min_xyz.z, box->bed1.max_xyz.x);
			}
			else {
				handleRoomCameraCollision(*box, *cam_obb, *camera);
			}
			if (((camera->position.z<box->mattress1.max_xyz.z && camera->position.z > box->mattress1.min_xyz.z) ||
				(camera->position.x<box->mattress1.max_xyz.x && camera->position.x > box->mattress1.min_xyz.x)) && ((distance_from_bed < distance_from_drawer) && (distance_from_bed < distance_from_toy))) {
				handleObjectsCameraCollision(*camera, *cam_obb, box->mattress1.min_xyz.x, box->mattress1.max_xyz.z, box->mattress1.min_xyz.z, box->mattress1.max_xyz.x);
			}
			else {
				handleRoomCameraCollision(*box, *cam_obb, *camera);
			}
			if ((camera->position.z<box->toy1.max_xyz.z && camera->position.z > box->toy1.min_xyz.z)/* ||
				(camera->position.x<toy1.max_xyz.x && camera->position.x > toy1.min_xyz.x))*/ && ((distance_from_toy < distance_from_drawer) && (distance_from_toy < distance_from_bed) && camera->position.x < 9.7)) {
				handleObjectsCameraCollision(*camera, *cam_obb, box->toy1.min_xyz.x, box->toy1.max_xyz.z, box->toy1.min_xyz.z, box->toy1.max_xyz.x);
			}
			else {
				handleRoomCameraCollision(*box, *cam_obb, *camera);
			}
		}
		calc_portal_cameras(inf);

		float currentTime = glfwGetTime();
		float dt = currentTime - t;

		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
			vel = 0;
			sphere->v = vec3(0);
			sphere->P = vec3(0);
			flag = 1;
		}
		if ((flag == 1) && (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)) {
			if (vel >= 10) {
				vel = 10;
			}
			vel += 0.05;
			if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
				flag = 2;
				direction_of_sphere = normalize(camera->direction);
				sphere->v = vel * direction_of_sphere;
				sphere->P = sphere->m * sphere->v;
			}
		}
		if (flag == 1) {
			sphere->x = vec3(camera->position.x+2*sin(camera->horizontalAngle), camera->position.y+2*sin(camera->verticalAngle), camera->position.z - 2 * (-cos(camera->horizontalAngle))*cos(camera->verticalAngle));
		}

		if (flag == 2||flag==3) {
			if (sphere->x.y > box->mattress1.max_xyz.y) {
				handleSphereObjCollision(*sphere, box->bed1_back.min_xyz, box->bed1_back.max_xyz);
			}
			else {
				handleSphereObjCollision(*sphere, box->bed1.min_xyz, box->bed1.max_xyz);
			}
			handleSphereObjCollision(*sphere, box->mattress1.min_xyz, box->mattress1.max_xyz);
			if (inf == 0) {
				if (((sphere->x.x < -3.0 || sphere->x.x>3.0) || (sphere->x.z > -9.7) || (sphere->x.y > 12.0)) &&
					((sphere->x.z > box->portal2.max_xyz.z || sphere->x.z < box->portal2.min_xyz.z) || (sphere->x.x > -9.7) || (sphere->x.y > 12.0))) {
					handleBoxSphereCollision(*box, *sphere);
				}
			}
			else if (inf !=0) {
				if (((sphere->x.z > box->inf_portal1.max_xyz.z || sphere->x.z < box->inf_portal1.min_xyz.z) || (sphere->x.x > box->inf_portal1.max_xyz.x || sphere->x.x < box->inf_portal1.min_xyz.x))) {
					handleBoxSphereCollision(*box, *sphere);
				}
			}
			if ((sphere->x.x > -3.0 && sphere->x.x < 3.0) && (sphere->x.z<-10) && (sphere->x.y<12.0)) {
				portal_time=glfwGetTime();
				float last_pos_x_sphere = sphere->x.x;
				vec3 direction = normalize(sphere->v);
				vec3  new_direction = normalize(vec3(-direction.z, direction.y, direction.x));
				sphere->x = vec3(-10.0, sphere->x.y,5+last_pos_x_sphere);
				sphere->v = length(sphere->v)*  new_direction;
				sphere->P = sphere->m * sphere->v;
			}
			float temp_time = portal_time + 1;
			if ((sphere->x.z > 2.0 && sphere->x.x < 8.0) && (sphere->x.x < -10) && (sphere->x.y < 12.0) && (currentTime > temp_time)) {
				float last_pos_z_sphere = sphere->x.z;
				vec3 direction = normalize(sphere->v);
				vec3  new_direction = normalize(vec3(direction.z, direction.y, -direction.x));
				sphere->x = vec3(-5 + last_pos_z_sphere, sphere->x.y, -10);
				sphere->v = length(sphere->v) * new_direction;
				sphere->P = sphere->m * sphere->v;
			}
			if ((sphere->x.x > box->inf_portal1.min_xyz.x && sphere->x.x < box->inf_portal1.max_xyz.x) && (sphere->x.z > box->inf_portal1.min_xyz.z && sphere->x.z < box->inf_portal1.max_xyz.z) && (sphere->x.y < 0) && (inf == 2)) {
				float last_pos_x_sphere = sphere->x.x;
				float last_pos_z_sphere = sphere->x.z;
				vec3  new_direction = vec3(0, 0, -1);
				sphere->x = vec3(last_pos_x_sphere, 6+(-5- last_pos_z_sphere), 9.7);
				sphere->v = length(sphere->v) * new_direction;
				sphere->P = sphere->m * sphere->v;
			}
			else if ((sphere->x.x > box->inf_portal1.min_xyz.x && sphere->x.x < box->inf_portal1.max_xyz.x) && (sphere->x.z > box->inf_portal1.min_xyz.z && sphere->x.z < box->inf_portal1.max_xyz.z) && (sphere->x.y <0) && (inf == 1)) {
				float last_pos_x_sphere = sphere->x.x;
				float last_pos_z_sphere = sphere->x.z;
				vec3  new_direction = vec3(0, -1, 0);
				sphere->x = vec3(last_pos_x_sphere, 20, last_pos_z_sphere);
				sphere->v = length(sphere->v) * new_direction;
				sphere->P = sphere->m * sphere->v;
			}
			
			handleSphereObjCollision(*sphere, box->drawer1.min_xyz, box->drawer1.max_xyz);
			handleSphereObjCollision(*sphere, box->toy1.min_xyz, box->toy1.max_xyz);
			
		}

		sphere->forcing = [&](float t, const vector<float>& y)->vector<float> {
			vector<float> f(6, 0.0f);
			if (flag == 1)f[1] = 0;
			else if(flag==2 && inf==0) {
				f[1] =-g*sphere->m;
			}
			else if (flag == 2 && (inf == 1 || inf ==2)) {
				f[1] = -g * sphere->m +0.9*length(sphere->v);
				if (f[1] >=-0.01 && inf==1) {
					f[1] = 0;
					return f;
				}
				else if (inf == 2) {
					f[1] = -g * sphere->m;
					return f;
				}
			}
			return f;
		};
		sphere->update(t, dt);

		
		
		depth_pass(shadowTransforms, light->lightPosition_worldspace, far_plane,flag);
		mat4 projectionMatrix1 = camera_portal_1->projectionMatrix;
		mat4 viewMatrix1 = camera_portal_1->viewMatrix;
		mat4 projectionMatrix = camera->projectionMatrix;
		mat4 viewMatrix = camera->viewMatrix;
		mat4 projectionMatrix2 = camera_portal_2->projectionMatrix;
		mat4 viewMatrix2 = camera_portal_2->viewMatrix;

		mat4 projectionMatinfup = camera_up_inf->projectionMatrix;
		mat4 viewMatInfup = camera_up_inf->viewMatrix;

		mat4 projectionMatinfdown = camera_down_inf->projectionMatrix;
		mat4 viewMatInfdown = camera_down_inf->viewMatrix;

		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
			inf = 0;
		}
		if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
			inf = 1;
		}
		if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
			inf = 2;
		}
		if (inf == 0) {
			portal_draw(viewMatrix, projectionMatrix,box->portal1.MM,box->portal);
			portal_draw_2(viewMatrix, projectionMatrix,box->portal2.MM,box->portal);
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glDepthMask(GL_TRUE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			portalPass(viewMatrix1, projectionMatrix1, flag,camera_portal_1);
			portalPass(viewMatrix2, projectionMatrix2, flag,camera_portal_2);
			portals_depth(viewMatrix, projectionMatrix,box->portal1.MM,box->portal2.MM,box->portal);
			if ((camera->position.x < box->portal1.min_xyz.x && camera->position.x>box->portal1.max_xyz.x) && (camera->position.z < -9.8)) {
				portal_time_player = glfwGetTime();
				float last_pos_x = camera->position.x;
				camera->position = vec3(-9.9, 4, 5 + last_pos_x);
				camera->horizontalAngle -= 3.14 / 2;
			}
			float temp_time = portal_time_player + 0.5;
			if ((camera->position.z > box->portal2.min_xyz.z && camera->position.z < box->portal2.max_xyz.z) && (camera->position.x < -9.8) && (currentTime >= temp_time)) {
				float last_pos_z = camera->position.z;
				camera->position = vec3(-5 + last_pos_z, 4, -9.8);
				camera->horizontalAngle += 3.14 / 2;
			}
		}
		else if(inf==1) {
			portal_draw(viewMatrix, projectionMatrix, box->inf_portal1.MM,box->inf_portal);
			portal_draw_2(viewMatrix, projectionMatrix, box->inf_portal2.MM, box->inf_portal);
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glDepthMask(GL_TRUE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			portalPass(viewMatInfup, projectionMatinfup, flag,camera_up_inf);
			portals_depth(viewMatrix, projectionMatrix,box->inf_portal1.MM,box->inf_portal2.MM,box->inf_portal);
		}
		else if (inf == 2) {
			box->inf_portal2.MM = translate(mat4(), vec3(0, 6, 9.9)) * rotate(mat4(), radians(180.0f), vec3(0, 0, 1)) * rotate(mat4(), radians(90.0f), vec3(1, 0, 0)) * scale(mat4(), vec3(2, 1, 2));
			portal_draw(viewMatrix, projectionMatrix, box->inf_portal1.MM, box->inf_portal);
			portal_draw_2(viewMatrix, projectionMatrix, box->inf_portal2.MM, box->inf_portal);
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glDepthMask(GL_TRUE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			portalPass(viewMatInfup, projectionMatinfup, flag, camera_up_inf);
			portalPass(viewMatInfdown, projectionMatinfdown, flag, camera_down_inf);
			portals_depth(viewMatrix, projectionMatrix, box->inf_portal1.MM, box->inf_portal2.MM, box->inf_portal);
		}
		lighting_pass(camera->viewMatrix, camera->projectionMatrix, flag);

		

		t += dt;
		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

}

void initialize() {
	// Initialize GLFW
	if (!glfwInit()) {
		throw runtime_error("Failed to initialize GLFW\n");
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// Open a window and create its OpenGL context
	window = glfwCreateWindow(W_WIDTH, W_HEIGHT, TITLE, NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		throw runtime_error(string(string("Failed to open GLFW window.") +
			" If you have an Intel GPU, they are not 3.3 compatible." +
			"Try the 2.1 version.\n"));
	}
	glfwMakeContextCurrent(window);

	// Start GLEW extension handler
	glewExperimental = GL_TRUE;

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		throw runtime_error("Failed to initialize GLEW\n");
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Hide the mouse and enable unlimited movement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwPollEvents();
	glfwSetCursorPos(window, W_WIDTH / 2, W_HEIGHT / 2);

	// Gray background color
	glClearColor(0.5f, 0.5f, 0.5f, 0.5f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
	
	// enable texturing and bind the depth texture
	glEnable(GL_TEXTURE_2D);

	// Log
	logGLParameters();

	// Create camera
	camera = new Camera(window);
	camera_portal_1 = new Camera(window);
	camera_portal_2 = new Camera(window);
	camera_down_inf = new Camera(window);
	camera_up_inf = new Camera(window);
	// Task 1.1 Creating a light source
	// Creating a custom light 
	light = new Light(window,
		vec4{ 1, 1, 1, 1 },
		vec4{ 1, 1, 1, 1 },
		vec4{ 1, 1, 1, 1 },
		vec3{ 0, 15,0 },
		150.0f
	);
	lightPos = light->lightPosition_worldspace;
}

int main(void) {
	try {
		initialize();
		createContext();
		mainLoop();
		free();
	}
	catch (exception& ex) {
		cout << ex.what() << endl;
		getchar();
		free();
		return -1;
	}
	return 0;
}