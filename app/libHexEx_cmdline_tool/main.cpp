// =============================================================================
// This file contains code derived from the following source(s):
//   Original Repository: https://github.com/feengg/libHexEx
//   Original File Path:  /demo/cmdline_tool/main.cc
//
// Copyright (c) 2016 MaxLyon
// -----------------------------------------------------------------------------
// Modifications made by Canjia Huang on 2025-8-3:
//   - Adjusted code formatting in selected sections
//   - Use CLI11 for command line control
//   - Added code comments
//
// =============================================================================

#include <fstream>
#include <HexEx.hh>
#include <iostream>
#include "CLI11/CLI11.hpp"
#include "utils/log.h"
#include "utils/parse_filepath.h"

bool isInFileGood(const std::string& filename) {
    const std::ifstream is(filename.c_str());
    return is.good();
}

bool isOutFileGood(const std::string& filename) {
    const std::ofstream os(filename.c_str());
    return os.good();
}

int main(const int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::info);

    /* Settings */
    std::string inFilename;
    std::string outFilename;

    /* App */
    CLI::App app{"HexEx"};
    argv = app.ensure_utf8(argv);

    app.add_option(
        "inFile",
        inFilename,
        "Input tet mesh with parametrization file (.hexex) path."
        )->check(CLI::ExistingFile)->required();
    app.add_option(
        "outFile",
        outFilename,
        "Output file (.ovm) path."
        );

    CLI11_PARSE(app, argc, argv);

    /* Process */
    if (outFilename.empty())
        outFilename = get_parentpath(inFilename) + get_filename(inFilename) + ".ovm";
    else if (get_filename(outFilename).empty())
        outFilename = outFilename + ".ovm";

    if (!isInFileGood(inFilename)) {
        LOG::ERROR("Could not open input File {}", inFilename);
        return 1;
    }

    if (!isOutFileGood(outFilename)) {
        LOG::ERROR("Could not open output File {}", outFilename);
        return 1;
    }

    HexEx::extractHexMesh(inFilename, outFilename);

    return 0;
}