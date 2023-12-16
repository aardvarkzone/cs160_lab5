.data

.text

.globl Main
    # Prologue
Main:
    pushl %ebp
    movl  %esp, %ebp
    subl  $48, %esp
    # visit VariableDecl
    # visit VariableDecl
    # visit VariableDecl
    # visit StringAssignment
    #visit Variable
    leal 4(%ebp), %eax
    pushl %eax
    .data
str_label_0: .ascii "foobar"
    .text
    leal (%ebp), %edi
    addl %eax, %edi
    movl $str_label_0, %esi
label0:
    movl (%esi), %eax
    movl %eax, (%edi)
    addl $4, %esi
    addl $4, %edi
    testl %eax, %eax
    jne label0
    #visit Assignment
    pushl $97
    # visit ArrayElement
    # v1
    leal 4(%ebp), %eax
    #visit IntLit
    pushl $0
    popl %ecx
    imull $4, %ecx
    addl %ecx, %eax
    pushl %eax
    popl %eax #offset
    popl %ebx #value
    negl %eax #offset
    movl %ebx, (%ebp, %eax)
    # assign
    # visit Return
    #visit Ident
    movl  -48(%ebp), %eax #move from ident
    pushl %eax #push
    popl  %eax # for return
    leave
    # Epilogue
    ret
