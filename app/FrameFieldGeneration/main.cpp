// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://dl.acm.org/doi/10.1145/2980179.2982408
//   Original File Path:  /src/main.cpp
//
// Copyright (c) 2016 Nicolas Ray
// -----------------------------------------------------------------------------
// Modifications made by Canjia Huang on 2025-8-4:
//   - Adjusted code formatting in selected sections
//   - Use CLI11 for command line control
//   - Added code comments
//
// =============================================================================

#include "FF.h"
#include <cmath>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "CLI11/CLI11.hpp"
#include "utils/log.h"
#include "utils/parse_filepath.h"
#include "CubeCover/readMeshFixed.h"

#ifdef _WIN32
#define RED "255 0 0"
#define GREEN "0 255 0"
#define BLUE "0 0 255"
#else
#define RED "1 0 0"
#define GREEN "0 1 0"
#define BLUE "0 0 1"
#endif

/*
* Triangle_cmp and Triangle_cmp are needed only to detect tet faces that participate in the mesh boundary
*/
struct Triangle {
	Triangle(
		const int v0, const int v1, const int v2
		) {
		v[0] = v0; v[1] = v1;v[2] = v2;
	}
	void reorder() {
		if (v[0] > v[2]) std::swap(v[0], v[2]);
		if (v[0] > v[1]) std::swap(v[0], v[1]);
		if (v[1] > v[2]) std::swap(v[1], v[2]);
	}
	int v[3]{};
};

struct Triangle_cmp {
	bool operator()(
		const Triangle& t0, const Triangle& t1
		) const {
		if (t0.v[0] > t1.v[0]) return true;
		if (t0.v[0] < t1.v[0]) return false;
		if (t0.v[1] > t1.v[1]) return true;
		if (t0.v[1] < t1.v[1]) return false;
		return t0.v[2] < t1.v[2] ;
	}
};


