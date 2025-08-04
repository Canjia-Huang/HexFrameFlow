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

#include "CurlCorrection.h"
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
#include "utils/log.h"
#include "utils/parse_filepath.h"

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
        "Tetrahedral mesh file (.mesh) path."
        )->check(CLI::ExistingFile)->required();
    app.add_option(
        "fra_path",
        frafile,
        "Frame field file (.fra) path."
        )->check(CLI::ExistingFile);
    app.add_option(
        "bad_verts_path",
        badverts,
        "bad_verts path."
        )->check(CLI::ExistingFile);
    app.add_option(
        "perm_path",
        permfile,
        "Parameterization file (.perm) path."
        )->check(CLI::ExistingFile);

    CLI11_PARSE(app, argc, argv);

    /* Process */
    if (permfile.empty())
        recomputeperms = true;

    if (badverts != "") {
        if (badverts != "1")
            showViz = false;
    }

    /* Read mesh */
    Eigen::MatrixXd V; // vertices of the tetrahedral mesh
    Eigen::MatrixXi T; // tets
    if (Eigen::MatrixXi F;
        !CubeCover::readMESH(meshfile, V, T, F)) {
        LOG::ERROR("could not read .mesh file: {}", meshfile);
        return -1;
    }
    CubeCover::TetMeshConnectivity mesh(T); // connected tetrahedral mesh

    /* Read frame field */
    Eigen::MatrixXd frames;
    Eigen::MatrixXi assignments;
    if (!CubeCover::readFrameField(frafile, permfile, T, frames, assignments, true)) {
        /* Try to read file: mesh_filename.fra */
        frafile = get_parentpath(meshfile) + get_filename(meshfile) + ".fra";
        if (!CubeCover::readFrameField(frafile, permfile, T, frames, assignments, true)) {
            LOG::ERROR("could not read frames/permutations");
            return -1;
        }
    }

    CubeCover::FrameField* field = CubeCover::fromFramesAndAssignments(mesh, frames, assignments, true);
    if (!field) {
        LOG::ERROR("could not build field");
        return -1;
    }

    if (recomputeperms) {
        LOG::INFO("No face assignments provided, recomputing");
        field->computeLocalAssignments();
        LOG::INFO("found {} singular edges", field->nSingularEdges());
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
            if (int tet = mesh.faceTet(i, 0);
                tet == -1)
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

    LOG::INFO("Non-identity face assignments: {}", nnontrivial);
    if (ninverted > 0)
        LOG::WARN("Warning: {} face assignments are orientation-reversing", ninverted);

    int nseamtris = seamfaces.size();

    Eigen::MatrixXd seamV(3 * nseamtris, 3);
    Eigen::MatrixXi seamF(nseamtris, 3);
    for (int i = 0; i < nseamtris; i++) {
        for (int j = 0; j < 3; j++) {
            seamF(i, j) = 3 * i + j;
            seamV.row(3 * i + j) = V.row(mesh.faceVertex(seamfaces[i], j));
        }
    }

    /* about curl */
    int vpf = field->vectorsPerFrame();
    Eigen::SparseMatrix<double> C;
    buildCurlMatrix(vpf,  V, *field, C);
    int ntets = field->meshConnectivity().nTets();

    Eigen::VectorXd unrolled(ntets * vpf * 3);
    double minval = 100000000;
    double maxval = -100000000;
    for (int i = 0; i < ntets; i++) {
        for (int j = 0; j < vpf; j++) {
            unrolled.segment<3>(3 * vpf * i + 3 * j) = field->tetFrame(i).row(j);

            for (int k = 0; k < 3 ; k++) {
                auto blah = field->tetFrame(i).row(j);
                if ( abs(blah(k)) > maxval)
                    maxval = abs(blah(k));
                if (abs(blah(k)) < minval)
                    minval = abs(blah(k));
            }
        }
    }

    LOG::INFO("min val: {}", minval);
    LOG::INFO("max val: {}", maxval);

    Eigen::VectorXd all_curl = C * unrolled;
    Eigen::VectorXd per_tet_sum_curl = Eigen::VectorXd::Zero(ntets);

    int row = 0;
    for (int i = 0; i < nfaces; i++) {
        int t0 = mesh.faceTet(i, 0);
        int t1 = mesh.faceTet(i, 1);
        if (t0 == -1 || t1 == -1)
            continue;

        for (int j = 0; j < vpf; j++) {
            for (int k = 0; k < 3; k++) {
                    int blockId = row;
                    per_tet_sum_curl(t0) += abs(all_curl(blockId + 3*j + k));
                    per_tet_sum_curl(t1) += abs(all_curl(blockId + 3*j + k));
            }
        }
        row += 3 * vpf;
    }


    LOG::INFO("allCurl.max: {}",all_curl.maxCoeff());
    LOG::INFO("allCurl.max: {}",all_curl.minCoeff());
    LOG::INFO("unrolled.max: {}",unrolled.maxCoeff());
    LOG::INFO("unrolled.max: {}",unrolled.minCoeff());

    double unrolled_min = 1000000;
    for(int i = 0; i < unrolled.size(); i++) {
        if ( unrolled_min > abs(unrolled(i)))
            unrolled_min = abs(unrolled(i));
    }

    LOG::INFO("unrolled abs min: {}", unrolled_min);

    double all_curl_min = 1000000;
    for(int i = 0; i < all_curl.size(); i++ ) {
        if ( all_curl_min > abs(all_curl(i)))
            all_curl_min = abs(all_curl(i));

        if ( abs(all_curl(i)) < 1e-13 && abs(all_curl(i)) > 0)
            LOG::INFO("FISHY {} {}", i, abs(all_curl(i)));
    }

    LOG::INFO("all_curl abs min: {}", all_curl_min);

    double per_tet_sum_curl_min = 1000000;
    for(int i = 0; i < per_tet_sum_curl.size(); i++ ) {
        if ( per_tet_sum_curl_min > abs(per_tet_sum_curl(i)))
            per_tet_sum_curl_min = abs(per_tet_sum_curl(i));

        if ( abs(per_tet_sum_curl(i)) < 1e-10 && abs(per_tet_sum_curl(i)) > 0)
            LOG::INFO("FISHY {} {}", i, abs(per_tet_sum_curl(i)));
    }

    LOG::INFO("per_tet_sum abs min: {}", per_tet_sum_curl_min);

    LOG::INFO("min frame entry: {}", frames.minCoeff());
    LOG::INFO("max frame entry: {}", frames.maxCoeff());

    /* Polyscope */
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    if (showViz) { // polyscope
        polyscope::init();

        auto *tetc = polyscope::registerPointCloud("Centroids", centroids);
        glm::vec3 dotcolor(0.1, 0.1, 0.1);
        tetc->setPointColor(dotcolor);
        tetc->setPointRadius(0.001);
        for (int i = 0, i_end = framefieldvecs.size(); i < i_end; i++) {
            if (i % 2 == 0)
                continue;
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

        polyscope::registerTetMesh("tet_mesh", V, T);
        auto scalarQ = polyscope::getVolumeMesh("tet_mesh")->addCellScalarQuantity("real_curl_sum", per_tet_sum_curl);
        scalarQ->setEnabled(true);

        auto *psMesh = polyscope::registerSurfaceMesh("Boundary Mesh", V, bdryF);
        psMesh->setTransparency(0.2);
        psMesh->setSurfaceColor({ 0.5,0.5,0.0 });

        auto* seammesh = polyscope::registerSurfaceMesh("Seam", seamV, seamF);
        seammesh->setSurfaceColor({ 0.0, 0.0, 0.0 });

        // visualize!
        polyscope::show();
    }
    else {
            std::ofstream out(badverts);
            if (!out) {
                LOG::ERROR( "Cannot write file: {}", badverts);
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

