#include "SceneMesh3D.h"
#include <chrono>
#include <queue>
#include <algorithm>
#define SPLIT_INSTEAD_OF_INTERSECT 0

using namespace std;
using namespace vvr;
using namespace Eigen;

Mesh3DScene::Mesh3DScene()
{
	//! Load settings.
	vvr::Shape::DEF_LINE_WIDTH = 4;
	vvr::Shape::DEF_POINT_SIZE = 10;
	m_perspective_proj = true;
	m_bg_col = Colour("768E77");
	m_obj_col = Colour("454545");
	num_of_file = std::to_string(12);
	const string objDir = getBasePath() + "Plane" + num_of_file + "/";
	const string objFile = objDir + "plane_n.obj";
	const string textDir = getBasePath() + "Plane" + num_of_file + "/";
	const string textFile = textDir + "plane.txt";
	m_file.open(textFile);
	m_model_original = vvr::Mesh(objFile);
	int tri = 0;
	string line;
	vector<string>coords(3);
	int counter = 0;
	while (true) {
		if (m_file.eof())break;
		getline(m_file, line);
		stringstream Line(line);
		while (getline(Line, line, ',')) {
			coords[tri] = line;
			tri++;
		}
		if (tri == 3) {
			C3DPoint point_wing;
			tri = 0;
			point_wing.x = 100 * stod(coords[0]);
			point_wing.y = 100 * stod(coords[1]);
			point_wing.z = 100 * stod(coords[2]);
			wing_points.push_back(point_wing);
		}

	}
	m_file.close();
	reset();
	
}

void Mesh3DScene::reset()
{
	Scene::reset();

	//! Define plane
	m_plane_d = 0;
	m_plane = Plane(vec(0, 1, 1).Normalized(), m_plane_d);

	//! Define what will be vissible by default
	m_style_flag = 0;
	m_style_flag |= FLAG_SHOW_SOLID;
	m_style_flag |= FLAG_SHOW_WIRE;
	m_style_flag |= FLAG_SHOW_AXES;
	m_style_flag |= FLAG_SHOW_AABB;
	//m_style_flag |= FLAG_SHOW_PLANE;
}

void Mesh3DScene::resize()
{
	//! By Making `first_pass` static and initializing it to true,
	//! we make sure that the if block will be executed only once.

	static bool first_pass = true;
	
	if (first_pass)
	{
		m_model = m_model_original;
		Tasks();
		first_pass = false;
	}
}

void Mesh3DScene::Tasks()
{
	cout << "Starting initialization" << endl;
	number_of_samples = 5000;
	m_center_mass = vec(-10, 0, 0);
	m_vertices = m_model.getVertices();

	int verticesCount = m_vertices.size();

	Task_1_FindCenterMass(m_model.getVertices(), m_center_mass);
	Task_2_FindAABB(m_model.getVertices(), m_aabb, m_center_mass);
	Task_3_AlignOriginTo(m_model.getVertices(), Task_3_Pick_Origin());
	for (int i = 0; i < wing_points.size(); i++) {
		wing_points[i].x -= m_center_mass.x;
		wing_points[i].y -= m_center_mass.y;
		wing_points[i].z -= m_center_mass.z;
	}
	Task_1_FindCenterMass(m_model.getVertices(), m_center_mass);
	Task_2_FindAABB(m_model.getVertices(), m_aabb, m_center_mass);
	m_vertices = m_model.getVertices();

	for (int i = 0; i < wing_points.size(); i++) {
		for (int j = 0; j < wing_points.size(); j++) {
			if (i == j)continue;
			if (wing_points[i].x == wing_points[j].x &&
				wing_points[i].y == wing_points[j].y &&
				wing_points[i].z == wing_points[j].z) {
				wing_points.erase(wing_points.begin() + j);
				j = 0;
			}
		}
	}
	MatrixXf Coords(m_model.getVertices().size(), 3);
	for (int i = 0; i < m_model.getVertices().size(); i++) {
		Coords(i, 0) = m_model.getVertices()[i].x;
		Coords(i, 1) = m_model.getVertices()[i].y;
		Coords(i, 2) = m_model.getVertices()[i].z;
	}
	C3DPoint fixed_point;
	for (int i = 0; i < wing_points.size(); i++) {
		for (int j = 0; j < Coords.size() / 3; j++) {
			if (sqrt(Pow(wing_points[i].x - m_model.getVertices()[j].x, 2.0f)
				+ Pow(wing_points[i].y - m_model.getVertices()[j].y, 2.0f)
				+ Pow(wing_points[i].z - m_model.getVertices()[j].z, 2.0f)) < 0.0001) {
				fixed_point.x = Coords(j, 0);
				fixed_point.y = Coords(j, 1);
				fixed_point.z = Coords(j, 2);
				wing_fixed.push_back(fixed_point);
			}
		}
	}
	for (int i = 0; i < wing_fixed.size(); i++) {
		for (int j = 0; j < wing_fixed.size(); j++) {
			if (i == j)continue;
			if (wing_fixed[i].x == wing_fixed[j].x &&
				wing_fixed[i].y == wing_fixed[j].y &&
				wing_fixed[i].z == wing_fixed[j].z) {
				wing_fixed.erase(wing_fixed.begin() + j);
				j = 0;
			}
		}
	}
	for (int i = 0; i < m_vertices.size(); i++) {
		for (int j = i+1; j < m_vertices.size(); j++) {
			if (m_vertices[i].x == m_vertices[j].x &&
				m_vertices[i].y == m_vertices[j].y &&
				m_vertices[i].z == m_vertices[j].z) {
				m_vertices.erase(m_vertices.begin() + j);
				j--;
			}
		}
	}
	vector<int>temp_labels(m_vertices.size());
	labels = temp_labels;
	for (int i = 0; i < m_vertices.size(); i++) {
		labels[i] = 0;
		for (int j = 0; j < wing_fixed.size(); j++) {
			if (m_vertices[i].x == wing_fixed[j].x &&
				m_vertices[i].y == wing_fixed[j].y &&
				m_vertices[i].z == wing_fixed[j].z) {
				labels[i]=1;
				break;
			}
		}
	}
	vector<My_Vertex_Struct>temp_indexed_vertices(m_vertices.size());
	vector<int>v(3);
	int counter1 = 0;
	int counter = 0;
	float3 v1, v2, v3;
	for (int i = 0; i < m_model.getTriangles().size(); i++) {
		for (int j = 0; j < m_vertices.size(); j++) {
			if (m_vertices[j].x == m_model.getTriangles()[i].v1().x &&
				m_vertices[j].y == m_model.getTriangles()[i].v1().y &&
				m_vertices[j].z == m_model.getTriangles()[i].v1().z) {
				v1 = m_vertices[j];
				v[0] = j;
				temp_indexed_vertices[j].tr.push_back(i);
			}
			else if (m_vertices[j].x == m_model.getTriangles()[i].v2().x &&
				m_vertices[j].y == m_model.getTriangles()[i].v2().y &&
				m_vertices[j].z == m_model.getTriangles()[i].v2().z) {
				v2 = m_vertices[j];
				v[1] = j;
				temp_indexed_vertices[j].tr.push_back(i);
			}
			else if (m_vertices[j].x == m_model.getTriangles()[i].v3().x &&
				m_vertices[j].y == m_model.getTriangles()[i].v3().y &&
				m_vertices[j].z == m_model.getTriangles()[i].v3().z) {
				v3 = m_vertices[j];
				v[2] = j;
				temp_indexed_vertices[j].tr.push_back(i);
			}
		}
		vec normal = m_model.getTriangles()[i].getNormal();
		indexed_tri.push_back({normal,v1,v2,v3,v });
	}
	indexed_vertices = temp_indexed_vertices;
	cout << "Finished initialization" << endl;

	cout << "Calculating Normals per vertex" << endl;
	getvertexNormals(indexed_vertices);
	cout << "Normals per vertex calculated" << endl;
	cout << "Calculating Gaussian curvature" << endl;
	calculate_gaussian_curvature(m_model.getTriangles(), Coords, indexed_tri);
	cout << "Gaussian curvature calculated" << endl;
	if (number_of_samples > m_vertices.size()) {
		cout << "Starting upsampling" << endl;
		Upsampling();
		cout << "Finished upsampling" << endl;
		Interpolation();
		all_vertices = m_vertices;
		for (int i = 0; i < samples.size(); i++) {
			all_vertices.push_back(samples[i].sampled_point);
		}
	}
	else {
		cout << "Starting downsampling" << endl;
		Segmentation();
		cout << "Finished downsampling" << endl;
		all_vertices = samples_down;
	}
	for (int i = 0; i < vertex_normals.size(); i++) {
		normals2draw.push_back(LineSeg3D(all_vertices[i].x, all_vertices[i].y, all_vertices[i].z, vertex_normals[i].x + all_vertices[i].x, vertex_normals[i].y + all_vertices[i].y, vertex_normals[i].z + all_vertices[i].z, Colour::green));
	}
	cout << "Writting Parameters to file" << endl;
	///*
	samples_file.open(getBasePath() + "Plane" + num_of_file + "/" + std::to_string(number_of_samples) +"/" + "points.txt");
	Gaussian_file.open(getBasePath() + "Plane" + num_of_file + "/" +std::to_string(number_of_samples) + "/" + "Gaussian.txt");
	labels_file.open(getBasePath() + "Plane" + num_of_file + "/" +std::to_string(number_of_samples)+ "/" + "labels.txt");
	normals_file.open(getBasePath() + "Plane" + num_of_file + "/" + std::to_string(number_of_samples) + "/" + "normals.txt");
	g_file.open(getBasePath() + "Plane" + num_of_file + "/" + std::to_string(number_of_samples) + "/" + "g.txt");
	for (int i = 0; i < all_vertices.size(); i++) {
		samples_file << all_vertices[i].x<<","<< all_vertices[i].y<<","<< all_vertices[i].z << endl;
		Gaussian_file << Gaussian2draw[i] << endl;
		normals_file << vertex_normals[i].x << "," << vertex_normals[i].y << "," << vertex_normals[i].z << endl;
		labels_file << labels[i] << endl;
		g_file << Gaussian[i] << endl;
	}
	cout << "Here are the results" << endl;
	samples_file.close();
	Gaussian_file.close();
	labels_file.close();
	normals_file.close();
	//*/
}

