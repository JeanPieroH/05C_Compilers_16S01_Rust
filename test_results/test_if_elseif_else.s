.data
print_fmt: .string "%ld\n"

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    
    movq $1, %rax
    cmpq $0, %rax
    jl if_true_0
    
    # Bloque else: evaluamos 1 > 0 (que es verdadera)
    movq $1, %rax
    cmpq $0, %rax
    jg if_true_1
    
    movq $4, %rax
    movq %rax, -24(%rbp)    # variable d en -24(%rbp)
    jmp end_if_1
    
if_true_1:
    movq $3, %rax
    movq %rax, -16(%rbp)    # variable c en -16(%rbp)
    
end_if_1:
    jmp end_if_0
    
if_true_0:
    movq $1, %rax
    movq %rax, -8(%rbp)     # variable a en -8(%rbp)
    
end_if_0:
    # Epilogo de la función
    movq $0, %rax           # return 0
    movq %rbp, %rsp
    popq %rbp
    ret
.section .note.GNU- stack,"",@progbits