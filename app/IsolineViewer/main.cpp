// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/evouga/CubeCover
//   Original File Path:  /tools/isolineviewer/src/main.cpp
//
// Copyright (c) 2025 evouga
// -----------------------------------------------------------------------------
// Modifications made by Canjia Huang on 2025-8-3:
//   - Adjusted code formatting in selected sections
//   - Use CLI11 for command line control
//   - Added code comments
//
// =============================================================================

#include <iostream>
#include <Eigen/Core>
#include "ExtractIsolines.h"
#include "ReadHexEx.h"
#include "TetMeshConnectivity.h"
#include "CLI11/CLI11.hpp"
#include "polyscope/curve_network.h"
#include "polyscope/polyscope.h"
#include "polyscope/surface_mesh.h"
#include "utils/log.h"

int main(const int argc, char *argv[]){
    /* Settings */
    std::string hexexfile;

    /* App */
    CLI::App app{"VolumeParamViewer"};
    argv = app.ensure_utf8(argv);

    app.add_option(
        "hexex_file",
        hexexfile,
        "Input tet mesh with parametrization file (.hexex) path."
        )->check(CLI::ExistingFile)->required();

    CLI11_PARSE(app, argc, argv);

    /* Process */
    Eigen::MatrixXd V;
    Eigen::MatrixXi T;

    Eigen::MatrixXd values;
    if (!CubeCover::readHexEx(hexexfile, V, T, values)) {
        LOG::ERROR("error reading the .hexex file");
        return -1;
    }

    const CubeCover::TetMeshConnectivity mesh(T);

    Eigen::MatrixXd P;
    Eigen::MatrixXi E;

    Eigen::MatrixXd P2;
    Eigen::MatrixXi E2;

    extractIsolines(V, mesh, values, P, E, P2, E2);

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
            const int tet = mesh.faceTet(i, 0);
            if (tet == -1)
                std::swap(bdryF(curidx, 0), bdryF(curidx, 1));
            curidx++;
        }
    }

    polyscope::init();

    auto *psCurves = polyscope::registerCurveNetwork("Isolines", P, E);
    psCurves->setRadius(0.003);
    auto *psCurves2 = polyscope::registerCurveNetwork("Bad Isolines", P2, E2);
    psCurves2->setRadius(0.003);
    auto *psMesh = polyscope::registerSurfaceMesh("Boundary Mesh", V, bdryF);
    psMesh->setTransparency(0.2);

    // visualize!
    polyscope::show();
}