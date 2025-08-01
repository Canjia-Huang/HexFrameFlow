// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/the13fools/Mint3D
//   Original File Path:  /src/CubeCover/SingularCurveNetwork.cpp
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

#include "SingularCurveNetwork.h"
#include "FrameField.h"
#include "TetMeshConnectivity.h"
#include <map>
#include <set>
#include "AssignmentGroup.h"

static double computeDihedralAngle(
  const Eigen::MatrixXd& V,
  const CubeCover::TetMeshConnectivity& mesh,
  const int edge,
  const int edge_tetid
  ) {
  const int v0 = mesh.edgeVertex(edge, 0);
  const int v1 = mesh.edgeVertex(edge, 1);

  const int tet_id = mesh.edgeTet(edge, edge_tetid);

  int fid0 = mesh.edgeTetFaceIndex(edge, edge_tetid, 0);
  fid0 = mesh.tetFace(tet_id, fid0);

  int fid1 = mesh.edgeTetFaceIndex(edge, edge_tetid, 1);
  fid1 = mesh.tetFace(tet_id, fid1);

  int v2 = -1;
  for(int j= 0; j < 3; j++) {
    if(mesh.faceVertex(fid0, j) != v0 && mesh.faceVertex(fid0, j) != v1) {
      v2 = mesh.faceVertex(fid0, j);
      break;
    }
  }
  assert(v2 != -1);

  int v3 = -1;
  for(int j= 0; j < 3; j++) {
    if(mesh.faceVertex(fid1, j) != v0 && mesh.faceVertex(fid1, j) != v1) {
      v3 = mesh.faceVertex(fid1, j);
      break;
    }
  }
  assert(v3 != -1);

  Eigen::Vector3d n0 = (V.row(v1) - V.row(v0)).segment<3>(0).cross((V.row(v2) - V.row(v0)).segment<3>(0));
  Eigen::Vector3d n1 = (V.row(v1) - V.row(v0)).segment<3>(0).cross((V.row(v3) - V.row(v0)).segment<3>(0));

  n0.normalize();
  n1.normalize();

  double cos_angle = std::clamp(n0.dot(n1), -1.0, 1.0);
  return acos(cos_angle);
}

