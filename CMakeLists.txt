cmake_minimum_required(VERSION 3.4.3)
project(Infect C)

enable_language(ASM)

if ("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
	set(COMPILE_FLAGS "-std=c99 -Wall -fmax-errors=1")
	set(LINK_FLAGS "${COMPILE_FLAGS}")
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
else()
	message(SEND_ERROR "Unknown compiler: ${CMAKE_C_COMPILER_ID}")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINK_FLAGS}")

set(SRCS	"src/main.c"
					"src/virus.c"
					"src/utility.c"
					"src/checksum.c"
					"src/infect.c"
					"src/ass.s")

add_executable(infect ${SRCS})

set_target_properties(infect PROPERTIES OUTPUT_NAME "infect.exe")

include_directories(/usr/include)
include_directories(src)
