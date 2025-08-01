// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/the13fools/Mint3D
//   Original File Path:  /src/CubeCover/ReadVTK.h
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

#ifndef READVTKVERTSTETS_H
#define READVTKVERTSTETS_H

#include <Eigen/Core>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace CubeCover {

template <typename DerivedV, typename DerivedT>
bool readVTK(const std::string& vtk_file_name, Eigen::PlainObjectBase<DerivedV>& V,
             Eigen::PlainObjectBase<DerivedT>& T) {
    V.resize(0, 0);
    T.resize(0, 0);

    std::ifstream ifs(vtk_file_name);
    if (!ifs) {
        std::cerr << "Error: couldn't read .vtk file: " << vtk_file_name << std::endl;
        return false;
    }

    std::string line;
    bool in_points_section = false;
    bool in_cells_section = false;
    int num_points = 0;
    int num_cells = 0;

    std::vector<Eigen::Vector3d> vertices;
    std::vector<Eigen::Vector4i> tetrahedra;

    while (std::getline(ifs, line)) {
        std::istringstream ss(line);
        std::string keyword;
        ss >> keyword;

        if (keyword == "POINTS") {
            ss >> num_points;
            std::string data_type;
            ss >> data_type;   // e.g., float or double

            if (num_points <= 0) {
                std::cerr << "Error: Invalid number of points." << std::endl;
                return false;
            }

            vertices.reserve(num_points);
            in_points_section = true;
            in_cells_section = false;
        } else if (keyword == "CELLS") {
            ss >> num_cells;
            int total_cell_entries;
            ss >> total_cell_entries;

            if (num_cells <= 0 || total_cell_entries <= 0) {
                std::cerr << "Error: Invalid cell data." << std::endl;
                return false;
            }

            tetrahedra.reserve(num_cells);
            in_cells_section = true;
            in_points_section = false;
        } else if (keyword == "CELL_TYPES") {
            // Skip the CELL_TYPES section (if any)
            in_cells_section = false;
            in_points_section = false;
        } else if (in_points_section) {
            double x, y, z;
            ss >> x >> y >> z;

            if (ss.fail()) {
                std::cerr << "Error: Malformed POINTS data." << std::endl;
                return false;
            }

            vertices.emplace_back(x, y, z);
            if (vertices.size() == static_cast<size_t>(num_points)) {
                in_points_section = false;
            }
        } else if (in_cells_section) {
            int num_vertices;
            ss >> num_vertices;

            if (num_vertices == 4) {   // Only process tetrahedra
                int v1, v2, v3, v4;
                ss >> v1 >> v2 >> v3 >> v4;

                if (ss.fail()) {
                    std::cerr << "Error: Malformed CELLS data." << std::endl;
                    return false;
                }

                tetrahedra.emplace_back(v1, v2, v3, v4);
            } else {
                // Skip non-tetrahedral cells
                std::string skip;
                std::getline(ss, skip);
            }

            if (tetrahedra.size() == static_cast<size_t>(num_cells)) {
                in_cells_section = false;
            }
        }
    }

    if (vertices.size() != static_cast<size_t>(num_points)) {
        std::cerr << "Error: Number of points read does not match header." << std::endl;
        return false;
    }

    // Copy data to Eigen matrices
    V.resize(vertices.size(), 3);
    for (size_t i = 0; i < vertices.size(); ++i) {
        V.row(i) = vertices[i];
    }

    T.resize(tetrahedra.size(), 4);
    for (size_t i = 0; i < tetrahedra.size(); ++i) {
        T.row(i) = tetrahedra[i];
    }

    return true;
}
}   // namespace core

#endif
