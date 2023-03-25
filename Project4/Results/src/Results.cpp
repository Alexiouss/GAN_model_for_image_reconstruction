#include "Results.h"
#include <chrono>

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
	string num_of_file[13];
	for (int i = 1; i < 14; i++) {
		num_of_file[i-1] = std::to_string(i);
		const string objFile = getBasePath() + "Plane" + num_of_file[i-1] + "/" + "plane.obj";
		models[i - 1] = vvr::Mesh(objFile);
	}
	
	reset();
}

void Mesh3DScene::reset()
{
	Scene::reset();


	//! Define what will be vissible by default
	m_style_flag = 0;
	feature = 0;
	m_style_flag |= FLAG_SHOW_SOLID;
	m_style_flag |= FLAG_SHOW_WIRE;
	m_style_flag |= FLAG_SHOW_AXES;
	
}

void Mesh3DScene::resize()
{

	static bool first_pass = true;
	
	if (first_pass)
	{
		Tasks();
		first_pass = false;
	}
}

void Mesh3DScene::Tasks()
{
	num_of_samples = 5000;
	labels_file.open(getBasePath() + "Plane" + std::to_string(counter+1) + "/" + std::to_string(num_of_samples) + "/" + "labels.txt");
	normals_file.open(getBasePath() + "Plane" + std::to_string(counter + 1) + "/" + std::to_string(num_of_samples) + "/" + "normals.txt");
	Gaussian_file.open(getBasePath() + "Plane" + std::to_string(counter + 1) + "/" + std::to_string(num_of_samples) + "/" + "Gaussian.txt");
	samped_points_file.open(getBasePath() + "Plane" + std::to_string(counter + 1) + "/" + std::to_string(num_of_samples) + "/" + "points.txt");
	my_labels_file.open(getBasePath() + "Plane" + std::to_string(counter + 1) + "/" + std::to_string(num_of_samples) + "/" + "My_labels" + std::to_string(counter + 1) + "_" + std::to_string(num_of_samples) + ".txt");
	int i = 0;
	string line_labels,line_normals,line_Gaussian,line_points,line_my_labels;
	vector<double>coords(3);
	int counter = 0;
	while (true) {
		if (labels_file.eof())break;
		getline(labels_file, line_labels);
		getline(my_labels_file, line_my_labels);
		getline(Gaussian_file, line_Gaussian);
		getline(normals_file, line_normals);
		getline(samped_points_file, line_points);
		stringstream Line1(line_labels);
		stringstream Line2(line_Gaussian);
		stringstream Line3(line_normals);
		stringstream Line4(line_points);
		stringstream Line5(line_my_labels);
		while (getline(Line1, line_labels)) {
			labels.push_back(stoi(line_labels));
		}
		while (getline(Line2, line_Gaussian)) {
			Gaussian.push_back(stod(line_Gaussian));
		}
		while (getline(Line3, line_normals, ',')) {
			coords[i] = stod(line_normals);
			i++;
		}
		if (i == 3) {
			normals.push_back(vec(coords[0], coords[1], coords[2]));
			i = 0;
		}
		while (getline(Line4, line_points, ',')) {
			coords[i] = stod(line_points);
			i++;
		}
		if (i == 3) {
			points.push_back(vec(coords[0], coords[1], coords[2]));
			i = 0;
		}
		while (getline(Line5, line_my_labels)) {
			My_labels.push_back(stoi(line_my_labels));
		}
	}

	m_vertices = models[counter].getVertices();
	Task_1_FindCenterMass(models[counter].getVertices(), m_center_mass);
	Task_3_AlignOriginTo(models[counter].getVertices(), Task_3_Pick_Origin());

	Task_1_FindCenterMass(models[counter].getVertices(), m_center_mass);
	m_vertices = models[counter].getVertices();
	pca(m_vertices, m_pca_cen, m_pca_dir);
	labels_file.close();
	normals_file.close();
	Gaussian_file.close();
	samped_points_file.close();
	my_labels_file.close();
	
}

