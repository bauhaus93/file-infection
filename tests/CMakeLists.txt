if(BUILD_TESTING)
    set(LIB_NAME "disasm")

    macro(create_instruction_size_test)
        add_test(
            NAME ${ARGV0}
            COMMAND
                python3 ${CMAKE_CURRENT_SOURCE_DIR}/check_instruction_size.py
                --bit-width 32 --disasm-lib "$<TARGET_FILE:${LIB_NAME}>"
                ${CMAKE_CURRENT_SOURCE_DIR}/${ARGV}.s)

        set_tests_properties(${ARGV0} PROPERTIES DEPENDS ${LIB_NAME})
    endmacro()

    list(
        APPEND DISASM_SOURCES
               ${CMAKE_SOURCE_DIR}/src/disasm/disasm.c
               ${CMAKE_SOURCE_DIR}/src/disasm/disasm_utility.c
               ${CMAKE_SOURCE_DIR}/src/disasm/instruction.c
               ${CMAKE_SOURCE_DIR}/src/disasm/instruction_op1.c
               ${CMAKE_SOURCE_DIR}/src/disasm/instruction_op2.c
               ${CMAKE_SOURCE_DIR}/src/disasm/prefix.c
               ${CMAKE_SOURCE_DIR}/src/disasm/size.c
               ${CMAKE_SOURCE_DIR}/src/memory.c
               ${CMAKE_SOURCE_DIR}/src/utility.c)

    add_library(${LIB_NAME} SHARED ${DISASM_SOURCES})
    target_include_directories(${LIB_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_compile_definitions(${LIB_NAME} PRIVATE USE_STANDARD_MALLOC)

    create_instruction_size_test(instructions_op1_basic)
    create_instruction_size_test(instructions_op1_extensions)
    create_instruction_size_test(instructions_op1_sib)
    create_instruction_size_test(instructions_op1_sib_displacement)
    create_instruction_size_test(instructions_op1_modrm_displacement)
endif()
