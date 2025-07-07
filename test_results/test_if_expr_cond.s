.data
print_fmt: .string "%ld\n"

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $24, %rsp
    
    movq $10, %rax
    movq %rax, -8(%rbp)     # variable x en -8(%rbp)
    
    movq -8(%rbp), %rax     # cargar x
    cmpq $10, %rax          # comparar x con 10
    jne end_if_0            # si x != 10, saltar al final (short-circuit)
    
    movq $1, %rax           # true = 1
    cmpq $1, %rax           # comparar con 1
    jne end_if_0            # si no es verdadero, saltar al final
    
    movq $1, %rax
    movq %rax, -16(%rbp)    # variable y en -16(%rbp)
    
end_if_0:
    movq $0, %rax           # return 0
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU- stack,"",@progbits