void Mesh3DScene::arrowEvent(ArrowDir dir, int modif)
{
	math::vec n = m_plane.normal;
	if (dir == LEFT) {
		num_of_samples = 0;
		feature = 0;
		labels.clear();
		normals.clear();
		points.clear();
		Gaussian.clear();
		My_labels.clear();
		if (counter == 0) {
			counter = 0;
		}
		else {
			counter--;
			Task_1_FindCenterMass(models[counter].getVertices(), m_center_mass);
			Task_3_AlignOriginTo(models[counter].getVertices(), Task_3_Pick_Origin());

			Task_1_FindCenterMass(models[counter].getVertices(), m_center_mass);
			m_vertices = models[counter].getVertices();
			pca(m_vertices, m_pca_cen, m_pca_dir);
		}
	}
	else if (dir == RIGHT) {
		feature = 0;
		num_of_samples = 0;
		labels.clear();
		normals.clear();
		points.clear();
		Gaussian.clear();
		My_labels.clear();
		if (counter == 12) {
			counter = 12;
		}
		else {
			counter++;
			Task_1_FindCenterMass(models[counter].getVertices(), m_center_mass);
			Task_3_AlignOriginTo(models[counter].getVertices(), Task_3_Pick_Origin());

			Task_1_FindCenterMass(models[counter].getVertices(), m_center_mass);
			m_vertices = models[counter].getVertices();
			pca(m_vertices, m_pca_cen, m_pca_dir);
		}
	}
	else if (dir == UP) {
		if (num_of_samples == 20000) {
			num_of_samples = 20000;
		}
		else {
			labels.clear();
			normals.clear();
			points.clear();
			Gaussian.clear();
			My_labels.clear();
			num_of_samples += 5000;
			labels_file.open(getBasePath() + "Plane" + std::to_string(counter + 1) + "/" + std::to_string(num_of_samples) + "/" + "labels.txt");
			normals_file.open(getBasePath() + "Plane" + std::to_string(counter + 1) + "/" + std::to_string(num_of_samples) + "/" + "normals.txt");
			Gaussian_file.open(getBasePath() + "Plane" + std::to_string(counter + 1) + "/" + std::to_string(num_of_samples) + "/" + "Gaussian.txt");
			samped_points_file.open(getBasePath() + "Plane" + std::to_string(counter + 1) + "/" + std::to_string(num_of_samples) + "/" + "points.txt");
			my_labels_file.open(getBasePath() + "Plane" + std::to_string(counter + 1) + "/" + std::to_string(num_of_samples) + "/" + "My_labels" + std::to_string(counter + 1) + "_" + std::to_string(num_of_samples) + ".txt");
			int i = 0;
			string line_labels, line_normals, line_Gaussian, line_points, line_my_labels;
			vector<double>coords(3);
			int counter = 0;
			while (true) {
				if (labels_file.eof())break;
				getline(labels_file, line_labels);
				getline(my_labels_file, line_my_labels);
				getline(Gaussian_file, line_Gaussian);
				getline(normals_file, line_normals);
				getline(samped_points_file, line_points);
				stringstream Line1(line_labels);
				stringstream Line2(line_Gaussian);
				stringstream Line3(line_normals);
				stringstream Line4(line_points);
				stringstream Line5(line_my_labels);
				while (getline(Line1, line_labels)) {
					labels.push_back(stoi(line_labels));
				}
				while (getline(Line2, line_Gaussian)) {
					Gaussian.push_back(stod(line_Gaussian));
				}
				while (getline(Line3, line_normals, ',')) {
					coords[i] = stod(line_normals);
					i++;
				}
				if (i == 3) {
					normals.push_back(vec(coords[0], coords[1], coords[2]));
					i = 0;
				}
				while (getline(Line4, line_points, ',')) {
					coords[i] = stod(line_points);
					i++;
				}
				if (i == 3) {
					points.push_back(vec(coords[0], coords[1], coords[2]));
					i = 0;
				}
				while (getline(Line5, line_my_labels)) {
					My_labels.push_back(stoi(line_my_labels));
				}
			}
			labels_file.close();
			normals_file.close();
			Gaussian_file.close();
			samped_points_file.close();
			my_labels_file.close();
		}
	}
	else if (dir == DOWN) {
		if (num_of_samples == 5000) {
			num_of_samples = 5000;
		}
		else {
			labels.clear();
			normals.clear();
			points.clear();
			Gaussian.clear();
			My_labels.clear();
			num_of_samples -= 5000;
			labels_file.open(getBasePath() + "Plane" + std::to_string(counter + 1) + "/" + std::to_string(num_of_samples) + "/" + "labels.txt");
			normals_file.open(getBasePath() + "Plane" + std::to_string(counter + 1) + "/" + std::to_string(num_of_samples) + "/" + "normals.txt");
			Gaussian_file.open(getBasePath() + "Plane" + std::to_string(counter + 1) + "/" + std::to_string(num_of_samples) + "/" + "Gaussian.txt");
			samped_points_file.open(getBasePath() + "Plane" + std::to_string(counter + 1) + "/" + std::to_string(num_of_samples) + "/" + "points.txt");
			my_labels_file.open(getBasePath() + "Plane" + std::to_string(counter + 1) + "/" + std::to_string(num_of_samples) + "/" + "My_labels" + std::to_string(counter + 1) + "_" + std::to_string(num_of_samples) + ".txt");
			int i = 0;
			string line_labels, line_normals, line_Gaussian, line_points, line_my_labels;
			vector<double>coords(3);
			int counter = 0;
			while (true) {
				if (labels_file.eof())break;
				getline(labels_file, line_labels);
				getline(my_labels_file, line_my_labels);
				getline(Gaussian_file, line_Gaussian);
				getline(normals_file, line_normals);
				getline(samped_points_file, line_points);
				stringstream Line1(line_labels);
				stringstream Line2(line_Gaussian);
				stringstream Line3(line_normals);
				stringstream Line4(line_points);
				stringstream Line5(line_my_labels);
				while (getline(Line1, line_labels)) {
					labels.push_back(stoi(line_labels));
				}
				while (getline(Line2, line_Gaussian)) {
					Gaussian.push_back(stod(line_Gaussian));
				}
				while (getline(Line3, line_normals, ',')) {
					coords[i] = stod(line_normals);
					i++;
				}
				if (i == 3) {
					normals.push_back(vec(coords[0], coords[1], coords[2]));
					i = 0;
				}
				while (getline(Line4, line_points, ',')) {
					coords[i] = stod(line_points);
					i++;
				}
				if (i == 3) {
					points.push_back(vec(coords[0], coords[1], coords[2]));
					i = 0;
				}
				while (getline(Line5, line_my_labels)) {
					My_labels.push_back(stoi(line_my_labels));
				}
			}
			labels_file.close();
			normals_file.close();
			Gaussian_file.close();
			samped_points_file.close();
			my_labels_file.close();
		}
	}
	
 
}

