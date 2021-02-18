file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/string_generator.c )

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/checksum_list.h
  COMMAND
    python3 ${CMAKE_CURRENT_SOURCE_DIR}/generate_checksums.py --input-file
    ${CMAKE_CURRENT_SOURCE_DIR}/checksums.txt --output-file
    ${CMAKE_CURRENT_BINARY_DIR}/include/checksum_list.h && clang-format -i
    ${CMAKE_CURRENT_BINARY_DIR}/include/checksum_list.h
  MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/checksums.txt
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generate_checksums.py)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/string_generator.c
         ${CMAKE_CURRENT_BINARY_DIR}/include/string_generator.h
  COMMAND
    python3 ${CMAKE_CURRENT_SOURCE_DIR}/string_generator.py --string-file
    ${CMAKE_CURRENT_SOURCE_DIR}/strings.txt --header-path
    ${CMAKE_CURRENT_BINARY_DIR}/include/string_generator.h --source-path
    ${CMAKE_CURRENT_BINARY_DIR}/string_generator.c && clang-format -i
    ${CMAKE_CURRENT_BINARY_DIR}/string_generator.c
    ${CMAKE_CURRENT_BINARY_DIR}/include/string_generator.h
  MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/strings.txt
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/string_generator.py)

add_custom_target(CHECKSUM_LIST
                  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/checksum_list.h)

add_custom_target(
  STRING_GENERATOR
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/string_generator.c
          ${CMAKE_CURRENT_BINARY_DIR}/include/string_generator.h)

get_property(SRCS GLOBAL PROPERTY ALL_SOURCES)
list(INSERT SRCS 1 ${CMAKE_CURRENT_BINARY_DIR}/string_generator.c)

set_property(GLOBAL PROPERTY ALL_SOURCES ${SRCS})
