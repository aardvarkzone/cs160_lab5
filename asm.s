.global Main
foo:
    pushl %ebp
    movl  %esp, %ebp
    subl  $4, %esp
    #visitAssignment
    pushl $1
# variable!
pushl $4
    popl %eax
    popl %ebx
    negl %eax
    movl %ebx, (%ebp, %eax)
    pushl $45
    popl  %eax
    leave
    ret
Main:
    pushl %ebp
    movl  %esp, %ebp
    subl  $4, %esp
    #visitAssignment
    pushl $5
# variable!
pushl $4
    popl %eax
    popl %ebx
    negl %eax
    movl %ebx, (%ebp, %eax)
    pushl $97
    popl  %eax
    leave
    ret
