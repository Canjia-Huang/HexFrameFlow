// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/the13fools/Mint3D
//   Original File Path:  /src/CubeCover/ReadMoments.h
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

#ifndef READMOMENTS_H
#define READMOMENTS_H

#include <string>
#include <Eigen/Core>

namespace CubeCover
{
    /*
     * Reads (currently) 4th and 2nd moments of a tetrahedral mesh and boundary .mom file. Only 
     * supports 3D parameterizations. Follows the file format specification
     * of the mint library.
     * Returns true if loading is successful. On success, ME will be a
     * matrix of (ntets+nboundelements) x 22 of moments per interior volume element and surface element
     */
    bool readMoments(const std::string& filename, Eigen::MatrixXd& ME, bool verbose = false);
};

#endif