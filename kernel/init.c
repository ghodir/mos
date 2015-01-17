#include "multiboot.h"
#include "console.h"
#include "gdt.h"


void init(struct multiboot_info *mb_info) {
	console_init();
	kprintf("Hello World!\n");

	gdt_init();
}