void extractSingularCurveNetwork(
  const Eigen::MatrixXd& V,
  const CubeCover::TetMeshConnectivity& mesh,
  const CubeCover::FrameField& field,
  Eigen::MatrixXd& Pgreen, Eigen::MatrixXi& Egreen,
  Eigen::MatrixXd& Pblue, Eigen::MatrixXi& Eblue,
  Eigen::MatrixXd& Pblack, Eigen::MatrixXi& Eblack
  ) {
  std::map<int, int> blacksingverts;
  std::map<int, int> greensingverts;
  std::map<int, int> bluesingverts;
  int nextblackidx = 0;
  int nextblueidx = 0;
  int nextgreenidx = 0;
  std::vector<int> blacksingedges;
  std::vector<int> bluesingedges;
  std::vector<int> greensingedges;
  int nsingedges = field.nSingularEdges();
  int vpf = field.vectorsPerFrame();

  for (int i = 0; i < nsingedges; i++) {
    int edge = field.singularEdge(i);
    int v0 = mesh.edgeVertex(edge, 0);
    int v1 = mesh.edgeVertex(edge, 1);

    bool isgreen = false;
    bool isblue = false;

    if (vpf == 3) {
      int nbtet = mesh.nEdgeTets(edge);
      CubeCover::AssignmentGroup o(vpf);
      for (int j = 0; j < nbtet; j++) {
        int tet = mesh.edgeTet(edge, j);
        int faceidx = mesh.edgeTetFaceIndex(edge, j, 1);
        int face = mesh.tetFace(tet, faceidx);
        int orient = mesh.tetFaceOrientation(tet, faceidx);
        CubeCover::AssignmentGroup faceo = field.faceAssignment(face);

        if (orient == 1) {
          faceo = faceo.inverse();
        }
        o = faceo * o;
      }

      int fixed = -1;
      for (int j = 0; j < 3; j++) {
        if (o.targetVector(j) == j && o.targetSign(j) == 1) {
          fixed = j;
        }
      }
      if (fixed != -1 && nbtet > 0) {
        Eigen::Vector3d edgevec = V.row(mesh.edgeVertex(edge, 1)).transpose() - V.row(mesh.edgeVertex(edge, 0)).transpose();

        int f1 = (fixed + 1) % 3;
        int f2 = (fixed + 2) % 3;
        double ave_voting = 0;
        CubeCover::AssignmentGroup o(vpf);
        for (int j = 0; j < nbtet; j++) {
          int tet = mesh.edgeTet(edge, j);
          int faceidx = mesh.edgeTetFaceIndex(edge, j, 1);
          int face = mesh.tetFace(tet, faceidx);
          int orient = mesh.tetFaceOrientation(tet, faceidx);

          Eigen::Vector3d f1_vec = field.tetFrame(tet).row(o.targetVector(f1)).transpose();
          f1_vec *= o.targetSign(f1);

          Eigen::Vector3d f2_vec = field.tetFrame(tet).row(o.targetVector(f2)).transpose();
          f2_vec *= o.targetSign(f2);
          Eigen::Vector3d  framevec = f1_vec.cross(f2_vec);

          double angle = computeDihedralAngle(V, mesh, edge, j);
          ave_voting += angle / M_PI / 2 * ((framevec.dot(edgevec) > 0) ? 1.0 : -1.0);

          CubeCover::AssignmentGroup faceo = field.faceAssignment(face);

          if (orient == 1)
            faceo = faceo.inverse();
          o = faceo * o;
        }
        int sign = (ave_voting >= 0 ? 1 : -1);

        if (o.targetVector(f1) == f2 && o.targetSign(f1) == -1 && o.targetVector(f2) == f1 && o.targetSign(f2) == 1) {
          if (sign == 1) {
            isgreen = true;
          } else {
            isblue = true;
          }
        }
        else if (o.targetVector(f1) == f2 && o.targetSign(f1) == 1 && o.targetVector(f2) == f1 && o.targetSign(f2) == -1) {
          if (sign == 1) {
            isblue = true;
          } else {
            isgreen = true;
          }
        }
      }
    }
    if (isgreen) {
      auto it = greensingverts.find(v0);
      if (it == greensingverts.end()) {
        greensingverts[v0] = nextgreenidx++;
      }
      it = greensingverts.find(v1);
      if (it == greensingverts.end()) {
        greensingverts[v1] = nextgreenidx++;
      }
      greensingedges.push_back(edge);
    }
    else if (isblue) {
      auto it = bluesingverts.find(v0);
      if (it == bluesingverts.end()) {
        bluesingverts[v0] = nextblueidx++;
      }
      it = bluesingverts.find(v1);
      if (it == bluesingverts.end()) {
        bluesingverts[v1] = nextblueidx++;
      }
      bluesingedges.push_back(edge);
    }
    else {
      auto it = blacksingverts.find(v0);
      if (it == blacksingverts.end()) {
        blacksingverts[v0] = nextblackidx++;
      }
      it = blacksingverts.find(v1);
      if (it == blacksingverts.end()) {
        blacksingverts[v1] = nextblackidx++;
      }
      blacksingedges.push_back(edge);
    }
  }

  Pblack.resize(blacksingverts.size(), 3);
  for (auto it : blacksingverts) {
    Pblack.row(it.second) = V.row(it.first);
  }

  int nblacksingedges = blacksingedges.size();
  Eblack.resize(nblacksingedges, 2);
  for (int i = 0; i < nblacksingedges; i++) {
    int edge = blacksingedges[i];
    int v0 = mesh.edgeVertex(edge, 0);
    int v1 = mesh.edgeVertex(edge, 1);
    Eblack(i, 0) = blacksingverts[v0];
    Eblack(i, 1) = blacksingverts[v1];
  }


  Pgreen.resize(greensingverts.size(), 3);
  for (auto it : greensingverts) {
    Pgreen.row(it.second) = V.row(it.first);
  }

  int ngreensingedges = greensingedges.size();
  Egreen.resize(ngreensingedges, 2);
  for (int i = 0; i < ngreensingedges; i++) {
    int edge = greensingedges[i];
    int v0 = mesh.edgeVertex(edge, 0);
    int v1 = mesh.edgeVertex(edge, 1);
    Egreen(i, 0) = greensingverts[v0];
    Egreen(i, 1) = greensingverts[v1];
  }


  Pblue.resize(bluesingverts.size(), 3);
  for (auto it : bluesingverts) {
    Pblue.row(it.second) = V.row(it.first);
  }

  int nbluesingedges = bluesingedges.size();
  Eblue.resize(nbluesingedges, 2);
  for (int i = 0; i < nbluesingedges; i++) {
    int edge = bluesingedges[i];
    int v0 = mesh.edgeVertex(edge, 0);
    int v1 = mesh.edgeVertex(edge, 1);
    Eblue(i, 0) = bluesingverts[v0];
    Eblue(i, 1) = bluesingverts[v1];
  }
}

