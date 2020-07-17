if(BUILD_TESTING)
  set(LIB_NAME "disasm")

  macro(create_instruction_size_test)
    set(TEST_DRIVER "${CMAKE_CURRENT_SOURCE_DIR}/instruction_test_driver.py")
    set(TEST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${ARGV0}.s")
    add_test(
      NAME ${ARGV0}
      COMMAND
	python3 ${TEST_DRIVER} --bit-width 32 --disasm-lib
	"$<TARGET_FILE:${LIB_NAME}>" ${TEST_FILE})

    set_tests_properties(
      ${ARGV0}
      PROPERTIES REQUIRED_FILES
		 "$<TARGET_FILE:${LIB_NAME}>;${TEST_DRIVER};${TEST_FILE}")
  endmacro()

  macro(create_random_instruction_test)
    set(TEST_DRIVER
	"${CMAKE_CURRENT_SOURCE_DIR}/generate_random_instructions.py")
    set(TEST_FILE "${CMAKE_CURRENT_BINARY_DIR}/random_${ARGV0}.s")

    if(NOT EXISTS ${TEST_FILE})
      execute_process(
	COMMAND
	  python3 ${TEST_DRIVER} --output-file ${TEST_FILE} --seed ${ARGV0}
	  --count ${ARGV1} --input-dir ${CMAKE_CURRENT_SOURCE_DIR})

      add_test(
	NAME ${ARGV0}
	COMMAND
	  python3 ${CMAKE_CURRENT_SOURCE_DIR}/instruction_test_driver.py
	  --bit-width 32 --disasm-lib "$<TARGET_FILE:${LIB_NAME}>" ${TEST_FILE})

      set_tests_properties(
	${ARGV0}
	PROPERTIES REQUIRED_FILES
		   "$<TARGET_FILE:${LIB_NAME}>;${TEST_DRIVER};${TEST_FILE}")
    endif()
  endmacro()

  list(
    APPEND DISASM_SOURCES
	   ${CMAKE_SOURCE_DIR}/src/disasm/disasm.c
	   ${CMAKE_SOURCE_DIR}/src/disasm/disasm_utility.c
	   ${CMAKE_SOURCE_DIR}/src/disasm/instruction.c
	   ${CMAKE_SOURCE_DIR}/src/disasm/instruction_op1.c
	   ${CMAKE_SOURCE_DIR}/src/disasm/instruction_op2.c
	   ${CMAKE_SOURCE_DIR}/src/disasm/instruction_op3.c
	   ${CMAKE_SOURCE_DIR}/src/disasm/prefix.c
	   ${CMAKE_SOURCE_DIR}/src/disasm/size.c
	   ${CMAKE_SOURCE_DIR}/src/memory.c
	   ${CMAKE_SOURCE_DIR}/src/utility.c)

  add_library(${LIB_NAME} SHARED ${DISASM_SOURCES})
  target_compile_options(${LIB_NAME} PRIVATE -Wall -Werror)
  target_include_directories(${LIB_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
  target_compile_definitions(${LIB_NAME} PRIVATE USE_STANDARD_MALLOC)

  create_instruction_size_test(instructions_op1_basic)
  create_instruction_size_test(instructions_op1_extensions)
  create_instruction_size_test(instructions_op1_sib)
  create_instruction_size_test(instructions_op1_sib_displacement)
  create_instruction_size_test(instructions_op1_modrm_displacement)
  create_instruction_size_test(instructions_op1_prefix)
  create_instruction_size_test(instructions_op2_basic)
  create_instruction_size_test(suspects)
  create_instruction_size_test(instructions_op2_extensions)

  create_random_instruction_test("RandomTest0_1000" 1000)

endif()
