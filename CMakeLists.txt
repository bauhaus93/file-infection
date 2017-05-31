cmake_minimum_required(VERSION 3.4.3)
project(Rel_Code CXX)

enable_language(ASM)

set(COMPILE_FLAGS "-std=c++14 -Wall -fmax-errors=1 -fpic")
set(LINK_FLAGS "-std=c++14 -Wall -fmax-errors=1 -fpic")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINK_FLAGS}")

set(SRCS	"src/main.c"
					"src/virus.c"
					"src/utility.c"
					"src/checksum.c"
					"src/infect.c"
					"src/ass.s")

add_executable(rel_code ${SRCS})

set_target_properties(rel_code PROPERTIES OUTPUT_NAME "rel_code.exe")

include_directories(/usr/include)
include_directories(src)
