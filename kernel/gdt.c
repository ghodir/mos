#include <stdint.h>
#include "gdt.h"

#define GDT_FLAG_DATASEG 0x02
#define GDT_FLAG_CODESEG 0x0a
#define GDT_FLAG_TSS     0x09

#define GDT_FLAG_SEGMENT 0x10
#define GDT_FLAG_RING0   0x00
#define GDT_FLAG_RING3   0x60
#define GDT_FLAG_PRESENT 0x80

#define GDT_FLAG_4K_GRAN 0x800
#define GDT_FLAG_32_BIT  0x400

#define GDT_ENTRIES 6
static uint64_t gdt[GDT_ENTRIES];
static uint32_t tss[32] = { 0, 0, 0x10 };

static void gdt_set_entry(int i, unsigned int base, unsigned int limit, int flags) {
	gdt[i] = limit & 0xffffLL;
	gdt[i] |= (base & 0xffffffLL) << 16;
	gdt[i] |= (flags & 0xffLL) << 40;
	gdt[i] |= ((limit >> 16) & 0xfLL) << 48;
	gdt[i] |= ((flags >> 8) & 0xffLL) << 52;
	gdt[i] |= ((base >> 24) & 0xffLL) << 56;
}

void gdt_init(void) {
	struct {
		uint16_t limit;
		void* pointer;
	} __attribute__((packed)) gdtp = {
		.limit = GDT_ENTRIES * 8 - 1,
		.pointer = gdt,
	};

	gdt_set_entry(0, 0, 0, 0);
	gdt_set_entry(1, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_CODESEG | GDT_FLAG_4K_GRAN | GDT_FLAG_PRESENT | GDT_FLAG_RING0);
	gdt_set_entry(2, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_DATASEG | GDT_FLAG_4K_GRAN | GDT_FLAG_PRESENT | GDT_FLAG_RING0);
	gdt_set_entry(3, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_CODESEG | GDT_FLAG_4K_GRAN | GDT_FLAG_PRESENT | GDT_FLAG_RING3);
	gdt_set_entry(4, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_DATASEG | GDT_FLAG_4K_GRAN | GDT_FLAG_PRESENT | GDT_FLAG_RING3);
	gdt_set_entry(5, (uint32_t)tss, sizeof(tss), GDT_FLAG_TSS | GDT_FLAG_PRESENT | GDT_FLAG_RING3);

	asm volatile("lgdt %0" :: "m" (gdtp));

	asm volatile(
		"mov $0x10, %ax;"
		"mov %ax, %ds;"
		"mov %ax, %es;"
		"mov %ax, %ss;"
		"ljmp $0x8, $.1;"
		".1:"
		);

	asm volatile("ltr %%ax" :: "a" (5 << 3));
}