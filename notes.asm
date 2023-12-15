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
    # offset from %ebp to get the variable x in memory
    # assign what is in $eax to x
    pushl $8 # <--- wrong, only if 8 is a parameter
    call foo
    #visit Variable
    pushl $8 #  <-- 
    popl  %ebx
    movl  %ebx, -4(%ebp)

    # visit Return
    pushl $97
    popl  %eax
    leave
    # Epilogue
    ret
