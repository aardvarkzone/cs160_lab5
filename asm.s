.globl Main
    # Prologue
foo:
    pushl %ebp
    movl  %esp, %ebp
    subl  $0, %esp
    # visit Return
    #visit IntLit
    pushl $2
    popl  %eax
    leave
    # Epilogue
    ret
    # Prologue
Main:
    pushl %ebp
    movl  %esp, %ebp
    subl  $4, %esp
    # visit VariableDecl
    #visit Call
    call foo
    pushl  %eax
    #visit Variable
    pushl $4 #offset from visitVar
    popl  %ebx
    popl  %eax
    negl  %ebx
    movl  %eax, (%ebp, %ebx)
    # visit Return
    pushl $97
    popl  %eax
    leave
    # Epilogue
    ret
