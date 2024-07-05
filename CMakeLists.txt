cmake_minimum_required(VERSION 3.20)

include(cmake/deploy_libraries.cmake)
deploy_libraries(libraries_url.ini)
if(EXISTS ${CMAKE_SOURCE_DIR}/libraries/vcpkg)
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/libraries/vcpkg/scripts/buildsystems/vcpkg.cmake)
    message(STATUS "Using vcpkg toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
endif()

project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

if(MSVC)
    add_compile_options(/wd4819)
    add_compile_options(/utf-8)
endif()

include(cmake/find_target_libraries.cmake)
find_target_libraries(assimp Stb glfw3 glad glm imgui spdlog)

include(cmake/add_sub_directory.cmake)
include_directories(${CMAKE_SOURCE_DIR}/source/include)

# 定义对资源文件的复制操作
set(COPIED_ASSETS_FILES "")
set(ASSETS_DIRECTORY "${CMAKE_SOURCE_DIR}/assets")
file(GLOB_RECURSE ASSETS_FILES "${ASSETS_DIRECTORY}/*")
foreach(assets_file IN LISTS ASSETS_FILES)
    # 计算每个文件的相对路径
    file(RELATIVE_PATH ASSET_REL_PATH ${ASSETS_DIRECTORY} ${assets_file})
    # message(STATUS "Copying ${ASSET_REL_PATH} to bin directory")
    # 设置目标文件路径
    set(OUTPUT_FILE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${ASSET_REL_PATH}")

    # 添加复制操作
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${assets_file} ${OUTPUT_FILE}
        DEPENDS ${assets_file}
    )

    list(APPEND COPIED_ASSETS_FILES ${OUTPUT_FILE})
endforeach()

add_custom_target(deploy_assets
    DEPENDS ${COPIED_ASSETS_FILES}
    COMMENT "Copying assets to bin directory"
)

add_sub_directory(source/1_getting_started)
add_sub_directory(source/demo)
