// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/evouga/CubeCover
//   Original File Path:  /tools/singularityviewer_fancy/src/ReadFancyData.h
//
// Copyright (c) 2025 evouga
// -----------------------------------------------------------------------------
// Modifications made by Canjia Huang on 2025-8-1:
//   - Adjusted code formatting in selected sections
//
// =============================================================================

#ifndef READFANCYDATA_H
#define READFANCYDATA_H

#include <Eigen/Core>

namespace CubeCover
{
    /*
    * This parses the stuff dumped by dumpExtraVizStuff in the mint repo
    */

    bool readEdgeCurl(const std::string& edges_mintFilename,
                      const std::string& edgeCurlFilename,
                      Eigen::MatrixXi& edges_mint,
                      Eigen::MatrixXd& edgeCurl);

};

#endif