int main(int argc, char* argv[]) {
	spdlog::set_level(spdlog::level::info);

	/* Settings */
	std::string inFile;
	std::string outFile;
	std::string visoutFile;
	bool output_vis_file = false;

	/* App */
	CLI::App app{"FrameFieldGeneration"};
	argv = app.ensure_utf8(argv);

	app.add_option(
		"input_path",
		inFile,
		"Tetrahedral mesh file (.tet/.mesh) path"
		)->check(CLI::ExistingFile)->required();
	app.add_option(
		"output_path",
		outFile,
		"Output frame field file (.fra) path"
		);
	app.add_flag(
		"-v",
		output_vis_file,
		"Output model file (.obj) which is used to visualize (little cubes)."
		);

	CLI11_PARSE(app, argc, argv);

	/* Process */
	if (outFile.empty())
		outFile = get_parentpath(inFile) + get_filename(inFile) + ".fra";
	else if (get_filename(outFile).empty())
		outFile = outFile + ".fra";
	visoutFile = get_parentpath(outFile) + get_filename(outFile) + ".obj";

	// mesh representation
	int nbv;						// number of vertices
	int nbt;						// number of tets
	std::vector<double> X;			// X[3*v+d] is the d^{th} coordinate of vertex v
	std::vector<double> n1;			// n1[3*v+d] is the d^{th} coordinate of the first locked normal of vertex v
	std::vector<double> n2;			// n2[3*v+d] is the d^{th} coordinate of the second locked normal of vertex v
	std::vector<int>	tet;		// tet[4*t+] is the i^{th} vertex d of tet t

	/*
	* Load the mesh
	*/
	if (const std::string ext = get_extension(inFile);
		ext == "tet") {
		std::string line;
		std::ifstream input_file(inFile);
		if (!input_file.is_open()) {
			LOG::ERROR("Unable to open file: {}", inFile);
			return 1;
		}

		int num = 0;
		while (std::getline(input_file, line)) {
			std::istringstream iss(line);
			if (num == 0) {
				iss >> nbv;
				X.resize(3 * nbv,0);
				n1.resize(3 * nbv, 0);
				n2.resize(3 * nbv, 0);
			} else if (num == 1) {
				iss >> nbt;
				tet.resize(4 * nbt, 0);
			} else if (num < nbv + 2) {
				iss >> X[3 * (num - 2)] >> X[3 * (num - 2) + 1] >> X[3 * (num - 2) + 2];
			} else  {
				int trash;
				iss >> trash >> tet[4 * (num - 2 - nbv)] >> tet[4 * (num - 2 - nbv)+1] >> tet[4 * (num - 2 - nbv)+2] >> tet[4 * (num - 2 - nbv)+3];
			}
			num++;
		}
		input_file.close();
	}
	else if (ext == "mesh") {
		Eigen::MatrixXd V; // vertices of the tetrahedral mesh
		Eigen::MatrixXi T; // tets
		if (Eigen::MatrixXi F;
			!CubeCover::readMESH(inFile, V, T, F)) {
			LOG::ERROR("could not read .mesh file: {}", inFile);
			return 1;
		}
		nbv = V.rows();
		X.resize(3 * nbv, 0);
		n1.resize(3 * nbv, 0);
		n2.resize(3 * nbv, 0);
		for (int i = 0; i < V.rows(); ++i) {
			for (int j = 0; j < 3; ++j)
				X[3*i+j] = V(i, j);
			// LOG::INFO("v: {}, {}, {}", V(i, 0), V(i, 1), V(i, 2));
		}
		nbt = T.rows();
		tet.resize(4 * nbt, 0);
		for (int i = 0; i < T.rows(); ++i) {
			for (int j = 0; j < 4; ++j)
				tet[4*i+j] = T(i, j);
			// LOG::INFO("t: {}, {}, {}, {}", T(i, 0), T(i, 1), T(i, 2), T(i, 3));
		}
	}
	else {
		LOG::ERROR("Invalid file type: {}", ext);
		return 1;
	}
	LOG::INFO("Load file: {}", inFile);

	/*
	* Compute normal constraints
	*/
	std::vector<Triangle> tr;
	int tr_in_tet[4][3] = {
		{0,1,3},{0,3,2},{0,2,1},{1,2,3}
	};
	for (int t = 0; t < nbt; t++) {
		for (auto & f : tr_in_tet)
			tr.emplace_back(tet[4 * t + f[0]], tet[4 * t + f[1]], tet[4 * t + f[2]]);
	}
	for (auto & t : tr)
		t.reorder();
	std::sort(tr.begin(), tr.end(), Triangle_cmp());
	for (int t = 1; t < static_cast<int>(tr.size() - 1); t++) {
		if (tr[t].v[0] == tr[t + 1].v[0] && tr[t].v[1] == tr[t + 1].v[1] && tr[t].v[2] == tr[t + 1].v[2]) continue;
		if (tr[t].v[0] == tr[t - 1].v[0] && tr[t].v[1] == tr[t - 1].v[1] && tr[t].v[2] == tr[t - 1].v[2]) continue;
		double* x[3] = { X.data()+3 * tr[t].v[0],X.data() + 3 * tr[t].v[1],X.data() + 3 * tr[t].v[2] };
		double n[3] = {
			(x[1][1] - x[0][1])*(x[2][2] - x[0][2]) - (x[1][2] - x[0][2])*(x[2][1] - x[0][1]),
			(x[1][2] - x[0][2])*(x[2][0] - x[0][0]) - (x[1][0] - x[0][0])*(x[2][2] - x[0][2]),
			(x[1][0] - x[0][0])*(x[2][1] - x[0][1]) - (x[1][1] - x[0][1])*(x[2][0] - x[0][0])
		};
		for(int e : tr[t].v) {
			for (int d = 0; d < 3; d++)
				n1[3 * e + d] = n[d];
		}
	}

	/*
	* Call our algorithm
	*/
	std::vector<double>	rot(9 * nbv);
	computeFF(nbv,nbt, n1.data(), tet.data(), rot.data());

	/*
	* save the field
	*/
	std::ofstream outfield;
	outfield.open(outFile);
	outfield << "FRA 1" << std::endl;
	outfield << nbt << " 3 4" << std::endl;
	for (int t = 0; t < nbt; t++) {
		double n[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
		for (int i = 0; i < 4; ++i) {
			for (int d = 0; d < 9; d++)
				n[d] += rot[9 * tet[4*t+i] + d];
		}
		for (int i = 0; i < 3; ++i)
			outfield << 0.25 * n[3*i+0] << " " << 0.25 * n[3*i+1] << " " << 0.25 * n[3*i+2] << std::endl;
	}
	// for (int v = 0; v < nbv; v++) {
	// 	for (int d = 0; d < 9; d++)
	// 		outfield << rot[9 * v + d] << "  ";
	// 	outfield << "\n";
	// }
	outfield.close();
	LOG::INFO("Save file: {}", outFile);

	/*
	* save the mesh (little cubes .obj)
	*/
	if (output_vis_file) {
		double min[3] = { 1e20, 1e20, 1e20 };
		double max[3] = { -1e20, -1e20, -1e20 };
		for (int v = 0; v < nbv; v++) {
			for (int d = 0; d < 3; d++) {
				if (min[d] > X[3 * v + d]) min[d] = X[3 * v + d];
				if (max[d] < X[3 * v + d]) max[d] = X[3 * v + d];
			}
		}
		double scale = 0.01 * sqrt(pow(max[0] - min[0], 2) + pow(max[1] - min[1], 2) + pow(max[2] - min[2], 2));

		std::ofstream outfile;
		outfile.open(visoutFile);
		for (int v = 0; v < nbv; v++) {
			double *G = X.data() + 3 * v;
			for (int dz = -1; dz < 3; dz += 2) {
				for (int dy = -1; dy < 3; dy += 2) {
					for (int dx = -1; dx < 3; dx += 2) {
						outfile << "v ";
						for (int d = 0; d < 3; d++)
							outfile << G[d] + scale * (dx*rot[9 * v + 3 * 0 + d] + dy*rot[9 * v + 3 * 1 + d] + dz*rot[9 * v + 3 * 2 + d]) << "  " ;
						outfile << std::endl;
					}
				}
			}
		}
		for (int v = 0; v < nbv; v++) {
			int off = 8 * v + 1;
			outfile << "f " << off + 0 << " " << off + 2 << " " << off + 3 << " " << off + 1 << std::endl;
			outfile << "f " << off + 4 << " " << off + 5 << " " << off + 7 << " " << off + 6 << std::endl;
			outfile << "f " << off + 0 << " " << off + 1 << " " << off + 5 << " " << off + 4 << std::endl;
			outfile << "f " << off + 1 << " " << off + 3 << " " << off + 7 << " " << off + 5 << std::endl;
			outfile << "f " << off + 3 << " " << off + 2 << " " << off + 6 << " " << off + 7 << std::endl;
			outfile << "f " << off + 0 << " " << off + 4 << " " << off + 6 << " " << off + 2 << std::endl;
		}
		outfile.close();

		LOG::INFO("Save file: {}", visoutFile);
	}

	return 0;
}