void Mesh3DScene::arrowEvent(ArrowDir dir, int modif)
{
	math::vec n = m_plane.normal;
	if (dir == UP) m_plane_d += 1;
	if (dir == DOWN) m_plane_d -= 1;
	else if (dir == LEFT) n = math::float3x3::RotateY(DegToRad(1)).Transform(n);
	else if (dir == RIGHT) n = math::float3x3::RotateY(DegToRad(-1)).Transform(n);
	m_plane = Plane(n.Normalized(), m_plane_d);
 
}

void Mesh3DScene::keyEvent(unsigned char key, bool up, int modif)
{
	Scene::keyEvent(key, up, modif);
	key = tolower(key);

	switch (key)
	{
	case 's': m_style_flag ^= FLAG_SHOW_SOLID; break;
	case 'w': m_style_flag ^= FLAG_SHOW_WIRE; break;
	case 'n': m_style_flag ^= FLAG_SHOW_NORMALS; break;
	case 'a': m_style_flag ^= FLAG_SHOW_AXES; break;
	case 'p': m_style_flag ^= FLAG_SHOW_PLANE; break;
	case 'b': m_style_flag ^= FLAG_SHOW_AABB; break;
	}
}

void Mesh3DScene::draw()
{

	
	if (m_style_flag & FLAG_SHOW_SOLID) m_model.draw(m_obj_col, SOLID);
	if (m_style_flag & FLAG_SHOW_WIRE) m_model.draw(Colour::black, WIRE);
	if (m_style_flag & FLAG_SHOW_NORMALS) m_model.draw(Colour::black, NORMALS);
	if (m_style_flag & FLAG_SHOW_AXES) m_model.draw(Colour::black, AXES);
	
	//! Draw center mass
	Point3D(m_center_mass.x, m_center_mass.y, m_center_mass.z, Colour::red).draw();
	
	if (m_style_flag & FLAG_SHOW_AABB) {
		m_aabb.setColour(Colour::black);
		m_aabb.setTransparency(1);
		m_aabb.draw();
	}
	/*
	for (int i = 0; i < all_vertices.size(); i++) {
		if (labels[i] == 1)Point3D(all_vertices[i].x, all_vertices[i].y, all_vertices[i].z, Colour::white).draw();
		else{
			Point3D(all_vertices[i].x, all_vertices[i].y, all_vertices[i].z, Colour::black).draw();
		}
	}
	
	for (int i = 0; i < m_vertices.size();i++) {
		//Point3D(m_vertices[i].x, m_vertices[i].y, m_vertices[i].z, Colour::Colour(Gaussian2draw[i], 255, Gaussian2draw[i])).draw();
	}
	*/
	for (auto normal : normals2draw) {
		normal.draw();
	}
	/*
	for (auto point : wing_fixed) {
		//Point3D(point.x, point.y, point.z, Colour::green).draw();
	}
	for (auto sample : samples_temp) {
		//Point3D(sample.x, sample.y, sample.z, Colour::red).draw();
	}
	///*
	for (int i = 0; i < all_vertices.size(); i++) {
		if (Gaussian2draw[i] <0) {
			Point3D(all_vertices[i].x, all_vertices[i].y, all_vertices[i].z, Colour::Colour(0,abs(100*Gaussian2draw[i]), 0)).draw();
		}
		else if (Gaussian2draw[i] >0 && Gaussian2draw[i] <=255) {
			Point3D(all_vertices[i].x, all_vertices[i].y, all_vertices[i].z, Colour::Colour(100*Gaussian2draw[i], 0, 255 - (100*Gaussian2draw[i]))).draw();
		}
		else {
			Point3D(all_vertices[i].x, all_vertices[i].y, all_vertices[i].z, Colour::Colour(255, 255, 255)).draw();
		}
		
	}//*/
}
 

