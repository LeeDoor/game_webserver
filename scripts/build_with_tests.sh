cd ..

mkdir build
cd build
cmake -DBUILD_TESTS=ON ..
cmake --build . -j 16