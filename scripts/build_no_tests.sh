cd ..

mkdir build
cd build
cmake -DBUILD_TESTS=OFF ..
cmake --build . -j 16