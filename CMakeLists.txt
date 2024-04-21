cmake_minimum_required(VERSION 3.22)

if (VCPKG_PATH)
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_PATH}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
else ()
    # message with error: You must set VCPKG_PATH to your vcpkg root directory
    message(FATAL_ERROR "You must set VCPKG_PATH to your vcpkg root directory\ne.g. cmake -S . -B build -DVCPKG_PATH=/path/to/vcpkg")
endif ()

project(LongMarch)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD 17)

find_package(fmt CONFIG REQUIRED)
set(FMT_LIB_NAME fmt::fmt-header-only) # fmt::fmt is also available
list(APPEND LIB_LIST ${FMT_LIB_NAME})

find_package(spdlog CONFIG REQUIRED)
set(SPDLOG_LIB_NAME spdlog::spdlog_header_only) # spdlog::spdlog is also available
list(APPEND LIB_LIST ${SPDLOG_LIB_NAME})

find_package(Eigen3 CONFIG REQUIRED)
set(EIGEN3_LIB_NAME Eigen3::Eigen)
list(APPEND LIB_LIST ${EIGEN3_LIB_NAME})

find_package(glfw3 CONFIG REQUIRED)
set(GLFW3_LIB_NAME glfw)
list(APPEND LIB_LIST ${GLFW3_LIB_NAME})

find_package(Vulkan REQUIRED)
set(VULKAN_LIB_NAME ${Vulkan_LIBRARIES})
list(APPEND LIB_LIST ${VULKAN_LIB_NAME})

find_package(VulkanMemoryAllocator CONFIG REQUIRED)
set(VMA_LIB_NAME Vulkan::Headers GPUOpen::VulkanMemoryAllocator)
list(APPEND LIB_LIST ${VMA_LIB_NAME})

find_package(glslang CONFIG REQUIRED)
set(GLSLANG_LIB_NAME glslang::OSDependent glslang::glslang glslang::MachineIndependent glslang::GenericCodeGen glslang::glslang-default-resource-limits glslang::OGLCompiler glslang::SPVRemapper glslang::SPIRV glslang::HLSL)
list(APPEND LIB_LIST ${GLSLANG_LIB_NAME})

find_package(Python3 COMPONENTS Interpreter REQUIRED)

include(scripts/pack_shaders.cmake)

add_subdirectory(code)

add_subdirectory(demo)
