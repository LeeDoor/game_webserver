cd ..

mkdir cmake-build-debug
cd cmake-build-debug
rm -r bin
conan install .. --build=missing -s compiler.libcxx=libstdc++11
cmake -DBUILD_TESTS=ON ..
make