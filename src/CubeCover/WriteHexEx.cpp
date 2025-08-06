// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/the13fools/Mint3D
//   Original File Path:  /src/CubeCover/WriteHexEx.cpp
//   Original License:    Mozilla Public License 2.0 (MPL-2.0)
//
// Copyright (c) 2025 Josh Vekhter
// -----------------------------------------------------------------------------
// Modifications made by Canjia Huang on 2025-8-1:
//   - Adjusted code formatting in selected sections
//   - Output more information
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.
// =============================================================================

#include "WriteHexEx.h"
#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include "utils/log.h"

namespace CubeCover
{
    bool writeHexEx(
        const std::string& filename,
        const Eigen::MatrixXd& V,
        const Eigen::MatrixXi& T,
        const Eigen::MatrixXd& vals
        ){
        LOG::TRACE(__FUNCTION__);

        if (4 * T.rows() != vals.rows()) {
            std::cerr << "dimension mismatch between T and vals" << std::endl;
            return false;
        }

        std::ofstream out(filename);
        if (!out) {
            std::cerr << "cannot open file " << filename << " for writing" << std::endl;
            return false;
        }

        int nverts = V.rows();
        out << nverts << std::endl;
        for (int i = 0; i < nverts; i++) {
            for (int j = 0; j < 3; j++) {
                if (j != 0)
                    out << " ";
                out << V(i, j);
            }
            out << std::endl;
        }

        int ntets = T.rows();
        int nvals = vals.cols();
        if (nvals != 3)
            std::cerr << "warning: number of parameter values per vertex is not equal to 3. HexEx file format only "
                         "officially supports 3D parameterization" << std::endl;
        out << ntets << std::endl;

        for (int i = 0; i < ntets; i++) {
            for (int j = 0; j < 4; j++) {
                if (j != 0)
                    out << " ";
                out << T(i, j);
            }
            for (int j = 0; j < 4; j++) {
                for (int k = 0; k < nvals; k++)
                    out << " " << vals(4 * i + j, k);
            }
            out << std::endl;
        }
        if (!out) {
            std::cerr << "error writing to file " << filename << std::endl;
            return false;
        }
        return true;
    }
};