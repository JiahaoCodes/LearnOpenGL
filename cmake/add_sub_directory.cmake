# 定义拷贝shader文件的自定义目标
function(define_depoly_shaders dir_name)
    string(REGEX MATCH "([^/]*)/?$" FOLDER_NAME ${dir_name})
    set(OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders")

    # 收集所有文件
    file(GLOB SHADER_FILES "${dir_name}/shaders/*")
    set(SHADER_OUTPUTS "")

    # 遍历所有文件
    foreach(shader_source IN LISTS SHADER_FILES)
        get_filename_component(file_name ${shader_source} NAME)
        set(output_file "${OUTPUT_DIR}/${file_name}")
        add_custom_command(
            OUTPUT ${output_file}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${shader_source} ${output_file}
            DEPENDS ${shader_source}
        )
        list(APPEND SHADER_OUTPUTS ${output_file})
    endforeach()

    add_custom_target("${FOLDER_NAME}_depoly_shaders" DEPENDS ${SHADER_OUTPUTS})
endfunction()

# 为当前目录中的所有源代码添加编译可执行文件
function(add_sub_directory dir_name)
    if(NOT EXISTS dir_name)
        file(MAKE_DIRECTORY ${dir_name})
        file(MAKE_DIRECTORY ${dir_name}/shaders)
    endif()
    string(REGEX MATCH "([^/]*)/?$" FOLDER_NAME ${dir_name})
    define_depoly_shaders(${dir_name})
    file(GLOB CPP_FILES "${dir_name}/*.cpp")

    foreach(file IN LISTS CPP_FILES)
        get_filename_component(name ${file} NAME_WE)
        add_executable(${name} ${file})
        target_link_libraries(${name} 
            PRIVATE opengl32 assimp::assimp glfw glad::glad glm::glm imgui::imgui spdlog::spdlog)
        target_include_directories(${name}
            PRIVATE ${Stb_INCLUDE_DIR})
        add_dependencies(${name} "${FOLDER_NAME}_depoly_shaders" deploy_assets)
    endforeach()
endfunction()
