rm -rf release
mkdir release
cd release && cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=cmake/mingw_32bit_toolchain.cmake -DCMAKE_BUILD_TYPE=Release ..

