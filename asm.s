.globl Main
    # Prologue
foo:
    pushl %ebp
    movl  %esp, %ebp
    subl  $0, %esp
    # visit Return
    pushl $1
    popl  %eax
    leave
    # Epilogue
    ret
    # Prologue
Main:
    pushl %ebp
    movl  %esp, %ebp
    subl  $8, %esp
    # visit VariableDecl
    # visit VariableDecl
    #visit Call
    #visit Variable
    pushl $8
    call foo
    #visit Variable
    pushl $8
    popl  %ebx
    movl  %eax, (%ebx)
    # visit Return
    pushl $97
    popl  %eax
    leave
    # Epilogue
    ret
