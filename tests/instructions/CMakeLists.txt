set(LIB_NAME "disasm")

set(TEST_DRIVER "${CMAKE_CURRENT_SOURCE_DIR}/instruction_test_driver.py")
set(TEST_GENERATOR
    "${CMAKE_CURRENT_SOURCE_DIR}/generate_random_instructions.py")

macro(create_instruction_size_test)
  set(TEST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}.s")
  add_test(NAME ${ARGV0}
           COMMAND python3 ${TEST_DRIVER} --bit-width 32 --disasm-lib
                   "$<TARGET_FILE:${LIB_NAME}>" ${TEST_FILE})

  set_tests_properties(
    ${ARGV0}
    PROPERTIES REQUIRED_FILES
               "$<TARGET_FILE:${LIB_NAME}>;${TEST_DRIVER};${TEST_FILE}")
endmacro()

macro(create_random_instruction_test)
  set(TEST_FILE "${CMAKE_CURRENT_BINARY_DIR}/${ARGV0}.s")

  if(NOT EXISTS ${TEST_FILE})
    execute_process(COMMAND python3 ${TEST_GENERATOR} --output-file
                            ${TEST_FILE} --seed ${ARGV0} --count ${ARGV1})
  endif()

  add_test(NAME test_${ARGV0}
           COMMAND python3 ${TEST_DRIVER} --bit-width 32 --disasm-lib
                   "$<TARGET_FILE:${LIB_NAME}>" ${TEST_FILE})

  set_tests_properties(
    test_${ARGV0}
    PROPERTIES REQUIRED_FILES
               "$<TARGET_FILE:${LIB_NAME}>;${TEST_GENERATOR};${TEST_FILE}")
endmacro()

get_property(SRCS GLOBAL PROPERTY ALL_SOURCES)

add_library(${LIB_NAME} SHARED)
target_compile_options(${LIB_NAME} PRIVATE -m64)
target_link_options(${LIB_NAME} PRIVATE -m64)
target_compile_definitions(${LIB_NAME} PUBLIC USE_STANDARD_MALLOC ENABLE_PRINT)
target_link_libraries(${LIB_NAME} ${TEST_INFECT_LIB64})
add_dependencies(${LIB_NAME} STRING_GENERATOR CHECKSUM_LIST)

create_instruction_size_test(instructions_op1_basic)
create_instruction_size_test(instructions_op1_extensions)
create_instruction_size_test(instructions_op1_sib)
create_instruction_size_test(instructions_op1_sib_displacement)
create_instruction_size_test(instructions_op1_modrm_displacement)
create_instruction_size_test(instructions_op1_prefix)
create_instruction_size_test(instructions_op2_basic)
create_instruction_size_test(instructions_op2_extensions)
create_instruction_size_test(instructions_op3_basic)
create_random_instruction_test("random_01" 1000)