void Mesh3DScene::keyEvent(unsigned char key, bool up, int modif)
{
	Scene::keyEvent(key, up, modif);
	key = tolower(key);

	switch (key)
	{
	case 's': m_style_flag ^= FLAG_SHOW_SOLID; break;
	case 'w': m_style_flag ^= FLAG_SHOW_WIRE; break;
	case 'a': m_style_flag ^= FLAG_SHOW_AXES; break;
	case '0': feature = 0; break;
	case 'p': feature = 1; break;
	case 'n': feature = 2; break;
	case 'c': feature = 3; break;
	case 'l': feature = 4; break;
	case 'm': feature = 5; break;
	}
}

void Mesh3DScene::draw()
{


	if (m_style_flag & FLAG_SHOW_SOLID) {
		models[counter].draw(m_obj_col, SOLID);
	}
	if (m_style_flag & FLAG_SHOW_WIRE) {
		models[counter].draw(Colour::black, WIRE);
	}
	if (m_style_flag & FLAG_SHOW_AXES) {
		models[counter].draw(Colour::black, AXES);
	}

	//! Draw center mass
	Point3D(m_center_mass.x, m_center_mass.y, m_center_mass.z, Colour::red).draw();

	Task_4_Draw_PCA(m_pca_cen, m_pca_dir);
	
	if (feature == 0) {
		for (int i = 0; i < m_vertices.size(); i++) {
			Point3D(m_vertices[i].x, m_vertices[i].y, m_vertices[i].z, Colour::yellow).draw();
		}
	}
	if (feature == 1) {
		for (int i = 0; i < points.size(); i++) {
			Point3D(points[i].x, points[i].y, points[i].z, Colour::cyan).draw();
		}
	}
	if (feature == 2) {
		if (counter == 0) {
			for (int i = 0; i < normals.size(); i++) {
				LineSeg3D(points[i].x, points[i].y, points[i].z, normals[i].x + points[i].x, normals[i].y + points[i].y, normals[i].z + points[i].z, Colour::green).draw();
			}
		}
		else {
			for (int i = 0; i < normals.size(); i++) {
				LineSeg3D(points[i].x, points[i].y, points[i].z, -normals[i].x + points[i].x, -normals[i].y + points[i].y, -normals[i].z + points[i].z, Colour::green).draw();
			}
		}
		
	}

	if (feature == 3) {
		for (int i = 0; i < Gaussian.size(); i++) {
			if (Gaussian[i] <0) {
				Point3D(points[i].x, points[i].y, points[i].z, Colour::Colour(0,abs(100* Gaussian[i]), 0)).draw();
			}
			else if (Gaussian[i] >0 && Gaussian[i] <=255) {
				Point3D(points[i].x, points[i].y, points[i].z, Colour::Colour(100* Gaussian[i], 0, 255 - (100* Gaussian[i]))).draw();
			}
			else {
				Point3D(points[i].x, points[i].y, points[i].z, Colour::Colour(255, 255, 255)).draw();
			}

		}
	}

	if (feature == 4) {
		for (int i = 0; i < points.size(); i++) {
			if (labels[i] == 1)Point3D(points[i].x, points[i].y, points[i].z, Colour::white).draw();
			else {
				Point3D(points[i].x, points[i].y, points[i].z, Colour::black).draw();
			}
		}
	}
	if (feature == 5) {
		for (int i = 0; i < points.size(); i++) {
			if (My_labels[i] == 1)Point3D(points[i].x, points[i].y, points[i].z, Colour::white).draw();
			else {
				Point3D(points[i].x, points[i].y, points[i].z, Colour::black).draw();
			}
		}
	}


}
 

