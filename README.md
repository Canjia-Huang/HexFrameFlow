# CubeCover

https://github.com/evouga/CubeCover

https://github.com/the13fools/Mint3D

- ISO C++17 does not allow 'register' storage class specifier [-Wregister]
    需要删除 cmake-build-release-/_deps/comiso-src/ext/gmm-4.2/include/gmm/gmm_domain_decomp.h 的 Line84 前的 ”register“

- 在 windows 上运行 IntegerGrid 时，需要将动态链接库（位于cmake-build-release-visual-studio\_deps\comiso-src\ext\OpenBLAS-v0.2.14-Win64-int64\bin） 
libgcc_s_seh-1.dll, libgfortran-3.dll, libopenblas.dll, libquadmath-0.dll 复制到可执行文件目录下