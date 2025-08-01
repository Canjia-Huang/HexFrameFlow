// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/the13fools/Mint3D
//   Original File Path:  /src/CubeCover/TetMeshDualLaplace.cpp
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

#include "TetMeshDualLaplace.h"

#include "TetMeshConnectivity.h"



namespace CubeCover
{

    TetMeshDualLaplace::TetMeshDualLaplace(const Eigen::MatrixXi &T, const Eigen::MatrixXd &V)
    {
        CubeCover::TetMeshConnectivity tmc(T);

        int ntets = tmc.nTets();
        int nedges = tmc.nEdges();
        int nfaces = tmc.nFaces();


        int num_elements = tmc.nBoundaryElements() + tmc.nTets();

        Eigen::MatrixXd tet_volumes = Eigen::MatrixXd::Zero(ntets, 4);
        tet_circumcenters.resize(ntets);
        Eigen::MatrixXd tet_dual_edge_lengths = Eigen::MatrixXd::Zero(ntets, 4);

        for(int i = 0; i < ntets; i++)
        {
            Eigen::Matrix<double, 4, 3> tet;
            for(int j = 0; j < 4; j++)
            {
                tet.row(j) = V.row(T(i, j));
            }

            circumcenter(tet, tet_circumcenters[i]);

            for (int j = 0; j < 4; j++)
            {
                Eigen::Vector3d a = V.row(tmc.faceVertex(tmc.tetFace(i, j), 0));
                Eigen::Vector3d b = V.row(tmc.faceVertex(tmc.tetFace(i, j), 1));
                Eigen::Vector3d c = V.row(tmc.faceVertex(tmc.tetFace(i, j), 2));
                tet_volumes(i, j) = std::abs(volume(tet_circumcenters[i], a, b, c));
                Eigen::Vector3d n = (b - a).cross(c - a);
                n.normalize();

                tet_dual_edge_lengths(i, j) = std::abs(n.dot(tet_circumcenters[i] - a));

            }
        }

        dual_volumes.resize(nfaces);
        dual_edge_lengths.resize(nfaces);
        weights.resize(nfaces);


        for(int i = 0; i < nfaces; i++)
        {
            dual_volumes.at(i) = 0.;
            dual_edge_lengths.at(i) = 0.;

            double factor = 1.;

            for(int j = 0; j < 2; j++)
            {
                int tet = tmc.faceTet(i, j);
                if (tet == -1)
                {
                    factor = 2;
                    continue;
                }

                int tetfaceidx = -1;
                for(int k = 0; k < 4; k++)
                {
                    if (tmc.tetFace(tet, k) == i)
                    {
                        tetfaceidx = k;
                        break;
                    }
                }

                dual_volumes.at(i) += tet_volumes(tet, tetfaceidx) * factor;
                dual_edge_lengths.at(i) += tet_dual_edge_lengths(tet, tetfaceidx) * factor;
            }

            weights.at(i) = dual_volumes.at(i) / (dual_edge_lengths.at(i) * dual_edge_lengths.at(i));
        }
        
    }

    double TetMeshDualLaplace::volume(const Eigen::Vector3d& a, const Eigen::Vector3d& b, const Eigen::Vector3d& c, const Eigen::Vector3d& d)
    {
        return (1.0 / 6.0) * (a - d).dot((b - d).cross(c - d));    
    }

    void TetMeshDualLaplace::circumcenter(const Eigen::Matrix<double, 4, 3>& t, Eigen::Vector3d& c)
    {
        Eigen::Matrix3d A;
        Eigen::Vector3d b;
        
        const double n0 = t.row(0).squaredNorm();
        
        for(int k = 0; k < 3; ++k)
        {
            A.row(k) = t.row(k + 1) - t.row(0);
            b(k) = t.row(k + 1).squaredNorm() - n0;
        }
        
        c = 0.5 * A.fullPivHouseholderQr().solve(b);
    }


    void TetMeshDualLaplace::barycenter(const Eigen::Matrix<double, 4, 3>& t, Eigen::Vector3d& c)
    {
        Eigen::Matrix3d A;
        Eigen::Vector3d b;

        c = t.row(0) + t.row(1) + t.row(2) + t.row(3);
        
        const double n0 = t.row(0).squaredNorm();
        
        for(int k = 0; k < 3; ++k)
        {
            A.row(k) = t.row(k + 1) - t.row(0);
            b(k) = t.row(k + 1).squaredNorm() - n0;
        }
        
        c = 0.5 * A.fullPivHouseholderQr().solve(b);
    }






}