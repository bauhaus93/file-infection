rm -rf debug
mkdir debug
cd debug && cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=cmake/mingw_32bit_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug ..
