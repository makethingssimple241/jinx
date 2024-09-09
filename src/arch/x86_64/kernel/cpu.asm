; void out_byte(u16 port, byte b)
global out_byte
out_byte:
    ; rdi is first parameter, therefore di is 'port'
    ; rsi is second parameter, therefore sil is 'byte'
    mov     dx, di
    mov     al, sil
    out     dx, al     ; send data in sil to di
    ret

; byte in_byte(u16 port)
global in_byte
in_byte:
    ; rdi is first parameter, therefore di is 'port'
    mov     rax, 0      ; zero return value register
    mov     dx, di
    in      ax, dx
    ret

; TODO: Identify which exception occurred

; void panic(void)
extern panic

; void isr_exception(void)
global isr_exception
isr_exception:
    call    panic
    iretq

; void idt_load(const idt_register *idtr)
global idt_load
idt_load:
    lidt    [rdi]       ; rdi is first parameter, therefore is 'idtr'
    sti
    ret

; void halt(void)
global halt
halt:
    hlt
