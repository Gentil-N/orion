cmake_minimum_required(VERSION 3.0)

project(orion)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

set(ORN_SRC_DIR  ${PROJECT_SOURCE_DIR}/code)
set(ORN_SRC_FILES    ${ORN_SRC_DIR}/asset/asset.cpp
                     ${ORN_SRC_DIR}/asset/stb_image.c
                     ${ORN_SRC_DIR}/asset/minimp3.c
                     ${ORN_SRC_DIR}/desktop/desktop.cpp
                     ${ORN_SRC_DIR}/sound/sound.cpp)

set(BUILD_SHARED_LIBS_SAVED "${BUILD_SHARED_LIBS}")
set(BUILD_STATIC_LIBS_SAVED "${BUILD_STATIC_LIBS}")
set(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(external/assimp)
set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)
add_subdirectory(external/glm)
add_subdirectory(external/glfw)
add_subdirectory(external/spdlog)
add_subdirectory(external/vulkan-headers)
add_subdirectory(external/entt)

find_library(OPENAL_LIB openal)
if(NOT OPENAL_LIB)
       message(FATAL "OpenAL library : Not found")
else()
       message(STATUS "OpenAL library : Found")
endif()

find_package(Threads)

if(ORION_BUILD_SHARED)
       add_library(orion SHARED ${ORN_SRC_FILES})
else()
       add_library(orion STATIC ${ORN_SRC_FILES})
endif()

target_link_libraries(orion glfw assimp glm spdlog ${OPENAL_LIB} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(orion PUBLIC ${PROJECT_SOURCE_DIR}/external/vulkan-headers/include)