#include <VVRScene/canvas.h>
#include <VVRScene/mesh.h>
#include <VVRScene/settings.h>
#include <VVRScene/utils.h>
#include <MathGeoLib.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <set>
#include "symmetriceigensolver3x3.h"
#include "canvas.h"
#include "Eigen/Dense"
#include "Eigen/Sparse"
#include "Eigen/SparseQR"
#include "Eigen/Eigenvalues"
#include "Eigen/SparseCholesky"
#include "math.h"


#define FLAG_SHOW_AXES       1
#define FLAG_SHOW_WIRE       2
#define FLAG_SHOW_SOLID      4
#define FLAG_SHOW_NORMALS    8
#define FLAG_SHOW_PLANE     16
#define FLAG_SHOW_AABB      32

void Task_1_FindCenterMass(std::vector<vec> &vertices, vec &cm);
void Task_3_AlignOriginTo(std::vector<vec>& vertices, const vec& cm);
void ShiftMesh(std::vector<vec>& vertices);
void pca(std::vector<vec>& vertices, vec& center, vec& dir);
void Task_4_Draw_PCA(vec& center, vec& dir);
class Mesh3DScene : public vvr::Scene
{
public:
	Mesh3DScene();
	const char* getName() const { return "3D Scene"; }
	void keyEvent(unsigned char key, bool up, int modif) override;
	void arrowEvent(vvr::ArrowDir dir, int modif) override;
	

private:
	void draw() override;
	void reset() override;
	void resize() override;
	void Tasks();
	vec Task_3_Pick_Origin();

private:
	int m_style_flag, counter = 0, num_of_samples,feature;
	float m_plane_d;
	vvr::Canvas2D m_canvas;
	vvr::Colour m_obj_col;
	vvr::Mesh models[13];
	vvr::Mesh m_model_original, m_model,m_model2;
	vvr::Box3D m_aabb;
	math::vec m_center_mass;
	math::vec m_pca_cen;
	math::vec m_pca_dir;
	math::Plane m_plane;
	std::vector<int>labels;
	std::vector<int>My_labels;
	std::vector<vec>normals;
	std::vector<double>Gaussian;
	std::vector<vec>points;
	std::vector<vec> m_vertices;
	std::ifstream labels_file,Gaussian_file,normals_file,samped_points_file,my_labels_file;
};
