// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/the13fools/Mint3D
//   Original File Path:  /src/CubeCover/CutToSimplyConnected.h
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

#ifndef CUTTOSPHERE_H
#define CUTTOSPHERE_H

#include <vector>

namespace CubeCover
{

    class TetMeshConnectivity;

    /*
     * Computes the set of faces (seams) along which a tet mesh needs to be cut to
     * make the mesh simply-connected. During this process, the protectedEdges
     * will be treated as if they've been removed from the mesh volume (forming
     * one-dimensional infinitesimal tunnels through the mesh). The ouput faces
     * will form a 2-complex whose boundary edges either lie on the boundary of the
     * original tet mesh, or are in protectedEdges.
     *
     * Note that cutting along the cutFaces does not always yield a topological
     * sphere (since this function does not try to "bore out" interior voids).
     *
     * Nothing is guaranteed regarding optimality of the cutFaces.
     *
     * The input tet mesh must be manifold and face-connected.
     */

    enum struct CST_RESULT
    {
        CST_OK = 0,
        CST_NOTMANIFOLD,        // Input mesh is not manfiold. You might get more
                                // details about the problem with verbose=true.
        CST_NOTCONNECTED,       // Input mesh is not face-connected.
        CST_BADEDGEIDX          // protectedEdges contain an invalid edge index.
    };

    CST_RESULT cutToSimplyConnected(
        const TetMeshConnectivity& mesh,
        const std::vector<int>& protectedEdges,
        std::vector<int>& cutFaces,
        bool verbose = false);

};

#endif