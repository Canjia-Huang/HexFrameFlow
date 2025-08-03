// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/the13fools/Mint3D
//   Original File Path:  /src/CubeCover/CurlCorrection.h
//   Original License:    Mozilla Public License 2.0 (MPL-2.0)
//
// Copyright (c) 2025 Josh Vekhter
// -----------------------------------------------------------------------------
// Modifications made by Canjia Huang on 2025-8-1:
//   - Adjusted code formatting in selected sections
//	 - Use CubeCover::FrameField
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.
// =============================================================================

#ifndef CURLCORRECTION_H
#define CURLCORRECTION_H

#include <Eigen/Core>
#include <Eigen/Sparse>
#include "FrameField.h"

// class FrameField;

namespace CubeCover
{

	void buildCurlMatrix(int vpf, const Eigen::MatrixXd& V, const FrameField &field, Eigen::SparseMatrix<double> &C);
    
    void curlCorrect(const Eigen::MatrixXd& V, FrameField& field, double maxCorrection, bool verbose);
};

#endif