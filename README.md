# CubeCover

https://github.com/evouga/CubeCover

https://github.com/the13fools/Mint3D

- ISO C++17 does not allow 'register' storage class specifier [-Wregister]
    需要删除 cmake-build-release-/_deps/comiso-src/ext/gmm-4.2/include/gmm/gmm_domain_decomp.h 的 Line84 前的 ”register“
