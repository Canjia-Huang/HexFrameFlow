./// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/evouga/CubeCover
//   Original File Path:  /tools/volumeparamviewer/src/main.cpp
//
// Copyright (c) 2025 evouga
// -----------------------------------------------------------------------------
// Modifications made by Canjia Huang on 2025-8-3:
//   - Adjusted code formatting in selected sections
//   - Use CLI11 for command line control
//   - Added code comments
//
// =============================================================================

#include <fstream>
#include <Eigen/Core>
#include "ExtractIsolines.h"
#include "ReadHexEx.h"
#include "TetMeshConnectivity.h"
#include "CLI11/CLI11.hpp"
#include "polyscope/curve_network.h"
#include "polyscope/point_cloud.h"
#include "polyscope/polyscope.h"
#include "polyscope/surface_mesh.h"
#include "utils/log.h"

int main(int argc, char *argv[]) {
    /* Settings */
    std::string hexexfile;
    std::string badverts;

    /* App */
    CLI::App app{"VolumeParamViewer"};
    argv = app.ensure_utf8(argv);

    app.add_option(
        "hexex_file",
        hexexfile,
        "Input tet mesh with parametrization file (.hexex) path."
        )->check(CLI::ExistingFile)->required();
    app.add_option(
        "bad_verts_path",
        badverts,
        "bad_verts path."
        )->check(CLI::ExistingFile);

    CLI11_PARSE(app, argc, argv);

    /* Process */
    Eigen::MatrixXd V;
    Eigen::MatrixXi T;

    Eigen::MatrixXd values;
    if (!CubeCover::readHexEx(hexexfile, V, T, values)) {
        LOG::ERROR("error reading the .hexex file");
        return -1;
    }

	// draw tet mesh
	int ntets = T.rows();
	Eigen::MatrixXd P_viztets(4 * ntets, 3);
	Eigen::MatrixXi E_viztets(6 * ntets, 2);

	for (int i = 0; i < ntets; i++) {
		int idx = 0;
		for (int j = 0; j < 4; j++) {
			P_viztets.row(4 * i + j) = values.row(4 * i + j);
			for (int k = j + 1; k < 4; k++) {
				E_viztets(6 * i + idx, 0) = 4 * i + j;
				E_viztets(6 * i + idx, 1) = 4 * i + k;
				idx++;
			}
		}

	}

    bool showViz = true;
    if (!badverts.empty() && badverts != "1")
        showViz = false;

    CubeCover::TetMeshConnectivity mesh(T);

    Eigen::MatrixXd P;
    Eigen::MatrixXi E;

    Eigen::MatrixXd P2;
    Eigen::MatrixXi E2;

    std::vector<glm::vec3> points;
    std::vector<std::array<double, 3>> colors;

    std::vector<glm::vec3> tet_centers;
    std::vector<int> badvert_ids;

    extractIsolines(V, mesh, values, P, E, P2, E2, badvert_ids);
        LOG::INFO("bad verts size: {}", badvert_ids.size());

    extractPoints(V, mesh, values, points, colors);

    for (size_t i = 0; i < V.rows(); i++)
      tet_centers.push_back( glm::vec3{V(i,0), V(i,1), V(i,2)} );

    // make a mesh out of all the boundary faces
    int nbdry = 0;
    int nfaces = mesh.nFaces();
    for (int i = 0; i < nfaces; i++) {
        if (mesh.isBoundaryFace(i))
            nbdry++;
    }
    Eigen::MatrixXi bdryF(nbdry, 3);
    int curidx = 0;
    for (int i = 0; i < nfaces; i++) {
        if (mesh.isBoundaryFace(i)) {
            for (int j = 0; j < 3; j++)
                bdryF(curidx, j) = mesh.faceVertex(i, j);

            // fix triangle orientations
            if (int tet = mesh.faceTet(i, 0);
                tet == -1)
                std::swap(bdryF(curidx, 0), bdryF(curidx, 1));
            curidx++;
        }
    }

    if (showViz) {
        polyscope::init();

        // auto *psCurves = polyscope::registerCurveNetwork("Isolines", P, E);
        // psCurves->setRadius(0.003);
    	auto *psVizTets = polyscope::registerCurveNetwork("viztets", P_viztets, E_viztets);
    	psVizTets->setRadius(0.003);
    	psVizTets->setEnabled(false);

        auto *psCurves2 = polyscope::registerCurveNetwork("Bad Isolines", P2, E2);
        psCurves2->setRadius(0.003);

        auto *psPoints = polyscope::registerPointCloud("Mesh Verts", points);
        psPoints->setPointRadius(0.003);
        psPoints->addColorQuantity("pos_color", colors)->setEnabled(true);
        psPoints->setEnabled(false);

        auto *psPoints2 = polyscope::registerPointCloud("Mesh Interior", tet_centers);
        psPoints2->setPointRadius(0.003);
        psPoints2->addColorQuantity("pos_color", colors)->setEnabled(true);

        auto *psMesh = polyscope::registerSurfaceMesh("Boundary Mesh", V, bdryF);
        psMesh->setTransparency(0.2);

        // visualize!
        polyscope::show();
    }
    else {
        std::ofstream ofs(badverts);
        if (!ofs)
            return false;

        int nbadverts = badvert_ids.size();
        // ofs << "ids " << V.rows() << " " << nbadverts << std::endl;
        // for (int i = 0; i < nbadverts; i++)
        // {
        //     ofs << badvert_ids[i] << std::endl;
        // }
        // std::cout << T.rows() << nbadverts << std::endl;
        ofs << "ids " << T.rows() << " " << nbadverts << std::endl;
        for (int i = 0; i < nbadverts; i++)
            ofs << badvert_ids[i] << std::endl;
    }

}