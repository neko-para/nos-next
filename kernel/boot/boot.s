.set ALIGN,    1 << 0
.set MEMINFO,  1 << 1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

.section .data
.global bootInfo

bootInfo:
.long 0

.section .text
.global _start
.type _start, @function

_start:
    mov $stack_top, %esp
    mov %ebx, (bootInfo)
    call _init
    call kernel_main
    cli
1:
    hlt
    jmp 1b

.size _start, . - _start
