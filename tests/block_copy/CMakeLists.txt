set(BLOCK_COPY_EXEC "block_copy")

get_property(SRCS GLOBAL PROPERTY ALL_SOURCES)

add_executable(${BLOCK_COPY_EXEC} block_copy.c ${SRCS})
target_compile_options(${BLOCK_COPY_EXEC} PRIVATE -m32)
target_link_options(${BLOCK_COPY_EXEC} PRIVATE -m32)
target_compile_definitions(
  ${BLOCK_COPY_EXEC} PRIVATE ENABLE_PRINT USE_STANDARD_MALLOC
                             WINDOWS_HEADER_DUMMY)
add_dependencies(${BLOCK_COPY_EXEC} STRING_GENERATOR)

add_test(NAME copy_blocks
         COMMAND ${BLOCK_COPY_EXEC} ${CMAKE_CURRENT_BINARY_DIR}/unmodified.bin
                 ${CMAKE_CURRENT_BINARY_DIR}/modified.bin)
add_test(
  NAME compare_copied_blocks
  COMMAND
    python3 ${CMAKE_CURRENT_SOURCE_DIR}/compare_binary.py --unmodified-file
    ${CMAKE_CURRENT_BINARY_DIR}/unmodified.bin --modified-file
    ${CMAKE_CURRENT_BINARY_DIR}/modified.bin)
set_tests_properties(compare_copied_blocks PROPERTIES DEPENDS copy_blocks)
