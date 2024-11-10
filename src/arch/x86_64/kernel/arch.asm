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
        ; rax (ax) is return value
        mov     rax, 0
        mov     dx, di
        in      ax, dx
        ret
    
    ; void enable_interrupts(void)
    global enable_interrupts
    enable_interrupts:
        sti
        ret

    ; void disable_interrupts(void)
    global disable_interrupts
    disable_interrupts:
        cli
        ret

    ; void interrupt_handler(const interrupt_stack *stack)
    extern interrupt_handler

    %macro pushaq 0
        push    rax
        push    rcx
        push    rdx
        push    rbx
        push    rbp
        push    rsi
        push    rdi
        push    r8
        push    r9
        push    r10
        push    r11
        push    r12
        push    r13
        push    r14
        push    r15
    %endmacro

    %macro popaq 0
        pop     r15
        pop     r14
        pop     r13
        pop     r12
        pop     r11
        pop     r10
        pop     r9
        pop     r8
        pop     rdi
        pop     rsi
        pop     rbp
        pop     rbx
        pop     rdx
        pop     rcx
        pop     rax
    %endmacro

    %macro isr_without_error_code 1
        global isr_%+%1
        isr_%+%1:
            push    0
            push    %1
            jmp     isr_common
    %endmacro

    %macro isr_with_error_code 1
        global isr_%+%1
        isr_%+%1:
            push    %1
            jmp     isr_common
    %endmacro

    isr_common:
        pushaq
        cld

        ; rdi is the first parameter, which is 'stack'
        mov     rdi, rsp
        call    interrupt_handler
        
        popaq
        add     rsp, 16     ; removes interrupt number and error code
        iretq

    isr_without_error_code  0
    isr_without_error_code  1
    isr_without_error_code  2
    isr_without_error_code  3
    isr_without_error_code  4
    isr_without_error_code  5
    isr_without_error_code  6
    isr_without_error_code  7
    isr_with_error_code     8
    isr_without_error_code  9
    isr_with_error_code     10
    isr_with_error_code     11
    isr_with_error_code     12
    isr_with_error_code     13
    isr_with_error_code     14
    isr_without_error_code  15
    isr_without_error_code  16
    isr_with_error_code     17
    isr_without_error_code  18
    isr_without_error_code  19
    isr_without_error_code  20
    isr_with_error_code     21
    isr_without_error_code  22
    isr_without_error_code  23
    isr_without_error_code  24
    isr_without_error_code  25
    isr_without_error_code  26
    isr_without_error_code  27
    isr_without_error_code  28
    isr_with_error_code     29
    isr_with_error_code     30
    isr_without_error_code  31

    isr_without_error_code  32
    isr_without_error_code  33

    isr_without_error_code  34
    isr_without_error_code  35
    isr_without_error_code  36
    isr_without_error_code  37
    isr_without_error_code  38
    isr_without_error_code  39
    isr_without_error_code  40
    isr_without_error_code  41
    isr_without_error_code  42
    isr_without_error_code  43
    isr_without_error_code  44
    isr_without_error_code  45
    isr_without_error_code  46
    isr_without_error_code  47
    isr_without_error_code  48
    isr_without_error_code  49
    isr_without_error_code  50
    isr_without_error_code  51
    isr_without_error_code  52
    isr_without_error_code  53
    isr_without_error_code  54
    isr_without_error_code  55
    isr_without_error_code  56
    isr_without_error_code  57
    isr_without_error_code  58
    isr_without_error_code  59
    isr_without_error_code  60
    isr_without_error_code  61
    isr_without_error_code  62
    isr_without_error_code  63
    isr_without_error_code  64
    isr_without_error_code  65
    isr_without_error_code  66
    isr_without_error_code  67
    isr_without_error_code  68
    isr_without_error_code  69
    isr_without_error_code  70
    isr_without_error_code  71
    isr_without_error_code  72
    isr_without_error_code  73
    isr_without_error_code  74
    isr_without_error_code  75
    isr_without_error_code  76
    isr_without_error_code  77
    isr_without_error_code  78
    isr_without_error_code  79
    isr_without_error_code  80
    isr_without_error_code  81
    isr_without_error_code  82
    isr_without_error_code  83
    isr_without_error_code  84
    isr_without_error_code  85
    isr_without_error_code  86
    isr_without_error_code  87
    isr_without_error_code  88
    isr_without_error_code  89
    isr_without_error_code  90
    isr_without_error_code  91
    isr_without_error_code  92
    isr_without_error_code  93
    isr_without_error_code  94
    isr_without_error_code  95
    isr_without_error_code  96
    isr_without_error_code  97
    isr_without_error_code  98
    isr_without_error_code  99
    isr_without_error_code  100
    isr_without_error_code  101
    isr_without_error_code  102
    isr_without_error_code  103
    isr_without_error_code  104
    isr_without_error_code  105
    isr_without_error_code  106
    isr_without_error_code  107
    isr_without_error_code  108
    isr_without_error_code  109
    isr_without_error_code  110
    isr_without_error_code  111
    isr_without_error_code  112
    isr_without_error_code  113
    isr_without_error_code  114
    isr_without_error_code  115
    isr_without_error_code  116
    isr_without_error_code  117
    isr_without_error_code  118
    isr_without_error_code  119
    isr_without_error_code  120
    isr_without_error_code  121
    isr_without_error_code  122
    isr_without_error_code  123
    isr_without_error_code  124
    isr_without_error_code  125
    isr_without_error_code  126
    isr_without_error_code  127
    isr_without_error_code  128
    isr_without_error_code  129
    isr_without_error_code  130
    isr_without_error_code  131
    isr_without_error_code  132
    isr_without_error_code  133
    isr_without_error_code  134
    isr_without_error_code  135
    isr_without_error_code  136
    isr_without_error_code  137
    isr_without_error_code  138
    isr_without_error_code  139
    isr_without_error_code  140
    isr_without_error_code  141
    isr_without_error_code  142
    isr_without_error_code  143
    isr_without_error_code  144
    isr_without_error_code  145
    isr_without_error_code  146
    isr_without_error_code  147
    isr_without_error_code  148
    isr_without_error_code  149
    isr_without_error_code  150
    isr_without_error_code  151
    isr_without_error_code  152
    isr_without_error_code  153
    isr_without_error_code  154
    isr_without_error_code  155
    isr_without_error_code  156
    isr_without_error_code  157
    isr_without_error_code  158
    isr_without_error_code  159
    isr_without_error_code  160
    isr_without_error_code  161
    isr_without_error_code  162
    isr_without_error_code  163
    isr_without_error_code  164
    isr_without_error_code  165
    isr_without_error_code  166
    isr_without_error_code  167
    isr_without_error_code  168
    isr_without_error_code  169
    isr_without_error_code  170
    isr_without_error_code  171
    isr_without_error_code  172
    isr_without_error_code  173
    isr_without_error_code  174
    isr_without_error_code  175
    isr_without_error_code  176
    isr_without_error_code  177
    isr_without_error_code  178
    isr_without_error_code  179
    isr_without_error_code  180
    isr_without_error_code  181
    isr_without_error_code  182
    isr_without_error_code  183
    isr_without_error_code  184
    isr_without_error_code  185
    isr_without_error_code  186
    isr_without_error_code  187
    isr_without_error_code  188
    isr_without_error_code  189
    isr_without_error_code  190
    isr_without_error_code  191
    isr_without_error_code  192
    isr_without_error_code  193
    isr_without_error_code  194
    isr_without_error_code  195
    isr_without_error_code  196
    isr_without_error_code  197
    isr_without_error_code  198
    isr_without_error_code  199
    isr_without_error_code  200
    isr_without_error_code  201
    isr_without_error_code  202
    isr_without_error_code  203
    isr_without_error_code  204
    isr_without_error_code  205
    isr_without_error_code  206
    isr_without_error_code  207
    isr_without_error_code  208
    isr_without_error_code  209
    isr_without_error_code  210
    isr_without_error_code  211
    isr_without_error_code  212
    isr_without_error_code  213
    isr_without_error_code  214
    isr_without_error_code  215
    isr_without_error_code  216
    isr_without_error_code  217
    isr_without_error_code  218
    isr_without_error_code  219
    isr_without_error_code  220
    isr_without_error_code  221
    isr_without_error_code  222
    isr_without_error_code  223
    isr_without_error_code  224
    isr_without_error_code  225
    isr_without_error_code  226
    isr_without_error_code  227
    isr_without_error_code  228
    isr_without_error_code  229
    isr_without_error_code  230
    isr_without_error_code  231
    isr_without_error_code  232
    isr_without_error_code  233
    isr_without_error_code  234
    isr_without_error_code  235
    isr_without_error_code  236
    isr_without_error_code  237
    isr_without_error_code  238
    isr_without_error_code  239
    isr_without_error_code  240
    isr_without_error_code  241
    isr_without_error_code  242
    isr_without_error_code  243
    isr_without_error_code  244
    isr_without_error_code  245
    isr_without_error_code  246
    isr_without_error_code  247
    isr_without_error_code  248
    isr_without_error_code  249
    isr_without_error_code  250
    isr_without_error_code  251
    isr_without_error_code  252
    isr_without_error_code  253
    isr_without_error_code  254
    isr_without_error_code  255

    ; void idt_load(const idt_register *idtr)
    global idt_load
    idt_load:
        lidt    [rdi]       ; rdi is first parameter, therefore is 'idtr'
        ret

    ; void halt(void)
    global halt
    halt:
        hlt
        ret

section .bss:
    global isrs
    isrs:
        %assign i 0
        %rep 255
            dq      isr_%+i
            %assign i i+1
        %endrep
