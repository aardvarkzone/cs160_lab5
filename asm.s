.data

.text

.globl Main
    # Prologue
foo:
    pushl %ebp
    movl  %esp, %ebp
    subl  $0, %esp
    movl  8(%ebp), %eax
    movl  %eax, -12(%ebp)
    movl  12(%ebp), %eax
    movl  %eax, -8(%ebp)
    movl  16(%ebp), %eax
    movl  %eax, -4(%ebp)
    # visit VariableDecl
    # visit Return
    #visit Ident
    movl  -4(%ebp), %eax #move from ident
    pushl %eax #push
    popl  %eax # for return
    leave
    # Epilogue
    ret
    # Prologue
Main:
    pushl %ebp
    movl  %esp, %ebp
    subl  $4, %esp
    # visit VariableDecl
    #visit IntLit
    pushl $1
    #visit IntLit
    pushl $2
    #visit IntLit
    pushl $3
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
    #visit Ident
    movl  -4(%ebp), %eax #move from ident
    pushl %eax #push
    popl  %eax # for return
    leave
    # Epilogue
    ret
