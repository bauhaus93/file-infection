set(DISCOVERY_EXEC "function_discovery")

get_property(SRCS GLOBAL PROPERTY ALL_SOURCES)

add_executable(${DISCOVERY_EXEC} "function_discovery.c" ${SRCS})
target_compile_options(${DISCOVERY_EXEC} PRIVATE -m32)
target_link_options(${DISCOVERY_EXEC} PRIVATE -m32)
target_compile_definitions(
  ${DISCOVERY_EXEC} PRIVATE USE_STANDARD_MALLOC WINDOWS_HEADER_DUMMY
                            ENABLE_PRINT)
add_dependencies(${DISCOVERY_EXEC} STRING_GENERATOR)

add_test(NAME discover_own_functions COMMAND ${DISCOVERY_EXEC})
