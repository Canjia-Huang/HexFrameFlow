// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/evouga/CubeCover
//   Original File Path:  /tools/volumeparamviewer/src/ExtractIsolines.h
//
// Copyright (c) 2025 evouga
// -----------------------------------------------------------------------------
// Modifications made by Canjia Huang on 2025-8-3:
//   - Adjusted code formatting in selected sections
//   - Added code comments
//
// =============================================================================

#ifndef EXTRACTISOLINES_H
#define EXTRACTISOLINES_H

#include <Eigen/Core>
#include <vector>
#include <glm/glm.hpp>


namespace CubeCover
{
    class TetMeshConnectivity;
};

void extractIsolines(const Eigen::MatrixXd& V, const CubeCover::TetMeshConnectivity& mesh, const Eigen::MatrixXd& values,
    Eigen::MatrixXd& P,
    Eigen::MatrixXi& E,
    Eigen::MatrixXd& P2,
    Eigen::MatrixXi& E2,
    std::vector<int> badverts);


void extractPoints(const Eigen::MatrixXd& V, const CubeCover::TetMeshConnectivity& mesh, const Eigen::MatrixXd& values,
    std::vector<glm::vec3>& points,
    std::vector<std::array<double, 3>>& colors );

#endif
