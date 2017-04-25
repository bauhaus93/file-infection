.file	"ass.s"
.text

.globl	_strKernel32
.def	_strKernel32;	.scl	2;	.type	32;	.endef
_strKernel32:
	.ascii "KERNEL32.dll\0"

.globl	_get_image_base
.def	_get_image_base;	.scl	2;	.type	32;	.endef
_get_image_base:
  pushl	%ebp
  movl	%esp, %ebp

  movl %fs:0x30, %eax
  movl 0xC(%eax), %eax
  movl 0x14(%eax), %eax
  movl 0x10(%eax), %eax

  popl %ebp
  ret

.globl	_get_kernel32_base
.def	_get_kernel32_base;	.scl	2;	.type	32;	.endef
_get_kernel32_base:
  pushl	%ebp
  movl	%esp, %ebp

  movl %fs:0x30, %eax
  movl 0xC(%eax), %eax
  movl 0x14(%eax), %eax
  movl (%eax), %eax
  movl (%eax), %eax
  movl 0x10(%eax), %eax

  popl %ebp
  ret

.globl	_end_code
.def	_end_code;	.scl	2;	.type	32;	.endef
_end_code:
  ret


.ident	"GCC: (GNU) 6.3.1 20170109"
