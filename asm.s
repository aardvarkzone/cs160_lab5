.global Main
foo:
    pushl %ebp
    movl  %esp, %ebp
    subl  $4, %esp
    #visitAssignment
    pushl $45
    leal -0(%ebp), %eax
    pushl %eax
    popl %ebx
    popl %eax
    movl %eax, (%ebx)
    popl  %eax
    leave
    ret
Main:
    pushl %ebp
    movl  %esp, %ebp
    subl  $4, %esp
    leal -0(%ebp), %eax
    pushl %eax
    call foo
    leal -0(%ebp), %eax
    pushl %eax
    popl  %ebx
    movl  %eax, (%ebx)
    pushl $45
# Compiling ==
    popl  %ebx
    popl  %eax
    cmpl  %ebx, %eax
    je    label2
    pushl $0
    jmp   label3
label2:
    pushl $1
label3:
    popl  %eax
    cmpl  $0, %eax
    je    label0
    #visitAssignment
    pushl $3
    leal -0(%ebp), %eax
    pushl %eax
    popl %ebx
    popl %eax
    movl %eax, (%ebx)
    jmp   label1
label0:
    #visitAssignment
    pushl $4
    leal -0(%ebp), %eax
    pushl %eax
    popl %ebx
    popl %eax
    movl %eax, (%ebx)
label1:
    pushl $99
    popl  %eax
    leave
    ret
