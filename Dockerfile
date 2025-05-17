FROM ubuntu:24.04

# Instalar herramientas básicas
RUN apt update && apt install -y \
    cmake \
    g++ \
    git \
    build-essential \
    wget \
    curl \
    pkg-config \
    sudo

# Crear directorio de trabajo
WORKDIR /app

# Copiar el proyecto y el script de dependencias
COPY . .

# Asegurar permisos de ejecución del script
RUN chmod +x install_deps.sh

# Instalar dependencias del proyecto
RUN ./install_deps.sh

# Compilar el proyecto
RUN make