int main(int argc, char* argv[])
{
	try {
		return vvr::mainLoop(argc, argv, new Mesh3DScene());
	}
	catch (std::string exc) {
		cerr << exc << endl;
		return 1;
	}
	catch (...)
	{
		cerr << "Unknown exception" << endl;
		return 1;
	}
	
}

//! LAB Tasks

void Task_1_FindCenterMass(vector<vec> &vertices, vec &cm)
{

	float x = 0;
	float y = 0;
	float z = 0;
	int size = vertices.size();
	for (int i = 0; i < vertices.size(); i++) {
		x = x + vertices[i].x;
		y = y + vertices[i].y;
		z = z + vertices[i].z;

	}
	cm.x = x / float(size);
	cm.z = z / float(size);
	cm.y = y / float(size);
}
 

void Mesh3DScene::find_A_Matrix(vector<vvr::Triangle>& m_triangles, MatrixXf& Coords, vector<My_Tri_Struct> indexed_tri) {
	int verticesCount = m_vertices.size();
	int trianglesCount = m_triangles.size();
	A.resize(verticesCount, verticesCount);
	A.data().squeeze();
	const string textFile = "neighbors.txt";
	neighboors_file.open(getBasePath()+"Plane"+num_of_file+"/"+ std::to_string(number_of_samples) + "/" + textFile);
	for (const auto triangle : indexed_tri) {
		for (int i = 0; i < 3; i++) {
			for (int j = i + 1; j < 3; j++) {
				if (A.coeffRef(triangle.v[i], triangle.v[j]) == 0) {
					A.coeffRef(triangle.v[i], triangle.v[j]) = 1;
					A.coeffRef(triangle.v[j], triangle.v[i]) = 1;
				}
			}
		}
	}
	for (int i = 0; i < A.outerSize(); i++) {
		for (SparseMatrix<double>::InnerIterator it(A, i); it; ++it) {
			if (it.value() == 1) {
				neighboors_file << it.row() << " ";
			}
		}
		neighboors_file << endl;
	}
	neighboors_file.close();

}
vec Mesh3DScene::calc_normal(int v,vector<int>tri_tri) {
	vec sum;
	float3 v1, v2, v3;
	vector<vvr::Triangle>triangles;
	triangles = m_model.getTriangles();
	vec e1, e2;
	double angle=0;
	vec cross = vec(0, 0, 0);
	vec triangle_normal = vec(0, 0, 0);
	double fixed_cos;
	sum = vec(0, 0, 0);
	double k = 0;
	for (int i = 0; i < tri_tri.size(); i++) {
		
		v1 = indexed_tri[tri_tri[i]].v1;
		v2 = indexed_tri[tri_tri[i]].v2;
		v3 = indexed_tri[tri_tri[i]].v3;
		float3 mid_point = (v1 + v2 + v3) / 3;
		double Area = calculate_area(v1, v2, v3);
		if (Area <=0.000001)Area = 0.0001;
		if (m_vertices[v].x == v1.x && m_vertices[v].y == v1.y && m_vertices[v].z == v1.z) {
			e1 = v2 - v1;
			e2 = v3 - v1;
			if ((Dot(e1, e2) / (Length(e1) * Length(e2))) > 1) {
				fixed_cos = 1;
			}
			else if ((Dot(e1, e2) / (Length(e1) * Length(e2))) < -1) {
				fixed_cos = -1;
			}
			else {
				fixed_cos = (Dot(e1, e2) / (Length(e1) * Length(e2)));
			}
			angle = RadToDeg(Acos(fixed_cos));
		}
		else if (m_vertices[v].x == v2.x && m_vertices[v].y == v2.y && m_vertices[v].z == v2.z) {
			e1 = v1 - v2;
			e2 = v3 - v2;
			if ((Dot(e1, e2) / (Length(e1) * Length(e2))) > 1) {
				fixed_cos = 1;
			}
			else if ((Dot(e1, e2) / (Length(e1) * Length(e2))) < -1) {
				fixed_cos = -1;
			}
			else {
				fixed_cos = (Dot(e1, e2) / (Length(e1) * Length(e2)));
			}
			angle = RadToDeg(Acos(fixed_cos));
		}
		else if (m_vertices[v].x == v3.x && m_vertices[v].y == v3.y && m_vertices[v].z == v3.z) {
			e1 = v1 - v3;
			e2 = v2 - v3;
			if ((Dot(e1, e2) / (Length(e1) * Length(e2))) > 1) {
				fixed_cos = 1;
			}
			else if ((Dot(e1, e2) / (Length(e1) * Length(e2))) < -1) {
				fixed_cos = -1;
			}
			else {
				fixed_cos = (Dot(e1, e2) / (Length(e1) * Length(e2)));
			}
			angle = RadToDeg(Acos(fixed_cos));

		}
		vec temp_normal = indexed_tri[tri_tri[i]].normal * Area * (angle+0.00001);
		sum += temp_normal;
	}
	sum /= tri_tri.size();
	if (Length(sum) == 0) {
		sum = vec(1, 1, 1);
	}
	sum /= Length(sum);
	if (tri_tri.size() == 1) {
		sum = indexed_tri[tri_tri[0]].normal;
	}
	return sum;
}

