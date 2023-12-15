.global Main
Main:
    pushl %ebp
    movl  %esp, %ebp
    subl  $12, %esp
    pushl $45
    leal -1(%ebp), %eax
    pushl %eax
    pushl $1
    popl %eax
    neg %eax
    pushl %eax
    leal -5(%ebp), %eax
    pushl %eax
    pushl $99
    leal -0(%ebp), %eax
    pushl %eax
    popl  %eax
    leave
    ret
