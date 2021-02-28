set(EXEC_COPY_EXEC "copy_execution")

get_property(SRCS GLOBAL PROPERTY ALL_SOURCES)

add_executable(${EXEC_COPY_EXEC} copy_execution.c test_code.c ${SRCS})
target_compile_options(${EXEC_COPY_EXEC} PRIVATE -m32)
target_link_options(${EXEC_COPY_EXEC} PRIVATE -m32)
target_compile_definitions(
  ${EXEC_COPY_EXEC} PRIVATE ENABLE_PRINT USE_STANDARD_MALLOC
                            WINDOWS_HEADER_DUMMY)
add_dependencies(${EXEC_COPY_EXEC} STRING_GENERATOR)

add_test(NAME copy_execution COMMAND ${EXEC_COPY_EXEC})
