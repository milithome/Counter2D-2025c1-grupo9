#!/bin/bash
# install_deps.sh

echo "Instalando dependencias..."

sudo apt update

sudo apt install -y libopus-dev libopusfile-dev libxmp-dev libfluidsynth-dev fluidsynth libwavpack1 libwavpack-dev libfreetype-dev wavpack

sudo apt install -y libfontconfig1-dev libfreetype6-dev libx11-dev libx11-xcb-dev libxext-dev libxfixes-dev libxi-dev libxrender-dev libxcb1-dev libxcb-glx0-dev libxcb-keysyms1-dev libxcb-image0-dev libxcb-shm0-dev libxcb-icccm4-dev libxcb-sync-dev libxcb-xfixes0-dev libxcb-shape0-dev libxcb-randr0-dev libxcb-render-util0-dev libxkbcommon-dev libxkbcommon-x11-dev qt6-base-dev

sudo apt install -y libyaml-cpp-dev
