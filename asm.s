.data

.text

.globl Main
    # Prologue
Main:
    pushl %ebp
    movl  %esp, %ebp
    subl  $404, %esp
    # visit VariableDecl
    # visit VariableDecl
    # visit StringAssignment
    #visit Variable
    leal 0(%ebp), %eax
    pushl %eax
    .data
str_label_0: .ascii "apple"
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
    # string input
    #visit Variable
    pushl $404 #offset from visitVar
    popl %eax #offset
    popl %ebx #value
    negl %eax #offset
    movl %ebx, (%ebp, %eax)
    # assign
    # visit Return
    pushl $99
    popl  %eax # for return
    leave
    # Epilogue
    ret
