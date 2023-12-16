.globl Main
    # Prologue
Main:
    pushl %ebp
    movl  %esp, %ebp
    subl  $8, %esp
    # visit VariableDecl
    # visit VariableDecl
    #visit Assignment
    #visit IntLit
    pushl $1
    #visit Variable
    pushl $4 #offset from visitVar
    popl %eax #offset
    popl %ebx #value
    negl %eax #offset
    movl %ebx, (%ebp, %eax)
    # assign
    #visit Assignment
    #visit IntLit
    pushl $10
    #visit Variable
    pushl $8 #offset from visitVar
    popl %eax #offset
    popl %ebx #value
    negl %eax #offset
    movl %ebx, (%ebp, %eax)
    # assign
    # visit Return
    #visit Ident
    popl  %eax # for return
    leave
    # Epilogue
    ret
