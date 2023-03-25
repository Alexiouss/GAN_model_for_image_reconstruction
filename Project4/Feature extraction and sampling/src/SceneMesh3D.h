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
#include <queue>

#define FLAG_SHOW_AXES       1
#define FLAG_SHOW_WIRE       2
#define FLAG_SHOW_SOLID      4
#define FLAG_SHOW_NORMALS    8
#define FLAG_SHOW_PLANE     16
#define FLAG_SHOW_AABB      32

void Task_1_FindCenterMass(std::vector<vec> &vertices, vec &cm);
void Task_2_FindAABB(std::vector<vec>& vertices, vvr::Box3D& aabb, vec& cm);
void Task_3_AlignOriginTo(std::vector<vec>& vertices, const vec& cm);
void Task_4_Draw_PCA(vec& center, vec& dir);

struct Triangleforsampling
{
	float3 v1,v2,v3;
	int index_to_triangle;
	double area;
}tri_s;

struct CustomCompare
{
	bool operator()(Triangleforsampling t1, Triangleforsampling t2)
	{
		return t1.area < t2.area;
	}
};

struct DownSampling_tr {
	float3 v1, v2, v3;
	std::vector<int>v;
	int starting_index;
	double area;
};


class Mesh3DScene: public vvr::Scene
{
	
	struct My_Tri_Struct {
		vec normal;
		float3 v1, v2, v3;
		std::vector<int>v;
	};

	struct My_Vertex_Struct {
		std::vector<int>tr;
	};
	
	struct Sampled_points {
		float3 sampled_point;
		int starting_triangle_index;
	};


public:
	Mesh3DScene();
	const char* getName() const { return "3D Scene"; }
	void keyEvent(unsigned char key, bool up, int modif) override;
	void arrowEvent(vvr::ArrowDir dir, int modif) override;

private:
	void draw() override;
	void reset() override;
	void resize() override;
	int initialize(int argc, char* argv[]);
	void Tasks();
	void find_A_Matrix(std::vector<vvr::Triangle>& m_triangles, Eigen::MatrixXf& Coords,std::vector<My_Tri_Struct> indexed_tri);
	void calculate_gaussian_curvature(std::vector<vvr::Triangle>& m_triangles, Eigen::MatrixXf& Coords, std::vector<My_Tri_Struct> indexed_tri);	void pca(std::vector<vec>& vertices, vec& center, vec& dir);
	void getvertexNormals(std::vector<My_Vertex_Struct>indexed_vertices);
	vec Task_3_Pick_Origin(); 
	void Upsampling();
	void Segmentation();
	void Interpolation();
	void Interpolation_downsampling(vec p, vec v1, vec v2, vec v3,std::vector<int>v);
	vec calc_normal(int v,std::vector<int>tri_tri);

private:
	std::string num_of_file;
	int m_style_flag,counter=0;
	float m_plane_d;
	vvr::Canvas2D m_canvas;
	vvr::Colour m_obj_col;
	vvr::Mesh m_model_original, m_model;
	vvr::Box3D m_aabb;
	math::vec m_center_mass;
	math::Plane m_plane;
	std::vector<int> m_intersections;
	std::vector<C3DPoint>wing_points;
	std::vector<C3DPoint>wing_fixed;
	std::vector<vec> m_vertices;
	std::ifstream m_file;
	std::ofstream neighboors_file, L_file,gaussian_curv,normals_file,Gaussian_file,labels_file,samples_file,g_file;
	std::ifstream neighbors_to_read,gauss_to_read,eigenvec,eigenval;
	Eigen::SparseMatrix<double>A;
	math::vec m_pca_cen;
	math::vec m_pca_dir;
	std::vector<vec>vertex_normals;
	std::vector<Sampled_points> samples;
	std::vector<vec>samples_down;
	std::vector<vec>samples_temp;
	std::vector<vvr::LineSeg3D>normals2draw;
	std::vector<My_Tri_Struct>indexed_tri;
	std::vector<My_Tri_Struct>temp_tri;
	std::vector<My_Vertex_Struct>indexed_vertices;
	std::vector<Triangleforsampling>sampling_triangles;
	std::vector<DownSampling_tr>triangles_for_downsampling;
	Eigen::MatrixXd Eigen_vectors;
	std::vector<double>eigen_values;
	std::vector<double>Gaussian;
	std::vector<double>Gaussian2draw;
	std::vector<vec>all_vertices;
	std::vector<double>heat_kernel_signature;
	double mean;
	std::vector<int>labels;
	int number_of_samples;
};
double calculate_area(float3 v1, float3 v2, float3 v3);