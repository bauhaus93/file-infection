cmake_minimum_required(VERSION 3.4.3)
project(infect C)

#enable_language(ASM)

macro(add_sources)
	file (RELATIVE_PATH _relPath "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
	foreach(_src ${ARGN})
		if(_relPath)
			file(TO_NATIVE_PATH "${_relPath}/${_src}" _nativePath)
		else()
			file(TO_NATIVE_PATH ${_src} _nativePath)
		endif()
		set_property(TARGET infect APPEND PROPERTY SRC_LIST ${_nativePath})
	endforeach()
endmacro()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose build type" FORCE)
endif()

set(EXEC_NAME "infect")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(${EXEC_NAME} "")

set_property(TARGET ${EXEC_NAME} PROPERTY C_STANDARD 99)
if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
	target_compile_options(${EXEC_NAME} PRIVATE -Wall -fmax-errors=1 -Werror -fno-builtin)
else()
	message(SEND_ERROR "Unknown compiler: ${CMAKE_C_COMPILER_ID}")
endif()

add_subdirectory(src)
get_property(SRCS TARGET ${EXEC_NAME} PROPERTY SRC_LIST)
target_sources(${EXEC_NAME} PRIVATE ${SRCS})

target_include_directories(${EXEC_NAME} PRIVATE src)
#target_include_directories(${EXEC_NAME} PRIVATE D:/Programme/MinGW/include)