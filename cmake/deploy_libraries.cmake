# 定义一个下载和解压库的函数
function(deploy_libraries url_filename)
    # 下载并解压库文件
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/libraries")
        if(NOT EXISTS "${CMAKE_SOURCE_DIR}/libraries.zip")
            file(STRINGS ${url_filename} LIBRARIES_URL)
            message(STATUS "Downloading libraries.zip from: ${LIBRARIES_URL}")

            file(DOWNLOAD 
                ${LIBRARIES_URL} "${CMAKE_SOURCE_DIR}/libraries.zip" 
                SHOW_PROGRESS
                STATUS DOWNLOAD_STATUS)

            list(GET DOWNLOAD_STATUS 0 DOWNLOAD_STATUS_CODE)
            list(GET DOWNLOAD_STATUS 1 DOWNLOAD_STATUS_MSG)
            if(DOWNLOAD_STATUS_CODE)
                message(FATAL_ERROR "Failed to download libraries.zip! -- Error: ${DOWNLOAD_STATUS_MSG}")
            else()
                message(STATUS "Download complete!")
            endif()
        endif()
        message(STATUS "Extracting libraries.zip...")
        file(ARCHIVE_EXTRACT
            INPUT "${CMAKE_SOURCE_DIR}/libraries.zip"
            DESTINATION "${CMAKE_SOURCE_DIR}/libraries")
        message(STATUS "Extraction complete!")
    endif()
endfunction()