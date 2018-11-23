set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86)

set(mingw_path D:/Programme/MinGW/mingw64)
set(CMAKE_C_COMPILER ${mingw_path}/bin/x86_64-w64-mingw32-gcc.exe)
set(CMAKE_CXX_COMPILER ${mingw_path}/bin/x86_64-w64-mingw32-c++.exe)
set(CMAKE_LINKER ${mingw_path}/bin/ld.exe)
set(CMAKE_AR ${mingw_path}/bin/ar.exe)