void Mesh3DScene::calculate_gaussian_curvature(vector<vvr::Triangle>& m_triangles, MatrixXf& Coords, vector<My_Tri_Struct> indexed_tri) {
	int verticesCount = Coords.rows();
	int trianglesCount = m_triangles.size();

	find_A_Matrix(m_triangles, Coords,indexed_tri);
	double area = 0;
	
	double Gaussian_curvature;
	int v = 0;
	int j = 0;
	float3 v1, v2, v3;
	double temp_angle = 0, fixed_cos = 0, total_angle = 0;
	double angle;
	float3 e1;
	float3 e2;
	vector<vec>triangle_normals;
	vector<int>tri_tri;
	string line_v;
	vector<vector<int>>neigbors(m_vertices.size());
	neighbors_to_read.open(getBasePath() + "Plane" + num_of_file + "/" +std::to_string(number_of_samples) + "/" + "neighbors.txt");
	int k = 0;
	while (true) {
		if (neighbors_to_read.eof())break;
		getline(neighbors_to_read, line_v);
		stringstream Line(line_v);
		while (getline(Line, line_v,' ')) {
			neigbors[k].push_back(stoi(line_v));
		}
		k++;
	}
	neighbors_to_read.close();
	k = 0;
	vector<int>tri_for_check;
	for (auto vertex : indexed_vertices) {
		if (v == 233) {
			cout << endl;
		}
		area = 0;
		tri_tri = vertex.tr;
		for (int i = 0; i < neigbors[v].size(); i++) {
			for (int j = 0; j < tri_tri.size(); j++) {
				if (indexed_tri[tri_tri[j]].v[0] == neigbors[v][i] || indexed_tri[tri_tri[j]].v[1] == neigbors[v][i]
					|| indexed_tri[tri_tri[j]].v[2] == neigbors[v][i]) {
					tri_for_check.push_back(j);
				}
			}
		}
		if (tri_for_check.size() == 3) {
			//vec n1 = indexed_tri[tri_tri[tri_for_check[0]]].new_tri.getNormal();
			//vec n2 = indexed_tri[tri_tri[tri_for_check[1]]].new_tri.getNormal();
			//vec n3 = indexed_tri[tri_tri[tri_for_check[2]]].new_tri.getNormal();
			vec n1 = indexed_tri[tri_tri[tri_for_check[0]]].normal;
			vec n2 = indexed_tri[tri_tri[tri_for_check[1]]].normal;
			vec n3 = indexed_tri[tri_tri[tri_for_check[2]]].normal;
			double cos1_2 = Dot(n1, n2)/ (Length(n1) * Length(n2));
			double cos1_3 = Dot(n1, n3)/ (Length(n1) * Length(n3));
			double cos2_3 = Dot(n2, n3)/ (Length(n2) * Length(n3));
			if (cos1_2 > 1)cos1_2 = 1;
			else if (cos1_2 < -1)cos1_2 = -1;
			if (cos1_3 > 1)cos1_3 = 1;
			else if (cos1_3 < -1)cos1_3 = -1;
			if (cos2_3 > 1)cos2_3 = 1;
			else if (cos2_3 < -1)cos2_3 = -1;
			double angle1_2 = Acos(cos1_2);
			double angle1_3 = Acos(cos1_3);
			double angle2_3 = Acos(cos2_3);
			if (angle1_2 > angle2_3 && angle1_3 > angle2_3) {
				tri_tri.erase(tri_tri.begin() + tri_for_check[0]);
			}
			else if (angle1_2 > angle1_3 && angle2_3 > angle1_3) {
				tri_tri.erase(tri_tri.begin() + tri_for_check[1]);
			}
			else if (angle1_3 > angle1_2 && angle2_3 > angle1_3) {
				tri_tri.erase(tri_tri.begin() + tri_for_check[2]);
			}
		}
		vertex_normals[v] = calc_normal(v, tri_tri);
		for (int i = 0; i < tri_tri.size(); i++) {
			//vec normalt1 = indexed_tri[tri_tri[i]].new_tri.getNormal();
			vec normalt1 = indexed_tri[tri_tri[i]].normal;
			double cos = Dot(normalt1, vertex_normals[v]) / (Length(normalt1) * Length(vertex_normals[v]));
			if (cos > 1)cos = 1;
			else if (cos < -1)cos = -1;
			double angle = Acos(cos);
			if (angle > 0.8) {
				tri_tri.erase(tri_tri.begin() + i);
				i = -1;
				vertex_normals[v] = calc_normal(v, tri_tri);
			}
		}

		for (auto triangle : tri_tri) {
			area += calculate_area(indexed_tri[triangle].v1, indexed_tri[triangle].v2, indexed_tri[triangle].v3);
		}
		for (int i = 0; i < tri_tri.size(); i++) {
			v1 = indexed_tri[tri_tri[i]].v1;
			v2 = indexed_tri[tri_tri[i]].v2;
			v3 = indexed_tri[tri_tri[i]].v3;
			if (m_vertices[v].x == v1.x && m_vertices[v].y == v1.y && m_vertices[v].z == v1.z) {
				e1 = v2 - v1;
				e2 = v3 - v1;
				if ((Dot(e1, e2) / (Length(e1) * Length(e2))) > 1) {
					fixed_cos = 1;
				}
				else if ((Dot(e1, e2) / (Length(e1) * Length(e2))) < -1) {
					fixed_cos = -1;
				}
				else {
					fixed_cos = (Dot(e1, e2) / (Length(e1) * Length(e2)));
				}
				temp_angle = Acos(fixed_cos);
			}
			else if (m_vertices[v].x == v2.x && m_vertices[v].y == v2.y && m_vertices[v].z == v2.z) {
				e1 = v1 - v2;
				e2 = v3 - v2;
				if ((Dot(e1, e2) / (Length(e1) * Length(e2))) > 1) {
					fixed_cos = 1;
				}
				else if ((Dot(e1, e2) / (Length(e1) * Length(e2))) < -1) {
					fixed_cos = -1;
				}
				else {
					fixed_cos = (Dot(e1, e2) / (Length(e1) * Length(e2)));
				}
				temp_angle = Acos(fixed_cos);
			}
			else if (m_vertices[v].x == v3.x && m_vertices[v].y == v3.y && m_vertices[v].z == v3.z) {
				e1 = v1 - v3;
				e2 = v2 - v3;
				if ((Dot(e1, e2) / (Length(e1) * Length(e2))) > 1) {
					fixed_cos = 1;
				}
				else if ((Dot(e1, e2) / (Length(e1) * Length(e2))) < -1) {
					fixed_cos = -1;
				}
				else {
					fixed_cos = (Dot(e1, e2) / (Length(e1) * Length(e2)));
				}
				temp_angle = Acos(fixed_cos);
			}
			total_angle += temp_angle;
		}
		double numerator = (2 * pi) - total_angle;
		double denomenator = (area+0.000001)/3;
		Gaussian_curvature=numerator/(denomenator);
		Gaussian.push_back(Gaussian_curvature);
		total_angle = 0;
		v++;
		tri_tri.clear();
		tri_for_check.clear();
	}
	vector<double>temp_gauss;
	temp_gauss = Gaussian;
	sort(temp_gauss.begin(), temp_gauss.end());
	Gaussian2draw = Gaussian;
	double min = temp_gauss[0], max = temp_gauss[temp_gauss.size() - 1];
	for (int i = 0; i < Gaussian2draw.size(); i++) {
		if (Gaussian2draw[i] < 0) {
			max = 0;
			min = temp_gauss[0];
			double fixed_gaussian = (Gaussian2draw[i] - min) / (max - min);
			fixed_gaussian = fixed_gaussian - 1;
			Gaussian2draw[i] = fixed_gaussian * 255;
		}
		else if(Gaussian2draw[i] >= 0 && Gaussian2draw[i] <=200) {
			max = 200;
			min = 0;
			double fixed_gaussian = (Gaussian2draw[i] - min) / (max - min);
			Gaussian2draw[i] = fixed_gaussian * 255;
		}
		else {
			max = temp_gauss[temp_gauss.size() - 1];
			min = 200;
			double fixed_gaussian = (Gaussian2draw[i] - min) / (max - min);
			Gaussian2draw[i] = fixed_gaussian * 255;
			Gaussian2draw[i] = Gaussian2draw[i] + 255;
		}
	}
	
}

