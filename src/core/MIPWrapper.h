// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/the13fools/Mint3D
//   Original File Path:  /src/CubeCover/MIPWrapper.h
//   Original License:    Mozilla Public License 2.0 (MPL-2.0)
//
// Copyright (c) 2025 Josh Vekhter
// -----------------------------------------------------------------------------
// Modifications made by Canjia Huang on 2025-8-1:
//   - Adjusted code formatting in selected sections
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.
// =============================================================================

#ifndef MIPWRAPPER_H
#define MIPWRAPPER_H

#include <Eigen/Sparse>
#include <Eigen/Core>
#include "CubeCover.h"

namespace CubeCover {

    /*
     * Solves the following MIP to tolerance tol:
     * min_x  x^T A x + b x
     *  s.t.  C [x 1]^T = 0
     *        x_i is integer, for each i in intvars
     * Note that A is n x n, b is n x 1, and C is m x (n+1).
     */

    bool GurobiMIPWrapper(const Eigen::SparseMatrix<double>& constraints,
        const Eigen::SparseMatrix<double>& A,
        Eigen::VectorXd& result,
        const Eigen::VectorXd& b,
        const std::vector<int>& intvars,
        double tol,
        bool verbose);

    bool CoMISoMIPWrapper(const Eigen::SparseMatrix<double>& constraints,
        const Eigen::SparseMatrix<double>& A,
        Eigen::VectorXd& result,
        const Eigen::VectorXd& b,
        const std::vector<int>& intvars,
        double tol,
        bool verbose);


    inline bool MIP(const Eigen::SparseMatrix<double>& constraints,
        const Eigen::SparseMatrix<double>& A,
        Eigen::VectorXd& result,
        const Eigen::VectorXd& b,
        const std::vector<int>& intvars,
        const CubeCover::CubeCoverOptions& opts)
    {
        if (opts.solver == CubeCoverOptions::MIPSolver::MS_GUROBI)
        {
            return GurobiMIPWrapper(constraints, A, result, b, intvars, opts.MIPtol, opts.verbose);
        }
        else if (opts.solver == CubeCoverOptions::MIPSolver::MS_COMISO)
        {
            return CoMISoMIPWrapper(constraints, A, result, b, intvars, opts.MIPtol, opts.verbose);
        }

        // unreachable
        return false;
    }
    
};

#endif
