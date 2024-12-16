FROM gcc:13.2 AS build
RUN apt update && \
apt install -y cmake=3.* libstdc++6 curl zip unzip tar bison flex

RUN cd / && git clone https://github.com/microsoft/vcpkg.git --depth 1 && \
cd vcpkg && ./bootstrap-vcpkg.sh
COPY CMakeLists.txt CMakePresets.json vcpkg.json vcpkg-configuration.json /app/

RUN export VCPKG_ROOT=/vcpkg && \
export PATH=$VCPKG_ROOT:$PATH && \
cd /app/ && \
vcpkg install

COPY ./src /app/src
RUN apt install gcc
RUN export VCPKG_ROOT=/vcpkg && \
export PATH=$VCPKG_ROOT:$PATH && \
mkdir /app/bulld && cd /app/bulld && \
cmake --preset=default -DBUILD_TESTS=OFF ..
RUN cd app/build/ && make
RUN find / -name 'application'
COPY ./static /app/static

ENTRYPOINT ["/app/build/src/application", "--static_path", "/app/static", "--postgres_credentials", "postgres:1234"]