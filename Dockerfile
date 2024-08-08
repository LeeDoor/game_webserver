
FROM gcc:13.2 AS build
RUN apt update && \
apt install -y cmake python3 python3-pip libstdc++6

RUN pip3 install conan==1.* --break-system-packages


COPY conanfile.txt /app/

RUN mkdir /app/cmake-build-debug && cd /app/cmake-build-debug && \
conan install .. --build=missing -s compiler.libcxx=libstdc++11 -s compiler.version=13

COPY ./src /app/src
COPY CMakeLists.txt /app/

RUN cd /app/cmake-build-debug && \
cmake -DBUILD_TESTS=OFF .. && \
make -j 16

FROM ubuntu:latest AS run

RUN apt-get update && apt-get install -y \
    software-properties-common \
    && add-apt-repository ppa:ubuntu-toolchain-r/test \
    && apt-get update \
    && apt-get install -y libstdc++6

RUN groupadd -r www && useradd -r -g www www
USER www
COPY --from=build /app/cmake-build-debug/bin/application /app/
COPY ./static /app/static

ENTRYPOINT ["/app/application", "--static_path", "/app/static", "--postgres_credentials", "postgres:1234"]