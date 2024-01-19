# CLion remote docker environment (How to build docker container, run and stop it)
# Build and run:
#   docker build -t clion/remote-cpp-env:0.5 -f Dockerfile.remote-cpp-env .
#   docker run -d --cap-add sys_ptrace --cap-add SYS_ADMIN -p 127.0.0.1:2222:22 --name clion_remote_env --device /dev/fuse clion/remote-cpp-env:0.5
#   ssh-keygen -f "$HOME/.ssh/known_hosts" -R "[localhost]:2222"
#   Clion 2021.1
# stop:
#   docker stop clion_remote_env
# 
# ssh credentials (test user):
#   user@password
# Post Install (pendind to install in image):
# wget https://github.com/Kitware/CMake/releases/download/3.27.9/cmake-3.27.9-linux-x86_64.sh
# sh cmake-3.27.9-linux-x86_64.sh --skip-license
#
# Install mongocxx library from source

FROM ubuntu:20.04

RUN DEBIAN_FRONTEND="noninteractive" apt-get update && apt-get -y install tzdata

RUN apt-get update \
  && apt-get install -y ssh \
      sudo \
      build-essential \
      gcc \
      g++ \
      gdb \
      clang \
      cmake \
      git \
      libssl-dev \
      rsync \
      tar \
      python \
      libfuse-dev \
      libcurlpp-dev \
      libcurl4-openssl-dev \
      fuse3 \
  && apt-get clean

RUN ( \
    echo 'LogLevel DEBUG2'; \
    echo 'PermitRootLogin yes'; \
    echo 'PasswordAuthentication yes'; \
    echo 'Subsystem sftp /usr/lib/openssh/sftp-server'; \
  ) > /etc/ssh/sshd_config_test_clion \
  && mkdir /run/sshd

RUN useradd -m user \
  && yes password | passwd user

RUN usermod -s /bin/bash user && usermod -aG sudo user
RUN mkdir -p /home/user/tidal && chown user:user /home/user/tidal

CMD ["/usr/sbin/sshd", "-D", "-e", "-f", "/etc/ssh/sshd_config_test_clion"]