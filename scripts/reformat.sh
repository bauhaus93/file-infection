#!/bin/sh

if command -v clang-format &> /dev/null
then
find $PWD/src -type f -regex ".*\.[ch]" -exec clang-format -i {} +
else
	echo "Install clang-format for c/h file formatting".
fi

if command -v cmake-format &> /dev/null
then
find $PWD -path "build-*" -prune -type f -regex ".*CMakeLists\.txt" -exec cmake-format -i {} +
else
	echo "Install cmake-format for cmake file formatting".
fi

if command -v black &> /dev/null
then
find $PWD -path "build-*" -prune -type f -regex ".*\.py" -exec black {} +
else
	echo "Install black for py file formatting".
fi
