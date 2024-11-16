#include "core/base.h"
#include "core/string.h"
#include "drivers/serial.h"
#include "drivers/gop.h"
#include "kernel/arch.h"

typedef enum : u64 {
    isr_division_by_zero,
    isr_debug,
    isr_non_maskable_interrupt,
    isr_breakpoint,
    isr_overflow,
    isr_bound_range_exceeded,
    isr_invalid_opcode,
    isr_device_not_available,
    isr_double_fault,
    isr_coprocessor_segment_overrun,
    isr_invalid_tss,
    isr_segment_not_present,
    isr_stack_segment_fault,
    isr_general_protection_fault,
    isr_page_fault,

    isr_reserved1,

    isr_x87_floating_point_exception,
    isr_alignment_check,
    isr_machine_check,
    isr_simd_floating_point_exception,
    isr_virtualization_exception,
    isr_control_protection_exception,

    isr_reserved2       = 22,
    isr_reserved3       = 23,
    isr_reserved4       = 24,
    isr_reserved5       = 25,
    isr_reserved6       = 26,
    isr_reserved7       = 27,

    isr_hypervisor_injection_exception,
    isr_vmm_communication_exception,
    isr_security_exception,

    isr_reserved8       = 31,

    isr_exception_max   = 31,

    isr_irq_pit         = 32,
} isr;

const char *exception_messages[] = {
    [isr_division_by_zero] = "Division by zero exception",
    [isr_debug] = "Debug interrupt",
    [isr_non_maskable_interrupt] = "Non-maskable interrupt",
    [isr_breakpoint] = "Breakpoint",
    [isr_overflow] = "Overflow",
    [isr_bound_range_exceeded] = "Bound range exceeded",
    [isr_invalid_opcode] = "Invalid opcode",
    [isr_device_not_available] = "Device not available",
    [isr_double_fault] = "Double fault",
    [isr_coprocessor_segment_overrun] = "Coprocessor segment overrun",
    [isr_invalid_tss] = "Invalid TSS",
    [isr_segment_not_present] = "Segment not present exception",
    [isr_stack_segment_fault] = "Stack-segment fault",
    [isr_general_protection_fault] = "General protection fault",
    [isr_page_fault] = "Page fault",
    [isr_x87_floating_point_exception] = "x87 floating-point exception",
    [isr_alignment_check] = "Alignment check",
    [isr_machine_check] = "Machine check",
    [isr_simd_floating_point_exception] = "SIMD floating-point exception",
    [isr_virtualization_exception] = "Virtualization exception",
    [isr_control_protection_exception] = "Control protection exception",
    [isr_hypervisor_injection_exception] = "Hypervisor injection exception",
    [isr_vmm_communication_exception] = "VMM communication exception",
    [isr_security_exception] = "Security exception",
};

typedef enum {
    general_protection_fault_selector_type_gdt  = 0b00,
    general_protection_fault_selector_type_idt1 = 0b01,
    general_protection_fault_selector_type_ldt  = 0b10,
    general_protection_fault_selector_type_idt2 = 0b11,
} general_protection_fault_selector_type;

const char *general_protection_fault_selector_type_to_str(general_protection_fault_selector_type type) {
    switch (type) {        
    case general_protection_fault_selector_type_gdt:
        return "GDT";
    case general_protection_fault_selector_type_idt1:
    case general_protection_fault_selector_type_idt2:
        return "IDT";
    case general_protection_fault_selector_type_ldt:
        return "LDT";
    }
}

typedef union {
    struct packed {
        bool external_to_processor                              : 1;
        general_protection_fault_selector_type selector_type    : 2;
        u16 selector                                            : 13;
        u16 reserved;
    };

    u32 data;
} general_protection_fault_error_code;

typedef union {
    struct packed {
        bool present                            : 1;
        bool caused_by_write                    : 1;
        bool in_user_mode                       : 1;
        bool reserved_write                     : 1;
        bool caused_by_instruction_fetch        : 1;
        bool caused_by_protection_key_violation : 1;
        bool caused_by_shadow_stack             : 1;
        u8 reserved1;
        bool caused_by_sgx_violation            : 1;
        u16 reserved2;
    };
    u32 data;
} page_fault_error_code;

typedef struct {
    u64 r15, r14, r13, r12, r11, r10, r9, r8;
    u64 rdi, rsi, rbp, rbx, rdx, rcx, rax;
    isr interrupt;
    u64 error_code, return_rip;
} interrupt_stack;

static void print_interrupt_stack(const interrupt_stack *stack) {
    static char buffer[256];

    string_format(buffer, sizeof(buffer), "rax=0x%x64 rbx=0x%x64 rcx=0x%x64 rdx=0x%x64\n", stack->rax, stack->rbx, stack->rcx, stack->rdx);
    gop_put_string(buffer);

    string_format(buffer, sizeof(buffer), "rbp=0x%x64 rsi=0x%x64 rdi=0x%x64\n", stack->rbp, stack->rsi, stack->rdi);
    gop_put_string(buffer);

    string_format(buffer, sizeof(buffer), "r8=0x%x64 r9=0x%x64 r10=0x%x64 r11=0x%x64\n", stack->r8, stack->r9, stack->r10, stack->r11);
    gop_put_string(buffer);

    string_format(buffer, sizeof(buffer), "r12=0x%x64 r13=0x%x64 r14=0x%x64 r15=0x%x64\n", stack->r12, stack->r13, stack->r14, stack->r15);
    gop_put_string(buffer);

    string_format(buffer, sizeof(buffer), "return rip=0x%x64\n", stack->return_rip);
    gop_put_string(buffer);
}

void interrupt_handler(const interrupt_stack *stack) {
    if (stack->interrupt > isr_exception_max) {
        switch (stack->interrupt) {
        case isr_irq_pit:
            serial_write_string(COM1, "Timer\n");
            break;
        default:
            break;
        }

        return;
    }

    char buffer[256];

    switch (stack->interrupt) {
    case isr_general_protection_fault: {
        general_protection_fault_error_code gpf;
        gpf.data = stack->error_code;

        char gpf_error_code_buffer[256];

        if (stack->error_code != 0) {
            string_format(gpf_error_code_buffer, sizeof(gpf_error_code_buffer), " with error code 0b%b64%s (type=\"%s\", selector=0x%x)",
                          stack->error_code, general_protection_fault_selector_type_to_str(gpf.selector_type), gpf.selector);
        }

        string_format(buffer, sizeof(buffer), "%s occured%s",
                      exception_messages[stack->interrupt], gpf_error_code_buffer);
    } break;
    case isr_page_fault: {
        page_fault_error_code pf;
        pf.data = (u32)stack->error_code;

        string_format(buffer, sizeof(buffer), "%s occured with error code 0b%b64 (P=%u, W=%u, U=%u)",
                      exception_messages[stack->interrupt],
                      stack->error_code,
                      pf.present, pf.caused_by_write, pf.in_user_mode);
    } break;
    default: {
        if (stack->interrupt >= len(exception_messages)) {
            string_format(buffer, sizeof(buffer), "Unhandled interrupt %u64", stack->interrupt);
        } else {
            string_format(buffer, sizeof(buffer), "%s occured with error code 0x%x64 0b%b64", exception_messages[stack->interrupt], stack->error_code, stack->error_code);
        }
    } break;
    }

    print_interrupt_stack(stack);
    panic(buffer);

    // Auto end of interrupt is enabled, so end of interrupt do not need to be sent to PIC
}
