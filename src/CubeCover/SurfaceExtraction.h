// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/the13fools/Mint3D
//   Original File Path:  /src/CubeCover/SurfaceExtraction.h
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

#ifndef SURFACEEXTRACTION_H
#define SURFACEEXTRACTION_H

#include <Eigen/Core>

namespace CubeCover {

    class TetMeshConnectivity;

    void isosurfaceSoup(const Eigen::MatrixXd& V, const TetMeshConnectivity& mesh, Eigen::MatrixXd& phivals, Eigen::MatrixXd& isoV, Eigen::MatrixXi& isoF);

    void isosurfaceSoupForSingleIsoVal(const Eigen::MatrixXd& V, const TetMeshConnectivity& mesh, Eigen::MatrixXd& phival, double isoval, Eigen::MatrixXd& isoV, Eigen::MatrixXi& isoF);

};

#endif