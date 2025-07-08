.data
print_fmt: .string "%ld\n"

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp

    movq $1, %rax           # true = 1
    cmpq $1, %rax           # comparar con 1
    jne end_if_0            # si no es verdadero, saltar

    movq $1, %rax
    movq %rax, -8(%rbp)    

end_if_0:
    movq $0, %rax         
    movq %rbp, %rsp
    popq %rbp
    ret