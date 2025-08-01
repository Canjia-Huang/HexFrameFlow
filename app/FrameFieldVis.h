// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/evouga/CubeCover
//   Original File Path:  /tools/singularityviewer/src/FrameFieldVis.h
//
// Copyright (c) 2025 evouga
// -----------------------------------------------------------------------------
// Modifications made by Canjia Huang on 2025-8-1:
//   - Adjusted code formatting in selected sections
//
// =============================================================================

#ifndef FRAMEFIELDVIS_H
#define FRAMEFIELDVIS_H

#include <Eigen/Core>
#include <vector>

namespace CubeCover
{
    class TetMeshConnectivity;
    class FrameField;
};

void buildFrameVectors(const Eigen::MatrixXd& V,
    const CubeCover::TetMeshConnectivity& mesh,
    const CubeCover::FrameField& field,
    double scale,
    Eigen::MatrixXd& centroids,
    std::vector<Eigen::MatrixXd>& frameVectors
    );

#endif