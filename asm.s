.global Main
foo:
    pushl %ebp
    movl  %esp, %ebp
    subl  $4, %esp
    #visitAssignment
    pushl $45
    movl -0(%ebp), %eax
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
#call
    movl -0(%ebp), %eax
    pushl %eax
    call foo
    movl -0(%ebp), %eax
    pushl %eax
    popl  %ebx
    movl  %eax, (%ebx)
#endcall
    pushl $99
    popl  %eax
    leave
    ret
