cmake_minimum_required(VERSION 3.19)
project(infect C ASM)

macro(add_sources)
  file(RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}"
       "${CMAKE_CURRENT_SOURCE_DIR}")
  foreach(_src ${ARGN})
    if(_relPath)
      file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/${_relPath}/${_src}" _nativePath)
    else()
      file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/${_src}" _nativePath)
    endif()
    set_property(GLOBAL APPEND PROPERTY ALL_SOURCES ${_nativePath})
  endforeach()
endmacro()

set(EXEC_NAME "infect")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      Debug
      CACHE STRING "Choose build type" FORCE)
endif()

if(NOT BUILD_TESTING)
  set(BUILD_TESTING
      OFF
      CACHE STRING "Enable testing" FORCE)
endif()

if(${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
  add_compile_options(-fno-asynchronous-unwind-tables -fno-jump-tables
                      -fmax-errors=1 -Wall -fno-builtin)
  add_link_options(-fmax-errors=1 -Wall -Werror)
else()
  message(SEND_ERROR "Unknown compiler: ${CMAKE_C_COMPILER_ID}")
endif()

include_directories(src)
include_directories(${CMAKE_BINARY_DIR}/filegen/include)
add_subdirectory(src)
add_subdirectory(filegen)

get_property(SRCS GLOBAL PROPERTY ALL_SOURCES)

add_executable(${EXEC_NAME} "src/main.c" ${SRCS})
target_compile_options(${EXEC_NAME} PRIVATE -m32)
target_link_options(${EXEC_NAME} PRIVATE -m32)
target_compile_definitions(${EXEC_NAME} PRIVATE ENABLE_POISON)
set_property(TARGET ${EXEC_NAME} PROPERTY C_STANDARD 99)

add_dependencies(${EXEC_NAME} CHECKSUM_LIST STRING_GENERATOR)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
