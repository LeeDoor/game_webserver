# Downloading
There are many dependencies required to download. If something goes wrong, you can see the Dockerfile in the root directory to see the complete sequence of actions to build everything.
The list of dependencies: 
* boost
	* algorithm
	* program_options (build_required)
	* asio
	* beast
* redis++
* lipqxx
* spdlog
* catch2

# Download
These lines of bash code will copy the source codes of required library and prepares them to build. 
> You need to have `git`, `wget`, `tar`, `unzip` installed:
>  `apt-update && apt-get install git wget tar unzip`
```bash
wget 'https://boostorg.jfrog.io/artifactory/main/release/1.87.0/source/boost_1_87_0.tar.bz2' && tar -xf boost_1_87_0.tar.bz2 
git clone https://github.com/catchorg/Catch2.git --depth 1
git clone https://github.com/jtv/libpqxx --depth 1
git clone https://github.com/redis/hiredis.git --depth 1
git clone https://github.com/sewenew/redis-plus-plus.git --depth 1
git clone https://github.com/gabime/spdlog.git --depth 1
git clone https://github.com/nlohmann/json --depth 1
```
> `--depth 1` flag means that you wouldn't download entire repo, but just latest commit.

# Build & install
After everything is on your machine, lets build and install that mess. Almost every library can be built with cmake common commands. Boost has it's own build script.
> From Boost we only need to build `program_options`, because other libraries are header-only and doesn't require building them.
```bash
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
```
> `-j 16` or `--parallel 16` sets the amount of working threads to 16. It means at least of 16 CPU cores will be working at once.
  