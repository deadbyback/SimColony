cmake_minimum_required(VERSION 3.15.0)
project(SimColony VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(VCPKG_INSTALLED_DIR "C:/dev/tools/vcpkg/installed/x64-windows")
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};C:/dev/tools/vcpkg/installed/x64-windows")

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenMP REQUIRED)

set(SOURCES
    src/main.cpp
    src/engine/Window.cpp
    src/engine/core/Renderer.cpp
    src/engine/core/ResourceCache.cpp
    src/engine/rendering/Camera.cpp
    src/engine/rendering/Shader.cpp
    src/engine/rendering/ShaderLoader.cpp
    src/engine/rendering/Texture.cpp
    src/engine/rendering/TileMap.cpp
    src/engine/rendering/RenderableTile.cpp
    src/engine/rendering/IsometricTile.cpp

    src/game/world/WorldMap.cpp
    src/game/world/LocalMapGenerator.cpp
    src/game/world/TileRegistry.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/content
        ${CMAKE_BINARY_DIR}/content
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${VCPKG_INSTALLED_DIR}/include  # Прямой путь к заголовочным файлам vcpkg
    ${CMAKE_CURRENT_SOURCE_DIR}/src/third_party
)

find_path(GLM_INCLUDE_DIRS "glm/glm.hpp")
find_path(GLAD_INCLUDE_DIRS "glad/glad.h")
find_path(STB_INCLUDE_DIRS "stb_image.h")

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${GLM_INCLUDE_DIRS}
    ${GLAD_INCLUDE_DIRS}
    ${STB_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    glfw
    glad::glad
    glm::glm
    imgui::imgui
    nlohmann_json::nlohmann_json
    OpenMP::OpenMP_CXX
)

# Копируем зависимые DLL в директорию с исполняемым файлом
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:glfw>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
    
    # Если есть другие DLL, добавляем их аналогично
    if(TARGET glad::glad)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:glad::glad>
                $<TARGET_FILE_DIR:${PROJECT_NAME}>
        )
    endif()
endif()