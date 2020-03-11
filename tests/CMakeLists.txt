if(BUILD_TESTING)
    set(EXEC_TEST_INSTR "check_instruction_length")

    macro(create_test)
        string(REPLACE ".s" ".bin" OUTPUT_FILENAME ${ARGV2})
        add_custom_target(
            COMMAND
            nasm -o ${OUTPUT_FILENAME} ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV2}
            SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV2})

        add_test(NAME ${ARGV0} COMMAND ${EXEC_TEST_INSTR} ${ARGV1}
                                       ${OUTPUT_FILENAME})
    endmacro()

    list(
        APPEND DISASM_SOURCES
               ${CMAKE_SOURCE_DIR}/src/disasm.c
               ${CMAKE_SOURCE_DIR}/src/utility.c
               ${CMAKE_SOURCE_DIR}/src/function_kernel32.c
               ${CMAKE_SOURCE_DIR}/src/checksum.c
               ${CMAKE_SOURCE_DIR}/src/pe.c
               ${CMAKE_SOURCE_DIR}/src/process_info.c
               ${CMAKE_SOURCE_DIR}/src/string_generator.c)

    add_executable(${EXEC_TEST_INSTR} check_instruction_length.c
                                      ${DISASM_SOURCES})
    target_include_directories(
        ${EXEC_TEST_INSTR} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                   ${CMAKE_SOURCE_DIR}/src)

    create_test(test_instructions_1byte 1 "instruction_1byte.s")
endif()
