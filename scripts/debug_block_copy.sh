gdb -ex 'b copy_block' \
	-ex 'r' \
	-ex 'p *block' \
	-ex 'x/16xb block->start' \
	--args build-native/tests/block_copy/block_copy build-native/tests/infect.bin /tmp/a.out
