
FROM gcc:13.2 AS build
RUN apt update && \
apt install -y cmake=3.* libstdc++6 curl zip unzip tar bison flex

RUN cd / && git clone https://github.com/microsoft/vcpkg.git --depth 1 && \
    cd vcpkg && ./bootstrap-vcpkg.sh
COPY ./src /app/src
COPY CMakeLists.txt CMakePresets.json vcpkg.json vcpkg-configuration.json /app/

RUN export VCPKG_ROOT=/vcpkg && \
    export PATH=$VCPKG_ROOT:$PATH && \
    cd /app/ && \
    vcpkg install --classic boost-algorithm boost-asio boost-beast boost-program-options

RUN export VCPKG_ROOT=/vcpkg && \
    export PATH=$VCPKG_ROOT:$PATH && \
    cd /app/ && \
    vcpkg install

RUN export VCPKG_ROOT=/vcpkg && \
    export PATH=$VCPKG_ROOT:$PATH && \
    mkdir /app/bulld && cd /app/bulld && \
    cmake --preset=default -DBUILD_TESTS=OFF ..

RUN make -j 16

FROM ubuntu:latest AS run

RUN apt-get update && apt-get install -y \
    software-properties-common \
    && add-apt-repository ppa:ubuntu-toolchain-r/test \
    && apt-get update \
    && apt-get install -y libstdc++6

RUN groupadd -r www && useradd -r -g www www
USER www
COPY --from=build /app/bulld/bin/application /app/
COPY ./static /app/static

ENTRYPOINT ["/app/application", "--static_path", "/app/static", "--postgres_credentials", "postgres:1234"]