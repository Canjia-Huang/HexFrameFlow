// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/the13fools/Mint3D
//   Original File Path:  /src/CubeCover/ReadFrameField.cpp
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

#include "ReadFrameField.h"
#include <fstream>
#include <iostream>
#include "FrameField.h"
#include "TetMeshConnectivity.h"

namespace CubeCover {

bool readFrameField(
    const std::string& fraFilename,
    const std::string& permFilename,
    const Eigen::MatrixXi& T,
    Eigen::MatrixXd& frames,
    Eigen::MatrixXi& assignments,
    const bool verbose
    ) {
    // check that fraFilename ends in .fra
    // if ends .fra call v1
    // if ends in .bfra call v2
    // else return false

    const std::string fraExt = fraFilename.substr(fraFilename.find_last_of('.') + 1);
    if (fraExt == "fra") {
        const bool is_success = readFrameField_v1(fraFilename, permFilename, T, frames, assignments, verbose);

        // convert to gpgpt frame storage
        const int nvpe = frames.rows() / T.rows();
        Eigen::MatrixXd cur_frames(T.rows(), 3 * nvpe);
        for (int i = 0; i < T.rows(); i++) {
            for (int j = 0; j < nvpe; j++) {
                cur_frames.row(i).segment<3>(3 * j) = frames.row(nvpe * i + j);
            }
        }
        frames = std::move(cur_frames);
        return is_success;
    }
    if (fraExt == "bfra") {
        const bool is_success = readFrameField_v2(fraFilename, permFilename, T, frames, assignments, verbose);

        // convert to gpgpt frame storage
        const int nvpe = frames.rows() / T.rows();
        Eigen::MatrixXd cur_frames(T.rows(), 3 * nvpe);
        for (int i = 0; i < T.rows(); i++) {
            for (int j = 0; j < nvpe; j++) {
                cur_frames.row(i).segment<3>(3 * j) = frames.row(nvpe * i + j);
            }
        }
        frames = std::move(cur_frames);
        return is_success;
    }
    if (fraExt == "ff3") {
        // check that file name ends in _ascii

        // std::cout << fraFilename.substr(fraFilename.find_last_of("_") + 1 ) << std::endl;

        if (fraFilename.substr(fraFilename.find_last_of('_') + 1) == "ascii.ff3") {
            return deserializeFF3FramesFromMetricDrivenFrames3D(frames, fraFilename);
        } else {
            std::cerr << "please pass in ff3 in ASCII not binary! filename: " << fraFilename << std::endl;
            return false;
        }

    }
    if (verbose) std::cerr << "Unknown file extension: " << fraExt << std::endl;
    return false;
}

bool readFrameField_v1(
    const std::string& fraFilename,
    const std::string& permFilename,
    const Eigen::MatrixXi& T,
    Eigen::MatrixXd& frames,
    Eigen::MatrixXi& assignments,
    bool verbose
    ) {
    TetMeshConnectivity tetMesh(T);

    std::ifstream ifs(fraFilename);
    if (!ifs) {
        if (verbose) std::cerr << "Cannot open file: " << fraFilename << std::endl;
        return false;
    }
    std::string header;
    ifs >> header;
    if (header != "FRA") {
        if (verbose) std::cerr << "Expected: FRA, read: " << header << std::endl;
        return false;
    }
    int version;
    ifs >> version;
    if (version != 1) {
        if (verbose) std::cerr << "Only version 1 of .fra files supported (read: " << version << ")" << std::endl;
        return false;
    }

    int nframes, vpt, type;
    ifs >> nframes >> vpt >> type;
    if (nframes != tetMesh.nTets()) {
        if (verbose)
            std::cerr << "Mismatch between .fra file and tet mesh: " << nframes << " frames != " << tetMesh.nTets()
                      << " tetrahedra" << std::endl;
        return false;
    }

    if (vpt <= 0) {
        if (verbose) std::cerr << "Must have at least one vector per tet" << std::endl;
        return false;
    }

    int ntets = tetMesh.nTets();

    frames.resize(ntets * vpt, 3);

    bool ok = true;
    for (int i = 0; i < ntets * vpt; i++) {
        for (int k = 0; k < 3; k++) {
            ifs >> frames(i, k);
            if (!ifs) {
                if (verbose) std::cerr << "Error reading frame data" << std::endl;
                return false;
            }
        }
    }

    assignments.resize(0, 2 + vpt);

    if (!permFilename.empty()) {
        std::ifstream permfs(permFilename);
        if (!permfs) {
            if (verbose) std::cerr << "Cannot open file: " << permFilename << std::endl;
            return false;
        }
        permfs >> header >> version;
        if (header != "PERM") {
            if (verbose) std::cerr << "Expected: PERM, read: " << header << std::endl;
            return false;
        }
        if (version != 1) {
            if (verbose) std::cerr << "Only version 1 of .perm files supported (read: " << version << ")" << std::endl;
            return false;
        }

        int ntet, nf, nv;
        permfs >> ntet >> nf >> nv;
        if (ntet != tetMesh.nTets()) {
            if (verbose)
                std::cerr << "Mismatch between .perm file and tet mesh: " << nframes << " != " << tetMesh.nTets()
                          << " tetrahedra" << std::endl;
            return false;
        }
        if (nf < 0) {
            if (verbose) std::cerr << "Bad number of assignment faces specified" << std::endl;
            return false;
        }
        if (nv != vpt) {
            if (verbose)
                std::cerr << "Mismatch between .perm file and .fra file: " << nv << " assignments per face != " << vpt
                          << " vectors per frame" << std::endl;
            return false;
        }

        assignments.resize(nf, 2 + nv);

        for (int i = 0; i < nf; i++) {
            for (int j = 0; j < 2 + nv; j++) {
                permfs >> assignments(i, j);
                if (!permfs) {
                    if (verbose) std::cerr << "Error reading assignment data" << std::endl;
                    return false;
                }
            }
        }
    }
    return true;
}

bool readFrameField_v2(
    const std::string& fraFilename,
    const std::string& permFilename,
    const Eigen::MatrixXi& T,
    Eigen::MatrixXd& frames,
    Eigen::MatrixXi& assignments,
    bool verbose
    ) {
    TetMeshConnectivity tetMesh(T);

    try {
        std::ifstream inFile(fraFilename, std::ios::binary);
        if (!inFile.is_open()) {
            std::cerr << "Error: Unable to open file for reading: " << fraFilename << std::endl;
            return false;
        }

        char* filename = new char[5];
        inFile.read(reinterpret_cast<char*>(&filename), sizeof("FRA 2"));

        // Read matrix rows and cols
        int rows, cols, vpe;
        inFile.read(reinterpret_cast<char*>(&rows), sizeof(int));
        inFile.read(reinterpret_cast<char*>(&cols), sizeof(int));
        inFile.read(reinterpret_cast<char*>(&vpe), sizeof(int));

        // Read matrix data
        frames.resize(rows, cols);
        inFile.read(reinterpret_cast<char*>(frames.data()), rows * cols * sizeof(double));
        inFile.close();

        vpe = vpe != cols / 3 ? cols / 3 : vpe;

        Eigen::MatrixXd cur_frames(rows * vpe, 3);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < vpe; j++) {
                cur_frames.row(vpe * i + j) = frames.row(i).segment<3>(3 * j);
            }
        }
        frames = std::move(cur_frames);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error: Unable to deserialize matrix: " << e.what() << std::endl;
        return false;
    }
}

