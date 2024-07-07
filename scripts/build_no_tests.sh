cd ..

mkdir cmake-build-debug
cd cmake-build-debug
conan install .. --build=missing -s compiler.libcxx=libstdc++11
cmake -DBUILD_TESTS=OFF ..
make