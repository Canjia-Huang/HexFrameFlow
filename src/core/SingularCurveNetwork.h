// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/the13fools/Mint3D
//   Original File Path:  /src/CubeCover/SingularCurveNetwork.h
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

#ifndef SINGULARCURVENETWORK_H
#define SINGULARCURVENETWORK_H

#include <Eigen/Core>

namespace CubeCover
{
    class FrameField;
    class TetMeshConnectivity;
};


void extractSingularCurveNetwork(const Eigen::MatrixXd& V,
    const CubeCover::TetMeshConnectivity& mesh,
    const CubeCover::FrameField& field,
    Eigen::MatrixXd& Pgreen, Eigen::MatrixXi& Egreen,
    Eigen::MatrixXd& Pblue, Eigen::MatrixXi& Eblue,
    Eigen::MatrixXd& Pblack, Eigen::MatrixXi& Eblack
);

// 0: +1/4 singularity, 1: 1/4 singularity, 2: irregular singularity
std::vector<int> getSingularEdgeLabels(const Eigen::MatrixXd& V,
    const CubeCover::TetMeshConnectivity& mesh,
    const CubeCover::FrameField& field
);

#endif