void Task_2_FindAABB(vector<vec>& vertices, Box3D& aabb, vec& cm)
{
	float min_x = cm.x, max_x = cm.x, min_y = cm.y, max_y = cm.y, min_z = cm.z, max_z = cm.z;


	for (int i = 0; i < vertices.size(); i++) {
		if (vertices[i].x > max_x)max_x = vertices[i].x;
		if (vertices[i].x <= min_x)min_x = vertices[i].x;
		if (vertices[i].y > max_y)max_y = vertices[i].y;
		if (vertices[i].y <= min_y)min_y = vertices[i].y;
		if (vertices[i].z > max_z)max_z = vertices[i].z;
		if (vertices[i].z <= min_z)min_z = vertices[i].z;
	}
	aabb = *new Box3D(min_x, min_y, min_z, max_x, max_y, max_z);
}


vec Mesh3DScene::Task_3_Pick_Origin()
{
	return m_center_mass;
}

void Task_3_AlignOriginTo(vector<vec>& vertices, const vec& cm)
{
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i] = vertices[i] - cm;
	}
}

void Mesh3DScene::getvertexNormals(vector<My_Vertex_Struct>indexed_vertices) {
	
	vec sum;
	float3 v1, v2, v3;
	vector<vvr::Triangle>triangles;
	triangles = m_model.getTriangles();
	vec e1,e2;
	int v = 0;
	double angle;
	vec cross=vec(0,0,0);
	vec triangle_normal = vec(0, 0, 0);
	double fixed_cos;
	for (auto vertex : indexed_vertices) {
		sum = vec(0, 0, 0);
		double k = 0;
		for (int i = 0; i < vertex.tr.size(); i++) {
			v1 = indexed_tri[vertex.tr[i]].v1;
			v2 = indexed_tri[vertex.tr[i]].v2;
			v3 = indexed_tri[vertex.tr[i]].v3;
			float3 mid_point = (v1 + v2 + v3) / 3;
			double Area = calculate_area(v1, v2, v3);
			if (Area <= 0.00001)Area = 0.0001;
			if (m_vertices[v].x == v1.x && m_vertices[v].y == v1.y && m_vertices[v].z == v1.z) {
				e1 = v2 - v1;
				e2 = v3 - v1;
				cross=math::Cross(e1, e2);
				triangle_normal = cross / Length(cross);
				if ((Dot(e1, e2) / (Length(e1) * Length(e2))) > 1) {
					fixed_cos = 1;
				}
				else if ((Dot(e1, e2) / (Length(e1) * Length(e2))) < -1) {
					fixed_cos = -1;
				}
				else {
					fixed_cos = (Dot(e1, e2) / (Length(e1) * Length(e2)));
				}
				angle = RadToDeg(Acos(fixed_cos));
			}
			else if (m_vertices[v].x == v2.x && m_vertices[v].y == v2.y && m_vertices[v].z == v2.z) {
				e1 = v1 - v2;
				e2 = v3 - v2;
				cross = math::Cross(e1, e2);
				triangle_normal = cross / Length(cross);
				if ((Dot(e1, e2) / (Length(e1) * Length(e2))) > 1) {
					fixed_cos = 1;
				}
				else if ((Dot(e1, e2) / (Length(e1) * Length(e2))) < -1) {
					fixed_cos = -1;
				}
				else {
					fixed_cos = (Dot(e1, e2) / (Length(e1) * Length(e2)));
				}
				angle = RadToDeg(Acos(fixed_cos));
			}
			else if (m_vertices[v].x == v3.x && m_vertices[v].y == v3.y && m_vertices[v].z == v3.z) {
				e1 = v1 - v3;
				e2 = v2 - v3;
				cross = math::Cross(e1, e2);
				triangle_normal = cross / Length(cross);
				if ((Dot(e1, e2) / (Length(e1) * Length(e2))) > 1) {
					fixed_cos = 1;
				}
				else if ((Dot(e1, e2) / (Length(e1) * Length(e2))) < -1) {
					fixed_cos = -1;
				}
				else {
					fixed_cos = (Dot(e1, e2) / (Length(e1) * Length(e2)));
				}
				angle = RadToDeg(Acos(fixed_cos));

			}
			vec temp_normal = vec(indexed_tri[vertex.tr[i]].normal.x * Area * (angle+0.00001), indexed_tri[vertex.tr[i]].normal.y * Area * (angle + 0.00001), indexed_tri[vertex.tr[i]].normal.z * Area * (angle + 0.00001));
			sum += temp_normal;
		}
		sum /= vertex.tr.size();
		if (Length(sum) == 0) {
			sum = vec(1, 1, 1);
		}
		sum /=  Length(sum);
		if (vertex.tr.size() == 1) {
			sum = indexed_tri[vertex.tr[0]].normal;
		}
		vertex_normals.push_back(sum);
		v++;
	}
}

