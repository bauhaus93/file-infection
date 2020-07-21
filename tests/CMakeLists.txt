if(BUILD_TESTING)
  set(LIB_NAME "disasm")
  set(FIND_FUNC_EXEC "find_functions")

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
    set(TEST_FILE "${CMAKE_CURRENT_BINARY_DIR}/${ARGV0}.s")

    execute_process(
      COMMAND
	python3 ${TEST_DRIVER} --output-file ${TEST_FILE} --seed ${ARGV0}
	--count ${ARGV1} --input-dir ${CMAKE_CURRENT_SOURCE_DIR} --info-dir
	${CMAKE_CURRENT_BINARY_DIR}/instruction_info)

    add_test(
      NAME test_${ARGV0}
      COMMAND
	python3 ${CMAKE_CURRENT_SOURCE_DIR}/instruction_test_driver.py
	--bit-width 32 --disasm-lib "$<TARGET_FILE:${LIB_NAME}>" ${TEST_FILE})

    set_tests_properties(
      test_${ARGV0}
      PROPERTIES REQUIRED_FILES
		 "$<TARGET_FILE:${LIB_NAME}>;${TEST_DRIVER};${TEST_FILE}")
  endmacro()

  get_property(SRCS GLOBAL PROPERTY ALL_SOURCES)

  add_library(${LIB_NAME} SHARED ${SRCS})
  target_compile_options(${LIB_NAME} PRIVATE -m64)
  target_link_options(${LIB_NAME} PRIVATE -m64)
  target_compile_definitions(${LIB_NAME} PRIVATE USE_STANDARD_MALLOC
						 WINDOWS_HEADER_DUMMY)

  add_executable(${FIND_FUNC_EXEC} "find_own_functions.c" ${SRCS})
  target_compile_options(${FIND_FUNC_EXEC} PRIVATE -m32)
  target_link_options(${FIND_FUNC_EXEC} PRIVATE -m32)
  target_compile_definitions(${FIND_FUNC_EXEC} PRIVATE USE_STANDARD_MALLOC
						       WINDOWS_HEADER_DUMMY)
  add_test(NAME find_own_functions COMMAND ${FIND_FUNC_EXEC})

  create_instruction_size_test(instructions_op1_basic)
  create_instruction_size_test(instructions_op1_extensions)
  create_instruction_size_test(instructions_op1_sib)
  create_instruction_size_test(instructions_op1_sib_displacement)
  create_instruction_size_test(instructions_op1_modrm_displacement)
  create_instruction_size_test(instructions_op1_prefix)
  create_instruction_size_test(instructions_op2_basic)
  create_instruction_size_test(suspects)
  create_instruction_size_test(instructions_op2_extensions)
  create_instruction_size_test(instructions_op3_basic)

endif()
