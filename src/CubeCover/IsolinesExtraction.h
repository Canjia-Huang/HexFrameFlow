// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/the13fools/Mint3D
//   Original File Path:  /src/CubeCover/IsolinesExtraction.cpp
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

#pragma once

#include <Eigen/Core>
#include "TetMeshConnectivity.h"

namespace CubeCover {
  // Extract isolines of the given values (ntetx x 3)
  void ExtractIsolines(const Eigen::MatrixXd& V, const CubeCover::TetMeshConnectivity& mesh, const Eigen::MatrixXd& values,
                       Eigen::MatrixXd& P,
                       Eigen::MatrixXi& E,
                       Eigen::MatrixXd& P2,
                       Eigen::MatrixXi& E2);
}