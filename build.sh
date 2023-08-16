mkdir build
cd build
conan install .. --build=missing -s compiler.libcxx=libstdc++11
cmake ..
make
