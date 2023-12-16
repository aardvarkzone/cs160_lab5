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
    addl $4, %esi
    subl $4, %edi
    testl %eax, %eax
    jne label0
    # visit Return
    pushl $99
    popl  %eax # for return
    leave
    # Epilogue
    ret
