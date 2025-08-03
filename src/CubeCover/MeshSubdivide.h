// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/the13fools/Mint3D
//   Original File Path:  /src/CubeCover/MeshSubdivide.h
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

#include <Eigen/Dense>
#include "TetMeshConnectivity.h"
/*
 * This file contains the implementation of the mesh subdivision algorithm, which is used to subdivide the mesh to make sure that the singular curve network is well represented, that is,
 * Each tetrahedron has at most ONE singular edge
 */

namespace CubeCover {
/* 
 * Subdivide mesh the mesh according to the face_to_subdiv vector.
 * Input:
 * - V: the vertices of the mesh
 * - mesh: the connectivity of the mesh
 * - face_to_subdiv: a vector that specifies which faces to subdivide.
 *
 * Output:
 * - V_sub: the vertices of the subdivided mesh
 * - mesh_sub: the connectivity of the subdivided mesh
 * - sub_to_initial_tet_map: a vector that maps the tetrahedra of the subdivided mesh to the tetrahedra of the original mesh
 * - initial_to_sub_tet_map: a vector that maps the tetrahedra of the original mesh to the tetrahedra of the subdivided mesh
 */
void MeshSubdivision(const Eigen::MatrixXd& V, const TetMeshConnectivity& mesh, const std::vector<int>& face_to_subdiv,
                   Eigen::MatrixXd& V_sub, TetMeshConnectivity& mesh_sub,
                   std::vector<int>* sub_to_initial_tet_map = nullptr,
                   std::vector<std::vector<int>>* initial_to_sub_tet_map = nullptr);

/* 
 * Subdivide the mesh and the frames, such that each tet has at most one singular edge.
 * Input:
 * - V: the vertices of the mesh
 * - mesh: the connectivity of the mesh
 * - frames: the frames of the mesh (each tets has 3 frames)
 *
 * Output:
 * - V_sub: the vertices of the subdivided mesh
 * - mesh_sub: the connectivity of the subdivided mesh
 * - frames_sub: the frames of the subdivided mesh
 * - sub_to_initial_tet_map: a vector that maps the tetrahedra of the subdivided mesh to the tetrahedra of the original mesh
 * - initial_to_sub_tet_map: a vector that maps the tetrahedra of the original mesh to the tetrahedra of the subdivided mesh
 */
void MeshSubdivision(const Eigen::MatrixXd& V, const TetMeshConnectivity& mesh, const Eigen::MatrixXd& frames,
                   Eigen::MatrixXd& V_sub, TetMeshConnectivity& mesh_sub, Eigen::MatrixXd& frames_sub,
                   std::vector<int>* sub_to_initial_tet_map = nullptr,
                   std::vector<std::vector<int>>* initial_to_sub_tet_map = nullptr);

/*
* Test function for the mesh subdivision algorithm.
*/
bool TestMeshSubdivision(const Eigen::MatrixXd& V, const TetMeshConnectivity& mesh, const Eigen::MatrixXd& frames,
                         Eigen::MatrixXd& V_sub, TetMeshConnectivity& mesh_sub, Eigen::MatrixXd& frames_sub);
}