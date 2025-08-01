// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/the13fools/Mint3D
//   Original File Path:  /src/CubeCover/WriteFrameField.cpp
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

#include "WriteFrameField.h"
#include "TetMeshConnectivity.h"
#include "FrameField.h"
#include <fstream>
#include <iostream>

namespace CubeCover
{

    bool writeFrameField(const std::string& fraFilename, const std::string& permFilename, const FrameField& field)
    {
        {
            // write the frames
            std::ofstream ofs(fraFilename);
            if (!ofs)
            {
                return false;
            }
            ofs << "FRA 1" << std::endl;

            int nframes = field.meshConnectivity().nTets();
            int vpt = field.vectorsPerFrame();
            int type = 1;
            ofs << nframes << " " << vpt << " " << type << std::endl;

            for (int i = 0; i < nframes; i++)
            {
                for (int j = 0; j < vpt; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        if (k != 0)
                            ofs << " ";
                        ofs << field.tetFrame(i)(j, k);
                    }
                    ofs << std::endl;
                }
            }

            if (!ofs)
                return false;
        }

        // write the assignments
        {
            std::ofstream ofs(permFilename);
            if (!ofs)
                return false;
            ofs << "PERM 1" << std::endl;
            int ntets = field.meshConnectivity().nTets();
            int nonidentifyf = 0;
            for (int i = 0; i < field.meshConnectivity().nFaces(); i++)
            {
                if (!field.faceAssignment(i).isIdentity())
                    nonidentifyf++;
            }
            int nv = field.vectorsPerFrame();
            ofs << ntets << " " << nonidentifyf << " " << nv << std::endl;
            for (int i = 0; i < field.meshConnectivity().nFaces(); i++)
            {
                AssignmentGroup o = field.faceAssignment(i);
                if (!o.isIdentity())
                {
                    ofs << field.meshConnectivity().faceTet(i, 0) << " " << field.meshConnectivity().faceTet(i, 1);
                    for (int j = 0; j < nv; j++)
                    {                        
                        ofs << " ";
                        ofs << o.targetSign(j) * (1 + o.targetVector(j));
                    }
                    ofs << std::endl;
                }                
            }
            if (!ofs)
                return false;
        }

        return true;
    }    
};