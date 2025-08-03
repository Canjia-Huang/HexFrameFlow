// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/the13fools/Mint3D
//   Original File Path:  /src/CubeCover/Integration.h
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

#ifndef INTEGRATION_H
#define INTEGRATION_H

#include <Eigen/Core>
#include "CubeCover.h"

namespace CubeCover {

    class FrameField;

    /*
     * Performs the actual integration, by solving the problem
     *  min_phi \sum_{tets i} \sum_{frame vector j} ||\nabla \phi_j - f^i_j||^2
     *      s.t. \phi differing by an integer jump, as well as the provided 
     *           local reassignment of vectors and signs, across each interior
     *           face.
     * using the Gurobi MIP solver.
     * Optionally also enforces integer-grid and boundary alignment constraints.
     */
    bool integrate(const Eigen::MatrixXd& V, const FrameField& field, Eigen::MatrixXd& soupValues, const CubeCoverOptions &options);

};

#endif