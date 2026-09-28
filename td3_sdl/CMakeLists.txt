cmake_minimum_required(VERSION 3.16)
project(td3_sdl C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED)

# app/ n'est pas duplique : ce sont litteralement les memes fichiers que
# ceux compiles pour la cible STM32 dans ../td3_complete (une seule source
# de verite), juste recompiles ici avec la BSP simulateur.
set(TD3_APP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../td3_complete/app)

add_executable(td3_sdl
    main.c
    bsp/bsp.c
    bsp/led.c
    bsp/buttons.c
    ${TD3_APP_DIR}/app.c
)

target_include_directories(td3_sdl PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/bsp
    ${TD3_APP_DIR}
)
target_link_libraries(td3_sdl PRIVATE SDL3::SDL3)
