FROM gcc:13.2 AS build
# install requirements
RUN apt update && \
apt install -y cmake=3.* libstdc++6 curl zip unzip tar bison flex gcc 
# download libs
RUN wget 'https://boostorg.jfrog.io/artifactory/main/release/1.87.0/source/boost_1_87_0.tar.bz2'
RUN git clone https://github.com/catchorg/Catch2.git
# build libs
RUN tar -xf boost_1_87_0.tar.bz2 && \
    cd boost_1_87_0 && \
    ./bootstrap.sh --with-libraries="program_options" && \
    ./b2
RUN cd Catch2 && \ 
    cmake -B build -S . -DBUILD_TESTING=OFF && \ 
    cmake --build build/ --target install --parallel 16
# install boost
RUN echo 'export CMAKE_INCLUDE_PATH=/boost_1_87_0' >> /root/.bashrc
RUN echo 'export boost_program_options_DIR=/boost_1_87_0/stage/lib' >> /root/.bashrc
RUN cat /root/.bashrc
# build project
COPY CMakeLists.txt CMakePresets.json /app/
COPY ./src /app/src
COPY ./static /app/static
RUN mkdir /app/bulld && cd /app/bulld && \
    cmake --preset=default -DBUILD_TESTS=OFF ..
RUN cd app/build/ && make -j 16

ENTRYPOINT ["/app/build/src/application", "--static_path", "/app/static", "--postgres_credentials", "postgres:1234"]    