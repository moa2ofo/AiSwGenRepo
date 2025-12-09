cmake_minimum_required(VERSION 3.16)
project(UdsComm C)

# Attiva compile_commands.json (utile per cppcheck)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Includi directory header
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/pltf
    ${CMAKE_CURRENT_SOURCE_DIR}/cfg
)

# Sorgenti
set(SOURCES
    pltf/diagnostic.c
    cfg/diagnostic_cfg.c
)

# Crea una libreria o eseguibile di test
add_library(udscomm STATIC ${SOURCES})