int main(int argc, char* argv[])
{
	try {
		return vvr::mainLoop(argc, argv, new Mesh3DScene);
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
void pca(vector<vec>& vertices, vec& center, vec& dir)
{
	const int count = vertices.size();

	float w0 = 0;
	float x0 = 0, y0 = 0, z0 = 0;
	float x2 = 0, y2 = 0, z2 = 0, xy = 0, yz = 0, xz = 0;
	float dx2, dy2, dz2, dxy, dxz, dyz;
	float det[9];

	for (int i = 0; i < count; i++)
	{
		float x = vertices[i].x;
		float y = vertices[i].y;
		float z = vertices[i].z;

		x2 += x * x;
		xy += x * y;
		xz += x * z;
		y2 += y * y;
		yz += y * z;
		z2 += z * z;
		x0 += x;
		y0 += y;
		z0 += z;
	}
	w0 = (float)count;

	x2 /= w0;
	xy /= w0;
	xz /= w0;
	y2 /= w0;
	yz /= w0;
	z2 /= w0;

	x0 /= w0;
	y0 /= w0;
	z0 /= w0;

	dx2 = x2 - x0 * x0;
	dxy = xy - x0 * y0;
	dxz = xz - x0 * z0;
	dy2 = y2 - y0 * y0;
	dyz = yz - y0 * z0;
	dz2 = z2 - z0 * z0;

	det[0] = dz2 + dy2;
	det[1] = -dxy;
	det[2] = -dxz;
	det[3] = det[1];
	det[4] = dx2 + dz2;
	det[5] = -dyz;
	det[6] = det[2];
	det[7] = det[5];
	det[8] = dy2 + dx2;

	/* Searching for a eigenvector of det corresponding to the minimal eigenvalue */
	gte::SymmetricEigensolver3x3<float> solver;
	std::array<float, 3> eval;
	std::array<std::array<float, 3>, 3> evec;
	solver(det[0], det[1], det[2], det[4], det[5], det[8], true, 1, eval, evec);

	center.x = x0;
	center.y = y0;
	center.z = z0;

	dir.x = evec[0][0];
	dir.y = evec[0][1];
	dir.z = evec[0][2];
}

void Task_4_Draw_PCA(vec& center, vec& dir)
{
	//!//////////////////////////////////////////////////////////////////////////////////
	//! TASK:
	//!
	//!  - Apeikoniste to kentro mazas kai ton Principal Axis tou PCA.
	//!    Gia tin apeikonisi, xreiazeste ena simeio kai mia eytheia.
	//!
	//! HINTS:
	//!  - Auti i synartisi kaleitai mesa apo tin `Mesh3DScene::draw()`.
	//!    Ara, mporeite na kalesete amesa tis methodous draw ton diaforwn antikeimenwn
	//!
	//!//////////////////////////////////////////////////////////////////////////////////
	LineSeg3D line(center.x - 50 * dir.x, center.y - 50 * dir.y, center.z - 50 * dir.z, 50 * dir.x, 50 * dir.y, 50 * dir.z);
	line.draw();

}

