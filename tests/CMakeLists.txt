if(BUILD_TESTING)
    set(LIB_NAME "disasm")

    list(
        APPEND DISASM_SOURCES
               ${CMAKE_SOURCE_DIR}/src/disasm.c
               ${CMAKE_SOURCE_DIR}/src/memory.c
               ${CMAKE_SOURCE_DIR}/src/utility.c)

    add_library(${LIB_NAME} SHARED ${DISASM_SOURCES})
    target_include_directories(${LIB_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_compile_definitions(${LIB_NAME} PRIVATE USE_STANDARD_MALLOC)

    add_test(
        NAME test_instruction_size_opcode1
        COMMAND
            python3 ${CMAKE_CURRENT_SOURCE_DIR}/check_instruction_size.py
            --bit-width 32 --disasm-lib "$<TARGET_FILE:${LIB_NAME}>"
            ${CMAKE_CURRENT_SOURCE_DIR}/instruction_op1.s)

    set_tests_properties(test_instruction_size_opcode1 PROPERTIES DEPENDS
                                                                  ${LIB_NAME})
endif()
