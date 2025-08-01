// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/the13fools/Mint3D
//   Original File Path:  /src/CubeCover/MeshSubdivide.cpp
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

#include "MeshSubdivide.h"

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <iostream>

#include "FrameField.h"
#include "SingularCurveNetwork.h"

namespace CubeCover {
void MeshSubdivision(const Eigen::MatrixXd& V, const TetMeshConnectivity& mesh, const std::vector<int>& face_to_subdiv,
                     Eigen::MatrixXd& V_sub, TetMeshConnectivity& mesh_sub, std::vector<int>* sub_to_initial_tet_map,
                     std::vector<std::vector<int>>* initial_to_sub_tet_map) {
  int nfaces_to_subdiv = face_to_subdiv.size();
  std::vector<int> sub_v_to_initial_v;
  for (int i = 0; i < V.rows(); i++) {
    sub_v_to_initial_v.push_back(i);
  }

  int max_num_new_verts =
      3 * nfaces_to_subdiv;  // at most 3 new vertices per face: 1 in the center and 2 for the adjacent tet centers

  V_sub = V;
  V_sub.conservativeResize(V.rows() + max_num_new_verts, Eigen::NoChange);

  int n_init_verts = V.rows();
  int n_subdiv_verts = n_init_verts;

  std::unordered_map<int, int> faceid_to_subdiv_vert;

  for (auto fid : face_to_subdiv) {
    V_sub.row(n_subdiv_verts) =
        (V.row(mesh.faceVertex(fid, 0)) + V.row(mesh.faceVertex(fid, 1)) + V.row(mesh.faceVertex(fid, 2))) / 3.0;
    faceid_to_subdiv_vert[fid] = n_subdiv_verts;
    n_subdiv_verts++;
  }

  int n_init_tets = mesh.nTets();
  std::vector<std::vector<Eigen::RowVector4i>> new_tets(n_init_tets);

  for (int tet_id = 0; tet_id < n_init_tets; tet_id++) {
    Eigen::Vector4i tet_face_to_subdiv(0, 0, 0, 0);
    for (int j = 0; j < 4; j++) {
      if (faceid_to_subdiv_vert.count(mesh.tetFace(tet_id, j))) {
        tet_face_to_subdiv(j) = 1;
      }
    }
    int total_subdiv = tet_face_to_subdiv.sum();

    // this tet is not needed to subdivide
    if (!total_subdiv) {
      new_tets[tet_id].push_back(Eigen::RowVector4i(mesh.tetVertex(tet_id, 0), mesh.tetVertex(tet_id, 1),
                                                    mesh.tetVertex(tet_id, 2), mesh.tetVertex(tet_id, 3)));
    } else if (total_subdiv == 1) {
      // we subdivide this tet into 3 tets
      int sub_fidix = -1;
      for (int j = 0; j < 4; j++) {
        if (tet_face_to_subdiv[j] != 0) {
          sub_fidix = j;
          break;
        }
      }
      assert(sub_fidix != -1);
      int new_vid = faceid_to_subdiv_vert[mesh.tetFace(tet_id, sub_fidix)];
      int opp_vid =
          mesh.tetVertex(tet_id, sub_fidix);  // the tetFace returns that face id which is opposite to the vertex id

      for (int j = 1; j <= 3; j++) {
        int v0 = mesh.tetVertex(tet_id, (sub_fidix + j) % 4);
        int next_id = j == 3 ? 1 : j + 1;
        int v1 = mesh.tetVertex(tet_id, (sub_fidix + next_id) % 4);

        Eigen::Matrix3d A;
        A.row(0) = V_sub.row(v1) - V_sub.row(v0);
        A.row(1) = V_sub.row(new_vid) - V_sub.row(v0);
        A.row(2) = V_sub.row(opp_vid) - V_sub.row(v0);

        // make sure it is oriented correctly
        if (A.determinant() < 0) {
          new_tets[tet_id].push_back(Eigen::RowVector4i(v0, v1, opp_vid, new_vid));
        } else {
          new_tets[tet_id].push_back(Eigen::RowVector4i(v0, v1, new_vid, opp_vid));
        }
      }
    } else {
      // In these cases, a new vertex is added to the center of the tetrahedron
      // We first divide the tetrahedron into 4 smaller tetrahedra with center vertex
      // Then we divide each of the 4 smaller tetrahedra into 3 tetrahedra if one of its face is subdivided
      V_sub.row(n_subdiv_verts) = (V.row(mesh.tetVertex(tet_id, 0)) + V.row(mesh.tetVertex(tet_id, 1)) +
                                   V.row(mesh.tetVertex(tet_id, 2)) + V.row(mesh.tetVertex(tet_id, 3))) /
                                  4.0;

      Eigen::RowVector4i init_tet(mesh.tetVertex(tet_id, 0), mesh.tetVertex(tet_id, 1), mesh.tetVertex(tet_id, 2),
                                  mesh.tetVertex(tet_id, 3));

      for (int j = 0; j < 4; j++) {
        if (tet_face_to_subdiv[j] == 0) {
          // this face is not subdivided
          Eigen::RowVector4i new_tet = init_tet;
          new_tet(j) = n_subdiv_verts;

          Eigen::Matrix3d A;
          A.row(0) = V_sub.row(new_tet[1]) - V_sub.row(new_tet[0]);
          A.row(1) = V_sub.row(new_tet[2]) - V_sub.row(new_tet[0]);
          A.row(2) = V_sub.row(new_tet[3]) - V_sub.row(new_tet[0]);

          if (A.determinant() < 0) {
            std::swap(new_tet[2], new_tet[3]);
          }
          new_tets[tet_id].push_back(new_tet);
        } else {
          // this face is subdivided
          int new_face_vid = faceid_to_subdiv_vert[mesh.tetFace(tet_id, j)];
          for (int k = 1; k <= 3; k++) {
            int v0 = mesh.tetVertex(tet_id, (j + k) % 4);
            int next_id = k == 3 ? 1 : k + 1;
            int v1 = mesh.tetVertex(tet_id, (j + next_id) % 4);

            // make sure it is oriented correctly
            Eigen::Matrix3d A;
            A.row(0) = V_sub.row(v1) - V_sub.row(v0);
            A.row(1) = V_sub.row(new_face_vid) - V_sub.row(v0);
            A.row(2) = V_sub.row(n_subdiv_verts) - V_sub.row(v0);

            if (A.determinant() < 0) {
              new_tets[tet_id].push_back(Eigen::RowVector4i(v0, v1, n_subdiv_verts, new_face_vid));
            } else {
              new_tets[tet_id].push_back(Eigen::RowVector4i(v0, v1, new_face_vid, n_subdiv_verts));
            }
          }
        }
      }
      n_subdiv_verts++;
    }
  }

  int max_num_new_tets = 6 * nfaces_to_subdiv;  // each added face will add 3 new subfaces, which are contained at most
                                                // twice in the new subdivided mesh
  Eigen::MatrixXi T_sub(n_init_tets + max_num_new_tets, 4);

  int n_sub_tets = 0;
  if (initial_to_sub_tet_map) {
    initial_to_sub_tet_map->resize(n_init_tets);
  }
  for (int tet_id = 0; tet_id < n_init_tets; tet_id++) {
    for (auto& new_tet : new_tets[tet_id]) {
      T_sub.row(n_sub_tets) = new_tet;
      if (sub_to_initial_tet_map) {
        sub_to_initial_tet_map->push_back(tet_id);
      }
      if (initial_to_sub_tet_map) {
        (*initial_to_sub_tet_map)[tet_id].push_back(n_sub_tets);
      }
      n_sub_tets++;
    }
  }
  T_sub.conservativeResize(n_sub_tets, Eigen::NoChange);
  V_sub.conservativeResize(n_subdiv_verts, Eigen::NoChange);

  mesh_sub = TetMeshConnectivity(T_sub);
}

void MeshSubdivision(const Eigen::MatrixXd& V, const TetMeshConnectivity& mesh, const Eigen::MatrixXd& frames,
                     Eigen::MatrixXd& V_sub, TetMeshConnectivity& mesh_sub, Eigen::MatrixXd& frames_sub,
                     std::vector<int>* sub_to_initial_tet_map, std::vector<std::vector<int>>* initial_to_sub_tet_map) {
  Eigen::MatrixXi assiangments;
  std::unique_ptr<FrameField> frame_fields(fromFramesAndAssignments(mesh, frames, assiangments, true));
  frame_fields->computeLocalAssignments();
  frame_fields->combAssignments();
  int n_singular_edges = frame_fields->nSingularEdges();

  std::unordered_set<int> singular_edge_ids;
  for (int i = 0; i < n_singular_edges; i++) {
    singular_edge_ids.insert(frame_fields->singularEdge(i));
  }

  std::unordered_set<int> face_to_subdiv_set;

  // for(int i = 0; i < mesh.nFaces(); i++) {
  //   face_to_subdiv_set.insert(i);
  // }

  for (int tid = 0; tid < mesh.nTets(); tid++) {
    std::vector<int> edge_ids;
    for (int j = 0; j < 6; j++) {
      int eid = mesh.tetEdge(tid, j);
      if (singular_edge_ids.count(eid)) {
        edge_ids.push_back(eid);
      }
    }
    if (edge_ids.size() > 2) {
      for (int j = 0; j < 4; j++) {
        face_to_subdiv_set.insert(mesh.tetFace(tid, j));
      }
    } else if (edge_ids.size() == 2) {
      std::unordered_set<int> edge_verts;
      for (auto eid : edge_ids) {
        edge_verts.insert(mesh.edgeVertex(eid, 0));
        edge_verts.insert(mesh.edgeVertex(eid, 1));
      }
      // in this case we need to subdivide the face. Actually we only need to split the tet into four tets. But for convience, we subdivide all the face
      if (edge_verts.size() == 4) {
        for (int j = 0; j < 4; j++) {
          face_to_subdiv_set.insert(mesh.tetFace(tid, j));
        }
      } else {
          // in this case the tet has two singular edges on the same face, we only need to subdivide that face
          for (int j = 0; j < 4; j++) {
		  int fid = mesh.tetFace(tid, j);
		  int singular_edges_on_face = 0;
          for (int k = 0; k < 3; k++) {
			int eid = mesh.faceEdge(fid, k);
            if (edge_ids[0] == eid || edge_ids[1] == eid) {
			  singular_edges_on_face++;
			}
		  }
          if (singular_edges_on_face > 1) {
			face_to_subdiv_set.insert(fid);
		  }
		}
      }
    }
  }
  std::vector<int> face_to_subdiv(face_to_subdiv_set.begin(), face_to_subdiv_set.end());
  std::vector<int> sub_to_init_tet_map;
  MeshSubdivision(V, mesh, face_to_subdiv, V_sub, mesh_sub, &sub_to_init_tet_map, initial_to_sub_tet_map);
  frames_sub.resize(mesh_sub.nTets(), 9);

  for (int i = 0; i < mesh_sub.nTets(); i++) {
    int init_tet_id = sub_to_init_tet_map[i];
    frames_sub.row(i) = frames.row(init_tet_id);
  }

  if (sub_to_initial_tet_map) {
    *sub_to_initial_tet_map = sub_to_init_tet_map;
  }
}

bool TestMeshSubdivision(const Eigen::MatrixXd& V, const TetMeshConnectivity& mesh, const Eigen::MatrixXd& frames,
                         Eigen::MatrixXd& V_sub, TetMeshConnectivity& mesh_sub, Eigen::MatrixXd& frames_sub) {
  std::vector<int> sub_to_initial_tet_map;
  std::vector<std::vector<int>> initial_to_sub_tet_map;
  CubeCover::MeshSubdivision(V, mesh, frames, V_sub, mesh_sub, frames_sub, &sub_to_initial_tet_map,
                             &initial_to_sub_tet_map);

  // correct map
  bool sub_to_init_correct = true;
  for (int tid = 0; tid < mesh_sub.nTets(); tid++) {
    int init_tid = sub_to_initial_tet_map[tid];
    if (frames.row(init_tid) != frames_sub.row(tid)) {
      sub_to_init_correct = false;
      std::cout << "Miss match in tid: " << tid << ", init tid: " << init_tid << std::endl;
      std::cout << "sub frames:  " << frames_sub.row(tid) << std::endl;
      std::cout << "init frames: " << frames.row(init_tid) << std::endl;
    }
  }

  bool init_to_sub_correct = true;
  for (int init_tid = 0; init_tid < mesh.nTets(); init_tid++) {
    for (auto tid : initial_to_sub_tet_map[init_tid]) {
      if (frames.row(init_tid) != frames_sub.row(tid)) {
        init_to_sub_correct = false;
        std::cout << "Miss match in init tid: " << init_tid << ", tid: " << tid << ", " << sub_to_initial_tet_map[tid] << std::endl;
        std::cout << "init frames: " << frames.row(init_tid) << std::endl;
        std::cout << "sub frames:  " << frames_sub.row(tid) << std::endl;
      }
    }
  }

  Eigen::MatrixXi assiangments;
  std::unique_ptr<FrameField> frame_fields(fromFramesAndAssignments(mesh, frames, assiangments, false));
  frame_fields->computeLocalAssignments();
  frame_fields->combAssignments();
  int n_singular_edges = frame_fields->nSingularEdges();
  std::vector<int> singular_edge_labels = getSingularEdgeLabels(V, mesh, *frame_fields);
  int n_black = 0, n_green = 0, n_blue = 0;
  for(int i = 0; i < singular_edge_labels.size(); i++) {
    n_black += (singular_edge_labels[i] == 2);
    n_green += (singular_edge_labels[i] == 0);
    n_blue += (singular_edge_labels[i] == 1);
  }

  Eigen::MatrixXi assiangments_sub;
  std::unique_ptr<FrameField> frame_fields_sub(fromFramesAndAssignments(mesh_sub, frames_sub, assiangments_sub, false));
  frame_fields_sub->computeLocalAssignments();
  frame_fields_sub->combAssignments();
  int n_singular_edges_sub = frame_fields_sub->nSingularEdges();
  std::vector<int> singular_edge_labels_sub = getSingularEdgeLabels(V_sub, mesh_sub, *frame_fields_sub);
  int n_black_sub = 0, n_green_sub = 0, n_blue_sub = 0;
  for(int i = 0; i < singular_edge_labels_sub.size(); i++) {
    n_black_sub += (singular_edge_labels_sub[i] == 2);
    n_green_sub += (singular_edge_labels_sub[i] == 0);
    n_blue_sub += (singular_edge_labels_sub[i] == 1);
  }

  // the singular edges should be the same
  if (n_singular_edges != n_singular_edges_sub) {
	std::cout << "Before subdivision the frames have " << n_singular_edges << " singular edges, " << n_black << " black edges, " << n_green << ", green edges, " << n_blue << " blue edges" << std::endl;
	std::cout << "After subdivision the frames have " << n_singular_edges_sub << " singular edges, " << n_black_sub << " black edges, " << n_green_sub << ", green edges, " << n_blue_sub << " blue edges" << std::endl;
	return false;
  }

  std::vector<std::tuple<int, int, int>> singular_edge_vec, singular_edge_sub_vec;
  for (int i = 0; i < n_singular_edges; i++) {
    int eid = frame_fields->singularEdge(i);
    singular_edge_vec.push_back(std::make_tuple(mesh.edgeVertex(eid, 0), mesh.edgeVertex(eid, 1), singular_edge_labels[i]));
  }

  std::sort(singular_edge_vec.begin(), singular_edge_vec.end());

  for (int i = 0; i < n_singular_edges_sub; i++) {
    int eid = frame_fields_sub->singularEdge(i);
    singular_edge_sub_vec.push_back(std::make_tuple(mesh_sub.edgeVertex(eid, 0), mesh_sub.edgeVertex(eid, 1), singular_edge_labels_sub[i]));
  }
  std::sort(singular_edge_sub_vec.begin(), singular_edge_sub_vec.end());

  for (int i = 0; i < n_singular_edges; i++) {
      if (singular_edge_vec[i] != singular_edge_sub_vec[i]) {
	  std::cout << "singular edge " << i << " is not the same" << std::endl;
	  std::cout << "before subdivision: " << std::get<0>(singular_edge_vec[i]) << " " << std::get<1>(singular_edge_vec[i]) << ", color: 0 for green, 1 for blue, 2 for black: " << std::get<2>(singular_edge_vec[i]) << std::endl;
	  std::cout << "after subdivision: " << std::get<0>(singular_edge_sub_vec[i]) << " " << std::get<1>(singular_edge_sub_vec[i]) << ", color: 0 for green, 1 for blue, 2 for black: " << std::get<2>(singular_edge_sub_vec[i]) << std::endl;
	  return false;
	}
  }

  // Each tetrahedron has at most one singular edge
  std::cout << "After subdivision the frames have " << n_singular_edges_sub << " singular edges" << std::endl;
  std::unordered_set<int> edge_set;
  for (int i = 0; i < n_singular_edges_sub; i++) {
    edge_set.insert(frame_fields_sub->singularEdge(i));
  }

  bool is_valid = true;
  for (int i = 0; i < mesh_sub.nTets(); i++) {
    int count = 0;
    for (int j = 0; j < 6; j++) {
      int eid = mesh_sub.tetEdge(i, j);
      if (edge_set.count(eid)) {
        count++;
      }
    }
    if (count >= 2) {
      std::cout << "tet " << i << " has " << count << " singular edges" << std::endl;
      is_valid = false;
    }
  }

  return init_to_sub_correct && sub_to_init_correct && is_valid;
}
}  // namespace core