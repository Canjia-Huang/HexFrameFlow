// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/the13fools/Mint3D
//   Original File Path:  /src/CubeCover/ReadMoments.cpp
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

#include "ReadMoments.h"
#include <iostream>
#include <fstream>

namespace CubeCover
{
    bool readMoments(const std::string& filename, Eigen::MatrixXd& ME, bool verbose)
    {
        // TetMeshConnectivity tetMesh(T);

        std::ifstream ifs(filename);
        if (!ifs)
        {
            if (verbose)
                std::cerr << "Cannot open file: " << filename << std::endl;
            return false;
        }
        std::string header;
        ifs >> header;
        if (header != "MOM")
        {
            if(verbose)
                std::cerr << "Expected: MOM, read: " << header << std::endl;
            return false;
        }
        int version;
        ifs >> version;
        if (version != 1)
        {
            if(verbose)
                std::cerr << "Only version 1 of .mom files supported (read: " << version << ")" << std::endl;
            return false;
        }

        int nelements, momentsPerTet;
        ifs >> nelements >> momentsPerTet;
        // if (nframes != tetMesh.nTets())
        // {
        //     if(verbose)
        //         std::cerr << "Mismatch between .fra file and tet mesh: " << nframes << " frames != " << tetMesh.nTets() << " tetrahedra" << std::endl;
        //     return false;
        // }
// nboundelements
        if (momentsPerTet <= 0)
        {
            if (verbose)
                std::cerr << "Must have at least one moment per element" << std::endl;
            return false;
        }

        // int ntets = tetMesh.nTets();

        ME.resize(nelements, momentsPerTet);

        bool ok = true;
        for (int i = 0; i < nelements; i++)
        {
            for (int k = 0; k < momentsPerTet; k++)
            {
                ifs >> ME(i, k);
                if (!ifs)
                {
                    if (verbose)
                        std::cerr << "Error reading moment data" << std::endl;
                    return false;
                }
            }
        }


        if (!ifs)
        {
            return false;
        }
        return true;
    }
};