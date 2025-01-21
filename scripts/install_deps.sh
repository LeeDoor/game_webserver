cd ../
mkdir trash
cd trash
apt install -y libstdc++6 curl zip unzip tar gcc wget cmake libpqxx-dev build-essential bison
if [ ! -f boost_1_87_0.tar.bz2 ]; then
	wget 'https://archives.boost.io/release/1.87.0/source/boost_1_87_0.tar.bz2' 
fi
if [ ! -d boost_1_87_0 ]; then
	tar -xf boost_1_87_0.tar.bz2 
fi
git clone https://github.com/catchorg/Catch2.git --depth 1
git clone https://github.com/jtv/libpqxx --depth 1
git clone https://github.com/redis/hiredis.git --depth 1
git clone https://github.com/sewenew/redis-plus-plus.git --depth 1
git clone https://github.com/gabime/spdlog.git --depth 1
git clone https://github.com/nlohmann/json --depth 1

cd boost_1_87_0
./bootstrap.sh --with-libraries="program_options" --prefix=/usr/local
./b2 install -j 16
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

cd redis-plus-plus 
cmake -DCMAKE_PREFIX_PATH=/usr/local .
make -j 16
make install
cd ..

cd spdlog 
cmake . && cmake --build . --parallel 16
cmake --install .
cd ..

cd json && cmake . && cmake --install .
