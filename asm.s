.data

.text

.globl Main
    # Prologue
Main:
    pushl %ebp
    movl  %esp, %ebp
    subl  $12, %esp
    # visit VariableDecl
    # visit StringAssignment
    #visit Variable
    pushl $4 #offset from visitVar
    .data
str_label_0: .ascii "hi"
    .text
    popl %eax
    negl %eax
    movl %ebp, %edi
    addl %eax, %edi
    movl $str_label_0, %esi
label0:
    movzbl (%esi), %eax
    movl %eax, (%edi)
    subl $4, %esi
    subl $4, %edi
    testl %eax, %eax
    jne label0
    # visit Return
    # visit ArrayAccess
# made it
    leal 0(%ebp), %eax
    #visit IntLit
    pushl $0
    popl %ecx
    addl %ecx, %eax
    pushl %eax
    popl  %eax # for return
    leave
    # Epilogue
    ret
