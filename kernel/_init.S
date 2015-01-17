// define multiboot header for grub
.section multiboot
#define MB_MAGIC 0x1badb002
#define MB_FLAGS 0x0
#define MB_CHECKSUM -(MB_MAGIC + MB_FLAGS)

.align 4
.int MB_MAGIC
.int MB_FLAGS
.int MB_CHECKSUM

.section .text

.extern init

// let's get started
.global _start
_start:
	// set kernel stack
	mov $kernel_stack, %esp

	//store pointer to multiboot structure on stack
	push %ebx
	call init

	// this should never happen - idle
	jmp .

.section .bss
.space 8192
kernel_stack: