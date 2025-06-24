FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y \
    cmake \
    g++ \
    git \
    build-essential \
    wget \
    curl \
    pkg-config \
    sudo

WORKDIR /app

COPY . .

RUN chmod +x install.sh
