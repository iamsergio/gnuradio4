# docker build -t fair .
# docker run -it -v /data/sources/fair/:/fair/  --cap-add=SYS_PTRACE --security-opt seccomp=unconfined --ulimit nofile=1024:4096 -u 1000:1000 kddw-tests


FROM ubuntu:24.04

ENV TZ=Europe/Berlin
ENV LC_CTYPE=C.UTF-8

ENV LSAN_OPTIONS=detect_leaks=0

RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt update -qq && apt install \
    gcc-14 g++-14 build-essential software-properties-common git cmake ninja-build valgrind pip \
    vim wget curl sudo file unzip zip which xz-utils lldb-20 clang-tidy-20 lld-20 clangd-20 clang-format-20 \
    doxygen lld mold ccache python3-pip npm \
    -y

RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100 && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 && \
    update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-20 100 && \
    update-alternatives --install /usr/bin/lld lld /usr/bin/lld-20 100 && \
    update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-20 100

RUN echo 'ALL ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER ubuntu
WORKDIR /home/ubuntu
