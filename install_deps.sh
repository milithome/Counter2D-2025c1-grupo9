#!/bin/bash
# install_deps.sh

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
        libxcb-render-util0-dev libxkbcommon-dev libxkbcommon-x11-dev qt6-base-dev \
        qt6-multimedia-dev \
        gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad \
        gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools
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

function compile_project() {
    echo -e "${GREEN}Compilando el proyecto con make...${RESET}"
    make || echo -e "${RED}Error al compilar el proyecto.${RESET}"
}

function run_tests() {
    echo -e "${GREEN}Ejecutando test unitarios...${RESET}"
    if [ -f "./build/taller_tests" ]; then
        ./build/taller_tests
    else
        echo -e "${YELLOW}No se encontró un ejecutable de tests.${RESET}"
    fi
}

function post_install() {
    local NAME="taller"
    local BIN_DIR="build"
    local BINARIES=("taller_client" "taller_server" "taller_editor" "taller_tests") #agregar el editor en algun momento
    local ASSETS_DIR="assets"
    local CONFIG_FILES=("config_server.yaml" "config_client.yaml")

    echo -e "${GREEN}Instalando binarios, assets y configuración del proyecto '${NAME}'...${RESET}"

    for binary in "${BINARIES[@]}"; do
        local BIN_PATH="${BIN_DIR}/${binary}"
        if [ -f "$BIN_PATH" ]; then
            echo -e "${CYAN}Copiando binario ${binary} a /usr/bin...${RESET}"
            sudo cp "$BIN_PATH" /usr/bin/
        else
            echo -e "${RED}Binario no encontrado: ${BIN_PATH}${RESET}"
        fi
    done

    if [ -d "$ASSETS_DIR" ]; then
        echo -e "${CYAN}Copiando assets a /var/${NAME}/assets...${RESET}"
        sudo mkdir -p "/var/${NAME}"
        sudo cp -r "${ASSETS_DIR}/"* "/var/${NAME}/"
    else
        echo -e "${YELLOW}Carpeta de assets no encontrada en '${ASSETS_DIR}'${RESET}"
    fi

    echo -e "${CYAN}Copiando archivos de configuración a /etc/${NAME}/...${RESET}"
    sudo mkdir -p "/etc/${NAME}/"
    for config in "${CONFIG_FILES[@]}"; do
        if [ -f "$config" ]; then
            sudo cp "$config" "/etc/${NAME}/"
        else
            echo -e "${YELLOW}Archivo de configuración no encontrado: ${config}${RESET}"
        fi
    done

    echo -e "${GREEN}Instalación final completada. Revisá los mensajes anteriores por si hubo errores.${RESET}"
}

# ================= MENÚ PRINCIPAL =================

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
    echo "8. Compilar proyecto con make"
    echo "9. Ejecutar tests unitarios"
    echo "10. Mover binarios a /usr/bin, asseta a /var/NAME/ y configs a  /etc/NAME/" 
    echo "11. Ejecutar TODO (deps + compilación + tests)"
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
        8) compile_project ;;
        9) run_tests ;;
        10) post_install ;; 
        11)
            install_all
            compile_project
            run_tests
            post_install
            ;;
        0) echo "Saliendo."; exit 0 ;;
        *) echo -e "${YELLOW}Opción no válida.${RESET}" ;;
    esac
done
