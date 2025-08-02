// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/evouga/CubeCover
//   Original File Path:  /tools/integergrid/src/main.cpp
//
// Copyright (c) 2025 evouga
// -----------------------------------------------------------------------------
// Modifications made by Canjia Huang on 2025-8-2:
//   - Adjusted code formatting in selected sections
//   - Use CLI11 for command line control
//   - Added code comments
//
// =============================================================================

#include <iostream>
#include <Eigen/Core>
#include "CubeCover.h"
#include "ReadFrameField.h"
#include "readMeshFixed.h"
#include "WriteHexEx.h"
#include "CLI11/CLI11.hpp"

int main(const int argc, char *argv[]) {
    /* Settings */
    std::string meshfile;
    std::string frafile;
    std::string permfile;
    std::string outfile;
    double curl_correction = 0.0;

    /* App */
    CLI::App app{"IntegerGrid"};
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
        "--perm",
        permfile,
        "Parameterization file (.perm) path"
        )->check(CLI::ExistingFile);
    app.add_option(
        "output_path",
        outfile,
        "Output file (.hexex) path"
        );

    CLI11_PARSE(app, argc, argv);

    /* Process */

    /* Read mesh */
    Eigen::MatrixXd V;
    Eigen::MatrixXi T;
    if (Eigen::MatrixXi F;
        !CubeCover::readMESH(meshfile, V, T, F)) {
        std::cerr << "could not read .mesh file: " << meshfile << std::endl;
        return -1;
    }

    /* Read frame field */
    Eigen::MatrixXd frames;
    Eigen::MatrixXi assignments;
    if (!CubeCover::readFrameField(frafile, permfile, T, frames, assignments, true)) {
        std::cerr << "could not read frames/permutations" << std::endl;
        return -1;
    }

    /* CubeCover */
    CubeCover::CubeCoverOptions opt;
    opt.parameterizationType = CubeCover::CubeCoverOptions::ParameterizationType::PT_INTEGERGRID;
    opt.assignmentHandling = (!permfile.empty() ?
        CubeCover::CubeCoverOptions::AssignmentHandling::AH_USEPROVIDED :
        CubeCover::CubeCoverOptions::AssignmentHandling::AH_RECOMPUTE);
    opt.boundaryConditions = CubeCover::CubeCoverOptions::BoundaryConditions::BC_FORCEINTEGER;
    opt.solver = CubeCover::CubeCoverOptions::MIPSolver::MS_COMISO;
    opt.curlCorrection = curl_correction; /* set to something non-zero if you want curl-correction. 1.0 == 100% change
    in the input frames allowed. */

    opt.verbose = true;
    Eigen::MatrixXd values;
    if (!CubeCover::cubeCover(V, T, frames, assignments, values, opt)) {
        std::cerr << "cubeCover failed" << std::endl;
        return -1;
    }

    if (!CubeCover::writeHexEx(outfile, V, T, values)) {
        std::cerr << "could not write output file " << outfile << std::endl;
        return -1;
    }

    return 0;
}