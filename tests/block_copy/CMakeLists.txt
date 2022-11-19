set(BLOCK_COPY_EXEC "block_copy")
set(INFECT_BIN_PATH "${CMAKE_BINARY_DIR}/tests/infect.bin")
set(INFECT_DISASM_PATH "${CMAKE_BINARY_DIR}/tests/infect.s")
set(INFECT_MOD_BIN_PATH "${CMAKE_CURRENT_BINARY_DIR}/infect_modified.bin")
set(TEST_DRIVER "${CMAKE_CURRENT_SOURCE_DIR}/compare_binary.py")

add_executable(${BLOCK_COPY_EXEC} block_copy.c)
target_compile_options(${BLOCK_COPY_EXEC} PRIVATE -m32)
target_link_options(${BLOCK_COPY_EXEC} PRIVATE -m32)
target_link_libraries(${BLOCK_COPY_EXEC} ${TEST_INFECT_LIB32}
                      ${TEST_UTILITY_LIB})
target_compile_definitions(${BLOCK_COPY_EXEC} PRIVATE ENABLE_PRINT
                                                      USE_STANDARD_MALLOC)

add_test(
  NAME check_generated_bin_disasm
  COMMAND
    python3 ${TEST_DRIVER} --unmodified-binary ${INFECT_BIN_PATH}
    --modified-disassembly ${INFECT_DISASM_PATH} --check-type instruction_count)

add_test(NAME copy_blocks_infect COMMAND ${BLOCK_COPY_EXEC} ${INFECT_BIN_PATH}
                                         ${INFECT_MOD_BIN_PATH})

add_test(
  NAME check_copied_blocks_instruction_count
  COMMAND
    python3 ${TEST_DRIVER} --unmodified-binary ${INFECT_BIN_PATH}
    --modified-binary ${INFECT_MOD_BIN_PATH} --check-type instruction_count)

add_test(
  NAME check_copied_blocks_call_targets
  COMMAND python3 ${TEST_DRIVER} --unmodified-binary ${INFECT_BIN_PATH}
          --modified-binary ${INFECT_MOD_BIN_PATH} --check-type call_targets)

set_tests_properties(check_generated_bin_disasm PROPERTIES LABELS "copy_code")
set_tests_properties(copy_blocks_infect PROPERTIES LABELS "copy_code")
set_tests_properties(check_copied_blocks_instruction_count
                     PROPERTIES DEPENDS "copy_blocks_infect" LABELS "copy_code")
set_tests_properties(
  check_copied_blocks_call_targets
  PROPERTIES DEPENDS "copy_blocks_infect" LABELS "copy_code" DEPENDS
             check_copied_blocks_instruction_count)