double calculate_area(float3 v1,float3 v2,float3 v3) {
	double area;
	double side1;
	double side2;
	double side3;
	double half_perimeter;
	side1 = sqrt(pow((v1.x - v2.x), 2) + pow((v1.y - v2.y), 2) + pow((v1.z - v2.z), 2));
	side2 = sqrt(pow((v3.x - v2.x), 2) + pow((v3.y - v2.y), 2) + pow((v3.z - v2.z), 2));
	side3 = sqrt(pow((v1.x - v3.x), 2) + pow((v1.y - v3.y), 2) + pow((v1.z - v3.z), 2));
	half_perimeter = (side1 + side2 + side3) / 2;
	area = sqrt(half_perimeter * abs(half_perimeter - side1) * abs(half_perimeter - side2) * abs(half_perimeter - side3));

	return area;
}

bool compareAreas_up(Triangleforsampling t1, Triangleforsampling t2)
{
	return (t1.area > t2.area);
}

bool compareAreas_down(DownSampling_tr t1, DownSampling_tr t2)
{
	return (t1.area < t2.area);
}

void Mesh3DScene::Upsampling() {
	int num_of_samples = number_of_samples-m_vertices.size();
	float3 v1, v2, v3;
	double area;
	for (int i = 0; i < indexed_tri.size();i++) {
		//v1 = starting_triangles[i].v1();
		//v2 = starting_triangles[i].v2();
		//v3 = starting_triangles[i].v3();
		v1 = indexed_tri[i].v1;
		v2 = indexed_tri[i].v2;
		v3 = indexed_tri[i].v3;
		area = calculate_area(v1, v2, v3);
		tri_s.area = area;
		tri_s.index_to_triangle = i;
		tri_s.v1 = v1; tri_s.v2 = v2; tri_s.v3 = v3;
		sampling_triangles.push_back(tri_s);
	}
	sort(sampling_triangles.begin(), sampling_triangles.end(), compareAreas_up);
	
	float3 new_point;
	int index2triangle;
	float3 tempv1, tempv2;
	Triangleforsampling temp;
	for (int j = 0; j < num_of_samples; j++) {
		temp = sampling_triangles[0];
		index2triangle = temp.index_to_triangle;
		new_point = (temp.v1 + temp.v2 + temp.v3) / 3;
		samples.push_back({new_point,index2triangle});
		for (int i = 0; i < 3; i++) {
			switch (i)
			{
			case 0:
				tempv1 = temp.v1;
				tempv2 = temp.v2;
				area = calculate_area(tempv1, tempv2, new_point);
				tri_s.area = area;
				tri_s.index_to_triangle = temp.index_to_triangle;
				tri_s.v1 = temp.v1; tri_s.v2 = temp.v2; tri_s.v3 = new_point;
				sampling_triangles.push_back(tri_s);
				break;
			case 1:
				tempv1 = temp.v1;
				tempv2 = temp.v3;
				area = calculate_area(tempv1, tempv2, new_point);
				tri_s.area = area;
				tri_s.index_to_triangle = temp.index_to_triangle;
				tri_s.v1 = temp.v1; tri_s.v2 =new_point ; tri_s.v3 = temp.v3;
				sampling_triangles.push_back(tri_s);
				break;
			case 2:
				tempv1 = temp.v2;
				tempv2 = temp.v3;
				area = calculate_area(tempv1, tempv2, new_point);
				tri_s.area = area;
				tri_s.index_to_triangle = temp.index_to_triangle;
				tri_s.v1 = new_point; tri_s.v2 = temp.v2; tri_s.v3 = temp.v3;
				sampling_triangles.push_back(tri_s);
				break;
			default:
				break;
			}
		}
		sampling_triangles.erase(sampling_triangles.begin());
		sort(sampling_triangles.begin(),sampling_triangles.end(),compareAreas_up);
	}
}

