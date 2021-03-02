set(DISCOVERY_EXEC "function_discovery")

set(FUNCTION_INFO_FILE "${CMAKE_CURRENT_BINARY_DIR}/function_info.txt")
set(FUNCTION_INFO_GENERATOR
    "${CMAKE_CURRENT_SOURCE_DIR}/create_function_info.py")

add_custom_command(
  OUTPUT ${FUNCTION_INFO_FILE}
  COMMAND python3 ${FUNCTION_INFO_GENERATOR} --disassembly ${INFECT_BIN_DISASM}
          --output ${FUNCTION_INFO_FILE}
  DEPENDS ${FUNCTION_INFO_GENERATOR} INFECT_BINARY_DISASSEMBLY)

add_custom_target(FUNCTION_INFO DEPENDS ${FUNCTION_INFO_FILE})

add_executable(${DISCOVERY_EXEC} "function_discovery.c")
target_compile_options(${DISCOVERY_EXEC} PRIVATE -m32)
target_link_options(${DISCOVERY_EXEC} PRIVATE -m32)
target_link_libraries(${DISCOVERY_EXEC} ${TEST_INFECT_LIB32}
                      ${TEST_UTILITY_LIB})
target_compile_definitions(${DISCOVERY_EXEC} PUBLIC USE_STANDARD_MALLOC
                                                    ENABLE_PRINT)
add_dependencies(${DISCOVERY_EXEC} INFECT_BINARY INFECT_BINARY_DISASSEMBLY
                 FUNCTION_INFO)
add_test(NAME discover_own_functions COMMAND ${DISCOVERY_EXEC} ${INFECT_BIN}
                                             ${FUNCTION_INFO_FILE})
