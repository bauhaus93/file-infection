set(CHECKSUM_TXT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/checksums.txt)
set(CHECKSUM_H_PATH ${CMAKE_CURRENT_BINARY_DIR}/include/checksum_list.h)

set(STRING_GEN_TXT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/strings.txt)
set(STRING_GEN_C_PATH ${CMAKE_CURRENT_BINARY_DIR}/string_generator.c)
set(STRING_GEN_H_PATH ${CMAKE_CURRENT_BINARY_DIR}/include/string_generator.h)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include)

if(NOT EXISTS ${STRING_GEN_C_PATH})
  file(WRITE ${STRING_GEN_C_PATH} )
endif()

add_custom_command(
  OUTPUT ${CHECKSUM_H_PATH}
  COMMAND
    python3 ${CMAKE_CURRENT_SOURCE_DIR}/generate_checksums.py --input-file
    ${CHECKSUM_TXT_PATH} --output-file ${CHECKSUM_H_PATH} && clang-format -i
    ${CHECKSUM_H_PATH}
  MAIN_DEPENDENCY ${CHECKSUM_TXT_PATH}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generate_checksums.py)

add_custom_command(
  OUTPUT ${STRING_GEN_H_PATH} ${STRING_GEN_C_PATH}
  COMMAND
    python3 ${CMAKE_CURRENT_SOURCE_DIR}/string_generator.py --string-file
    ${STRING_GEN_TXT_PATH} --header-path ${STRING_GEN_H_PATH} --source-path
    ${STRING_GEN_C_PATH} && clang-format -i ${STRING_GEN_H_PATH}
    ${STRING_GEN_C_PATH}
  MAIN_DEPENDENCY ${STRING_GEN_TXT_PATH}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/string_generator.py)

add_custom_target(CHECKSUM_LIST DEPENDS ${CHECKSUM_H_PATH})

add_custom_target(STRING_GENERATOR DEPENDS ${STRING_GEN_H_PATH}
                                           ${STRING_GEN_C_PATH})

get_property(SRCS GLOBAL PROPERTY ALL_SOURCES)
list(INSERT SRCS 1 ${STRING_GEN_C_PATH})
set_property(GLOBAL PROPERTY ALL_SOURCES ${SRCS})
