FROM debian:latest

RUN apt-get update -qq && apt install build-essential -qqy && mkdir /data

WORKDIR /project

# TODO: Build with `docker build -t cs736/mini-project .`
# TODO: Go to this project and run `docker run -it -v ~/Projects/CS736/mini-project:/project -m="1g" cs736/mini-project bash`