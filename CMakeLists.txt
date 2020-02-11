cmake_minimum_required(VERSION 3.4)
project(infect C)

macro(add_sources)
    file(RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}"
         "${CMAKE_CURRENT_SOURCE_DIR}")
    foreach(_src ${ARGN})
        if(_relPath)
            file(TO_NATIVE_PATH "${_relPath}/${_src}" _nativePath)
        else()
            file(TO_NATIVE_PATH ${_src} _nativePath)
        endif()
        set_property(
            TARGET infect
            APPEND
            PROPERTY SRC_LIST ${_nativePath})
    endforeach()
endmacro()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE
        Debug
        CACHE STRING "Choose build type" FORCE)
endif()

set(EXEC_NAME "infect")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(${EXEC_NAME} "")

set_property(TARGET ${EXEC_NAME} PROPERTY C_STANDARD 99)
if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(${EXEC_NAME} PRIVATE -m32 -Wall -fmax-errors=1
                                                -Werror -fno-builtin)
    target_link_libraries(${EXEC_NAME} -Wall -m32 -fmax-errors=1 -Werror)
else()
    message(SEND_ERROR "Unknown compiler: ${CMAKE_C_COMPILER_ID}")
endif()

add_subdirectory(src)
get_property(
    SRCS
    TARGET ${EXEC_NAME}
    PROPERTY SRC_LIST)
target_sources(${EXEC_NAME} PRIVATE ${SRCS})

target_include_directories(${EXEC_NAME} PRIVATE src)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include)
target_include_directories(${EXEC_NAME} PRIVATE ${CMAKE_BINARY_DIR}/include)
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/include/checksum_list.h
    COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/generate_checksums.py
            --input-file ${CMAKE_SOURCE_DIR}/src/checksums.txt --output-file ${CMAKE_BINARY_DIR}/include/checksum_list.h
    MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/src/checksums.txt)

target_sources(${EXEC_NAME} PRIVATE ${CMAKE_BINARY_DIR}/include/checksum_list.h)

