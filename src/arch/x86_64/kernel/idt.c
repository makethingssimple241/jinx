#include "core/base.h"
#include "core/memory.h"

#define IDT_MAX_ENTRIES                         256
#define LIMINE_64_BIT_CODE_DESCRIPTOR_OFFSET    40

typedef enum {
    idt_entry_type_task_gate             = 0b0101,
    idt_entry_type_16_bit_interrupt_gate = 0b0110,
    idt_entry_type_16_bit_trap_gate      = 0b0111,
    idt_entry_type_32_bit_interrupt_gate = 0b1110,
    idt_entry_type_32_bit_trap_gate      = 0b1111,
} idt_entry_type;

typedef enum {
    privilege_level_ring0,
    privilege_level_ring1,
    privilege_level_ring2,
    privilege_level_ring3,
} privilege_level;

typedef struct {
    u16 offset1;
    u16 code_segment_selector;
    u8 ist                          : 2;
    u8 reserved1                    : 6;
    idt_entry_type type             : 4;
    bool reserved2                  : 1;
    privilege_level privilege_level : 2;
    bool present                    : 1;
    u16 offset2;
    u32 offset3;
    u32 reserved3;
} idt_entry;

typedef struct packed {
    /// @brief One less than the size of the IDT
    u16 limit;
    u64 base;
} idt_register;

static idt_entry idt[IDT_MAX_ENTRIES];
static idt_register idtr = {0};

/// @param isr_address Address of the interrupt service routine
static void idt_set(u8 vector, void *isr_address, idt_entry_type type, privilege_level privilege_level) {
    idt_entry *entry = &idt[vector];
    memory_set(entry, 0, sizeof(idt_entry));

    entry->offset1 = (u64)isr_address;
    entry->code_segment_selector = LIMINE_64_BIT_CODE_DESCRIPTOR_OFFSET;
    entry->type = type;
    entry->privilege_level = privilege_level;
    entry->present = true;
    entry->offset2 = (u64)isr_address >> 16;
    entry->offset3 = (u64)isr_address >> 32;
}

void idt_load(const idt_register *idtr);

void isr_exception(void);

void idt_init(void) {
    for (u8 vector = 0; vector < 32; ++vector) {
        idt_set(vector, isr_exception, idt_entry_type_32_bit_interrupt_gate, privilege_level_ring0);
    }

    idtr.limit = sizeof(idt) - 1;
    idtr.base = (u64)&idt[0];
    idt_load(&idtr);
}
