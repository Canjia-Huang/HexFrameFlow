// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/evouga/CubeCover
//   Original File Path:  /tools/singularityviewer/src/main.cpp
//
// Copyright (c) 2025 evouga
// -----------------------------------------------------------------------------
// Modifications made by Canjia Huang on 2025-8-1:
//   - Adjusted code formatting in selected sections
//   - Use CLI11 for command line control
//   - Added code comments
//
// =============================================================================

#include <fstream>
#include <iostream>
#include <random>
#include <Eigen/Core>
#include "FrameField.h"
#include "FrameFieldVis.h"
#include "ReadFrameField.h"
#include "readMeshFixed.h"
#include "SingularCurveNetwork.h"
#include "TetMeshConnectivity.h"
#include "CLI11/CLI11.hpp"
#include "polyscope/curve_network.h"
#include "polyscope/point_cloud.h"
#include "polyscope/polyscope.h"
#include "polyscope/surface_mesh.h"
#include "polyscope/volume_mesh.h"

int main(int argc, char *argv[]) {
    /* Settings */
    std::string meshfile;
    std::string frafile;
    std::string badverts;
    std::string permfile;
    bool recomputeperms = false;
    bool showViz = true;

    /* App */
    CLI::App app{"SingularityViewer"};
    argv = app.ensure_utf8(argv);

    app.add_option(
        "mesh_path",
        meshfile,
        "Tetrahedral mesh file (.mesh) path"
        )->check(CLI::ExistingFile)->required();
    app.add_option(
        "fra_path",
        frafile,
        "Frame field file (.fra) path"
        )->check(CLI::ExistingFile)->required();
    app.add_option(
        "bad_verts_path",
        badverts,
        "bad_verts path"
        )->check(CLI::ExistingFile);
    app.add_option(
        "perm_path",
        permfile,
        "Parameterization file (.perm) path"
        )->check(CLI::ExistingFile);

    CLI11_PARSE(app, argc, argv);

    /* Process */
    if (permfile == "")
        recomputeperms = true;

    if (badverts != "") {
        if (badverts != "1")
            showViz = false;
    }
    std::cout << badverts << std::endl;

    /* Read mesh */
    Eigen::MatrixXd V; // vertices of the tetrahedral mesh
    Eigen::MatrixXi T; // tetrahedra
    if (Eigen::MatrixXi F;
        !CubeCover::readMESH(meshfile, V, T, F)) {
        std::cerr << "could not read .mesh file: " << meshfile << std::endl;
        return -1;
    }
    CubeCover::TetMeshConnectivity mesh(T); // connected tetrahedral mesh

    /* Read frame field */
    Eigen::MatrixXd frames;
    Eigen::MatrixXi assignments;
    if (!CubeCover::readFrameField(frafile, permfile, T, frames, assignments, true)) {
        std::cerr << "could not read frames/permutations" << std::endl;
        return -1;
    }

    CubeCover::FrameField* field = CubeCover::fromFramesAndAssignments(mesh, frames, assignments, true);
    if (!field) {
        std::cerr << "could not build field" << std::endl;
        return -1;
    }

    if (recomputeperms) {
        std::cout << "No face assignments provided, recomputing: ";
        std::cout.flush();
        field->computeLocalAssignments();
        std::cout << "found " << field->nSingularEdges() << " singular edges" << std::endl;
    }
    field->combAssignments();

    /* Extract singular curve network */
    Eigen::MatrixXd Pblack;
    Eigen::MatrixXi Eblack;
    Eigen::MatrixXd Pblue;
    Eigen::MatrixXi Eblue;
    Eigen::MatrixXd Pgreen;
    Eigen::MatrixXi Egreen;
    extractSingularCurveNetwork(V, mesh, *field, Pgreen, Egreen, Pblue, Eblue, Pblack, Eblack);

    /* For visualization */
    Eigen::MatrixXd centroids;
    std::vector<Eigen::MatrixXd> framefieldvecs;
    buildFrameVectors(V, mesh, *field, 1.0, centroids, framefieldvecs);

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
            int tet = mesh.faceTet(i, 0);
            if (tet == -1)
                std::swap(bdryF(curidx, 0), bdryF(curidx, 1));
            curidx++;
        }
    }

    // visualize the seams
    std::vector<int> seamfaces;

    int ninverted = 0;
    int nnontrivial = 0;

    for (int i = 0; i < nfaces; i++) {
        if (!field->faceAssignment(i).isIdentity()) {
            seamfaces.push_back(i);
            nnontrivial++;
        }

        if (field->faceAssignment(i).orientation() == -1)
            ninverted++;
    }

    std::cout << "Non-identity face assignments: " << nnontrivial << std::endl;
    if (ninverted > 0)
        std::cout << "Warning: " << ninverted << " face assignments are orientation-reversing" << std::endl;

    int nseamtris = seamfaces.size();

    Eigen::MatrixXd seamV(3 * nseamtris, 3);
    Eigen::MatrixXi seamF(nseamtris, 3);
    for (int i = 0; i < nseamtris; i++) {
        for (int j = 0; j < 3; j++) {
            seamF(i, j) = 3 * i + j;
            seamV.row(3 * i + j) = V.row(mesh.faceVertex(seamfaces[i], j));
        }
    }

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    if (showViz) { // polyscope
        polyscope::init();

        auto *tetc = polyscope::registerPointCloud("Centroids", centroids);
        glm::vec3 dotcolor(0.1, 0.1, 0.1);
        tetc->setPointColor(dotcolor);
        tetc->setPointRadius(0.001);
        int vpf = framefieldvecs.size();
        for (int i = 0; i < vpf; i++) {
            std::stringstream ss;
            ss << "Frame Vector " << i;
            auto *vf = tetc->addVectorQuantity(ss.str(), framefieldvecs[i]);
            vf->setVectorColor({ dist(rng),dist(rng),dist(rng) });
            vf->setVectorRadius(0.001);
            vf->setEnabled(true);
        }

        auto *green = polyscope::registerCurveNetwork("Singular Curves (+1/4)", Pgreen, Egreen);
        green->setColor({ 0.0,1.0,0.0 });

        auto *blue = polyscope::registerCurveNetwork("Singular Curves (-1/4)", Pblue, Eblue);
        blue->setColor({ 0.0,0.0,1.0 });

        auto *black = polyscope::registerCurveNetwork("Singular Curves (irregular)", Pblack, Eblack);
        black->setColor({ 0.0,0.0,0.0 });

        auto *psMesh = polyscope::registerSurfaceMesh("Boundary Mesh", V, bdryF);
        psMesh->setTransparency(0.2);
        psMesh->setSurfaceColor({ 0.5,0.5,0.0 });

        auto* seammesh = polyscope::registerSurfaceMesh("Seam", seamV, seamF);
        seammesh->setSurfaceColor({ 0.0, 0.0, 0.0 });

        // auto* tetMesh = polyscope::registerTetMesh("Tetrahedral Mesh", V, T);

        // visualize!
        polyscope::show();
    }
    else {
            std::ofstream out(badverts);
            if (!out) {
                std::cerr << "Cannot write file: " << badverts << std::endl;
                return -1;
            }
            int nsing = field->nSingularEdges();
            out << "ids " << V.rows() << " " << 2 * nsing << std::endl;
            for (int i = 0; i < nsing; ++i) {
                int edge = field->singularEdge(i);
                int v0 = mesh.edgeVertex(edge, 0);
                int v1 = mesh.edgeVertex(edge, 1);
                out << v0 << std::endl << v1 << std::endl;
            }

    }

    delete field;
}

