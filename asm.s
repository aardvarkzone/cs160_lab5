.global Main
foo:
    pushl %ebp
    movl  %esp, %ebp
    subl  $4, %esp
    #visitAssignment
    pushl $40
    movl -4(%ebp), %eax
    pushl %eax
    popl %ebx
    # pop var
    popl %eax
    # pop val
    movl %eax, (%ebx)
    # assign
    popl $40
    pushl $45
    popl  %eax
    leave
    ret
Main:
    pushl %ebp
    movl  %esp, %ebp
    subl  $4, %esp
#call
    movl -0(%ebp), %eax
    pushl %eax
    call foo
    popl  %ebx
    movl  %eax, (%ebx)
#endcall
    pushl $97
    popl  %eax
    leave
    ret
