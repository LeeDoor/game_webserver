FROM gcc:13.2 AS build
# install requirements
RUN apt update && \
apt install -y cmake=3.* libstdc++6 curl zip unzip tar bison flex gcc 
# download libs
RUN apt-get install -y libpqxx-dev
RUN wget 'https://archives.boost.io/release/1.87.0/source/boost_1_87_0.tar.bz2' && tar -xf boost_1_87_0.tar.bz2
RUN git clone https://github.com/catchorg/Catch2.git --depth 1 
RUN git clone https://github.com/jtv/libpqxx --depth 1 
RUN git clone https://github.com/redis/hiredis.git --depth 1 
RUN git clone https://github.com/sewenew/redis-plus-plus.git --depth 1 
RUN git clone https://github.com/gabime/spdlog.git --depth 1 
RUN git clone https://github.com/nlohmann/json --depth 1
# build libs
    #boost
RUN cd /boost_1_87_0 && \ 
    ./bootstrap.sh --with-libraries="program_options" --prefix=/usr/local && \
    ./b2 install
    
RUN cd /Catch2 && \
    cmake -B build -S . -DBUILD_TESTING=OFF && \ 
    cmake --build build/ --target install --parallel 16
    
RUN cd /libpqxx && cmake . -DSKIP_BUILD_TEST=on && cmake --build . --parallel 16 && cmake --install .
    
RUN cd /hiredis && \
    make -j 16 && \
    make install
    
RUN cd /redis-plus-plus && mkdir build && cd build && \
    cmake -DCMAKE_PREFIX_PATH=/usr/local .. && \
    make -j 16 && \
    make install
    
RUN cd /spdlog && mkdir build && cd build &&\
    cmake .. && cmake --build . --parallel 16 && \
    cmake --install . 
    
RUN mkdir /json/build && cd json/build && cmake .. && cmake --install .  
    
COPY CMakeLists.txt CMakePresets.json /app/
COPY ./src /app/src
COPY ./static /app/static
RUN mkdir /app/bulld && cd /app/bulld && \
    cmake --preset=default -DBUILD_TESTS=OFF ..
RUN cd app/build/ && make -j 16 && ldconfig
ENTRYPOINT ["/app/build/src/application", "--static_path", "/app/static", "--postgres_credentials", "postgres:1234"]    