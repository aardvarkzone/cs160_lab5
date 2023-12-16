.data

.text

.globl Main
    # Prologue
Main:
    pushl %ebp
    movl  %esp, %ebp
    subl  $400, %esp
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
    # visit Return
    pushl $99
    popl  %eax # for return
    leave
    # Epilogue
    ret
