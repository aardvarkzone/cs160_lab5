.globl Main
    # Prologue
Main:
    pushl %ebp
    movl  %esp, %ebp
    subl  $4, %esp
    # visit VariableDecl
    #visit Assignment
    #visit IntLit
    pushl $2
    #visit Variable
    pushl $4 #offset from visitVar
    popl %eax #offset
    popl %ebx #value
    negl %eax #offset
    movl %ebx, (%ebp, %eax)
    # assign
    #visit Assignment
    # visit Plus
    popl  %ebx
    popl  %eax
    addl  %ebx, %eax
    pushl  %eax
    #visit Variable
    pushl $4 #offset from visitVar
    popl %eax #offset
    popl %ebx #value
    negl %eax #offset
    movl %ebx, (%ebp, %eax)
    # assign
    # visit Return
    #visit Ident
    movl  -4(%ebp), %eax #move from ident
    pushl %eax #push
    popl  %eax # for return
    leave
    # Epilogue
    ret
