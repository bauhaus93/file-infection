set(BLOCK_COPY_EXEC "block_copy")

add_executable(${BLOCK_COPY_EXEC} block_copy.c)
target_compile_options(${BLOCK_COPY_EXEC} PRIVATE -m32)
target_link_options(${BLOCK_COPY_EXEC} PRIVATE -m32)
target_link_libraries(${BLOCK_COPY_EXEC} ${TEST_INFECT_LIB32}
                      ${TEST_UTILITY_LIB})
target_compile_definitions(${BLOCK_COPY_EXEC} PRIVATE ENABLE_PRINT
                                                      USE_STANDARD_MALLOC)

add_test(NAME copy_blocks
         COMMAND ${BLOCK_COPY_EXEC} ${CMAKE_BINARY_DIR}/tests/infect.bin
                 ${CMAKE_CURRENT_BINARY_DIR}/infect_modified.bin)
add_test(
  NAME compare_copied_blocks
  COMMAND
    python3 ${CMAKE_CURRENT_SOURCE_DIR}/compare_binary.py --unmodified-file
    ${CMAKE_BINARY_DIR}/tests/infect.bin --modified-file
    ${CMAKE_CURRENT_BINARY_DIR}/infect_modified.bin)
set_tests_properties(compare_copied_blocks PROPERTIES DEPENDS "copy_blocks")
