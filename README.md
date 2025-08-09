# HexFrameFlow

This repo provides a integrated workflow for automatically generating all-hexahedral (hex) meshes from tetrahedral inputs using classic frame-field related methodologies.
This workflow integrates three key works:

- [**Practical 3D frame field generation**](https://dl.acm.org/doi/10.1145/2980179.2982408) - it is designed to generate a frame field from an input tetrahedral mesh, with the code adapted from the supplementary materials of the cited paper [[supp]](https://dl.acm.org/doi/suppl/10.1145/2980179.2982408/suppl_file/233-0220.zip).
- [**CubeCover– Parameterization of 3D Volumes**](https://onlinelibrary.wiley.com/doi/abs/10.1111/j.1467-8659.2011.02014.x) - it generates an integer-grid parametrization from an input tetrahedral mesh and its associated frame field， with the code adapted from repos [[evouga/CubeCover]](https://github.com/evouga/CubeCover) and [[the13fools/Mint3D]](https://github.com/the13fools/Mint3D).
- [**HexEx: robust hexahedral mesh extraction**](https://dl.acm.org/doi/abs/10.1145/2897824.2925976) - it robustly extracts an all-hex mesh from the input integer grid parametrization, with the code adapted from the official open-source implementation [[feengg/libHexEx]](https://github.com/feengg/libHexEx) and [[dbrandes-welfenlab/libHexExFull]](https://github.com/dbrandes-welfenlab/libHexExFull).

If you use this code in your research, please cite the following original work:

<details>
<summary>"Practical 3D frame field generation" BibTeX </summary>

```
@article{10.1145/2980179.2982408,
  author = {Ray, Nicolas and Sokolov, Dmitry and L\'{e}vy, Bruno},
  title = {Practical 3D frame field generation},
  year = {2016},
  issue_date = {November 2016},
  publisher = {Association for Computing Machinery},
  address = {New York, NY, USA},
  volume = {35},
  number = {6},
  issn = {0730-0301},
  url = {https://doi.org/10.1145/2980179.2982408},
  doi = {10.1145/2980179.2982408},
  journal = {ACM Trans. Graph.},
  month = dec,
  articleno = {233},
  numpages = {9},
  keywords = {smooth frame fields, remeshing}
}
```
</details>

<details>
<summary>"CubeCover– Parameterization of 3D Volumes" BibTeX </summary>

```
@article{https://doi.org/10.1111/j.1467-8659.2011.02014.x,
  author = {Nieser, M. and Reitebuch, U. and Polthier, K.},
  title = {CubeCover– Parameterization of 3D Volumes},
  journal = {Computer Graphics Forum},
  volume = {30},
  number = {5},
  pages = {1397-1406},
  keywords = {I.3.5 Computer Graphics: Curve, surface, solid, and object representations—Computational Geometry and Object Modeling},
  doi = {https://doi.org/10.1111/j.1467-8659.2011.02014.x},
  url = {https://onlinelibrary.wiley.com/doi/abs/10.1111/j.1467-8659.2011.02014.x},
  eprint = {https://onlinelibrary.wiley.com/doi/pdf/10.1111/j.1467-8659.2011.02014.x},
  year = {2011}
}
```
</details>

<details>
<summary>"HexEx: robust hexahedral mesh extraction" BibTeX </summary>

```
@article{10.1145/2897824.2925976,
  author = {Lyon, Max and Bommes, David and Kobbelt, Leif},
  title = {HexEx: robust hexahedral mesh extraction},
  year = {2016},
  issue_date = {July 2016},
  publisher = {Association for Computing Machinery},
  address = {New York, NY, USA},
  volume = {35},
  number = {4},
  issn = {0730-0301},
  url = {https://doi.org/10.1145/2897824.2925976},
  doi = {10.1145/2897824.2925976},
  journal = {ACM Trans. Graph.},
  month = jul,
  articleno = {123},
  numpages = {11},
  keywords = {hex meshing, mesh extraction, parametrization}
}
```
</details>

# :bulb: What did I do?
- Improved code organization
- Updated CMake build configurations
- Added command line applications

# :link: Requirements / dependencies
- [CLI11](https://github.com/CLIUtils/CLI11) (already included)
- [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page) (version >= 3.4)
- [Gurobi](https://www.gurobi.com) (optional)
- [libigl (with comiso)](https://libigl.github.io) (already included)
- [OpenVolumeMesh](https://www.graphics.rwth-aachen.de/software/openvolumemesh/) (already included)
- [Polyscope](https://polyscope.run)
- [spdlog](https://github.com/gabime/spdlog) (already included)

# :checkered_flag: Getting started

```
https://github.com/Canjia-Huang/HexFrameFlow.git
```

# :hammer: Build

```
mkdir build && cd build
cmake ..
make -j
```

> [!NOTE]
> By default, use **comiso** to solve the MIP problem (rather than **Gurobi**). If you wish to use **Gurobi** instead, you can add the CMake option `-DUSE_GUROBI`.

# :computer: Usage

After successful compilation, the following executable files will be present in the dir: `build/bin`.

## framefield



https://github.com/evouga/CubeCover

https://github.com/the13fools/Mint3D

https://github.com/feengg/libHexEx/tree/master

https://dl.acm.org/doi/10.1145/2980179.2982408

```
/Users/canjia/HexFrameFlow/cmake-build-release-/_deps/comiso-src/ext/gmm-4.2/include/gmm/gmm_domain_decomp.h:84:2: error: ISO C++17 does not allow 'register' storage class specifier [-Wregister]
   84 |         register double a = (pts[i][k] - pmin[k]) / msize;
      |         ^~~~~~~~
```

- ISO C++17 does not allow 'register' storage class specifier [-Wregister]
  需要删除 cmake-build-release-/_deps/comiso-src/ext/gmm-4.2/include/gmm/gmm_domain_decomp.h 的 Line84 前的 ”register“
- 在 windows 上运行 IntegerGrid 时，需要将动态链接库（位于cmake-build-release-visual-studio\_deps\comiso-src\ext\OpenBLAS-v0.2.14-Win64-int64\bin）
  libgcc_s_seh-1.dll, libgfortran-3.dll, libopenblas.dll, libquadmath-0.dll 复制到可执行文件目录下