void Mesh3DScene::Segmentation() {

	int num_of_samples = m_vertices.size()-number_of_samples;
	cout << num_of_samples << endl;
	if (num_of_samples % 2 != 0) {
		num_of_samples = RoundInt(float(num_of_samples) / 2);
	}
	else {
		num_of_samples = int(num_of_samples / 2);
	}
	cout << m_vertices.size() << endl;
	samples_down = m_vertices;
	float3 v1, v2, v3;
	double area;
	for (int i = 0; i < indexed_tri.size(); i++) {
		//v1 = indexed_tri[i].new_tri.v1();
		//v2 = indexed_tri[i].new_tri.v2();
		//v3 = indexed_tri[i].new_tri.v3();
		v1 = indexed_tri[i].v1;
		v2 = indexed_tri[i].v2;
		v3 = indexed_tri[i].v3;
		area = calculate_area(v1, v2, v3);
		triangles_for_downsampling.push_back({v1,v2,v3,indexed_tri[i].v,i,area});
	}
	sort(triangles_for_downsampling.begin(), triangles_for_downsampling.end(), compareAreas_down);
	///*
	float3 new_point;
	DownSampling_tr temp;
	DownSampling_tr temp_sort;
	vector<int>tri_n;
	vector<int>tri2delete;
	vector<int>all_changed_tri;
	My_Vertex_Struct new_temp;
	for (int i = 0; i < num_of_samples; i++) {
		int counter1 = 0;
		int starting_size = indexed_vertices.size();
		temp = triangles_for_downsampling[0];
		new_point = (temp.v1 + temp.v2 + temp.v3) / 3;
		for (int j = 0; j < 3; j++) {
			for (auto k : indexed_vertices[temp.v[j]].tr) {
				tri_n.push_back(k);
			}
		}
		
		for (int j = 0; j < tri_n.size(); j++) {
			for (int k = j + 1; k < tri_n.size(); k++) {
				if (tri_n[j] == tri_n[k]) {
					tri_n.erase(tri_n.begin() + k);
					k--;
				}
			}
		}
		
		all_changed_tri = tri_n;
		for (int j = 0; j < tri_n.size(); j++) {
			counter1 = 0;
			for (int o = 0; o < 3; o++) {
				if (indexed_tri[tri_n[j]].v[o] == temp.v[0] ||
					indexed_tri[tri_n[j]].v[o] == temp.v[1] ||
					indexed_tri[tri_n[j]].v[o] == temp.v[2]) {
					counter1++;
				}
			}
			if (counter1 > 1) {
				tri2delete.push_back(tri_n[j]);
				tri_n.erase(tri_n.begin() + j);
				j--;
			}
		}

		for (int j = 0; j < tri_n.size(); j++) {
			temp_tri.push_back(indexed_tri[tri_n[j]]);
		}

		sort(tri2delete.begin(), tri2delete.end());
		temp_sort = temp;
		
		sort(temp_sort.v.begin(), temp_sort.v.end(), greater<int>());
		Interpolation_downsampling(new_point, temp.v1, temp.v2, temp.v3,temp.v);
		for (int j = 0; j < 3; j++) {
			samples_down.erase(samples_down.begin() + temp_sort.v[j]);
			Gaussian.erase(Gaussian.begin() + temp_sort.v[j]);
			Gaussian2draw.erase(Gaussian2draw.begin() + temp_sort.v[j]);
			vertex_normals.erase(vertex_normals.begin() + temp_sort.v[j]);
			labels.erase(labels.begin() + temp_sort.v[j]);
		}
		samples_down.push_back(new_point);		
		samples_temp.push_back(new_point);
		vector<int>indexes;
		sort(tri2delete.begin(), tri2delete.end(), greater<int>());
		for (int j = 0; j < tri2delete.size(); j++) {
			indexed_tri.erase(indexed_tri.begin() + tri2delete[j]);
		}
		for (int j = 0; j < indexed_tri.size(); j++) {
			for (int o = 0; o < 3; o++) {
				if (indexed_tri[j].v[o] == temp.v[0]) {
					indexes.push_back(j);
					indexed_tri[j].v[o] = indexed_vertices.size();
				}
				else if (indexed_tri[j].v[o] == temp.v[1]) {
					indexes.push_back(j);
					indexed_tri[j].v[o] = indexed_vertices.size();
				}
				else if (indexed_tri[j].v[o] == temp.v[2]) {
					indexes.push_back(j);
					indexed_tri[j].v[o] = indexed_vertices.size();
				}
			}
		}

		for (int j = 0; j < indexed_tri.size(); j++) {
			for (int l = 0; l < 3; l++) {
				for (int k = 0; k < 3; k++) {
					if (indexed_tri[j].v[l] > temp_sort.v[k]) {
						indexed_tri[j].v[l]--;
					}
				}
			}
		}

		sort(all_changed_tri.begin(), all_changed_tri.end(), greater<int>());

		new_temp.tr=indexes;
		indexed_vertices.push_back(new_temp);

		for (int o = 0; o < indexed_vertices.size()-1; o++) {
			for (int l = 0; l < indexed_vertices[o].tr.size(); l++) {
				bool flag = true;
				for (int k = 0; k < tri2delete.size(); k++) {
					if (indexed_vertices[o].tr[l] == tri2delete[k]) {
						indexed_vertices[o].tr.erase(indexed_vertices[o].tr.begin() + l);
						l--;
						flag = false;
						break;
					}
				}
				if (flag == false)continue;
				for (int k = 0; k < tri2delete.size(); k++) {
					if (indexed_vertices[o].tr[l] > tri2delete[k]) {
						indexed_vertices[o].tr[l]--;
					}
				}

			}
		}
		if (temp.v[0] > temp.v[1] && temp.v[0] > temp.v[2]) {
			indexed_vertices.erase(indexed_vertices.begin() + temp.v[0]);
			if (temp.v[1] > temp.v[2]) {
				indexed_vertices.erase(indexed_vertices.begin() + temp.v[1]);
				indexed_vertices.erase(indexed_vertices.begin() + temp.v[2]);
			}
			else {
				indexed_vertices.erase(indexed_vertices.begin() + temp.v[2]);
				indexed_vertices.erase(indexed_vertices.begin() + temp.v[1]);
			}
		}
		else if (temp.v[1] > temp.v[0] && temp.v[1] > temp.v[2]) {
			indexed_vertices.erase(indexed_vertices.begin() + temp.v[1]);
			if (temp.v[0] > temp.v[2]) {
				indexed_vertices.erase(indexed_vertices.begin() + temp.v[0]);
				indexed_vertices.erase(indexed_vertices.begin() + temp.v[2]);
			}
			else {
				indexed_vertices.erase(indexed_vertices.begin() + temp.v[2]);
				indexed_vertices.erase(indexed_vertices.begin() + temp.v[0]);
			}
		}
		else if (temp.v[2] > temp.v[1] && temp.v[2] > temp.v[0]) {
			indexed_vertices.erase(indexed_vertices.begin() + temp.v[2]);
			if (temp.v[1] > temp.v[0]) {
				indexed_vertices.erase(indexed_vertices.begin() + temp.v[1]);
				indexed_vertices.erase(indexed_vertices.begin() + temp.v[0]);
			}
			else {
				indexed_vertices.erase(indexed_vertices.begin() + temp.v[0]);
				indexed_vertices.erase(indexed_vertices.begin() + temp.v[1]);
			}
		}
		
		triangles_for_downsampling.clear();
		for (int j = 0; j < indexed_tri.size(); j++) {
			v1 = samples_down[indexed_tri[j].v[0]];
			v2 = samples_down[indexed_tri[j].v[1]];
			v3 = samples_down[indexed_tri[j].v[2]];
			area = calculate_area(v1, v2, v3);
			triangles_for_downsampling.push_back({v1,v2,v3,indexed_tri[j].v,j,area});
		}
		sort(triangles_for_downsampling.begin(), triangles_for_downsampling.end(), compareAreas_down);
		tri_n.clear();
		tri2delete.clear();
		temp_tri.clear();
		new_temp.tr.clear();
		all_changed_tri.clear();
		indexes.clear();
	}
	if (samples_down.size() < number_of_samples) {
		temp = triangles_for_downsampling[triangles_for_downsampling.size() - 1];
		new_point = (temp.v1 + temp.v2 + temp.v3) / 3;
		samples_down.push_back(new_point);
		vec v1, v2, v3, p;
		double big_area, areav1v2p, areav1v3p, areav2v3p;
		double weights[3];
		double interpolated_gauss;
		double interpolated_label;
		double interpolated_g;
		vec interpolated_normal;
		cout << vertex_normals.size() << endl;
		p = new_point;
		v1 = temp.v1;
		v2 = temp.v2;
		v3 = temp.v3;
		big_area = calculate_area(v1, v2, v3);
		areav1v2p = calculate_area(v1, v2, p);
		areav2v3p = calculate_area(v2, v3, p);
		areav1v3p = calculate_area(v1, v3, p);
		weights[0] = areav2v3p / big_area;
		weights[1] = areav1v3p / big_area;
		weights[2] = areav1v2p / big_area;
		interpolated_gauss = (weights[0] * Gaussian2draw[temp.v[0]]) +
			(weights[1] * Gaussian2draw[temp.v[1]]) +
			(weights[2] * Gaussian2draw[temp.v[2]]);
		interpolated_normal = (weights[0] * vertex_normals[temp.v[0]]) +
			(weights[1] * vertex_normals[temp.v[1]]) +
			(weights[2] * vertex_normals[temp.v[2]]);
		interpolated_g = (weights[0] * Gaussian[temp.v[0]]) +
			(weights[1] * Gaussian[temp.v[1]]) +
			(weights[2] * Gaussian[temp.v[2]]);
		interpolated_normal = interpolated_normal / Length(interpolated_normal);
		interpolated_label = (weights[0] * labels[temp.v[0]]) +
			(weights[1] * labels[temp.v[1]]) +
			(weights[2] * labels[temp.v[2]]);
		if (interpolated_label > 0.5) {
			interpolated_label = 1;
		}
		else {
			interpolated_label = 0;
		}
		Gaussian2draw.push_back(interpolated_gauss);
		Gaussian.push_back(interpolated_g);
		vertex_normals.push_back(interpolated_normal);
		labels.push_back(interpolated_label);
	}
}

