// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/the13fools/Mint3D
//   Original File Path:  /src/CubeCover/ReadHexEx.h
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

#ifndef READHEXEX_H
#define READHEXEX_H

#include <string>
#include <Eigen/Core>

namespace CubeCover
{
    /*
     * Reads a tetrahedral mesh and parameterization from a .hexex file. Only 
     * supports 3D parameterizations. Follows the file format specification
     * of the libhexex library.
     * Returns true if loading is successful. On success, V will be a
     * nverts x 3 matrix of vertex positions (one row per vertex) and T a
     * ntets x 4 matrix of tetrahedra vertex indices.
     * vals contains the parameterization values. They are defined indepedently
     * on each tet (i.e. on a tetrahedral soup); vals will have size
     * 4*ntets x 3, with the first four rows the parameter values on the four
     * vertices of the first tet, etc.
     */
    bool readHexEx(const std::string& filename, Eigen::MatrixXd& V, Eigen::MatrixXi& T, Eigen::MatrixXd &vals);
};

#endif