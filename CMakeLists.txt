cmake_minimum_required(VERSION 3.4)
project(infect C ASM)

set(EXEC_NAME "infect")
set(DELTA_NAME "delta")
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

if((NOT ENV32) AND (NOT ENV64))
  set(ENV32
      ON
      CACHE STRING "Enable 32 bit compilation")
endif()

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

macro(strip_relocation_before_linking)
	if(STRIP_PROGRAM) 
	add_custom_command(
     TARGET ${ARGV0}
	 PRE_LINK
	 COMMAND ${STRIP_PROGRAM} --remove-relocations .text $<TARGET_OBJECTS:${ARGV0}>)
	else()
		message(SEND_ERROR "No strip program given, but need one for stripping relocation")
	endif()
endmacro()

#add_library(${DELTA_NAME} OBJECT "src/delta.c")

# macro(link_target_with_delta)
#   target_sources(${ARGV0} PRIVATE $<TARGET_OBJECTS:${DELTA_NAME}>)
#   add_custom_command(
#     TARGET ${ARGV0}
#     PRE_BUILD
#     COMMAND ${STRIP_PROGRAM} --remove-relocations .text
# 	    $<TARGET_FILE:${DELTA_NAME}> DEPENDS ${DELTA_NAME})
# endmacro()

if(${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
  add_compile_options(-fmax-errors=1 -Wall -Werror -fno-builtin)
  add_link_options(-fmax-errors=1 -Wall -Werror)

  if(ENV32)
    add_compile_options(-m32)
    add_link_options(-m32)
  elseif(ENV64)
    add_compile_options(-m64)
    add_link_options(-m64)
  endif()
else()
  message(SEND_ERROR "Unknown compiler: ${CMAKE_C_COMPILER_ID}")
endif()

include_directories(src)
add_subdirectory(src)
get_property(SRCS GLOBAL PROPERTY ALL_SOURCES)

add_executable(${EXEC_NAME} "src/main.c" ${SRCS})
set_property(TARGET ${EXEC_NAME} PROPERTY C_STANDARD 99)

if(STRIP_PROGRAM)
  if(NOT STRIP_ARGS)
    set(STRIP_ARGS "")
  endif()
  add_custom_command(
    TARGET ${EXEC_NAME}
    POST_BUILD
    COMMAND ${STRIP_PROGRAM} ${STRIP_ARGS} $<TARGET_FILE:${EXEC_NAME}>)


  add_custom_command(
    TARGET ${EXEC_NAME}
	PRE_LINK
    COMMAND ${STRIP_PROGRAM} --remove-relocations .text $<TARGET_OBJECTS:${EXEC_NAME}>)
endif()


#link_target_with_delta(${EXEC_NAME})

add_custom_command(
  OUTPUT ${CMAKE_SOURCE_DIR}/src/checksum_list.h
  COMMAND
    python3 ${CMAKE_SOURCE_DIR}/scripts/file_generation/generate_checksums.py
    --input-file ${CMAKE_SOURCE_DIR}/resources/checksums.txt --output-file
    ${CMAKE_SOURCE_DIR}/src/checksum_list.h && clang-format -i
    ${CMAKE_SOURCE_DIR}/src/checksum_list.h
  MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/resources/checksums.txt
  DEPENDS ${CMAKE_SOURCE_DIR}/scripts/file_generation/generate_checksums.py)

target_sources(${EXEC_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/checksum_list.h)

add_custom_command(
  OUTPUT ${CMAKE_SOURCE_DIR}/src/string_generator.c
	 ${CMAKE_SOURCE_DIR}/src/string_generator.h
  COMMAND
    python3 ${CMAKE_SOURCE_DIR}/scripts/file_generation/string_generator.py
    --string-file ${CMAKE_SOURCE_DIR}/resources/strings.txt --header-path
    ${CMAKE_SOURCE_DIR}/src/string_generator.h --source-path
    ${CMAKE_SOURCE_DIR}/src/string_generator.c && clang-format -i
    ${CMAKE_SOURCE_DIR}/src/string_generator.c
    ${CMAKE_SOURCE_DIR}/src/string_generator.h
  MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/resources/strings.txt
  DEPENDS ${CMAKE_SOURCE_DIR}/scripts/file_generation/string_generator.py)

target_sources(${EXEC_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/string_generator.h
				    ${CMAKE_SOURCE_DIR}/src/string_generator.c)

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