void Mesh3DScene::Interpolation() {
	vec v1, v2, v3,p;
	double big_area, areav1v2p, areav1v3p, areav2v3p;
	double weights[3];
	double interpolated_gauss;
	double interpolated_label;
	double interpolated_g;
	vec interpolated_normal;
	for (int i = 0; i < samples.size(); i++) {
		p = samples[i].sampled_point;
		//v1 = indexed_tri[samples[i].starting_triangle_index].new_tri.v1();
		//v2 = indexed_tri[samples[i].starting_triangle_index].new_tri.v2();
		//v3 = indexed_tri[samples[i].starting_triangle_index].new_tri.v3();
		v1 = indexed_tri[samples[i].starting_triangle_index].v1;
		v2 = indexed_tri[samples[i].starting_triangle_index].v2;
		v3 = indexed_tri[samples[i].starting_triangle_index].v3;
		big_area = calculate_area(v1, v2, v3);
		areav1v2p = calculate_area(v1, v2, p);
		areav2v3p = calculate_area(v2, v3, p);
		areav1v3p = calculate_area(v1, v3, p);
		weights[0] = areav2v3p / big_area;
		weights[1] = areav1v3p / big_area;
		weights[2] = areav1v2p / big_area;
		interpolated_gauss = (weights[0] * Gaussian2draw[indexed_tri[samples[i].starting_triangle_index].v[0]]) +
			(weights[1] * Gaussian2draw[indexed_tri[samples[i].starting_triangle_index].v[1]]) +
			(weights[2] * Gaussian2draw[indexed_tri[samples[i].starting_triangle_index].v[2]]);
		interpolated_g = (weights[0] * Gaussian[indexed_tri[samples[i].starting_triangle_index].v[0]]) +
			(weights[1] * Gaussian[indexed_tri[samples[i].starting_triangle_index].v[1]]) +
			(weights[2] * Gaussian[indexed_tri[samples[i].starting_triangle_index].v[2]]);
		interpolated_normal = (weights[0] * vertex_normals[indexed_tri[samples[i].starting_triangle_index].v[0]]) +
			(weights[1] * vertex_normals[indexed_tri[samples[i].starting_triangle_index].v[1]]) +
			(weights[2] * vertex_normals[indexed_tri[samples[i].starting_triangle_index].v[2]]);
		interpolated_normal = interpolated_normal / Length(interpolated_normal);
		interpolated_label = (weights[0] * labels[indexed_tri[samples[i].starting_triangle_index].v[0]]) +
			(weights[1] * labels[indexed_tri[samples[i].starting_triangle_index].v[1]]) +
			(weights[2] * labels[indexed_tri[samples[i].starting_triangle_index].v[2]]);
		if (interpolated_label > 0.5) {
			interpolated_label = 1;
		}
		else {
			interpolated_label = 0;
		}
		Gaussian2draw.push_back(interpolated_gauss);
		Gaussian.push_back(interpolated_g);
		vertex_normals.push_back(interpolated_normal);
		labels.push_back(interpolated_label);
	}
}
void Mesh3DScene::Interpolation_downsampling(vec p,vec v1,vec v2,vec v3,vector<int>v) {
	double big_area, areav1v2p, areav1v3p, areav2v3p;
	double weights[3];
	double interpolated_gauss;
	double interpolated_label;
	vec interpolated_normal;
	double interpolated_g;
	big_area = calculate_area(v1, v2, v3);
	areav1v2p = calculate_area(v1, v2, p);
	areav2v3p = calculate_area(v2, v3, p);
	areav1v3p = calculate_area(v1, v3, p);
	weights[0] = areav2v3p / (big_area);
	weights[1] = areav1v3p / (big_area);
	weights[2] = areav1v2p / (big_area);
	if (big_area == 0) {
		weights[0] = 0.3333;
		weights[1] = 0.3333;
		weights[2] = 0.3333;
	}
	interpolated_gauss = (weights[0] * Gaussian2draw[v[0]]) +(weights[1] * Gaussian2draw[v[1]]) +
		(weights[2] * Gaussian2draw[v[2]]);
	Gaussian2draw.push_back(interpolated_gauss);
	interpolated_g= (weights[0] * Gaussian[v[0]]) + (weights[1] * Gaussian[v[1]]) +
		(weights[2] * Gaussian[v[2]]);
	Gaussian.push_back(interpolated_g);
	interpolated_normal = (weights[0] * vertex_normals[v[0]]) +
		(weights[1] * vertex_normals[v[1]]) +
		(weights[2] * vertex_normals[v[2]]);
	interpolated_normal = interpolated_normal / Length(interpolated_normal);
	vertex_normals.push_back(interpolated_normal);
	interpolated_label = (weights[0] * labels[v[0]]) +
		(weights[1] * labels[v[1]]) +
		(weights[2] * labels[v[2]]);
	if (interpolated_label > 0.5) {
		interpolated_label = 1;
	}
	else {
		interpolated_label = 0;
	}
	labels.push_back(interpolated_label);
}
