#!/bin/bash
# install_deps.sh

# Colores
GREEN="\033[1;32m"
CYAN="\033[1;36m"
YELLOW="\033[1;33m"
RED="\033[1;31m"
RESET="\033[0m"

function info() {
    echo -e "${CYAN}Este script instala las siguientes dependencias necesarias para compilar y ejecutar el proyecto:${RESET}"
    echo ""
    echo -e "${YELLOW}1. Dependencias de audio:${RESET}"
    echo "   - libopus-dev, libopusfile-dev, libxmp-dev, libfluidsynth-dev, fluidsynth"
    echo "   - libwavpack1, libwavpack-dev, wavpack"
    echo ""
    echo -e "${YELLOW}2. Dependencias gráficas y de renderizado:${RESET}"
    echo "   - libfreetype-dev, libfontconfig1-dev, libfreetype6-dev"
    echo ""
    echo -e "${YELLOW}3. Dependencias de X11 y Qt6:${RESET}"
    echo "   - libx11-dev, libx11-xcb-dev, libxext-dev, libxfixes-dev, libxi-dev"
    echo "   - libxrender-dev, libxcb* (varios), libxkbcommon* (varios), qt6-base-dev"
    echo ""
    echo -e "${YELLOW}4. YAML:${RESET}"
    echo "   - libyaml-cpp-dev"
    echo ""
}

function update_system() {
    echo -e "${GREEN}Actualizando lista de paquetes...${RESET}"
    sudo apt update || echo -e "${RED}Error al actualizar los paquetes.${RESET}"
}

function safe_install() {
    description="$1"
    shift
    echo -e "${GREEN}Instalando: ${description}...${RESET}"
    sudo apt install -y "$@" || echo -e "${RED}Error al instalar ${description}.${RESET}"
}

function install_audio_deps() {
    safe_install "Dependencias de audio" \
        libopus-dev libopusfile-dev libxmp-dev libfluidsynth-dev fluidsynth \
        libwavpack1 libwavpack-dev libfreetype-dev wavpack
}

function install_graphics_deps() {
    safe_install "Dependencias gráficas" \
        libfontconfig1-dev libfreetype6-dev
}

function install_x11_qt6_deps() {
    safe_install "Dependencias de X11 y Qt6" \
        libx11-dev libx11-xcb-dev libxext-dev libxfixes-dev libxi-dev libxrender-dev \
        libxcb1-dev libxcb-glx0-dev libxcb-keysyms1-dev libxcb-image0-dev libxcb-shm0-dev \
        libxcb-icccm4-dev libxcb-sync-dev libxcb-xfixes0-dev libxcb-shape0-dev libxcb-randr0-dev \
        libxcb-render-util0-dev libxkbcommon-dev libxkbcommon-x11-dev qt6-base-dev
}

function install_yaml_dep() {
    safe_install "Soporte para YAML" libyaml-cpp-dev
}

function install_all() {
    update_system
    install_audio_deps
    install_graphics_deps
    install_x11_qt6_deps
    install_yaml_dep
    echo -e "${GREEN}Proceso de instalación completo. Revise los errores arriba si hubo problemas.${RESET}"
}

# Menú principal
while true; do
    echo ""
    echo -e "${CYAN}===== MENÚ DE INSTALACIÓN =====${RESET}"
    echo "1. Mostrar información de dependencias"
    echo "2. Instalar todas las dependencias"
    echo "3. Actualizar sistema (apt update)"
    echo "4. Instalar dependencias de audio"
    echo "5. Instalar dependencias gráficas"
    echo "6. Instalar dependencias de X11 y Qt6"
    echo "7. Instalar soporte YAML"
    echo "0. Salir"
    echo -n "Seleccione una opción: "
    read -r option
    echo ""

    case $option in
        1) info ;;
        2) install_all ;;
        3) update_system ;;
        4) install_audio_deps ;;
        5) install_graphics_deps ;;
        6) install_x11_qt6_deps ;;
        7) install_yaml_dep ;;
        0) echo "Saliendo."; exit 0 ;;
        *) echo -e "${YELLOW}Opción no válida.${RESET}" ;;
    esac
done
