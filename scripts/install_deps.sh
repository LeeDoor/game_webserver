cd ../
mkdir trash
cd trash
wget 'https://boostorg.jfrog.io/artifactory/main/release/1.87.0/source/boost_1_87_0.tar.bz2' && tar -xf boost_1_87_0.tar.bz2 
git clone https://github.com/catchorg/Catch2.git --depth 1
git clone https://github.com/jtv/libpqxx --depth 1
git clone https://github.com/redis/hiredis.git --depth 1
git clone https://github.com/sewenew/redis-plus-plus.git --depth 1
git clone https://github.com/gabime/spdlog.git --depth 1
git clone https://github.com/nlohmann/json --depth 1

cd boost_1_87_0
./bootstrap.sh --with-libraries="program_options" --prefix=/usr/local
./b2 install
cd ..

cd Catch2
cmake -B build -S . -DBUILD_TESTING=OFF
cmake --build build/ --target install --parallel 16
cd ..

cd libpqxx && cmake . -DSKIP_BUILD_TEST=on && cmake --build . --parallel 16 && cmake --install .
cd ..

cd hiredis
make -j 16
make install
cd ..

cd redis-plus-plus && mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH=/usr/local ..
make -j 16
make install
cd ..

cd spdlog && mkdir build && cd build
cmake .. && cmake --build . --parallel 16
cmake --install .
cd ..

mkdir json/build && cd json/build && cmake .. && cmake --install .