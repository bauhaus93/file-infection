get_property(SRCS GLOBAL PROPERTY ALL_SOURCES)

set(TEST_INFECT_LIB32 "test_infect_lib32")
set(TEST_INFECT_LIB64 "test_infect_lib64")

set(INFECT_BIN "${CMAKE_CURRENT_BINARY_DIR}/infect.bin")
set(INFECT_BIN_DISASM "${CMAKE_CURRENT_BINARY_DIR}/infect.s")

add_custom_command(
  OUTPUT ${INFECT_BIN}
  COMMAND objcopy -O binary -j .text ${CMAKE_BINARY_DIR}/infect ${INFECT_BIN}
  DEPENDS infect)

add_custom_command(
  OUTPUT ${INFECT_BIN_DISASM}
  COMMAND objdump -d ${CMAKE_BINARY_DIR}/infect > ${INFECT_BIN_DISASM}
  DEPENDS infect)

add_custom_target(INFECT_BINARY DEPENDS ${INFECT_BIN})
add_custom_target(INFECT_BINARY_DISASSEMBLY DEPENDS ${INFECT_BIN_DISASM}
                                                    ${INFECT_BIN})

add_library(${TEST_INFECT_LIB32} OBJECT ${SRCS})
target_compile_options(${TEST_INFECT_LIB32} PUBLIC -m32)
target_link_options(${TEST_INFECT_LIB32} PUBLIC -m32)
target_compile_definitions(${TEST_INFECT_LIB32} PUBLIC USE_STANDARD_MALLOC
                                                       ENABLE_PRINT)
add_dependencies(${TEST_INFECT_LIB32} CHECKSUM_LIST STRING_GENERATOR)

add_library(${TEST_INFECT_LIB64} OBJECT ${SRCS})
target_compile_options(${TEST_INFECT_LIB64} PUBLIC -m64)
target_link_options(${TEST_INFECT_LIB64} PUBLIC -m64)
target_compile_definitions(${TEST_INFECT_LIB64} PUBLIC USE_STANDARD_MALLOC
                                                       ENABLE_PRINT)
add_dependencies(${TEST_INFECT_LIB64} CHECKSUM_LIST STRING_GENERATOR)

add_subdirectory(instructions)
add_subdirectory(function_discovery)
add_subdirectory(block_copy)
add_subdirectory(copy_execution)
