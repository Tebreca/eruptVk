cmake_minimum_required(VERSION 3.30)

set(PROJECT_NAME Quantcpp)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 26)

# Enable ExternalProject module
include(ExternalProject)
include(FetchContent)


# Set variables for dependencies
set(DEPENDENCIES_DIR "${CMAKE_SOURCE_DIR}/dependencies")
set(DEPS_DOWNLOAD_DIR "${DEPENDENCIES_DIR}/download")
set(GIT_DEPS_SCRIPT "${CMAKE_SOURCE_DIR}/cmake/update_dependencies.cmake")

# Create directories if they don't exist
file(MAKE_DIRECTORY ${DEPENDENCIES_DIR})
file(MAKE_DIRECTORY ${DEPS_DOWNLOAD_DIR})

# Define options to control dependency updates
option(UPDATE_DEPENDENCIES "Update git dependencies" ON)

# Execute the git dependency update script before configuring the project
if (UPDATE_DEPENDENCIES)
    execute_process(
            COMMAND ${CMAKE_COMMAND} -P ${GIT_DEPS_SCRIPT}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            RESULT_VARIABLE GIT_DEPS_RESULT
    )
    if (NOT GIT_DEPS_RESULT EQUAL 0)
        message(WARNING "Failed to update git dependencies. Error code: ${GIT_DEPS_RESULT}")
    endif ()
endif ()

set(SOURCES
        src/main.cpp
        src/Window.cpp
        src/Window.h
        src/constants/GameConstants.h
        dependencies/vkb/src/VkBootstrap.h
        dependencies/vkb/src/VkBootstrap.cpp
        dependencies/vkb/src/VkBootstrapDispatch.h
        src/render/RenderManager.cpp
        src/render/RenderManager.h
        src/render/Swapchain.cpp
        src/render/Swapchain.h
        src/constants/GraphicsConstants.h
        src/render/Pipeline.cpp
        src/render/Pipeline.h
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

if (EXISTS "${DEPENDENCIES_DIR}/vma")
    add_subdirectory("${DEPENDENCIES_DIR}/vma" "${CMAKE_BINARY_DIR}/vma")
    target_link_libraries(${PROJECT_NAME} PRIVATE GPUOpen::VulkanMemoryAllocator)
endif ()


if (EXISTS "${DEPENDENCIES_DIR}/glfw")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
    add_subdirectory("${DEPENDENCIES_DIR}/glfw" "${CMAKE_BINARY_DIR}/glfw")
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
endif ()

if (EXISTS "${DEPENDENCIES_DIR}/vkb")
    include_directories("${DEPENDENCIES_DIR}/vkb" "${CMAKE_BINARY_DIR}/vkb")
endif ()

if (EXISTS "${DEPENDENCIES_DIR}/volk")
    if (WIN32)
        set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
    elseif ()
    endif ()
    include_directories("${DEPENDENCIES_DIR}/volk" "${CMAKE_BINARY_DIR}/volk")
    target_link_libraries(${PROJECT_NAME} PRIVATE volk_headers)
endif ()

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
)

set(SHADERS
    vertex.vert
)

find_package(Vulkan REQUIRED)

# Find glslc shader compiler
find_program(GLSLC glslc HINTS Vulkan::glslc)

if(NOT GLSLC)
    message(FATAL_ERROR "glslc shader compiler not found!")
endif()

# Shader compilation function
function(target_shaders target shaders)
    set(SHADERS_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")
    file(MAKE_DIRECTORY "${SHADERS_BUILD_DIR}")
    foreach(SHADER_PATH ${shaders})
        get_filename_component(SHADER_FILENAME "${SHADER_PATH}" NAME_WLE)
        set(SHADER_SPIRV_PATH "${SHADERS_BUILD_DIR}/${SHADER_FILENAME}.spv")
        set(DEPFILE "${SHADER_SPIRV_PATH}.d")
        add_custom_command(
          COMMENT "Building ${SHADER_FILENAME}"
          OUTPUT "${SHADER_SPIRV_PATH}"
          COMMAND ${GLSLC} "${SHADER_PATH}" -o "${SHADER_SPIRV_PATH}" -MD -MF ${DEPFILE} -g
          DEPENDS "${SHADER_PATH}"
          DEPFILE "${DEPFILE}"
        )
        list(APPEND SPIRV_BINARY_FILES ${SHADER_SPIRV_PATH})
    endforeach()

    set(shaders_target_name "${target}_build_shaders")
    add_custom_target(${shaders_target_name}
      DEPENDS ${SPIRV_BINARY_FILES}
    )
    add_dependencies(${target} ${shaders_target_name})
endfunction()

# Define shader files with full paths
set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shader")
set(SHADER_SOURCES
    "${SHADER_DIR}/vertex.vert"
    # Add other shader files here as needed
)

target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)

# Compile the shaders
target_shaders(${PROJECT_NAME} "${SHADER_SOURCES}")

# Optional: Copy compiled shaders to the install directory
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders/ 
        DESTINATION bin/shaders
        FILES_MATCHING PATTERN "*.spv")
