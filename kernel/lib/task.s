.global switchToTask
.type switchToTask, @function
switchToTask:
    push %ebx
    push %esi
    push %edi
    push %ebp

    movl (currentTask), %edi
    movl %esp, (%edi)

    movl 20(%esp), %esi
    movl %esi, (currentTask)

    movl (%esi), %esp
    movl 4(%esi), %eax
    movl 8(%esi), %ebx
    movl %ebx, (tss + 4)
    movl %cr3, %ecx

    cmpl %ecx, %eax
    je .done
    mov %eax, %cr3

.done:
    popl %ebp
    popl %edi
    popl %esi
    popl %ebx
    ret
