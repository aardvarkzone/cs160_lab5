.data

.text

.globl Main
    # Prologue
Main:
    pushl %ebp
    movl  %esp, %ebp
    subl  $8, %esp
    # visit VariableDecl
    # visit VariableDecl
    #visit Assignment
    pushl $0
    pushl $0
    pushl $1
# Compiling p
    orl %ebx, %eax
# Compiling p
    orl %ebx, %eax
    #visit Variable
    pushl $8 #offset from visitVar
    popl %eax #offset
    popl %ebx #value
    negl %eax #offset
    movl %ebx, (%ebp, %eax)
    # assign
    #visit Assignment
    #visit IntLit
    pushl $0
    #visit Variable
    pushl $4 #offset from visitVar
    popl %eax #offset
    popl %ebx #value
    negl %eax #offset
    movl %ebx, (%ebp, %eax)
    # assign
label0:
    #visit Ident
    movl  -8(%ebp), %eax #move from ident
    pushl %eax #push
    popl  %eax
    cmpl  $0, %eax
    je    label1
    #visit Assignment
    #visit Ident
    movl  -4(%ebp), %eax #move from ident
    pushl %eax #push
    #visit IntLit
    pushl $1
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
    jmp   label0
label1:
    # visit Return
    #visit Ident
    movl  -4(%ebp), %eax #move from ident
    pushl %eax #push
    popl  %eax # for return
    leave
    # Epilogue
    ret
