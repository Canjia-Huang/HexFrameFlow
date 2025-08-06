// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/the13fools/Mint3D
//   Original File Path:  /src/CubeCover/CubeCover.cpp
//   Original License:    Mozilla Public License 2.0 (MPL-2.0)
//
// Copyright (c) 2025 Josh Vekhter
// -----------------------------------------------------------------------------
// Modifications made by Canjia Huang on 2025-7-31:
//   - Adjusted code formatting in selected sections
//   - Output more information
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.
// =============================================================================

#include "CubeCover.h"
#include "TetMeshConnectivity.h"
#include "FrameField.h"
#include "Integration.h"
#include "CurlCorrection.h"
#include <iostream>
#include "utils/log.h"

namespace CubeCover
{
    bool cubeCover(
        const Eigen::MatrixXd& V,
        const Eigen::MatrixXi& T,
        const Eigen::MatrixXd& frames,
        const Eigen::MatrixXi& assignments,
        Eigen::MatrixXd& parameterization,
        const CubeCoverOptions &opt
        ){
        LOG::TRACE(__FUNCTION__);

        const TetMeshConnectivity mesh(T);
        if (!mesh.isManifold(opt.verbose))
            return false;
        if (!mesh.isFaceConnected()) {
            if (opt.verbose) {
                LOG::ERROR("input mesh is not face-connected");
                return false;
            }
        }

        
        FrameField* field = nullptr;
        LOG::TRACE("Creating frame field...");
        if (opt.assignmentHandling == CubeCoverOptions::AssignmentHandling::AH_USEPROVIDED) {
            field = fromFramesAndAssignments(mesh, frames, assignments, opt.verbose);
        }
        else {
            const Eigen::MatrixXi identity(0, 2 + frames.rows() / mesh.nTets());
            field = fromFramesAndAssignments(mesh, frames, identity, opt.verbose);
            field->computeLocalAssignments();
            field->combAssignments();
        }

        if (!field)
            return false;

        if (opt.curlCorrection) {
            LOG::TRACE("Correcting curl...");
            curlCorrect(V, *field, opt.curlCorrection, opt.verbose);
        }

        LOG::TRACE("Integrating...");
        if (!integrate(V, *field, parameterization, opt)) {
            delete field;
            return false;
        }

        delete field;
        return true;
    }

    bool cubeCover(
        const Eigen::MatrixXd& V,
        const Eigen::MatrixXi& T,
        const Eigen::MatrixXd& frames,
        const Eigen::MatrixXi& assignments,
        Eigen::MatrixXd& parameterization
        ){
        const CubeCoverOptions opt;
        return cubeCover(V, T, frames, assignments, parameterization, opt);
    }

};