std::vector<int> getSingularEdgeLabels(
  const Eigen::MatrixXd& V,
  const CubeCover::TetMeshConnectivity& mesh,
  const CubeCover::FrameField& field
  ) {
  int nsingedges = field.nSingularEdges();
  std::vector labels(nsingedges, 2);
  int vpf = field.vectorsPerFrame();

  for (int i = 0; i < nsingedges; i++) {
    int edge = field.singularEdge(i);

    bool isgreen = false;
    bool isblue = false;

    if (vpf == 3) {
      int nbtet = mesh.nEdgeTets(edge);
      CubeCover::AssignmentGroup o(vpf);
      for (int j = 0; j < nbtet; j++) {
        int tet = mesh.edgeTet(edge, j);
        int faceidx = mesh.edgeTetFaceIndex(edge, j, 1);
        int face = mesh.tetFace(tet, faceidx);
        int orient = mesh.tetFaceOrientation(tet, faceidx);
        CubeCover::AssignmentGroup faceo = field.faceAssignment(face);

        if (orient == 1) faceo = faceo.inverse();
        o = faceo * o;
      }

      int fixed = -1;
      for (int j = 0; j < 3; j++) {
        if (o.targetVector(j) == j && o.targetSign(j) == 1)
          fixed = j;
      }
      if (fixed != -1 && nbtet > 0) {
        Eigen::Vector3d edgevec =
            V.row(mesh.edgeVertex(edge, 1)).transpose() - V.row(mesh.edgeVertex(edge, 0)).transpose();

        CubeCover::AssignmentGroup o(vpf);
        int f1 = (fixed + 1) % 3;
        int f2 = (fixed + 2) % 3;

        double ave_voting = 0;
        for (int j = 0; j < nbtet; ++j) {
          int tet = mesh.edgeTet(edge, j);
          int faceidx = mesh.edgeTetFaceIndex(edge, j, 1);
          int face = mesh.tetFace(tet, faceidx);
          int orient = mesh.tetFaceOrientation(tet, faceidx);

          Eigen::Vector3d f1_vec = field.tetFrame(tet).row(o.targetVector(f1)).transpose();
          f1_vec *= o.targetSign(f1);

          Eigen::Vector3d f2_vec = field.tetFrame(tet).row(o.targetVector(f2)).transpose();
          f2_vec *= o.targetSign(f2);
          Eigen::Vector3d framevec = f1_vec.cross(f2_vec);

          double angle = computeDihedralAngle(V, mesh, edge, j);
          ave_voting += angle / M_PI / 2 * ((framevec.dot(edgevec) > 0) ? 1.0 : -1.0);

          CubeCover::AssignmentGroup faceo = field.faceAssignment(face);

          if (orient == 1) {
            faceo = faceo.inverse();
          }
          o = faceo * o;
        }
        int sign = (ave_voting > 0 ? 1 : -1);

        if (o.targetVector(f1) == f2 && o.targetSign(f1) == -1 && o.targetVector(f2) == f1 && o.targetSign(f2) == 1) {
          if (sign == 1) {
            isgreen = true;
          } else {
            isblue = true;
          }
        } else if (o.targetVector(f1) == f2 && o.targetSign(f1) == 1 && o.targetVector(f2) == f1 &&
                   o.targetSign(f2) == -1) {
          if (sign == 1) {
            isblue = true;
          } else {
            isgreen = true;
          }
        }
      }
    }
    if (isgreen) {
      labels[i] = 0;
    } else if (isblue) {
      labels[i] = 1;
    } else {
      labels[i] = 2;
    }
  }

  return labels;
}