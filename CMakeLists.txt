cmake_minimum_required(VERSION 3.12)

project(kbasic)

set(SOURCE
    src/Lexer.cpp
    src/System.cpp
    src/FontManager.cpp
    src/Window.cpp
    src/MainWindow.cpp
    src/main.cpp
)

file(GLOB RESOURCE_FILES
    resources/*.otf
    resources/*.ttf
    resources/*.png
)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_file(SDL2_INCLUDE_DIR NAME SDL.h HINTS SDL2)
#find_path(SDL2_TTF_DIR SDL_ttf.h PATHS /Library/Frameworks/SDL2_image.framework/Headers)
#find_path(SDL2_IMAGE_DIR NAMES SDL_image.h)

if (APPLE)
    set(SDL2_TTF_DIR /Library/Frameworks/SDL2_ttf.framework/Headers)
endif()

find_library(COREFOUNDATION CoreFoundation)
if (COREFOUNDATION)
    set(CF_INCLUDE_DIR "${COREFOUNDATION}/Headers")
    set(CF_LIBRARY "${COREFOUNDATION}/CoreFoundation.tbd")
endif()
find_library(SDL2_LIBRARY NAME SLD2)

if (NOT SDL2_LIBRARY)
    if (APPLE)
        set(SDL2_LIBRARY /Library/Frameworks/SDL2.framework/Versions/Current/SDL2)
    endif()
endif()

if (NOT SDL2_TTF_LIB)
    if (APPLE)
        set(SDL2_TTF_LIB /Library/Frameworks/SDL2_ttf.framework/Versions/Current/SDL2_ttf)
    endif()
endif()

add_executable(kbasic ${SOURCE} ${RESOURCE_FILES})

if (APPLE) 
    set_target_properties(
        kbasic PROPERTIES
        MACOSX_BUNDLE TRUE
        RESOURCE "${RESOURCE_FILES}")
endif()

set_property(TARGET kbasic PROPERTY CXX_STANDARD 17)

target_include_directories (
    kbasic
    PUBLIC
    ${SDL2_INCLUDE_DIR}
    ${SDL2_TTF_DIR}
)

target_link_libraries (
    kbasic 
    PUBLIC
    ${SDL2_LIBRARY}
    ${SDL2_TTF_LIB}
    ${CF_LIBRARY}
)

target_compile_features(kbasic PRIVATE cxx_lambda_init_captures)

