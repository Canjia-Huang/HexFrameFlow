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
#include "utils/log.h"
#include "utils/parse_filepath.h"

int main(const int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::info);

    /* Settings */
    std::string meshfile;
    std::string frafile;
    std::string permfile;
    std::string outfile;
    double curl_correction = 0.0;
    bool use_gurobi = false;
    bool seamless = false;
    bool free_boundary_constrained = false;

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
    app.add_option(
        "-c",
        curl_correction,
        "Set to something non-zero if you want curl-correction. If curlCorrection isn't zero, as a "
        "preprocessing step the frame field will be corrected to make the field locally integrable "
        "(default: 0.0, 1.0 == 100% change in the input frames allowed)"
        )->check(CLI::Range(0., 1.));
    app.add_flag(
        "-g,--gurobi",
        use_gurobi,
        "Use gurobi as the MIP solver. (default: comiso)");
    app.add_flag("-s,--seamless",
        seamless,
        "Whether to compute a seamless parameterization or integer-grid parameterization. "
        "(default: integer-grid)");
    app.add_flag("-f",
        free_boundary_constrained,
        "Free the require of the parameterization to align all boundary faces of the input tetrahedral "
        "mesh to the integer grid of the parameter domain. (default: true)");

    CLI11_PARSE(app, argc, argv);

    /* Process */
    if (use_gurobi) {
#ifndef HAS_GUROBI
        LOG::ERROR("The CMake option 'USE_GUROBI' has not been set.");
        return 1;
#endif
    }
    else {
#ifndef HAS_COMISO
        LOG::ERROR("The CMake option 'USE_COMISO' has not been set.");
        return 1;
#endif
    }

    if (outfile.empty())
        outfile = get_parentpath(frafile) + get_filename(frafile) + ".hexex";
    else if (get_filename(outfile).empty())
        outfile = outfile + ".hexex";

    /* Read mesh */
    Eigen::MatrixXd V;
    Eigen::MatrixXi T;
    if (Eigen::MatrixXi F;
        !CubeCover::readMESH(meshfile, V, T, F)) {
        LOG::ERROR("could not read .mesh file: {}", meshfile);
        return -1;
    }

    /* Read frame field */
    Eigen::MatrixXd frames;
    Eigen::MatrixXi assignments;
    if (!CubeCover::readFrameField(frafile, permfile, T, frames, assignments, true)) {
        LOG::ERROR("could not read frames/permutations");
        return -1;
    }

    /* CubeCover */
    CubeCover::CubeCoverOptions opt;
    opt.parameterizationType = seamless ?
        opt.parameterizationType = CubeCover::CubeCoverOptions::ParameterizationType::PT_SEAMLESS :
        opt.parameterizationType = CubeCover::CubeCoverOptions::ParameterizationType::PT_INTEGERGRID;
    opt.assignmentHandling = permfile.empty() ?
        CubeCover::CubeCoverOptions::AssignmentHandling::AH_RECOMPUTE :
        CubeCover::CubeCoverOptions::AssignmentHandling::AH_USEPROVIDED;
    opt.boundaryConditions = free_boundary_constrained ?
        CubeCover::CubeCoverOptions::BoundaryConditions::BC_FREE :
        CubeCover::CubeCoverOptions::BoundaryConditions::BC_FORCEINTEGER;
    opt.solver = use_gurobi ?
        opt.solver = CubeCover::CubeCoverOptions::MIPSolver::MS_GUROBI :
        opt.solver = CubeCover::CubeCoverOptions::MIPSolver::MS_COMISO;
    opt.curlCorrection = curl_correction;
    opt.verbose = true;

    Eigen::MatrixXd values;
    if (!CubeCover::cubeCover(V, T, frames, assignments, values, opt)) {
        LOG::ERROR("CubeCover failed!");
        return 1;
    }
    LOG::INFO("CubeCover done!");

    if (!CubeCover::writeHexEx(outfile, V, T, values)) {
        LOG::ERROR("could not write output file: {}", outfile);
        return 1;
    }

    return 0;
}
