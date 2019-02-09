FROM debian:latest

RUN apt-get update -qq && apt install build-essential wget cmake -qqy

WORKDIR /cmake
RUN wget https://github.com/Kitware/CMake/releases/download/v3.14.0-rc1/cmake-3.14.0-rc1.tar.gz && \
    tar -xzvf ./cmake-3.14.0-rc1.tar.gz

RUN cd ./cmake-3.14.0-rc1 && ./bootstrap && make -j4 && make install