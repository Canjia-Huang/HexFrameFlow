// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/the13fools/Mint3D
//   Original File Path:  /src/CubeCover/FrameField.h
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

#ifndef FRAMEFIELD_H
#define FRAMEFIELD_H

#include <vector>
#include <Eigen/Core>
#include "AssignmentGroup.h"

namespace CubeCover {

    class TetMeshConnectivity;

    /*
     * Represents a frame field on the 3-cells of a tetrahedral mesh.
     */

    class FrameField
    {
    public:
        FrameField(const TetMeshConnectivity& tetMesh);

        const TetMeshConnectivity& meshConnectivity() const { return mesh; }

        int vectorsPerFrame() const { return vpf; }

        /*
         * The number of interior edges of the tet mesh where the frame field
         * has a topological singularity, i.e. the elements of the assignment
         * group, composed together when circulating around the edge, do not
         * yield the identity assignment.
         */
        int nSingularEdges() const { return singularEdges.size(); }

        /*
         * The nSingularEdges() singular edges (in no particular order). The
         * integer returned is an index into the edges maintained in tetMesh.
         */
        int singularEdge(int idx) const { return singularEdges[idx]; }

        /*
         * Creates a frame field from per-tet vectors and per-face frame
         * assignments. frames should have size vectorsPerFrame * ntets x 3 or ntets x 3 * vectorsPerFrame,
         * with each group of vectorsPerFrame rows giving the frame of one
         * tetrahedron.
         * assignments should have rows of the form
         * t1 t2 p1 p2 p3 ...
         * where t1 and t2 are the indices of two adjacent tets, and p1 p2..
         * specify the frame assignment across their common face (see
         * .perm file format specification).
         * Returns NULL if there are problems with the input.
         */
        friend FrameField* fromFramesAndAssignments(const TetMeshConnectivity& tetMesh, const Eigen::MatrixXd &frames, const Eigen::MatrixXi &assignments, bool verbose);

        /*
         * Sets the frame field to one that is aligned with the Euclidean grid,
         * plus some Guassian noise. Only useful for debugging, really.
         */
        void setNoisyEuclidean(double magnitude);

        /*
         * Computes local frame field assignments on all internal faces, by
         * brute force. If V is provided, we project the frame field vectors on 
         * common faces, for minimizing the integrability error.
         */
        void computeLocalAssignments(const Eigen::MatrixXd* V = nullptr);

        /*
         * Attempts to reduce the number of nontrivial face assignments by
         * permuting the frame field vectors. In particular, combAssignments
         * will set all assignments to be orientation-preserving, if possible.
         */
        void combAssignments();

        const AssignmentGroup& faceAssignment(int faceid) const { return faceperms[faceid]; }
        const Eigen::MatrixXd& tetFrame(int tetid) const { return frames[tetid]; }
        void setFrame(int tetid, const Eigen::MatrixXd& newFrame) { frames[tetid] = newFrame; }

    private:
        const TetMeshConnectivity& mesh;
        int vpf;

        std::vector<Eigen::MatrixXd> frames;
        std::vector<AssignmentGroup> faceperms;

        std::vector<int> singularEdges;
        
        void setSingularEdges();
    };

    FrameField* fromFramesAndAssignments(const TetMeshConnectivity& tetMesh, const Eigen::MatrixXd &frames, const Eigen::MatrixXi &assignments, bool verbose);
};
#endif