bool deserializeFF3FramesFromMetricDrivenFrames3D(
    Eigen::MatrixXd& mat,
    const std::string& filepath
    ) {
    try {
        std::ifstream inFile(filepath);
        if (!inFile.is_open()) {
            std::cerr << "Error: Unable to open file for reading: " << filepath << std::endl;
            return false;
        }

        std::string line;
        int rows, cols;

        // Read the first line to get the dimensions of the matrix
        if (std::getline(inFile, line)) {
            std::istringstream iss(line);
            if (!(iss >> cols >> rows)) {
                std::cerr << "Error reading matrix dimensions" << std::endl;
                return false;
            }
        }

        // Initialize an Eigen matrix with the given dimensions
        mat = Eigen::MatrixXd::Zero(rows, cols);
        std::cout << "rows: " << rows << ", cols: " << cols << std::endl;
        int currentRow = 0;
        while (std::getline(inFile, line)) {
            std::istringstream iss(line);
            // std::cout << line << std::endl;
            for (int col = 0; col < cols; ++col) {
                double value;
                if (!(iss >> value)) {
                    std::cerr << "Error reading matrix data at row " << currentRow << ", column " << col << std::endl;
                    return false;
                }
                mat(currentRow, col) = value;
            }
            currentRow++;
        }

        inFile.close();

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error: Unable to deserialize matrix: " << e.what() << std::endl;
        return false;
    }
}

};   // namespace core