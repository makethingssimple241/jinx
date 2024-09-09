section .text:
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

    ; void exception_handler(u64 exception)
    extern exception_handler

    %macro isr 1
        global isr_%+%1
        isr%+%1:
            ; rdi is the first parameter, which is the exception
            mov     rdi, %1
            call    exception_handler
            iretq
    %endmacro

    isr 0
    isr 1
    isr 2
    isr 3
    isr 4
    isr 5
    isr 6
    isr 7
    isr 8
    isr 9
    isr 10
    isr 11
    isr 12
    isr 13
    isr 14
    isr 15
    isr 16
    isr 17
    isr 18
    isr 19
    isr 20
    isr 21
    isr 22
    isr 23
    isr 24
    isr 25
    isr 26
    isr 27
    isr 28
    isr 29
    isr 30
    isr 31

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

section .bss:
    global isrs
    isrs:
        %assign i 0
        %rep 32
            dq      isr%+i
        %endrep
        %assign i i+1